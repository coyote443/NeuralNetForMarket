#ifndef NEURON_H
#define NEURON_H

#include "header.h"

//// Neuron ma zapisany nr neuronu z którym się łączy i przypisaną doń wagę. Określone jest to krotką Connection;
///

struct Connect{
            Connect() : m_NeuronIndex(0), m_Weight(0), m_DeltaWeight(0), m_SignalGate(false) {}
            Connect(int ConNum, double WeiNum) : m_NeuronIndex(ConNum), m_Weight(WeiNum), m_SignalGate(false), m_DeltaWeight(0){}
    int     m_NeuronIndex;
    double  m_Weight;
    double  m_DeltaWeight;
    bool    m_SignalGate;
};


typedef QVector<Connect> Connections;


class Neuron
{
public:
                    Neuron();
    virtual         ~Neuron();

    virtual void    takeThisSignal(Response fromPrevLayer) = 0;
    virtual void    createConnection(int sourceIndex, double weightVal = 0) = 0;
    virtual void    pushSignal() = 0;
    double          output(){return m_Output;}
    int             index(){return m_Index;}
    QString         toQString(QString SEP = " ");

    //// zablokuj operatory przypisania, kopiowania i przenoszenia;

protected:
    Connections*    m_Connections;
    double          m_Output    = 0;
    double          m_Gradient  = 0;
    bool            m_IsBiasAdded = false;
    int             m_Index;

private:
    static int      createdNeurons;
    enum            {BIAS_INDEX = 0, BIAS_WEIGHT = 1};
};


class LinearNeuron : public Neuron{
public:
            LinearNeuron() : Neuron() {}
            ~LinearNeuron();


//// Czy dodanie przedrostka virtual do dwóch poniższych funkcji umożliwi mi ich wywołanie u klas potomnych?
/// w zasadzie tylko inputNeurony będą inaczej agregowały sygnał

    void            createConnection(int sourceIndex, double weightVal = 0) override; // virtual
    void            takeThisSignal(Response fromPrevLayer) override; // virtual
    void            pushSignal();

protected:
    double          m_tmpAgregatedSignal = 0;
    void            agregateThisSignal(Response fromPrvLayer);
    bool            possibleToSendSignal();
    inline void     prepareNeuronForNextSignals();
    const double    tranFun(double x){return tanh(x);}
    const double    transferFunctionDerr(double x){return 1.0 - x * x;}
    inline static double generateWeightVal();

};


class LinInputNeuron : public LinearNeuron{
public:
                    LinInputNeuron() : LinearNeuron(){}
                    ~LinInputNeuron(){}
    void            createConnection(int sourceIndex);
    void            pushSignal();
};


////////////////////////////////////////////////////////




class RBFNeuron : public Neuron{

};


#endif // NEURON_H
