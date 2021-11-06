#ifndef ATTRIBUTETABLEWIDGET_H
#define ATTRIBUTETABLEWIDGET_H

#include <QObject>
#include <QTableWidget>
#include <highfive/H5File.hpp>

#include "H5Cpp.h"

class AttributeTableWidget : public QTableWidget {
  Q_OBJECT
 public:
  AttributeTableWidget(QWidget* parent = nullptr);

  void show(H5::DataSet h5data, HighFive::DataSet hfdata);
};

#endif  // ATTRIBUTETABLEWIDGET_H
