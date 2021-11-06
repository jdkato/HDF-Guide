#include "AttributeTableWidget.h"

#include <QDebug>
#include <QHeaderView>

#include "H5Cpp.h"

AttributeTableWidget::AttributeTableWidget(QWidget* parent)
    : QTableWidget(parent) {
  this->setRowCount(0);
  this->setColumnCount(3);
  this->setHorizontalHeaderLabels({"Attribute", "Type", "Value"});
  this->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  this->setSelectionBehavior(QAbstractItemView::SelectRows);

  this->horizontalHeader()->hide();
  this->verticalHeader()->hide();
}

void AttributeTableWidget::show(H5::DataSet h5data, HighFive::DataSet hfdata) {
  this->clearContents();
  this->setRowCount(0);

  for (const auto& name : hfdata.listAttributeNames()) {
    if (!h5data.attrExists(name)) {
      continue;
    }

    this->insertRow(this->rowCount());
    int row = this->rowCount() - 1;

    QString k = QString::fromStdString(name);

    HighFive::Attribute hfa = hfdata.getAttribute(name);
    H5::Attribute h5a = h5data.openAttribute(name);

    QTableWidgetItem* itm1 = new QTableWidgetItem(k);
    itm1->setFlags(itm1->flags() ^ Qt::ItemIsEditable);
    this->setItem(row, 0, itm1);

    HighFive::DataType dt = hfa.getDataType();
    QString t = QString::fromStdString(dt.string());

    QTableWidgetItem* itm2 = new QTableWidgetItem(t);
    itm2->setFlags(itm2->flags() ^ Qt::ItemIsEditable);
    this->setItem(row, 1, itm2);

    QTableWidgetItem* itm3 = new QTableWidgetItem();
    itm3->setFlags(itm3->flags() ^ Qt::ItemIsEditable);

    switch (dt.getClass()) {
      case HighFive::DataTypeClass::String: {
        if (dt.isVariableStr()) {
          hsize_t dim = 0;

          H5::DataSpace attDataSpace = h5a.getSpace();
          attDataSpace.getSimpleExtentDims(&dim);

          char** rdata = new char*[dim];

          H5::StrType datatype(H5::PredType::C_S1, H5T_VARIABLE);
          h5a.read(datatype, (void*)rdata);

          QStringList values;
          for (int i = 0; i < dim; ++i) {
            values << rdata[i];
            delete[] rdata[i];
          }
          delete[] rdata;

          itm3->setData(Qt::DisplayRole, values.join(", "));
        } else {
          size_t sz = h5a.getInMemDataSize();
          char* value = new char[sz + 1];

          h5a.read(h5a.getStrType(), value);
          value[sz] = '\0';

          std::string sv(value);
          delete[] value;

          itm3->setData(Qt::DisplayRole, QString::fromStdString(sv));
        }
        break;
      }
      case HighFive::DataTypeClass::Integer: {
        short iv;
        h5a.read(h5a.getIntType(), &iv);
        itm3->setData(Qt::DisplayRole, iv);
        break;
      }
      case HighFive::DataTypeClass::Float: {
        float fv;
        h5a.read(h5a.getFloatType(), &fv);
        itm3->setData(Qt::DisplayRole, fv);
        break;
      }
      case HighFive::DataTypeClass::VarLen:
      case HighFive::DataTypeClass::Compound:
      case HighFive::DataTypeClass::Array:
      case HighFive::DataTypeClass::Time:
      case HighFive::DataTypeClass::BitField:
      case HighFive::DataTypeClass::Opaque:
      case HighFive::DataTypeClass::Reference:
      case HighFive::DataTypeClass::Enum:
        // TODO: What do we do in these cases?
        itm3->setData(Qt::DisplayRole, "<HDF5 object reference>");
        break;
      case HighFive::DataTypeClass::Invalid:
        itm3->setData(Qt::DisplayRole, "<Invalid>");
        break;
    }

    this->setItem(row, 2, itm3);
  }
}
