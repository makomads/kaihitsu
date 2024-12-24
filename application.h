#ifndef __APPLICATION__
#define __APPLICATION__

#include <QApplication>
#include <QLocalServer>
#include <QSharedMemory>
#include "mainwindow.h"


class Application: public QApplication
{
    Q_OBJECT
protected:
    MainWindow *mainwindow;
    QLocalServer localsv;
    QSharedMemory sharedsvlist;

public:
    Application(int &argc, char *argv[]);
    ~Application();
    bool registerLocalServer();
    int exec();

public slots:
    void onLocalServerNewConnection();

};





#endif

