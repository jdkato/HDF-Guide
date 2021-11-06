#include "AboutDialog.h"

#include "ui_AboutDialog.h"

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::AboutDialog) {
  ui->setupUi(this);

  const QString appInfo = QString(ui->messageLabel->text())
                              .arg(qApp->applicationDisplayName())
                              .arg(qApp->applicationVersion());
  ui->messageLabel->setText(appInfo);

  const QString description =
      QString("<p>%1</p><p>%2</p>")
          .arg(tr("An exploration app for HDF5 files: search, query, and "
                  "quickly navigate even the largest of files."))
          .arg(tr("Built with C++, Qt, libhdf5, and open-source on GitHub."));

  ui->descriptionLabel->setText(description);
}

AboutDialog::~AboutDialog() { delete ui; }
