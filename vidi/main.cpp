#include <stdio.h>

#include <QApplication>
#include <QtCore>
#include <QtGui>

#include "mainwindow.h"

using namespace minidis;

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(application);
    QApplication app(argc, argv);
    ExeFactory::init();

    MainWindow w;
    w.showMaximized();
    return app.exec();
}
