#ifndef HEADER_H
#define HEADER_H

#include <QVector>
#include <QTime>
#include <QDebug>
#include <QTextStream>
#include <QThread>

class Neuron;

typedef QVector<unsigned>   Topology;
typedef QVector<double>     Character;
typedef QVector<Neuron*>    Layer;
typedef QVector<Layer>      Network;
typedef QVector<double>     Signals;
typedef std::pair<int, double>  Response;
typedef QVector<Response>       Responses;

enum mainVals   {BIAS_VAL = 1, BETA, ETA, ALFA, BLUR, MIN_ERR};
enum biasGate   {OPEN = 0, CLOSED = 1};



#endif // HEADER_H
