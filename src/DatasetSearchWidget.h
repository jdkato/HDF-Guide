#ifndef DATASETSEARCHWIDGET_H
#define DATASETSEARCHWIDGET_H

#include <QAbstractItemView>
#include <QCompleter>
#include <QIcon>
#include <QLineEdit>
#include <QStandardItemModel>
#include <QStyledItemDelegate>
#include <QTreeWidget>
#include <QWidget>
#include <highfive/H5File.hpp>

class PopupDelegate : public QStyledItemDelegate {
 public:
  using QStyledItemDelegate::QStyledItemDelegate;

 protected:
  void initStyleOption(QStyleOptionViewItem *option,
                       const QModelIndex &index) const {
    QStyledItemDelegate::initStyleOption(option, index);
    option->text = index.data(Qt::UserRole + 1).toString();
  }
};

class CustomCompleter : public QCompleter {
 public:
  using QCompleter::QCompleter;
  QString pathFromIndex(const QModelIndex &index) const { 
    return index.data(Qt::UserRole + 1).toString();
  }
};

class DatasetSearchWidget : public QLineEdit {
  Q_OBJECT
 public:
  DatasetSearchWidget(QWidget *parent = nullptr);

  void setIcon(QIcon icon);
  void updateModel(QTreeWidget *root, QIcon icon);
};

#endif  // DATASETSEARCHWIDGET_H
