#include "teacher.h"

#include <algorithm>

Teacher::Teacher(QObject *parent): QObject(parent){
    qsrand(QTime::currentTime().msec());
}

void Teacher::teachOneNetwork(LinearNetwork &nets, int netNr, int netSize, const LearnVect &sig, const QMap<QString, int> sigClasses,
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

void Teacher::teachThoseNetworks(QVector<LinearNetwork *> &nets, const LearnVect &sig, const QMap<QString, int> sigClasses,
                                  double targetError, QProgressBar &progBarEpoch){
    if(nets.size() == 1){
        teachOneNetwork(*nets.first(), 0, 1, sig, sigClasses, progBarEpoch, targetError);
        emit netTrained();
    }
    else{
        for(int pos = 0; pos < nets.size(); pos++){
            LinearNetwork * linearNetwork = nets[pos];
            teachOneNetwork(*linearNetwork, pos, nets.size(), sig, sigClasses, progBarEpoch, targetError);
            emit netTrained();
        }
    }
}
