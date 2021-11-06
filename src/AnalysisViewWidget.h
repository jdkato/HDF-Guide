#ifndef ANALYSISVIEWWIDGET_H
#define ANALYSISVIEWWIDGET_H

#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QJsonObject>
#include <QList>
#include <QMap>
#include <QObject>
#include <QPalette>
#include <QTextBrowser>
#include <QTextEdit>

class AnalysisViewWidget : public QTextEdit {
  Q_OBJECT

 public:
  explicit AnalysisViewWidget(QWidget* parent = nullptr);
  ~AnalysisViewWidget();

  void showIntro();
  void showSearch();
  void preview(QString doc);

  void setFakeDisabled();
  void setFakeEnabled();

 protected:
  virtual void dragEnterEvent(QDragEnterEvent* event) override;
  virtual void dropEvent(QDropEvent* event) override;
  virtual void dragLeaveEvent(QDragLeaveEvent* event) override;

 signals:
  void pathChanged(const QString& path);

 private:
  bool m_wasReadOnly;
  QMap<QString, int> m_lines;
};

#endif
