#include "AnalysisViewWidget.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonValue>
#include <QList>
#include <QMimeData>
#include <QRegularExpression>
#include <QStyle>
#include <QTextBlock>
#include <QUrl>
#include <QVector>

AnalysisViewWidget::AnalysisViewWidget(QWidget* parent) : QTextEdit(parent) {
  setReadOnly(true);
  this->setCursorWidth(0);
  showIntro();
}

AnalysisViewWidget::~AnalysisViewWidget() {}

void AnalysisViewWidget::showIntro() {
  this->setFakeDisabled();

  QFile f(":/intro.html");
  if (f.open(QIODevice::ReadOnly)) {
    QString intro = QString::fromStdString(f.readAll().toStdString());
    setHtml(intro);
  }
}

void AnalysisViewWidget::showSearch() {
  this->setFakeDisabled();

  QFile f(":/search.html");
  if (f.open(QIODevice::ReadOnly)) {
    QString intro = QString::fromStdString(f.readAll().toStdString());
    setHtml(intro);
  }
}

void AnalysisViewWidget::setFakeDisabled() {
  // "Fake" the disabled look:
  QPalette palette;
  palette.setCurrentColorGroup(QPalette::Disabled);
  palette.setColorGroup(QPalette::Normal, palette.windowText(),
                        palette.button(), palette.light(), palette.dark(),
                        palette.mid(), palette.text(), palette.brightText(),
                        palette.base(), palette.window());
  this->setPalette(palette);
}

void AnalysisViewWidget::setFakeEnabled() {
  QPalette palette;
  palette.setCurrentColorGroup(QPalette::Normal);
  palette.setColorGroup(QPalette::Normal, palette.windowText(),
                        palette.button(), palette.light(), palette.dark(),
                        palette.mid(), palette.text(), palette.brightText(),
                        palette.base(), palette.window());
  this->setPalette(palette);
}

void AnalysisViewWidget::preview(QString doc) {
  this->setFakeEnabled();
  this->setPlainText(doc);
  this->setReadOnly(true);
}

void AnalysisViewWidget::dragEnterEvent(QDragEnterEvent* event) {
  m_wasReadOnly = isReadOnly();
  if (m_wasReadOnly) {
    setReadOnly(false);
  }

  if (event->mimeData()->hasFormat("text/uri-list")) {
    event->acceptProposedAction();
  }
}

void AnalysisViewWidget::dropEvent(QDropEvent* event) {
  event->acceptProposedAction();
  if (m_wasReadOnly) {
    setReadOnly(true);
  }

  const QMimeData* mimeData = event->mimeData();
  if (mimeData->hasUrls()) {
    QVector<QString> pathList;

    QList<QUrl> urlList = mimeData->urls();
    foreach (const QUrl& str, urlList) { pathList.append(str.toLocalFile()); }

    emit pathChanged(pathList[0]);
  }
}

void AnalysisViewWidget::dragLeaveEvent(QDragLeaveEvent*) {
  if (m_wasReadOnly) {
    setReadOnly(true);
  }
}
