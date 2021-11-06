#include "DatasetPreviewWidget.h"

#include <QDateTime>
#include <QFile>

DatasetPreviewWidget::DatasetPreviewWidget(QWidget* parent)
    : QTextEdit(parent) {
  this->setReadOnly(true);
}

void DatasetPreviewWidget::show(QString name, HighFive::DataSet data) {
  QFile f(":/dataset.html");
  if (f.open(QIODevice::ReadOnly)) {
    QString shape = this->fromDim(data.getDimensions());
    QString dtype = QString::fromStdString(data.getDataType().string());
    QString moddt = this->fromTime(data.getInfo().getModificationTime());
    QString elec = QString::number(data.getElementCount());

    QString intro = QString::fromStdString(f.readAll().toStdString())
                        .arg(name, moddt, shape, dtype, elec);

    this->setHtml(intro);
  }
}

QString DatasetPreviewWidget::fromDim(std::vector<size_t> dim) {
  std::ostringstream oss;
  std::copy(std::begin(dim), std::end(dim),
            std::ostream_iterator<int>(oss, ";"));

  std::string result(oss.str());
  QString s = QString::fromStdString(result);

  s.chop(1);
  return s.replace(";", ", ");
}

QString DatasetPreviewWidget::fromTime(time_t t) {
  // TODO: What do we do in cases that show 1969?
  QDateTime dt(QDateTime::QDateTime::fromSecsSinceEpoch(t));
  return dt.toString();
}
