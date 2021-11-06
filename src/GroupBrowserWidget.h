#ifndef GROUPBROWSERWIDGET_H
#define GROUPBROWSERWIDGET_H

#include <QIcon>
#include <QTreeWidget>
#include <highfive/H5File.hpp>

class GroupBrowserWidget : public QTreeWidget {
  Q_OBJECT

 public:
  explicit GroupBrowserWidget(QWidget* parent = nullptr);
  ~GroupBrowserWidget();

  void browse(HighFive::File* file);
  void setGroupIcon(QIcon group);
  void setDbIcon(QIcon db);

 private:
  void listObjects(HighFive::Group* group, QTreeWidgetItem* parent);

  QIcon groupIcon;
  QIcon dbIcon;
};

#endif
