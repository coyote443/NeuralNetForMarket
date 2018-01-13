#include "neuralnetwork.h"

/// EACH NEURON INDEX   >= 1
/// BIAS INDEX          == 0
/// EACH SIGNAL INDEX   <= -1

NeuralNetwork::NeuralNetwork(Topology & NetSpec) : m_LayersSizes(NetSpec){}

void NeuralNetwork::feedForward(Signals inSigs){
    /// SIEĆ MA FIZYCZNĄ MOŻLIWOŚĆ OBSŁUGI NIETYPOWYCH RZUTÓW SYGNAŁÓW NA WEJŚCIE, JEDNAK ZE
    /// WZGLĘDU NA ZMNIEJSZENIE SZYBKOŚCI OBLICZEŃ, NIE BĘDZIE TO NA TEN MOMENT IMPLEMENTOWANE

    /*  Sygnał do neuronów inputu musi mieć spis elementów, które są źródłem tego sygnału (np neurony innej sieci,
        reukurencyjne połączenia z wyjścia, bądź po prostu jakis inny sygnał. Bazowo, Sygnał jest pozbawiony indexów,
        więc się mu je sztucznie dodaje.
    */
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

Responses NeuralNetwork::takeOutput(const Layer &layer){
    Responses m_Responses;
    for(Neuron * neuron : layer)
        m_Responses.push_back({neuron->index(), neuron->output()});
    return m_Responses;
}

void NeuralNetwork::drawMe(){
    for(Layer lay : m_Net){
        for(Neuron * neu : lay){
            qDebug() << neu->toQString();
        }
        qDebug() << endl;
    }
}



void LinearNetwork::createNeurons(){
    bool isItFirstLayer = true;
    for(unsigned layerSize : m_LayersSizes){
        Layer   tmpLayer;
        for(unsigned nIndex = 0; nIndex < layerSize; nIndex++){
            if(isItFirstLayer == true){
                LinInputNeuron * LinNeu = new LinInputNeuron;
                tmpLayer.push_back(LinNeu);
            }
            else{
                LinearNeuron * LinNeu = new LinearNeuron;
                tmpLayer.push_back(LinNeu);
            }
        }
        m_Net.push_back(tmpLayer);
    }


//    Layer   tmpLayer;
//    int     nOfLayers = m_LayersSizes.size();
//    int     INPUT_LAYER = 0,
//            OUTPUT_LAYER = nOfLayers - 1;

//    for(int Layer = 0; Layer < nOfLayers; Layer++){
//        if(Layer == INPUT_LAYER)
//            for(unsigned neuronNum = 0; neuronNum < m_LayersSizes.first(); neuronNum++){
//                LinInputNeuron *newNeuron = new LinInputNeuron;
//                tmpLayer.push_back(newNeuron);
//            }
//        else if(Layer < OUTPUT_LAYER)
//            for(unsigned neuronNum = 0; neuronNum < m_LayersSizes[Layer]; neuronNum++){
//                LinHiddenNeuron *newNeuron = new LinHiddenNeuron;
//                tmpLayer.push_back(newNeuron);
//            }
//        else
//            for(unsigned neuronNum = 0; neuronNum < m_LayersSizes.back(); neuronNum++){
//                LinOutputNeuron *newNeuron = new LinOutputNeuron;
//                tmpLayer.push_back(newNeuron);
//            }
//        m_Net.push_back(tmpLayer);
//        tmpLayer.clear();
//    }
}

void LinearNetwork::createConnections(){
    bool INPUT_LAYER    = true;
    int sourceIndex     = 0;
    Layer *prevLayer;

    for(Layer &Lay : m_Net){
        for(Neuron *Neu : Lay){
            if(INPUT_LAYER == true){
                sourceIndex--;
                Neu->createConnection(sourceIndex, 1);
            }
            else{
                for(Neuron *prevNeu : *prevLayer){
                    Neu->createConnection(prevNeu->index());
                }
            }
        }
        INPUT_LAYER = false;
        prevLayer = &Lay;
    }
}

LinearNetwork::LinearNetwork(Topology &spec): NeuralNetwork(spec){
    createNeurons();
    createConnections();
}

/// DO TESTÓW
///
/// CZY FEED FORWARD DZIAŁA DLA PODANEJ SIECI
/// KONSTRUKTORY JAKICH OBIEKTÓW SĄ WYWOŁYWANE
/// DODAJ DESTRUKTOR DLA WSZYSTKICH WARSTW SIECI
/// DOCZYTAJ O FUNKCJACH WIRTUALNYCH I ICH ZWIAZKU Z WYBRANYMI WSKAŹNIKAMI
/// JEŚLI WSZYSTKO DZIAŁA TO DODAJ
///     OPCJĘ ZAPISU I ODCZYTU SIECI
///         WYKORZYSTAJ DO TEGO STRUMIENIE DANYCH
///         MOŻE BYĆ TO PRZEPROWADZONE W TEN SPOSÓB ŻE WCZYTUJE SIE ROZMIAR SIECI, ROBI NOWĄ SIEĆ A FUNKCJA WCZYTAJ
///         KORZYSTA Z FUNKCJI ADD_CONNECTION I SET_WEIGHT
















