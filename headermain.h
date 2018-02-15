#ifndef HEADERMAIN_H
#define HEADERMAIN_H

#include "header.h"
#include <QPair>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QProgressBar>
#include "neuralnetwork.h"

enum nType      {SIGMOID = 0, RBF = 1};
enum lern       {ONE_NET = 0, NET_PER_SIG = 1};
enum splitType  {OneNetwork = 0, ManyNetworks = 1};

typedef QPair<QString, Signals> LearnSig;
typedef QVector<LearnSig> LearnVect;

typedef QPair<double, LinearNetwork *> NetAndFit;
typedef QVector<NetAndFit> Population;

#endif // HEADERMAIN_H
