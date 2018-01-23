#include "teacher.h"

#include <algorithm>

Teacher::Teacher(QObject *parent): QObject(parent){
    qsrand(QTime::currentTime().msec());
}

void Teacher::teachThoseNetworks(QVector<LinearNetwork *> &nets, const LearnVect &sig,
                                 const QMap<QString, int> sigClasses, double targetError, QProgressBar &progBar){
    LearnVect learnVect = sig;

    LinearNetwork * net = nets.back();
    Signals targetVals(sig.size());
           std::random_shuffle(learnVect.begin(), learnVect.end());

    double currentError = 1;

    do{
        std::random_shuffle(learnVect.begin(), learnVect.end());

        QVector<int> progresPoints;
        for(int point = 100; point >= 0; point--)
            progresPoints.push_back(point);

        int progressCounter = 0,
            sigSize         = learnVect.size();

        for(LearnSig lernSig : learnVect){
            for(double &val : targetVals) val = 0;

            // Wanted responses vector are full with zeros. We fill pos related to de class by val 1
            QString sigClass = lernSig.first;
            int posToAddPosVal = sigClasses[sigClass];
            targetVals[posToAddPosVal] = 1;

//            qDebug() << sigClass;
//            for(double &val : targetVals) qDebug() << val;


            // FeedForward
            const Signals &givenSignal = lernSig.second;
            net->feedForward(givenSignal);
            currentError = net->backPropagation(targetVals);
            qDebug() << currentError;

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
