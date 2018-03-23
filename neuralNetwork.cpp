#include "neuralNetwork.h"

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

NeuralNetwork::NeuralNetwork(const NeuralNetwork &model){
    if(&model != this){
        this->operator =(model);
    }
}

NeuralNetwork::~NeuralNetwork(){
    for(Layer & layer: m_Net)
        for(Neuron * neuron : layer)
            delete neuron;
}

NeuralNetwork & NeuralNetwork::operator=(const NeuralNetwork & neuralNet){
    if(&neuralNet == this) return *this;

    this->ALPHA     = neuralNet.ALPHA;
    this->BETA      = neuralNet.BETA;
    this->ETA       = neuralNet.ETA;
    this->BLUR_FACT = neuralNet.BLUR_FACT;
    this->BIAS_VAL  = neuralNet.BIAS_VAL;
    this->m_Specifi = neuralNet.m_Specifi;
    this->m_Topology= neuralNet.m_Topology;

    const Network & parentNet = neuralNet.m_Net;
    Network & myNet = this->m_Net;

    for(int layPos = 0; layPos < parentNet.size(); layPos++){
        const Layer & parentLay = parentNet[layPos];
        Layer & myLay = myNet[layPos];

        for(int neuPos = 0; neuPos < parentLay.size(); neuPos++){
            Neuron & parentNeu = *parentLay[neuPos];
            Neuron & myNeu = *myLay[neuPos];
            myNeu = parentNeu;
        }
    }
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

LinearNetwork::LinearNetwork(const Topology &topol, const Specification & specif): NeuralNetwork(topol, specif){
    createLayers();
    createNewConnections();
}

void LinearNetwork::createGivenConnections(AllNetConn &netCon)
{
    for(int neuronNum = 0; neuronNum < netCon.size(); neuronNum++){
        NeuronConn &neurCon     = netCon[neuronNum];
        int neuronPos           = neuronNum;
        int layerPos            = 0;
        bool done               = false;

        for(Layer lay : m_Net){
            if(neuronPos > lay.size() - 1 && done == false){
                neuronPos -= lay.size();
                layerPos++;
            }
            else if(done == false)
                done = true;
        }

        Neuron &neuron          = *m_Net[layerPos][neuronPos];

        QVector<int> &indexes   = neurCon.first;
        QVector<double> &weights= neurCon.second;
        for(int takenC = 0; takenC < indexes.size(); takenC++)
            neuron.createConnection(indexes[takenC], weights[takenC]);
    }
}

LinearNetwork::LinearNetwork(const Topology &topol, const Specification &specif, AllNetConn &netCon): NeuralNetwork(topol, specif){
    createLayers();
    createGivenConnections(netCon);
}

LinearNetwork::~LinearNetwork(){}

LinearNetwork & LinearNetwork::operator=(const LinearNetwork & linNet){
    if(&linNet != this){
        (*this).NeuralNetwork::operator =(linNet);
    }
    return *this;
}

void LinearNetwork::feedForward(const Signals &inSigs){
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
            takenNeuron->pushSignal();
        }
        prevLayerRespo = takeOutput(takenLayer);
    }
}


/// Każda z sieci ma taką samą konstrukcję, więc można zamienić nie tylko wagi, ale całą strukturę połączenia od razu
/// Robię to na poziomie neuronów, bo Layer to wektor wskaźników
void LinearNetwork::swapLayer(Layer &layer, unsigned layerNum){
    Layer & myLayer = m_Net[layerNum];
    if(myLayer.size() == layer.size()){
        for(int pos = 0; pos < myLayer.size(); pos++)
            *myLayer[pos] = *layer[pos];
    }
}

void LinearNetwork::swapNeuron(Neuron &neuron, unsigned layerNum, int neuronNum){
    Layer &myLayer      = m_Net[layerNum];
    Neuron &myNeuron    = *myLayer[neuronNum];
    if(neuronNum < myLayer.size())
        myNeuron = neuron;
}

void LinearNetwork::swapConn(Connection &conn, unsigned layerNum, unsigned neuronNum, unsigned connNum){
    Layer &myLayer      = m_Net[layerNum];
    Neuron &myNeuron    = *myLayer[neuronNum];
    myNeuron.swapWeight(conn, connNum);
}






void LinearNetwork::makeAvarageError(){
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
}

double LinearNetwork::backPropagationOnlyError(const Signals &targetVals){
    calcAvarageError(targetVals);
    makeAvarageError();
    return m_Corectness;
}

double LinearNetwork::backPropagation(const Signals &targetVals){
    calcAvarageError(targetVals);
    makeAvarageError();
    calcOutputLayGradients(targetVals);
    calcHiddLayGradients();
    updateWeights();
    return m_Corectness;
}

void LinearNetwork::changeNetSpecification(const Specification &specify){
        m_Specifi = specify;
        BETA     =  m_Specifi[0],
        ETA      =  m_Specifi[1],
        ALPHA    =  m_Specifi[2],
        BLUR_FACT=  m_Specifi[3],
        BIAS_VAL =  m_Specifi[4];

        Neuron::setETA(ETA);
        Neuron::setALFA(ALPHA);
}

QString LinearNetwork::toQString(QString SEP){
    QString toOut;
    QTextStream stream(&toOut);
    QStringList tmpLst;

    for(double specification : m_Specifi){
        tmpLst.push_back( QString("%1").arg(specification) );
    }
    stream << tmpLst.join(SEP);
    stream << endl;

    tmpLst.clear();
    for(int val : m_Topology){
        tmpLst.push_back( QString("%1").arg(val) );
    }
    stream << tmpLst.join(SEP);
    stream << endl;

    tmpLst.clear();

    QStringList tmpList;
    for(Layer layer : m_Net){
        for(Neuron * neuron : layer){
            tmpList.push_back(neuron->toQString(SEP));
        }
    }
    toOut += tmpList.join("\n");

    return toOut;
}

void LinearNetwork::createLayers(){
    if(BIAS_VAL == 1)
        for(int x = 0; x < m_Topology.size() - 1; x++)      // w ostatniej warstwie nie trzeba neuronu od biasu
            m_Topology[x]++;

    int neuronNum = 0;
    for(int layerIndex = 0; layerIndex < m_Topology.size(); layerIndex++){
        Layer tmpLayer;
        int givenLaySize  = m_Topology[layerIndex];
        int lastLayIndx  = m_Topology.size() - 1,
            lastLayN = givenLaySize - 1;

        for(int takenNeuron = 0; takenNeuron < givenLaySize; takenNeuron++){
            neuronNum++;
            if(layerIndex == 0 && takenNeuron != lastLayN){
                LinInputNeuron * LinNeu = new LinInputNeuron(neuronNum);
                tmpLayer.push_back(LinNeu);
            }
            else if(takenNeuron == lastLayN && BIAS_VAL == 1 && layerIndex != lastLayIndx){
                LinBiasNeuron * LinBias = new LinBiasNeuron(neuronNum);
                tmpLayer.push_front(LinBias);
            }
            else{
                LinearNeuron * LinNeu = new LinearNeuron(neuronNum);
                tmpLayer.push_back(LinNeu);
            }
        }
        m_Net.push_back(tmpLayer);
    }
}

void LinearNetwork::createNewConnections(){
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

void LinearNetwork::calcOutputLayGradients(const Signals &targetVals){
    Layer &outputLayer = m_Net.back();
    for(int n = 0; n < outputLayer.size(); n++){
        outputLayer[n]->calcOutputGradients(targetVals[n]);
    }
}

double LinearNetwork::calcAvarageError(const Signals &targetVals){
    const Layer &outputLayer = m_Net.back();
    m_Error = 0.0;
    for(int n = 0; n < outputLayer.size(); n++){
        double delta = targetVals[n] - outputLayer[n]->getOutputVal();
        m_Error += std::pow(delta, 2);
    }
    m_Error /= outputLayer.size();
    m_Error = std::sqrt(m_Error);
    return (double)m_Error;
}
