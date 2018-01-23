#include "neuralnetwork.h"

NeuralNetwork::NeuralNetwork(const Topology & topology, const Specification & specify) : m_Topology(topology),
    m_Specifi(specify){
        BETA     =  m_Specifi[0],      //      Wsp. krzywej aktywacji || brak
        ETA      =  m_Specifi[1],      //      Wsp. uczenia
        ALPHA    =  m_Specifi[2],      //      Wsp. momentum
        BLUR_FACT=  m_Specifi[3],      //      Wsp. Określający w jakim zakresie uśredniać sqErr
        BIAS_VAL =  m_Specifi[4];

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

LinearNetwork::LinearNetwork(Topology &topol, Specification & specif): NeuralNetwork(topol, specif){
    createLayers();
    createConnections();
}

LinearNetwork::~LinearNetwork(){
    qDebug() << "Jestem w destruktorze linearNetwork";
    for(Layer & layer: m_Net)
        for(Neuron * neuron : layer)
            delete neuron;
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

double LinearNetwork::backPropagation(const Signals &targetVals){
    Layer &outputLayer = m_Net.back();
    calcAvarageError(targetVals, outputLayer);

    // Czasem sqErr = 0; pomijam te wyniki z zerem; usredniam Err zgodnie ze wsp. BLUR
    static int sqErrCounter = BLUR_FACT;
    if(m_Error != 0){
        sqErrCounter--;
        m_RecentAvarageErr += m_Error;
        if(sqErrCounter == 0){
            sqErrCounter        = BLUR_FACT;
            m_RecentAvarageErr  /= BLUR_FACT;
            m_Corectness        = m_RecentAvarageErr;
            m_RecentAvarageErr  = 0;
        }
    }
    calcOutputLayGradients(targetVals, outputLayer);
    calcHiddLayGradients();
    updateWeights();
    return m_Corectness;
}

QString LinearNetwork::toQString(QString SEP){
    QString toOut;
    QTextStream stream(&toOut);

    for(double specification : m_Specifi){
        stream << specification << SEP;
    }
    stream << endl;

    for(int val : m_Topology){
        stream << val << SEP;
    }
    stream << endl;

    for(Layer layer : m_Net){
        for(Neuron * neuron : layer){
            stream << neuron->toQString(SEP);
        }
    }
    stream << endl;
    return toOut;
}


void LinearNetwork::loadNetwork(QString local){
    // otwórz okno wczytania
}

void LinearNetwork::createLayers(){
    if(BIAS_VAL == 1)
        for(int x = 0; x < m_Topology.size() - 1; x++)      // w ostatniej warstwie nie trzeba neuronu od biasu
            m_Topology[x]++;

    for(int layerIndex = 0; layerIndex < m_Topology.size(); layerIndex++){
        Layer tmpLayer;
        int givenLaySize  = m_Topology[layerIndex];
        int lastLayIndx  = m_Topology.size() - 1,
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

void LinearNetwork::calcAvarageError(const Signals &targetVals, const Layer &outputLayer){
    m_Error = 0.0;
    for(int n = 0; n < outputLayer.size(); n++){
        double delta = targetVals[n] - outputLayer[n]->getOutputVal();
        m_Error += std::pow(delta, 2);
    }
    m_Error /= outputLayer.size();
    m_Error = std::sqrt(m_Error);
}



/// DO TESTÓW
///
/// CZY FEED FORWARD DZIAŁA DLA PODANEJ SIECI           #OK
/// KONSTRUKTORY JAKICH OBIEKTÓW SĄ WYWOŁYWANE          #OK
/// DODAJ DESTRUKTOR DLA WSZYSTKICH WARSTW SIECI        #OK// neurony BIASU usuwa automatycznie. Dlaczego?
/// PRZETESTUJ PROGRAM POD KĄTEM WYCIEKÓW PAMIĘCI
/// DOCZYTAJ O FUNKCJACH WIRTUALNYCH I ICH ZWIAZKU Z WYBRANYMI WSKAŹNIKAMI
/// JEŚLI WSZYSTKO DZIAŁA TO DODAJ
///     OPCJĘ ZAPISU I ODCZYTU SIECI
///         WYKORZYSTAJ DO TEGO STRUMIENIE DANYCH
///         MOŻE BYĆ TO PRZEPROWADZONE W TEN SPOSÓB ŻE WCZYTUJE SIE ROZMIAR SIECI, ROBI NOWĄ SIEĆ A FUNKCJA WCZYTAJ
///         KORZYSTA Z FUNKCJI ADD_CONNECTION I SET_WEIGHT

















