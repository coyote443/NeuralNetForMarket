#include "header.h"

#include "mainwindow.h"
#include <QApplication>

#include "neuralnetwork.h"

void teacher(LinearNetwork &toTeach){
    Signals S2 = {1.0, 0.0};       // na końcu każdego sygnału wartość BIAS
    Signals T2 = {1.0};

    do{
        toTeach.feedForward(S2);
    }while(toTeach.backPropagation(T2, 0.1));
    toTeach.drawMe();
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    MainWindow w;
//    w.show();

    double  beta     =  1.100,      //      Wsp. krzywej aktywacji || brak
            eta      =  0.200,      //      Wsp. uczenia
            alfa     =  0.600,      //      Wsp. momentum
            blur     =  20,         //      Wsp. Określający w jakim zakresie uśredniać sqErr
            bias     =  1;

    Topology        topol = {2, 4, 1};
    Specification   spec = {beta, eta, alfa, blur, bias};
    {
    LinearNetwork   nowaSiec(topol, spec);
    }
   // teacher(nowaSiec);


    return a.exec();
}



//Signals         sygnaly     = {0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,};
//Signals         targetVals  = {2, 2};

//nowaSiec.feedForward(sygnaly);
//nowaSiec.backPropagation(targetVals);
