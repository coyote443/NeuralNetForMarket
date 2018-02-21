#ifndef TEACHER_H
#define TEACHER_H

#include "headermain.h"

typedef const QMap<QString, int>    SigClasses;
typedef QVector<LinearNetwork*>     AllNets;

/// ***************************************************************
/// Dodaj destruktor który będzie usuwał sieci wygenerowane wcześniej,
/// tzn, po odpaleniu drugi raz nauki, musi usuwać to co jest już
/// utworzone.
///
/// ***************************************************************

class Teacher : public QObject
{
    Q_OBJECT    
public:
    Teacher(QObject * parent = 0);
    void teachThoseNetworksFF(AllNets &nets, const LearnVect &sig, SigClasses sigClasses, double targetError, QProgressBar &progBarEpoch);
    void teachThoseNetworksGen(AllNets &nets, const LearnVect &sig, SigClasses sigClasses,double targetError, QProgressBar &progBarEpoch,
                               Topology &topol, Specification &specif);
signals:
    void nextEpoch();
    void nextOnePercentOfEpoch();
    void netTrained();

private:
    void teachOneNetworkFF(LinearNetwork &nets, int netNr, int netSize, const LearnVect &sig, SigClasses sigClasses, QProgressBar &progBar,
                           double targetError);
    void teachOneNetworkGen(LinearNetwork &net, int netNr, int netSize, const LearnVect &sig, SigClasses sigClasses, QProgressBar &progBar,
                           double targetError, Topology &topol, Specification &specif);

    void calculateAvarageError(Population &population, int netSize, const LearnVect &AllSignals, const QMap<QString,int> sigClasses, int netNr);
    void killGivenPercOfPopulation(double SURVIVE_RATE, Population &population);
    void makeBreedRate(Population &population);
    NetAndCharacter &findParent(Population &population);
    void makeChildren(Population &population, Population &Offspring, Topology &topol, Specification &specif);
    void makeMutation(NetAndCharacter &individual, double MUTATION_RATE);
};

#endif // TEACHER_H
