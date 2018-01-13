#include "header.h"

#include "mainwindow.h"
#include <QApplication>

#include "neuralnetwork.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    MainWindow w;
//    w.show();

    Topology        topol = {10, 10, 4, 2};
    LinearNetwork   nowaSiec(topol);
    Signals         sygnaly = {0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,};

    nowaSiec.feedForward(sygnaly);
    nowaSiec.drawMe();
    Responses newRespo = nowaSiec.takeOutput();

//    for(double val : newRespo)
//        qDebug() << val << endl;


//    qDebug() << "Wszystko OK" << endl;


    return a.exec();
}
