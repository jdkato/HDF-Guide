#include <QApplication>

#include "MainWindow.h"

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  app.setApplicationName("HDF Guide");
  app.setApplicationVersion("1.0.0");

  MainWindow window;
  window.show();

  return app.exec();
}
