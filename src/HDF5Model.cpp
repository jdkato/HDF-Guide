#include "HDF5Model.h"

HDF5Model::HDF5Model(QObject *parent, HighFive::DataSet hf, H5::DataSet h5)
    : QAbstractTableModel(parent) {
  this->ds = hf;
  std::vector<size_t> dims = this->ds.getDimensions();
  this->h5ds = h5;

  auto dt = this->ds.getDataType();

  int members = 1;
  if (dt.getClass() == HighFive::DataTypeClass::Compound) {
    auto ct = this->h5ds.getCompType();

    members = ct.getNmembers();
    for (int i = 0; i < members; i++) {
      this->memberNames << QString::fromStdString(ct.getMemberName(i));
    }
  }

  this->rank = dims.size();
  if (this->rank == 0) {
    this->nRows = 1;
    this->nCols = 1;
  } else if (this->rank == 1) {
    this->nRows = dims[0];
    this->nCols = 1;
  } else {
    this->nRows = dims[this->rank - 1];
    this->nCols = dims[this->rank - 2];
  }

  this->nCols = this->nCols * members;
}

int HDF5Model::rowCount(const QModelIndex &parent) const {
  return this->nRows;
};

int HDF5Model::columnCount(const QModelIndex &parent) const {
  return this->nCols;
};

QVariant HDF5Model::headerData(int section, Qt::Orientation orientation,
                               int role) const {
  if (role != Qt::DisplayRole) {
    return QVariant();
  } else if (orientation == Qt::Orientation::Horizontal && section <
             this->memberNames.size()) {
    return this->memberNames[section];
  }
  return section;
}

QVariant HDF5Model::fromComp(int row, int col) const {
  std::string name = this->memberNames[col].toStdString();

  size_t coord[1][2];
  // TODO: handle dims
  coord[0][0] = row;
  coord[0][1] = col;

  hsize_t col_dims[1];
  col_dims[0] = 1;

  auto memspace = H5::DataSpace(this->rank, col_dims);
  auto filespace = this->h5ds.getSpace();
  filespace.selectElements(H5S_SELECT_SET, 1, (const hsize_t *)coord);

  auto ct = this->h5ds.getCompType();
  switch (ct.getMemberDataType(col).getClass()) {
    case H5T_INTEGER: {
      H5::CompType mtype2(sizeof(int));
      int s3[1];
      mtype2.insertMember(name, 0, H5::PredType::NATIVE_INT);
      this->h5ds.read(s3, mtype2, memspace, filespace);
      return s3[0];
    }
    case H5T_FLOAT: {
      H5::CompType mtype2(sizeof(float));
      float s3[1];
      mtype2.insertMember(name, 0, H5::PredType::NATIVE_FLOAT);
      this->h5ds.read(s3, mtype2, memspace, filespace);
      return s3[0];
    }
    case H5T_STRING: {
      // TODO: Does this only work if `sv` is in the first column?
      std::string sv;
      this->h5ds.read(sv, this->h5ds.getStrType(), memspace, filespace);
      return QString::fromStdString(sv);
    }
  }

  return QVariant();
}

QVariant HDF5Model::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || role != Qt::DisplayRole) {
    return QVariant();
  }

  unsigned long col = index.column();
  unsigned long row = index.row();
  if (col >= this->nCols || row >= this->nRows) {
    return QVariant();
  }

  auto s = HighFive::ElementSet({row});
  if (this->rank == 2) {
    s = HighFive::ElementSet({col, row});
  } else if (this->rank == 3) {
    s = HighFive::ElementSet({0, col, row});
  } else if (this->rank > 3) {
    // TODO: What do we do in these cases?
    return QVariant();
  }

  auto dt = this->ds.getDataType();
  switch (dt.getClass()) {
    case HighFive::DataTypeClass::Integer: {
      std::vector<int> di;
      this->ds.select(s).read(di);
      return (di.size() > 0) ? di[0] : QVariant();
    }
    case HighFive::DataTypeClass::Float: {
      std::vector<float> df;
      this->ds.select(s).read(df);
      return (df.size() > 0) ? df[0] : QVariant();
    }
    case HighFive::DataTypeClass::String: {
      std::vector<std::string> dss;
      this->ds.select(s).read(dss);
      return (dss.size() > 0) ? QString::fromStdString(dss[0]) : QVariant();
    }
    case HighFive::DataTypeClass::Compound:
      return this->fromComp(row, col);
    default:
      return QVariant();
  }
};
