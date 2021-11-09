#include "MainWindow.h"

#include <QCompleter>
#include <QDebug>
#include <QFileDialog>
#include <QTimer>
#include <QTreeWidgetItem>

#include "AboutDialog.h"
#include "AnalysisViewWidget.h"
#include "DockAreaTabBar.h"
#include "DockAreaTitleBar.h"
#include "DockAreaWidget.h"
#include "DockComponentsFactory.h"
#include "FloatingDockContainer.h"
#include "H5Cpp.h"
#include "HDF5Model.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  this->awesome = new QtAwesome(this);
  this->awesome->initFontAwesome();
  init();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::init() {
  ads::CDockManager::setConfigFlag(ads::CDockManager::OpaqueSplitterResize,
                                   true);
  ads::CDockManager::setConfigFlag(ads::CDockManager::XmlCompressionEnabled,
                                   true);
  ads::CDockManager::setConfigFlag(ads::CDockManager::FocusHighlighting, true);

  this->DockManager = new ads::CDockManager(this);
  // We clear the stylesheet because we set our own later on -- the default
  // stylesheet doesn't look good in dark modes.
  this->DockManager->setStyleSheet("");

  QVBoxLayout *layout = new QVBoxLayout;
  QWidget *mainArea = new QWidget;
  layout->addWidget(ui->searchLineEdit);
  layout->addWidget(ui->stackedWidget);
  layout->setContentsMargins(0, 0, 0, 0);
  mainArea->setLayout(layout);

  // The "Central" dock area consists of our search bar and stacked widget for
  // previewing datasets, which might be a table, image, chart, etc.
  ads::CDockWidget *CentralDockWidget = new ads::CDockWidget("CentralWidget");
  CentralDockWidget->setWidget(mainArea);
  auto *CentralDockArea =
      this->DockManager->setCentralWidget(CentralDockWidget);
  CentralDockArea->setAllowedAreas(ads::DockWidgetArea::OuterDockAreas);

  // The "Group" dock area is our file-like browser for the file's groups.
  this->TableDockWidget = new ads::CDockWidget("Groups");
  this->TableDockWidget->setWidget(ui->explorer);
  this->TableDockWidget->setMinimumSizeHintMode(
      ads::CDockWidget::MinimumSizeHintFromDockWidget);
  this->TableDockWidget->resize(250, 150);
  this->TableDockWidget->setMinimumSize(200, 150);

  this->DockManager->addDockWidget(ads::DockWidgetArea::LeftDockWidgetArea,
                                   TableDockWidget);

  // The "Summary" dock area shows an at-a-glance summary of a dataset. It's
  // designed to be similar to the right-hand file preview available in the
  // macOS Finder app.
  this->SummaryDockWidget = new ads::CDockWidget("Summary");
  this->SummaryDockWidget->setWidget(ui->summary);
  this->SummaryDockWidget->setMinimumSizeHintMode(
      ads::CDockWidget::MinimumSizeHintFromDockWidget);
  this->SummaryDockWidget->resize(250, 150);
  this->SummaryDockWidget->setMinimumSize(200, 150);

  auto SummaryArea =
      this->DockManager->addDockWidget(ads::DockWidgetArea::RightDockWidgetArea,
                                       SummaryDockWidget, CentralDockArea);

  // The "Attributes" dock area shows a table of a dataset's attributes, their
  // type, and their value.
  //
  // TODO: Should we support searching for a dataset by its attributes? Maybe
  // something like `dataset:my_attribute:my_type`?
  this->PropertiesDockWidget = new ads::CDockWidget("Attributes");
  this->PropertiesDockWidget->setWidget(ui->attributes);
  this->PropertiesDockWidget->setMinimumSizeHintMode(
      ads::CDockWidget::MinimumSizeHintFromDockWidget);
  this->PropertiesDockWidget->resize(250, 150);
  this->PropertiesDockWidget->setMinimumSize(200, 150);

  this->DockManager->addDockWidget(ads::DockWidgetArea::BottomDockWidgetArea,
                                   PropertiesDockWidget, SummaryArea);

  // The "Console" dock area is a simple read-only, text-based widget for
  // communicating with a user.
  //
  // TODO: hide by default?
  this->ConsoleDockWidget = new ads::CDockWidget("Console");
  this->ConsoleDockWidget->setWidget(ui->console);
  this->ConsoleDockWidget->setMinimumSizeHintMode(
      ads::CDockWidget::MinimumSizeHintFromDockWidget);
  this->ConsoleDockWidget->resize(250, 150);
  this->ConsoleDockWidget->setMinimumSize(200, 150);

  this->DockManager->addDockWidget(ads::DockWidgetArea::BottomDockWidgetArea,
                                   ConsoleDockWidget, CentralDockArea);

  ui->mainwidget->layout()->setContentsMargins(0, 0, 0, 0);
  ui->searchLineEdit->setIcon(this->awesome->icon(fa::search));
  ui->explorer->setGroupIcon(this->awesome->icon(fa::folderopen));
  ui->explorer->setDbIcon(this->awesome->icon(fa::database));

  this->createActions();

  connect(ui->editor, &AnalysisViewWidget::pathChanged, this,
          &MainWindow::loadFile);
  connect(ui->explorer, &QTreeWidget::currentItemChanged, this,
          &MainWindow::datasetChanged);

  // Handle the "Jump to dataset" functionality.
  //
  // The idea is that we need to go from the full path of a dataset, e.g.
  // foo/bar/baz, to its base `TreeWidgetItem`.
  connect(ui->searchLineEdit, &QLineEdit::returnPressed, this, [this]() {
    QStringList query = ui->searchLineEdit->text().split("/");
    QString row = query[query.length() - 1];
    QTimer::singleShot(0, ui->searchLineEdit, SLOT(selectAll()));

    auto items =
        ui->explorer->findItems(row, Qt::MatchExactly | Qt::MatchRecursive);

    int l = items.length();
    if (l == 1) {
      // If there's only one match, we don't need to search.
      ui->explorer->setCurrentItem(items[0]);
    } else if (l > 0) {
      // If there's more than one, we need to walk of each tree looking for the
      // item with a matching path.
      foreach (auto curr, items) {
        bool isMatch = true;

        QTreeWidgetItem *temp = curr;
        while (curr->parent() != nullptr && isMatch) {
          curr = curr->parent();
          isMatch = query.contains(curr->text(0));
        }

        if (isMatch) {
          ui->explorer->setCurrentItem(temp);
          break;
        }
      }
    }
  });

  this->adjustColors();
}

void MainWindow::closeEvent(QCloseEvent *event) {
  this->DockManager->deleteLater();
  event->accept();
}

void MainWindow::createActions() {
  this->fileMenu = ui->menubar->addMenu("&File");

  QAction *open = this->fileMenu->addAction("Open...");
  connect(open, &QAction::triggered, this, &MainWindow::onFileOpen);

  this->viewMenu = ui->menubar->addMenu("&View");

  this->viewMenu->addAction(this->TableDockWidget->toggleViewAction());
  this->viewMenu->addAction(this->ConsoleDockWidget->toggleViewAction());
  this->viewMenu->addAction(this->PropertiesDockWidget->toggleViewAction());
  this->viewMenu->addSeparator();

  this->helpMenu = ui->menubar->addMenu("&Help");
  this->helpMenu->addAction(ui->actionAbout);
}

void MainWindow::onAbout() {
  AboutDialog dialog;
  dialog.exec();
}

void MainWindow::onFileOpen() {
  QString path = QFileDialog::getOpenFileName(this, tr("Open File"), "");
  if (path.isEmpty()) {
    return;
  }
  this->loadFile(path);
}

bool MainWindow::isLightTheme() {
  return QPalette().color(QPalette::Window).lightness() > 100;
}

void MainWindow::adjustColors() {
  QFile styleFile(":/css/main.css");
  styleFile.open(QFile::ReadOnly);
  QString style(styleFile.readAll());
  this->DockManager->setStyleSheet(style);
}

void MainWindow::changeEvent(QEvent *event) {
#ifdef Q_OS_MACX
  if (event->type() == QEvent::PaletteChange) {
    this->adjustColors();
  }
#endif
  QMainWindow::changeEvent(event);
}

void MainWindow::loadFile(const QString &path) {
  this->HDF5.setFile(path);
  try {
    HighFive::File file(path.toStdString(), HighFive::File::ReadOnly);
    if (file.isValid()) {
      ui->editor->setEnabled(false);
      ui->editor->setHtml("");

      ui->editor->showSearch();
      ui->explorer->browse(&file);
      ui->searchLineEdit->updateModel(ui->explorer,
                                      this->awesome->icon(fa::database));

      ui->console->write(
          {QString("Successfully loaded '%1'.").arg(this->HDF5.fileName())});
    }
  } catch (const std::exception &e) {
    ui->console->write({QString("Failed to load '%1': %2")
                            .arg(this->HDF5.fileName(), e.what())});
  }
}

void MainWindow::datasetChanged(QTreeWidgetItem *curr, QTreeWidgetItem *prev) {
  QString path = curr->text(0);
  while (curr->parent() != nullptr) {
    curr = curr->parent();
    path = curr->text(0) + "/" + path;
  }
  this->openDataset(path);
}

void MainWindow::openDataset(QString name) {
  // FIXME: We currently use both HighFive *and* the official C++ library.
  //
  // Ideally, I'd like to move to only one (probably the official library) but
  // I (it could be developer error) wasn't able to do so just yet: HighFive
  // crashes while trying to read string-type attributes (see
  // `AttributeTableWidget.cpp`) and it is used extensively elsewhere.
  HighFive::File hf(this->HDF5.absoluteFilePath().toStdString(),
                    HighFive::File::ReadOnly);
  H5::H5File h5(this->HDF5.absoluteFilePath().toStdString(), H5F_ACC_RDONLY);

  std::string dn = name.toStdString();
  if (hf.exist(dn) && hf.getObjectType(dn) == HighFive::ObjectType::Dataset) {
    HighFive::DataSet hfdata = hf.getDataSet(dn);
    H5::DataSet h5data = h5.openDataSet(dn);

    ui->stackedWidget->setCurrentIndex(1);
    ui->data->setModel(new HDF5Model(this, hfdata, h5data));
    ui->summary->show(name, hfdata);
    ui->attributes->show(h5data, hfdata);
  }
}
