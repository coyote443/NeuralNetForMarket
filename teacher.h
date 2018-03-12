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
    double  MIN_ERROR       = 0.05,
            MUTATION_RATE   = 0.05,
            SURVIVE_RATE    = 0.75;
    int     POPULATION_SIZE = 2;

    QProgressBar        * m_ProgBar  = 0,
                        * m_EpochBar = 0;
    const Specification * m_Specif   = 0;
    const Topology      * m_Topol    = 0;
public:
    Teacher(QObject * parent = 0);
    void setSpecification(double err, double mut, double surv, int popSize);
    void setTopolAndGeneralSpecif(const Topology &topol, const Specification &generalSpecif);
    void linkProgBarrs(QProgressBar *progBar, QProgressBar *epochBarr);

    void teachThoseNetworksFF(AllNets &nets, const LearnVect &sig, SigClasses sigClasses);
    void teachThoseNetworksGen(AllNets &nets, const LearnVect &sig, SigClasses sigClasses);

signals:
    void nextEpoch();
    void netTrained();

private:
    void teachOneNetworkFF(LinearNetwork &nets, int netNr, int netSize, const LearnVect &sig, SigClasses sigClasses);
    void teachOneNetworkGen(LinearNetwork &net, int netNr, int netSize, const LearnVect &sig, SigClasses sigClasses);

    void calculateAvarageError(Population &population, int netSize, const LearnVect &AllSignals, SigClasses sigClasses, int netNr);
    void killGivenPercOfPopulation(double SURVIVE_RATE, Population &population);
    void makeBreedRate(Population &population);
    NetAndCharacter &findParent(Population &population);
    NetAndCharacter &makeChildren(Population &population);
    void makeMutation(NetAndCharacter &individual, double MUTATION_RATE);
};

#endif // TEACHER_H
