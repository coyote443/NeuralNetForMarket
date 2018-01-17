#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H

#include "header.h"
#include "neuron.h"


class NeuralNetwork
{
public:
                    NeuralNetwork(const Topology &topology, const Specification &specify);
    virtual         ~NeuralNetwork(){}
    virtual void    feedForward(const Signals &inSigs)        = 0;
    virtual void    backPropagation(const Signals &learnVect) = 0;
    Signals         getResults() const;
    void            drawMe() const;

protected:
    Responses       takeOutput(const Layer &layer) const;
    virtual void    createLayers() = 0;
    virtual void    createConnections() = 0;
    Network         m_Net;
    Topology        m_LayersSizes;
    double          m_Error             = 0;    //  Błąd dla wybranej probki
    double          m_RecentAvarageErr  = 0;    //  Uśrednienie błędu dla liczby próbek BLUR_FACT

    double  BETA,           //      Wsp. krzywej aktywacji || brak
            ETA,            //      Wsp. uczenia    (0.0 - Powolne, 0.2 Średnie, 1.0 B. duże)
            ALPHA,          //      Wsp. momentum   (0.0 - Brak, 0.5 Średnie)
            BLUR_FACT,      //      Wsp. Określający w jakim zakresie uśredniać sqErr
            MIN_ERR;        //      Błąd poniżej którego nauka jest przerwana
    int     BIAS_VAL;
};



class LinearNetwork : public NeuralNetwork
{
public:
    LinearNetwork(Topology &topol, Specification & specif);
    ~LinearNetwork(){}

    void feedForward(const Signals &inSigs);
    void backPropagation(const Signals &targetVals);

private:
    void createLayers();
    void createConnections();
    void calcAvarageError(const Signals &targetVals, Layer &outputLayer);
    void calcOutputLayGradients(const Signals &targetVals, Layer &outputLayer);
    void calcHiddLayGradients();
    void updateWeights();
};



class RBFNetwork : public NeuralNetwork
{
public:
    RBFNetwork(Topology &topol, Specification & specif) : NeuralNetwork(topol, specif) {}
    ~RBFNetwork(){}
private:
    void createLayers();
    void createConnections();
};





#endif // NEURALNETWORK_H
