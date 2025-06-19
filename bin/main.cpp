#include "bin/MainWindow.h"
#include <QApplication>

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon("res/IS.jpg"));
    MainWindow w;
    w.show();
    return app.exec();
}