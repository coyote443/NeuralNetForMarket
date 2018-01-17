#include "neuralnetwork.h"

NeuralNetwork::NeuralNetwork(const Topology & topology, const Specification & specify) : m_LayersSizes(topology){
        BETA     =  specify[0],      //      Wsp. krzywej aktywacji || brak
        ETA      =  specify[1],      //      Wsp. uczenia
        ALPHA    =  specify[2],      //      Wsp. momentum
        BLUR_FACT=  specify[3],      //      Wsp. Określający w jakim zakresie uśredniać sqErr
        MIN_ERR  =  specify[4],      //      Błąd poniżej którego nauka jest przerwana
        BIAS_VAL =  specify[5];

        Neuron::setETA(ETA);
        Neuron::setALFA(ALPHA);
}

Responses NeuralNetwork::takeOutput(const Layer &layer) const{
    Responses m_Responses;
    for(Neuron * neuron : layer)
        m_Responses.push_back({ neuron->getIndex(), neuron->getOutputVal() });
    return m_Responses;
}

void NeuralNetwork::drawMe() const{
    for(Layer lay : m_Net){
        for(Neuron * neu : lay){
            qDebug() << neu->toQString();
        }
        qDebug() << endl;
    }
}

Signals NeuralNetwork::getResults() const{
       Signals Out;
       for(Neuron *neuron : m_Net.back())
           Out.push_back(neuron->getOutputVal());
       return Out;
}

void LinearNetwork::createLayers(){
    if(BIAS_VAL == 1)
        for(int x = 0; x < m_LayersSizes.size() - 1; x++)      // w ostatniej warstwie nie trzeba neuronu od biasu
            m_LayersSizes[x]++;

    for(int layerIndex = 0; layerIndex < m_LayersSizes.size(); layerIndex++){
        Layer tmpLayer;
        int givenLaySize  = m_LayersSizes[layerIndex];
        int lastLayIndx  = m_LayersSizes.size() - 1,
            lastLayN = givenLaySize - 1;

        for(int takenNeuron = 0; takenNeuron < givenLaySize; takenNeuron++){
            if(layerIndex == 0 && takenNeuron != lastLayN){
                LinInputNeuron * LinNeu = new LinInputNeuron;
                tmpLayer.push_back(LinNeu);
            }
            else if(takenNeuron == lastLayN && BIAS_VAL == 1 && layerIndex != lastLayIndx){
                LinBiasNeuron * LinBias = new LinBiasNeuron;
                tmpLayer.push_front(LinBias);
            }
            else{
                LinearNeuron * LinNeu = new LinearNeuron;
                tmpLayer.push_back(LinNeu);
            }
        }
        m_Net.push_back(tmpLayer);
    }
}

void LinearNetwork::createConnections(){
    Layer *prevLayer;
    int sourceIndex    = 0;
    for(int layerIndex = 0; layerIndex < m_Net.size(); layerIndex++){
        Layer &layer = m_Net[layerIndex];
        int lastLayerIndex  = m_Net.size() - 1;

        for(int takenNeuron = 0; takenNeuron < layer.size(); takenNeuron++){
            Neuron *neuron = layer[takenNeuron];

            if(takenNeuron == 0 && BIAS_VAL == 1 && layerIndex == 0)    // INPUT BIAS NEURON
                neuron->createConnection(-1, 0);

            else if(layerIndex == 0){                                   // INPUT NEURON
                sourceIndex--;
                neuron->createConnection(sourceIndex, 1);
            }
            else if(takenNeuron == 0 && BIAS_VAL == 1 && layerIndex != lastLayerIndex)  // HIDD BIAS NEURON
                for(Neuron *prevNeu : *prevLayer)
                    neuron->createConnection(prevNeu->getIndex(), 0);
            else                                                                        // LINEAR NEURON
                for(Neuron *prevNeu : *prevLayer)
                    neuron->createConnection(prevNeu->getIndex());
        }
        prevLayer = &layer;
    }
}

LinearNetwork::LinearNetwork(Topology &topol, Specification & specif): NeuralNetwork(topol, specif){
    createLayers();
    createConnections();
}

void LinearNetwork::feedForward(const Signals &inSigs){
    /// EACH NEURON INDEX   >= 1
    /// BIAS INDEX          == 0
    /// EACH SIGNAL INDEX   <= -1

    /*  Sygnał do neuronów inputu musi mieć spis elementów, które są źródłem tego sygnału (np neurony innej sieci,
        rekurencyjne połączenia z wyjścia, bądź po prostu jakis inny sygnał. Bazowo, Sygnał jest pozbawiony indexów,
        więc się mu je sztucznie dodaje. */

    Responses prevLayerRespo;
    int sigIndex = 0;
    for(double sig : inSigs){
        --sigIndex;
        prevLayerRespo.push_back({sigIndex, sig});
    }

    for(Layer &takenLayer : m_Net){
        for(Neuron *takenNeuron : takenLayer){
            for(Response prevNeuronOut : prevLayerRespo){
                takenNeuron->takeThisSignal(prevNeuronOut);
            }
        }
        prevLayerRespo = takeOutput(takenLayer);
    }
}

void LinearNetwork::updateWeights(){
    for (int layerIndex = m_Net.size() - 1; layerIndex > 0; layerIndex--){
        Layer &acctLayer    = m_Net[layerIndex];
        Layer &prevLayer    = m_Net[layerIndex - 1];

        for(int N = 0; N < m_Net[layerIndex].size(); N++){

            acctLayer[N]->updateWeights(prevLayer);

        }
    }
}

void LinearNetwork::calcHiddLayGradients(){
    for(int layerIndex = m_Net.size() - 2 ; layerIndex > 0 ; layerIndex--){
        Layer &nextLay = m_Net[layerIndex + 1];
        Layer &acctLay = m_Net[layerIndex];

        for(int N = 0; N < acctLay.size(); N++){
            acctLay[N]->calcHiddenGradients(nextLay, N);
        }
    }
}

void LinearNetwork::calcOutputLayGradients(const Signals &targetVals, Layer &outputLayer){
    for(int n = 0; n < outputLayer.size(); n++){
        outputLayer[n]->calcOutputGradients(targetVals[n]);
    }
}

void LinearNetwork::calcAvarageError(const Signals &targetVals, Layer &outputLayer){
    m_Error = 0.0;
    for(int n = 0; n < outputLayer.size(); n++){
        double delta = targetVals[n] - outputLayer[n]->getOutputVal();
        m_Error += std::pow(delta, 2);
    }
    m_Error /= outputLayer.size();
    m_Error = std::sqrt(m_Error);

    m_RecentAvarageErr = (m_RecentAvarageErr * BLUR_FACT + m_Error) / (BLUR_FACT + 1.0);
}

void LinearNetwork::backPropagation(const Signals &targetVals){
    //bias removed
    Layer &outputLayer = m_Net.back();
    calcAvarageError(targetVals, outputLayer);
    calcOutputLayGradients(targetVals, outputLayer);
    calcHiddLayGradients();
    updateWeights();
}


/// DO TESTÓW
///
/// CZY FEED FORWARD DZIAŁA DLA PODANEJ SIECI           #OK
/// KONSTRUKTORY JAKICH OBIEKTÓW SĄ WYWOŁYWANE
/// DODAJ DESTRUKTOR DLA WSZYSTKICH WARSTW SIECI
/// DOCZYTAJ O FUNKCJACH WIRTUALNYCH I ICH ZWIAZKU Z WYBRANYMI WSKAŹNIKAMI
/// JEŚLI WSZYSTKO DZIAŁA TO DODAJ
///     OPCJĘ ZAPISU I ODCZYTU SIECI
///         WYKORZYSTAJ DO TEGO STRUMIENIE DANYCH
///         MOŻE BYĆ TO PRZEPROWADZONE W TEN SPOSÓB ŻE WCZYTUJE SIE ROZMIAR SIECI, ROBI NOWĄ SIEĆ A FUNKCJA WCZYTAJ
///         KORZYSTA Z FUNKCJI ADD_CONNECTION I SET_WEIGHT

















