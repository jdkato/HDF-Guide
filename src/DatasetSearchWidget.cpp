#include "DatasetSearchWidget.h"

#include <QTreeWidgetItemIterator>

DatasetSearchWidget::DatasetSearchWidget(QWidget *parent) : QLineEdit(parent) {
  this->setPlaceholderText("Search for a dataset ...");
  // TODO: The QIcon-based icon placement doesn't seem to work on macOS
  // consistently?
  //
  // ui->searchLineEdit->setPlaceholderText("🔍");
}

void DatasetSearchWidget::setIcon(QIcon icon) {
  this->addAction(icon, QLineEdit::LeadingPosition);
}

void DatasetSearchWidget::updateModel(QTreeWidget *root, QIcon icon) {
  QStandardItemModel *model = new QStandardItemModel(this);
  QTreeWidgetItemIterator it(root);

  while (*it) {
    QTreeWidgetItem *curr = (*it);
    QString path = curr->text(0);

    QStandardItem *item = new QStandardItem(path);
    item->setIcon(icon);

    while (curr->parent() != nullptr) {
      curr = curr->parent();
      path = curr->text(0) + "/" + path;
    }

    item->setData(path, Qt::UserRole + 1);
    model->appendRow(item);

    ++it;
  }

  CustomCompleter *completer = new CustomCompleter(this);
  completer->setModel(model);

  PopupDelegate *delegate = new PopupDelegate(this);
  completer->popup()->setItemDelegate(delegate);

  this->setCompleter(completer);
}
