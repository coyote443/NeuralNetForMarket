#ifndef HEADER_H
#define HEADER_H

#include <QVector>
#include <QTime>
#include <QDebug>
#include <QTextStream>
#include <QStringList>
#include <QThread>
#include <cmath>
#include <QFile>

class Neuron;

typedef QVector<unsigned>   Topology;
typedef QVector<double>     Specification;
typedef QVector<Neuron*>    Layer;
typedef QVector<Layer>      Network;
typedef QVector<double>     Signals;
typedef std::pair<int, double>  Response;
typedef QVector<Response>       Responses;

enum Weight{RANDOM = 666};
enum Gate {OPEN = 0, CLOSED = 1};

struct LearnSig{
    QString Class;
    Signals Signal;
    QString Dir;
};

typedef QVector<LearnSig>               LearnVect;
typedef QPair<QVector<int>, QVector<double>> NeuronConn;
typedef QVector<NeuronConn> AllNetConn;




#endif // HEADER_H
