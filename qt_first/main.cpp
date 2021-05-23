#include "mainwindow.h"

#include <QApplication>
#include <QDebug>
extern "C" {
    #include <libavcodec/avcodec.h>
}

int main(int argc, char *argv[])
{
    qDebug() << "hello qdebug";
//    qDebug() << av_version_info();
    qDebug() << av_version_info();

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
