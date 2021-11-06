#ifndef TERMINALWIDGET_H
#define TERMINALWIDGET_H

#include <QObject>
#include <QPlainTextEdit>

class TerminalWidget : public QPlainTextEdit {
  Q_OBJECT
 public:
  explicit TerminalWidget(QWidget *parent = nullptr);

  void write(const QStringList &lines);
};

#endif
