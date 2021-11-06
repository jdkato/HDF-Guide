#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFileInfo>
#include <QMainWindow>
#include <QTreeWidgetItem>
#include <highfive/H5File.hpp>

#include "DockAreaWidget.h"
#include "DockManager.h"
#include "DockWidget.h"
#include "fa/QtAwesome/QtAwesome.h"

QT_BEGIN_NAMESPACE namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow() override;

 protected:
  void init();

  void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
  void changeEvent(QEvent *event) Q_DECL_OVERRIDE;

 private slots:
  void onFileOpen();
  void onAbout();

 private:
  void createActions();

  void loadFile(const QString &path);
  void datasetChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
  void openDataset(QString name);

  bool isLightTheme();
  void adjustColors();

  Ui::MainWindow *ui;
  QToolBar *toolBar;

  ads::CDockManager *DockManager;
  ads::CDockAreaWidget *StatusDockArea;
  ads::CDockWidget *SummaryDockWidget;
  ads::CDockWidget *TableDockWidget;
  ads::CDockWidget *PropertiesDockWidget;
  ads::CDockWidget *ConsoleDockWidget;

  QtAwesome *awesome;

  QMenu *fileMenu;
  QMenu *viewMenu;
  QMenu *helpMenu;

  QFileInfo HDF5;
};

#endif  // MAINWINDOW_H
