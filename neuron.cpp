#include "neuron.h"

Neuron::Neuron(){
    qsrand(QTime::currentTime().msec());
    createdNeurons++;
    m_Index = createdNeurons;
}

Neuron::~Neuron(){qDebug()<< "uruchamiam destruktor neuronu";}

int     Neuron::createdNeurons  = 0;
double  Neuron::ALPHA           = 0;
double  Neuron::ETA             = 0;


QString Neuron::toQString(QString SEP){
    QStringList tmpLst;
    for(Connection con : m_Connections){
        tmpLst.push_back( QString("%1%2%3").arg(con.m_NeuronIndex).arg(SEP).arg(con.m_Weight) );
    }
    return tmpLst.join(SEP);
}


LinearNeuron::~LinearNeuron(){qDebug()<< "uruchamiam destruktor neuronuLiniowego";}

void LinearNeuron::createConnection(int sourceIndex, double weightVal){
    if(weightVal == RANDOM) weightVal = generateWeightVal();
    Connection connection(sourceIndex, weightVal);
    m_Connections.push_back( connection );
}

void LinearNeuron::takeThisSignal(Response fromPrevLayer){
    agregateThisSignal(fromPrevLayer);
    if(possibleToSendSignal())
        pushSignal();
}

void LinearNeuron::pushSignal(){
    m_Output = tranFun(m_AgregatedSignal);
    prepareNeuronForNextSignals();
}

double LinearNeuron::generateWeightVal(){
    double randVal = (((rand() % 1000000L) / 1700.0) - 9.8) * 0.05;
    return (randVal != 0 ? randVal : generateWeightVal());
}

void LinearNeuron::agregateThisSignal(Response fromPrvLayer){
    int     sIndex  = fromPrvLayer.first;
    double  sOutput = fromPrvLayer.second;

    for(Connection &givenConnection : m_Connections)
        if(givenConnection.m_NeuronIndex == sIndex){
            m_AgregatedSignal            +=  sOutput * givenConnection.m_Weight;
            givenConnection.m_SignalGate =   CLOSED;
        }
}

bool LinearNeuron::possibleToSendSignal(){
    for(Connection &givenConnection : m_Connections){
        if(givenConnection.m_SignalGate == OPEN){
            return false;
        }
    }
    return true;
}

void LinearNeuron::prepareNeuronForNextSignals(){
    m_AgregatedSignal = 0;
    for(Connection &givenConnection : m_Connections){
        givenConnection.m_SignalGate = OPEN;
    }
}



void LinInputNeuron::createConnection(int sourceIndex){
    int dontChangeInputSignal = 1;
    Connection connection(sourceIndex, dontChangeInputSignal);
    m_Connections.push_back( connection );
}

void LinInputNeuron::pushSignal(){
    m_Output = m_AgregatedSignal;
    prepareNeuronForNextSignals();
}


void LinearNeuron::calcOutputGradients(double targetVal){
    double delta = targetVal - m_Output;
    m_Gradient = delta * LinearNeuron::transferFunctionDerr(m_Output);
}

void LinearNeuron::calcHiddenGradients(Layer &nextLay, int neuron_index){
    double gradSum = 0;
    for(int N = 0; N < nextLay.size(); N++){
        // LinearNeuron ma dostęp do danych składowych klasy, jednak wskaźnik do Neuron już nie(jak używamy go w LinearNeuron,
        // stąd też rzutowanie) Tylko nie chwytam do końca dlaczego.

        LinearNeuron *neuron = static_cast<LinearNeuron *>(nextLay[N]);
        gradSum += neuron->m_Gradient * neuron->m_Connections[neuron_index].m_Weight;
    }
    m_Gradient = gradSum * LinearNeuron::transferFunctionDerr(m_Output);
}


//Założenie : prevLay ma tyle neuronów co jest wag w outpucie
void LinearNeuron::updateWeights(Layer & prevLay){
    for(int W = 0; W < m_Connections.size(); W++){
        double oldDelta = m_Connections[W].m_DeltaWeight;
        double newDelta = ETA * m_Gradient * prevLay[W]->getOutputVal() + ALPHA * oldDelta;

        m_Connections[W].m_DeltaWeight  =  newDelta;
        m_Connections[W].m_Weight       += newDelta;
    }
}


void LinBiasNeuron::takeThisSignal(Response){
    m_AgregatedSignal = 1;
    pushSignal();
}





