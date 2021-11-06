#ifndef DATASETPREVIEWWIDGET_H
#define DATASETPREVIEWWIDGET_H

#include <QObject>
#include <QTextEdit>
#include <highfive/H5File.hpp>

class DatasetPreviewWidget : public QTextEdit {
  Q_OBJECT
 public:
  DatasetPreviewWidget(QWidget* parent = nullptr);

  void show(QString name, HighFive::DataSet data);

 private:
  QString fromDim(std::vector<size_t> dim);
  QString fromTime(time_t t);
};

#endif  // DATASETPREVIEWWIDGET_H
