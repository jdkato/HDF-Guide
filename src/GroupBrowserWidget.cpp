#include "GroupBrowserWidget.h"

#include <QDebug>

GroupBrowserWidget::GroupBrowserWidget(QWidget* parent) : QTreeWidget(parent) {
  this->setHeaderHidden(true);
  this->setEnabled(false);
}

GroupBrowserWidget::~GroupBrowserWidget() {}

void GroupBrowserWidget::setGroupIcon(QIcon group) { this->groupIcon = group; }
void GroupBrowserWidget::setDbIcon(QIcon db) { this->dbIcon = db; }

void GroupBrowserWidget::listObjects(HighFive::Group* group,
                                     QTreeWidgetItem* parent) {
  for (const auto& c : group->listObjectNames()) {
    if (!group->exist(c) || group->getLinkType(c) != HighFive::LinkType::Hard) {
      // Not a hard link ...
      //
      // TODO: Do we need to preview these?
      continue;
    } else if (group->getObjectType(c) == HighFive::ObjectType::Group) {
      QTreeWidgetItem* itm = new QTreeWidgetItem(parent);
      itm->setText(0, QString::fromStdString(c));
      itm->setIcon(0, this->groupIcon);
      auto child = group->getGroup(c);
      this->listObjects(&child, itm);
    } else if (group->getObjectType(c) == HighFive::ObjectType::Dataset) {
      QTreeWidgetItem* itm = new QTreeWidgetItem(parent);
      itm->setText(0, QString::fromStdString(c));
      itm->setIcon(0, this->dbIcon);
    }
  }
}

void GroupBrowserWidget::browse(HighFive::File* file) {
  QString name = QString::fromStdString(file->getName());

  this->setEnabled(true);
  this->setHeaderLabel(name);
  this->setHeaderHidden(false);

  for (const auto& name : file->listObjectNames()) {
    auto type = file->getObjectType(name);
    QTreeWidgetItem* itm = new QTreeWidgetItem(this);

    itm->setText(0, QString::fromStdString(name));
    if (type == HighFive::ObjectType::Group) {
      itm->setIcon(0, this->groupIcon);
      auto group = file->getGroup(name);
      this->listObjects(&group, itm);
    } else if (type == HighFive::ObjectType::Dataset) {
      itm->setIcon(0, this->dbIcon);
    }
  }
}
