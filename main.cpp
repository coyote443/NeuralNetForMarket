#include "header.h"

#include "mainwindow.h"
#include <QApplication>

#include "neuralnetwork.h"

void Teacher(LinearNetwork &toTeach){
    QVector<Signals> sig = {{0.0, 0.0},{1.0, 0.0},{0.0, 1.0},{1.0, 1.0}};
    QVector<Signals> res = {{0.0},{1.0},{1.0}, {0.0}};

    qsrand(QTime::currentTime().msec());

    int val;
    for(int x = 0; x < 10000; x++){
        val = rand()%4;
        toTeach.feedForward(sig[val]);
        qDebug() << toTeach.backPropagation(res[val]);
    }
    toTeach.drawMe();
}




int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();


    double  beta     =  1.100,      //      Wsp. krzywej aktywacji || brak
            eta      =  0.200,      //      Wsp. uczenia
            alfa     =  0.600,      //      Wsp. momentum
            blur     =  20,         //      Wsp. Określający w jakim zakresie uśredniać sqErr
            bias     =  1;

    Topology        topol = {2, 4, 1};
    Specification   spec = {beta, eta, alfa, blur, bias};

    //LinearNetwork   nowaSiec(topol, spec);

    //Teacher(nowaSiec);


    return a.exec();
}
