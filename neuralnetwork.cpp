#include "neuralnetwork.h"

/// EACH NEURON INDEX   >= 1
/// BIAS INDEX          == 0
/// EACH SIGNAL INDEX   <= -1

NeuralNetwork::NeuralNetwork(Topology & NetSpec) : m_LayersSizes(NetSpec){}

void NeuralNetwork::feedForward(Signals inSigs){
    /// SIEĆ MA FIZYCZNĄ MOŻLIWOŚĆ OBSŁUGI NIETYPOWYCH RZUTÓW SYGNAŁÓW NA WEJŚCIE, JEDNAK ZE
    /// WZGLĘDU NA ZMNIEJSZENIE SZYBKOŚCI OBLICZEŃ, NIE BĘDZIE TO NA TEN MOMENT IMPLEMENTOWANE

    //  Generuję index źródłowy dla każdego sygnału
    Responses prevLayerRespo; // na podstawie sygnału tworzymy przejęcie syg
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
    }
}

Responses & NeuralNetwork::takeOutput(){
    m_Responses.clear();
//    for(Neuron * wej : m_Net.back())
//        m_Responses.push_back(wej->output());
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



void LinearNetwork::createNeurons()
{
    Layer   tmpLayer;
    int     nOfLayers = m_LayersSizes.size();
    int     INPUT_LAYER = 0,
            OUTPUT_LAYER = nOfLayers - 1;

    for(int Layer = 0; Layer < nOfLayers; Layer++){
        if(Layer == INPUT_LAYER)
            for(unsigned neuronNum = 0; neuronNum < m_LayersSizes.first(); neuronNum++){
                LinInputNeuron *newNeuron = new LinInputNeuron;
                tmpLayer.push_back(newNeuron);
            }
        else if(Layer < OUTPUT_LAYER)
            for(unsigned neuronNum = 0; neuronNum < m_LayersSizes[Layer]; neuronNum++){
                LinHiddenNeuron *newNeuron = new LinHiddenNeuron;
                tmpLayer.push_back(newNeuron);
            }
        else
            for(unsigned neuronNum = 0; neuronNum < m_LayersSizes.back(); neuronNum++){
                LinOutputNeuron *newNeuron = new LinOutputNeuron;
                tmpLayer.push_back(newNeuron);
            }
        m_Net.push_back(tmpLayer);
        tmpLayer.clear();
    }
}

void LinearNetwork::createConnections()
{
    Layer  *takenLayer;
    Layer  *prevLayer;
    int     nOfLayers = m_Net.size();
    int     INPUT_LAYER = 0;

    for(int layerNum = 0; layerNum < nOfLayers; layerNum++){
        takenLayer = &(m_Net[layerNum]);

        for(int neuronNum = 0; neuronNum < takenLayer->size(); neuronNum++){
            int sourceIndex;
            if(layerNum == INPUT_LAYER){
                sourceIndex = -(neuronNum + 1);
                (*takenLayer)[neuronNum]->createConnection(sourceIndex);
            }
            else
                for(int prevNeuron = 0; prevNeuron < prevLayer->size(); prevNeuron++){

                    sourceIndex = (*prevLayer)[prevNeuron]->index();
                    (*takenLayer)[neuronNum]->createConnection(sourceIndex);
                }
        }
        prevLayer = takenLayer;
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

















