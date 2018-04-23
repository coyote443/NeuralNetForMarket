#ifndef TEACHER_H
#define TEACHER_H

#include "headermain.h"

typedef const QMap<QString, int>    SigClasses;
typedef QVector<LinearNetwork*>     AllNets;

class ExtendedTestRes{
public:
    double P  = 0, N  = 0, TN = 0, TP = 0,                         /// Bazowe które trzeba obliczyć najpierw
        FP = 0, FN = 0;                                         /// Miary oparte na bazowych
    double  TPR = 0.0, TNR = 0.0, FPR = 0.0, FDR = 0.0,
            PPV = 0.0, NPV = 0.0, F1  = 0.0, MCC = 0.0;
    void makeTests(){
        TPR = TP / P;
        FN  = (TP - (TPR * TP));
        TPR != 0 ? FN /= TPR : FN /= 1;

        TNR = TN / N;
        FP  = (TN - (TNR * TN));
        TNR != 0 ? FP /= TNR : FP /= 1;

        FPR = FP / N;
        FDR = FP / (FP + TP);
        PPV = TP / (TP + FP);
        NPV = TN / (TN + FN);
        F1  = (2 * TP) / (2 * TP + FP + FN);

        qDebug() << "SQRT " << sqrt( (TP + FP) * (TP + FN) * (TN + FP) * (TN + FN) ) << endl;
        MCC = (TP * TN - FP * FN) / sqrt( (TP + FP) * (TP + FN) * (TN + FP) * (TN + FN) );
    }

    void print(){
        qDebug()    << endl << "P   = " <<  P   << endl << "N   = " <<  N   << endl << "TN  = " <<  TN
                    << endl << "TP  = " <<  TP  << endl << "FP  = " <<  FP  << endl << "FN  = " <<  FN
                    << endl << "TPR = " <<  TPR << endl << "TNR = " <<  TNR << endl << "FPR = " <<  FPR
                    << endl << "FDR = " <<  FDR << endl << "PPV = " <<  PPV << endl << "NPV = " <<  NPV
                    << endl << "F1  = " <<  F1  << endl << "MCC = " <<  MCC << endl;
    }
};

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
    void netProcessed();

private:
    void teachOneNetworkFF(LinearNetwork &nets, int netNr, int netSize, const LearnVect &sig, SigClasses sigClasses);
    void teachOneNetworkGen(LinearNetwork &net, int netNr, int netSize, const LearnVect &sig, SigClasses sigClasses);
    void testOneNetwork(LinearNetwork &nets, int netNr, int netSize, const LearnVect &sig,
                        SigClasses sigClasses, QTextStream &dirStream, AllNets & all);

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
    QVector<ExtendedTestRes> m_ExTestRes;
};

#endif // TEACHER_H
