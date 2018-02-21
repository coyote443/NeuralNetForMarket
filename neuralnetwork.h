#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H

#include "header.h"
#include "neuron.h"

class Teacher;

class NeuralNetwork
{
    friend class Teacher;
public:
    NeuralNetwork & operator=(const NeuralNetwork & neuralNet);
                    NeuralNetwork(const Topology &topology, const Specification &specify);
    virtual         ~NeuralNetwork();
    virtual void    feedForward(const Signals &inSigs) = 0;
    virtual double  backPropagation(const Signals &learnVect) = 0;
    virtual QString toQString(QString SEP = "[::]") = 0;
    virtual void    changeNetSpecification(const Specification &specify) = 0;
    int             size(){return m_Net.size();}
    Signals         getResults() const;
    void            drawMe() const;

protected:
    Responses       takeOutput(const Layer &layer) const;
    virtual void    createLayers() = 0;
    virtual void    createNewConnections() = 0;
    Network         m_Net;
    Topology        m_Topology;
    Specification   m_Specifi;
    double          m_Error             = 0;    //  Błąd dla wybranej probki
    double          m_RecentAvarageErr  = 0;    //  Uśrednienie błędu dla liczby próbek BLUR_FACT
    double          m_Progress          = 0;    //  Składowa dla funkcji
    double          m_Corectness        = 1;    //  Zaczynamy z błędem równym 100%
    double          BETA,           //      Wsp. krzywej aktywacji || brak
                    ETA,            //      Wsp. uczenia    (0.0 - Powolne, 0.2 Średnie, 1.0 B. duże)
                    ALPHA,          //      Wsp. momentum   (0.0 - Brak, 0.5 Średnie)
                    BLUR_FACT;      //      Wsp. Określający w jakim zakresie uśredniać sqErr
    int             BIAS_VAL;
};



class LinearNetwork : public NeuralNetwork
{
public:
            LinearNetwork(Topology &topol, Specification &specif);
            LinearNetwork(Topology &topol, Specification &specif, AllNetConn &netCon);
            ~LinearNetwork();
            LinearNetwork & operator=(const LinearNetwork & neuralNet);

    void    feedForward(const Signals &inSigs);
    double  backPropagation(const Signals &targetVals);
    void    changeNetSpecification(const Specification &specify);
    QString toQString(QString SEP = "[::]");
    double  backPropagationOnlyError(const Signals &targetVals);
    void    swapLayer(Layer &layer, unsigned layerNum);
    void    swapNeuron(Neuron &neuron, unsigned layerNum, unsigned neuronNum);
    void    swapConn(Connection &conn, unsigned layerNum, unsigned neuronNum, unsigned connNum);

private:
    void    createLayers();
    void    createNewConnections();
    void    createGivenConnections(AllNetConn &netCon);
    void    calcOutputLayGradients(const Signals &targetVals);
    void    calcHiddLayGradients();
    void    updateWeights();
    void    makeAvarageError();
    double  calcAvarageError(const Signals &targetVals);
};



class RBFNetwork : public NeuralNetwork
{
public:
    RBFNetwork(Topology &topol, Specification & specif) : NeuralNetwork(topol, specif) {}
    ~RBFNetwork(){}
private:
    void createLayers();
    void createNewConnections();
};





#endif // NEURALNETWORK_H
