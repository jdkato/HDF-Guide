#include "TerminalWidget.h"

#include <QSizePolicy>
#include <QTextCursor>
#include <QVBoxLayout>

TerminalWidget::TerminalWidget(QWidget *parent) : QPlainTextEdit(parent) {
  QSizePolicy spBottom(QSizePolicy::Preferred, QSizePolicy::Preferred);

  spBottom.setVerticalStretch(1);
  this->setSizePolicy(spBottom);

  QVBoxLayout *layout = new QVBoxLayout;

  layout->setContentsMargins(0, 0, 0, 0);
  layout->addStretch();

  this->setLayout(layout);

  this->setReadOnly(true);
  this->write({"HDF Guide is ready."});
}

void TerminalWidget::write(const QStringList &lines) {
  this->clear();
  foreach (const QString &line, lines) {
    this->moveCursor(QTextCursor::End);
    this->insertPlainText(">>> " + line + "\n");
    this->moveCursor(QTextCursor::End);
  }
}
