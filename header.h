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

typedef QPair<QString, QVector<double>> LearnSig;
typedef QVector<LearnSig> LearnVect;




#endif // HEADER_H
