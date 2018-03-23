#ifndef TEACHER_H
#define TEACHER_H

#include "headerMain.h"

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
    void stopStartMe(){m_Stop = !m_Stop;}

    void setSpecification(double err, double mut, double surv, int popSize);
    void setThresholds(double relative);
    void setTopolAndGeneralSpecif(const Topology &topol, const Specification &generalSpecif);
    void linkProgBarrs(QProgressBar *progBar, QProgressBar *epochBarr);
    const AllNetworksRespos &getRespos(){return m_AllResposAllNets;}
    void resetRespos(){m_AllResposAllNets.clear(), m_AllResposOneNet.clear();}

    void teachThoseNetworksFF(AllNets &nets, const LearnVect &sig, SigClasses sigClasses);
    void teachThoseNetworksGen(AllNets &nets, const LearnVect &sig, SigClasses sigClasses);
    void testThoseNetworks(AllNets &nets, const LearnVect &sig, SigClasses sigClasses, QTextStream &dirStream);

signals:
    void nextEpoch();
    void netTrained();

private:
    void teachOneNetworkFF(LinearNetwork &nets, int netNr, int netSize, const LearnVect &sig, SigClasses sigClasses);
    void teachOneNetworkGen(LinearNetwork &net, int netNr, int netSize, const LearnVect &sig, SigClasses sigClasses);
    void testOneNetwork(LinearNetwork &nets, int netNr, int netSize, const LearnVect &sig, SigClasses sigClasses, QTextStream &dirStream);

    void calculateAvarageError(Population &population, int netSize, const LearnVect &AllSignals, SigClasses sigClasses, int netNr);
    void killGivenPercOfPopulation(Population &population);
    void makeBreedRate(Population &population);
    NetAndCharacter &findParent(Population &population);
    NetAndCharacter &makeChildren(Population &population);
    void makeMutation(NetAndCharacter &individual);

    double  MIN_ERROR       = 0.05,
            MUTATION_RATE   = 0.05,
            SURVIVE_RATE    = 0.75;
    int     POPULATION_SIZE = 2;

    double  THRESHOLD_RELATIVE = 0.50;

    QProgressBar        * m_ProgBar  = 0,
                        * m_EpochBar = 0;
    const Specification * m_Specif   = 0;
    const Topology      * m_Topol    = 0;
    bool                  m_Stop     = false;
    OneNetworkRespos      m_AllResposOneNet;
    AllNetworksRespos     m_AllResposAllNets;
};

#endif // TEACHER_H
