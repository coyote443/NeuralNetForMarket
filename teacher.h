#ifndef TEACHER_H
#define TEACHER_H

#include "headermain.h"

class Teacher : public QObject
{
    Q_OBJECT    
public:
    Teacher(QObject * parent = 0);
    void teachThoseNetworksFF(QVector<LinearNetwork*> &nets, const LearnVect &sig, const QMap<QString, int> sigClasses,
                             double targetError, QProgressBar &progBarEpoch);
    void teachThoseNetworksGen(QVector<LinearNetwork *> &nets, const LearnVect &sig, const QMap<QString, int> sigClasses,
                              double targetError, QProgressBar &progBarEpoch, Topology &topol, Specification &specif);
signals:
    void nextEpoch();
    void nextOnePercentOfEpoch();
    void netTrained();

private:
    void teachOneNetworkFF(LinearNetwork &nets, int netNr, int netSize, const LearnVect &sig,
                         const QMap<QString, int> sigClasses, QProgressBar &progBar, double targetError);
    void teachOneNetworkGen(LinearNetwork &net, int netNr, int netSize, const LearnVect &sig, const QMap<QString, int> sigClasses,
                            QProgressBar &progBar, double targetError, Topology &topol, Specification &specif);
    void calculateAvarageErrorAlGen(Population &population, int netSize, const LearnVect &AllSignals, const QMap<QString,
                                    int> sigClasses, int netNr);
    double m_MutationRate = 0.01;
};

#endif // TEACHER_H
