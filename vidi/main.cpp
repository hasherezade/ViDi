#include <stdio.h>

#include <QApplication>
#include <QtCore>
#include <QtWidgets>

#include "mainwindow.h"

using namespace minidis;

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(application);
    QApplication app(argc, argv);
    ExeFactory::init();

    MainWindow w;
    w.showMaximized();
    if (argc >= 2) {
        w.loadFile(QString(argv[1]));
    }
    return app.exec();
}
