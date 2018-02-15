#include "teacher.h"

#include <algorithm>

Teacher::Teacher(QObject *parent): QObject(parent){
    qsrand(QTime::currentTime().msec());
}

void Teacher::teachOneNetworkFF(LinearNetwork &nets, int netNr, int netSize, const LearnVect &sig, const QMap<QString, int> sigClasses,
                              QProgressBar &progBar, double targetError){
    int howMany = (netSize > 1 ? 1 : sigClasses.size());
    Signals targetVals(howMany);

    LearnVect learnVect = sig;
    double currentError = 1;
    std::random_shuffle(learnVect.begin(), learnVect.end());

    do{ /// Teach those network until it reaches destinated error rate
        std::random_shuffle(learnVect.begin(), learnVect.end());

        QVector<int> progresPoints;
        for(int point = 100; point >= 0; point--)
            progresPoints.push_back(point);

        int progressCounter = 0,
            sigSize         = learnVect.size();

        for(LearnSig lernSig : learnVect){
            /// Make target vals Vector
            QString takenClass = lernSig.first;                       /// Take signal class

            for(double &val : targetVals)
                val = 0;

            switch (netSize) {
            case 1:
                targetVals[ sigClasses[takenClass] ]  = 1;            /// Only output related with takenClass have value '1'
                break;
            default:
                if(netNr == sigClasses[takenClass])                   /// Only network related with takenClass
                    targetVals[0] = 1;                                /// have one output with '1' value
                break;
            }

            /// FeedForward
            const Signals &givenSignal = lernSig.second;
            nets.feedForward(givenSignal);

            /// BackPropagation
            currentError = nets.backPropagation(targetVals);
           // qDebug() << currentError;

            /// Set Status Bar
            progressCounter++;
            double progtmp = (double)progressCounter / (double)sigSize * 100;
            if((int)progtmp > progresPoints.back()){
                progresPoints.pop_back();
                emit nextOnePercentOfEpoch();
            }
            progBar.setValue(100 - currentError * 100);
        }
        emit nextEpoch();
    }while(currentError > targetError);
}

void Teacher::teachThoseNetworksFF(QVector<LinearNetwork *> &nets, const LearnVect &sig, const QMap<QString, int> sigClasses,
                                  double targetError, QProgressBar &progBarEpoch){
    if(nets.size() == 1){
        teachOneNetworkFF(*nets.first(), 0, 1, sig, sigClasses, progBarEpoch, targetError);
        emit netTrained();
    }
    else{
        for(int pos = 0; pos < nets.size(); pos++){
            LinearNetwork * linearNetwork = nets[pos];
            teachOneNetworkFF(*linearNetwork, pos, nets.size(), sig, sigClasses, progBarEpoch, targetError);
            emit netTrained();
        }
    }
}


void Teacher::calculateAvarageErrorAlGen(Population &population, int netSize, const LearnVect &AllSignals, const QMap<QString,
                                         int> sigClasses, int netNr){
    for(NetAndFit &netandfit : population){
        int     divider     = AllSignals.size();
        double  errorRate   = 0;
        qDebug() << "simple;";

        int howMany = (netSize > 1 ? 1 : sigClasses.size());
        Signals targetVals(howMany);

        LinearNetwork * takenNetwork = netandfit.second;
        double & networkAvaErr       = netandfit.first;

        for(LearnSig learnSig : AllSignals){
            /// Make target vals Vector
            QString takenClass = learnSig.first;                       /// Take signal class

            for(double &val : targetVals)
                val = 0;

            switch (netSize) {
            case 1:
                targetVals[ sigClasses[takenClass] ]  = 1;            /// Only output related with takenClass have value '1'
                break;
            default:
                if(netNr == sigClasses[takenClass])                   /// Only network related with takenClass
                    targetVals[0] = 1;                                /// have one output with '1' value
                break;
            }

            /// FeedForward
            const Signals &givenSignal = learnSig.second;
            takenNetwork->feedForward(givenSignal);

            /// Error sum, and divider optimization
            int error = takenNetwork->calcAvarageError(targetVals);
            error != 0 ? errorRate += error : divider -= 1;
        }
        networkAvaErr = errorRate / divider;
    }
}

void Teacher::teachOneNetworkGen(LinearNetwork &net, int netNr, int netSize, const LearnVect &AllSignals, const QMap<QString,
                                 int> sigClasses, QProgressBar &progBar, double targetError, Topology &topol, Specification &specif){
    // Net nr. - dla której z kolei sieci przeprowadzamy naukę. netSize - Ile łacznie sieci mamy do nauki

    int POPULATION_SIZE = 24;

    // Wygeneruj losową populację sieci dla zadanego problemu.
    Population population;
    for(int indivd = 0; indivd < POPULATION_SIZE; indivd++){
        LinearNetwork * tmp = new LinearNetwork(topol, specif);
        population.push_back({0, tmp});
    }

    // Włączamy otrzymaną sieć do populacji
    population.push_back({0, &net});

    // Skonstruuj wektor prawidłowych odpowiedzi sieci. Licz usredniony Error dla każdej sieci.
    calculateAvarageErrorAlGen(population, netSize, AllSignals, sigClasses, netNr);
    qDebug() << "simple.first;";

    qSort(population.begin(), population.end());
    qDebug() << "simple;";
    for(NetAndFit simple : population)
        qDebug() << simple.first;



//    do{






//        currentAvError = errorRate / divider;
//    }while(currentAvError > targetError);




    //  Pamiętaj o różnym sposobie konstrukcji sygnału.

    // Wartość fitness będzie wyrażana poprzez uśredniony błąd



    ////    while(fitness najlepszego >= wartość porządana - wartość błędu)
    // Dla każdej sieci zrób ewaluację -  ustal fitness (określony przez wartość błędu dla materiału z jednej epoki [może 25% epoki?])

    // Posortuj ich listę

    // Uśmierć 25% najgorzej przystosowanych

    // Rozmnażanie - Operatory krzyżowania
        //      Zamień pojedynczą wagę między neuronem nr x obu rodziców	- 89% szans
        //      Wszystkie wagi neuronu x obu rodziców                       - 10% szans
        //      Wszystkie wagi z danej warstwu L obu rodziców               - 1 % szans


    // Mutacje     - Operatory mutacji [Ustalamy współczynnik mutacji]
        //      zastąp zupełnie nową wartością losową                       - 7%
        //      pomnóż wagę przez liczbę losową z przedziału od 0.1 do 2.0	- 39.5%
        //      dodaj lub odejmij liczbę losową z zakresu 0 - 1 od wagi		- 39.5%
        //      zmień znak wagi                                             - 7%
        //      zamień ze sobą wartości dwóch wag w neuronie                - 7%



    ////

}

void Teacher::teachThoseNetworksGen(QVector<LinearNetwork *> &nets, const LearnVect &sig, const QMap<QString, int> sigClasses,
                                    double targetError, QProgressBar &progBarEpoch, Topology &topol, Specification &specif){
    if(nets.size() == 1){
        teachOneNetworkGen(*nets.first(), 0, 1, sig, sigClasses, progBarEpoch, targetError, topol, specif);
        emit netTrained();
    }
    else{
        for(int pos = 0; pos < nets.size(); pos++){
            LinearNetwork * linearNetwork = nets[pos];
            teachOneNetworkGen(*linearNetwork, pos, nets.size(), sig, sigClasses, progBarEpoch, targetError, topol, specif);
            emit netTrained();
        }
    }
}
