#ifndef NEURON_H
#define NEURON_H

#include "header.h"

//// Neuron ma zapisany nr neuronu z którym się łączy i przypisaną doń wagę. Określone jest to strukturą Connection;
///

struct Connection{
            Connection() : m_NeuronIndex(0), m_Weight(0), m_DeltaWeight(0), m_SignalGate(false) {}
            Connection(int ConNum, double WeiNum) : m_NeuronIndex(ConNum), m_Weight(WeiNum), m_SignalGate(false), m_DeltaWeight(0){}
    int     m_NeuronIndex;
    bool    m_SignalGate;
    double  m_Weight;
    double  m_DeltaWeight;
};


typedef QVector<Connection> Connections;

class Neuron
{
public:
    explicit        Neuron(int neuronNum);
    virtual         ~Neuron();
    virtual void    takeThisSignal(Response fromPrevLayer) = 0;
    virtual void    createConnection(int sourceIndex, double weightVal = RANDOM) = 0;
    virtual void    pushSignal() = 0;

    virtual void    calcOutputGradients(double targetVal) = 0;
    virtual void    calcHiddenGradients(Layer &nextLayer, int neuron_index) = 0;
    virtual void    updateWeights(Layer &prevLayer) = 0;

    void            swapWeight(Connection & conn, unsigned connNum);
    Connections &   getConnections(){return m_Connections;}

    void            setOutputVal(double val){m_Output = val;}
    double          getOutputVal()const {return m_Output;}
    int             getIndex()const {return m_Index;}
    QString         toQString(QString SEP = "[::]");

    static void     setETA(double newETA){ETA = newETA;}
    static void     setALFA(double newALFA){ALPHA = newALFA;}

protected:
    int             m_Index;
    static double   ETA,
                    ALPHA;
    Connections     m_Connections;
    double          m_Output        = 0;
    double          m_Gradient      = 0;
};


class LinearNeuron : public Neuron{
public:
    explicit LinearNeuron(int neuronNum) : Neuron(neuronNum) {}
            ~LinearNeuron();
    void    createConnection(int sourceIndex, double weightVal = RANDOM) override;  // virtual
    void    takeThisSignal(Response fromPrevLayer) override;                        // virtual
    void    pushSignal();
    static inline double   generateWeightVal();

protected:
    double          m_AgregatedSignal = 0;
    void            agregateThisSignal(Response fromPrvLayer);
    bool            possibleToSendSignal();
    void            prepareNeuronForNextSignals();
    inline double   tranFun(double x){return std::tanh(x);}
    inline static double    transferFunctionDerr(double x){return 1.0 - x * x;}

    void            calcOutputGradients(double targetVal);
    void            calcHiddenGradients(Layer &nextLayer, int neuron_index);
    void            updateWeights(Layer &prevLayer);

};


class LinInputNeuron : public LinearNeuron{
public:
    explicit LinInputNeuron(int neuronNum) : LinearNeuron(neuronNum){}
            ~LinInputNeuron(){ }
    void    createConnection(int sourceIndex);
    void    pushSignal();
};

class LinBiasNeuron : public LinInputNeuron{
public:
    LinBiasNeuron(int neuronNum) : LinInputNeuron(neuronNum){}
    ~LinBiasNeuron(){ }
    void takeThisSignal(Response);
};


////////////////////////////////////////////////////////




class RBFNeuron : public Neuron{

};


#endif // NEURON_H
