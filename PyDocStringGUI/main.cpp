#include "mainwindow.h"

#include <QApplication>
#include <QStringList>

#include <QDebug>
#ifndef cout
#define cout qDebug()
#endif

QStringList mainArgsToStrList(int argc,char *argv[])
{
    QStringList out;

    for(int i{0}; i < argc;i++)
    {
        out += QString{argv[i]};
    }

    return out;
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    auto strList{mainArgsToStrList(argc,argv)};

    MainWindow w{strList};
    w.show();

    return a.exec();
}
