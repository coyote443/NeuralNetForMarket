#ifndef HEADERMAIN_H
#define HEADERMAIN_H

#include "header.h"
#include <QPair>
#include <QFileDialog>
#include <QFile>
#include <QInputDialog>
#include <QMessageBox>
#include <QProgressBar>
#include "neuralnetwork.h"

enum nType      {SIGMOID = 0, RBF = 1};
enum lern       {ONE_NET = 0, NET_PER_SIG = 1};
enum splitType  {OneNetwork = 0, ManyNetworks = 1};

typedef QVector<double>     ErrorLine;
typedef QVector<ErrorLine>  ErrorMatrix;

typedef QPair<QString, Signals> LearnSig;
typedef QVector<LearnSig> LearnVect;

struct NetAndCharacter{
    double errorRate;
    double breeadingRate;
    LinearNetwork * network;
};

typedef QVector<NetAndCharacter> Population;
typedef QVector<double>          OneClassRespos;
typedef QVector<OneClassRespos>  OneNetworkRespos;
typedef QVector<OneNetworkRespos>  AllNetworksRespos;

#endif // HEADERMAIN_H
