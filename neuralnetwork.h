#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H

#include "header.h"
#include "neuron.h"


class NeuralNetwork
{
public:
                    NeuralNetwork(Topology &NetSpec);
    virtual         ~NeuralNetwork(){}
    void            feedForward(Signals inSigs);
    Responses &     takeOutput();
    void            drawMe();

protected:
    virtual void    createNeurons() = 0;
    virtual void    createConnections() = 0;
    Network         m_Net;
    Topology        m_LayersSizes;
    Responses       m_Responses;
};



class LinearNetwork : public NeuralNetwork
{
public:
    LinearNetwork(Topology & spec);
    ~LinearNetwork(){}
private:
    void createNeurons();
    void createConnections();
};



class RBFNetwork : public NeuralNetwork
{
public:
    RBFNetwork(Topology & spec) : NeuralNetwork(spec) {}
    ~RBFNetwork(){}
private:
    void createNeurons();
    void createConnections();
};





#endif // NEURALNETWORK_H
