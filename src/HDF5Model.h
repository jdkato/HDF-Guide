#ifndef HDF5MODEL_H
#define HDF5MODEL_H

#include <QAbstractTableModel>
#include <QObject>
#include <highfive/H5File.hpp>

#include "H5Cpp.h"

class HDF5Model : public QAbstractTableModel {
  Q_OBJECT
 public:
  HDF5Model(QObject *parent, HighFive::DataSet hf, H5::DataSet h5);

  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  int columnCount(const QModelIndex &parent = QModelIndex()) const;

  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const;

 private:
  QVariant fromComp(int row, int col) const;

  HighFive::DataSet ds;
  H5::DataSet h5ds;

  QStringList memberNames;

  int nRows;
  int nCols;
  int rank;
};

#endif  // HDF5MODEL_H
