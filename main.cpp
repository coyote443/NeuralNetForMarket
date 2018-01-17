#include "header.h"

#include "mainwindow.h"
#include <QApplication>

#include "neuralnetwork.h"

void teacher(LinearNetwork &toTeach){
    Signals S2 = {1.0, 0.0};
    Signals T2 = {1.0};

    for(int x = 0; x < 100; x ++){
        toTeach.feedForward(S2);
        toTeach.backPropagation(T2);
        toTeach.drawMe();
    }
}

class wololo
{
public:
    wololo() {}
    static double ddd;
    static void set(double val){ddd = val;}
};

double wololo::ddd = 0.1;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    MainWindow w;
//    w.show();

    double  beta     =  1.100,      //      Wsp. krzywej aktywacji || brak
            eta      =  0.200,      //      Wsp. uczenia
            alfa     =  0.600,      //      Wsp. momentum
            blur     =  20,         //      Wsp. Określający w jakim zakresie uśredniać sqErr
            minErr   =  0.005,      //      Błąd poniżej którego nauka jest przerwana
            bias     =  1;

    Topology        topol = {2, 4, 1};
    Specification   spec = {beta, eta, alfa, blur, minErr, bias};
    LinearNetwork   nowaSiec(topol, spec);
    qDebug() << "Przed "<< endl;
    teacher(nowaSiec);
    qDebug() << "Po "<< endl;


    return a.exec();
}



//Signals         sygnaly     = {0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,};
//Signals         targetVals  = {2, 2};

//nowaSiec.feedForward(sygnaly);
//nowaSiec.backPropagation(targetVals);
