#include "neuron.h"



//// Każdy neuron tworzony jest z jednym podłączeniem do BIASU
        Neuron::Neuron(){
            qsrand(QTime::currentTime().msec());
            createdNeurons++;
            m_Index       = createdNeurons;
            m_Connections = new Connections;
            m_Connections->push_back(Connect(BIAS_INDEX, BIAS_WEIGHT));
        }

        Neuron::~Neuron(){
            delete m_Connections;
        }

int     Neuron::createdNeurons = 0;

QString Neuron::toQString(QString SEP){
            QString out;
            QTextStream stream(&out);
            for(Connect con : *m_Connections){
                stream << con.m_Weight << SEP;
            }
            stream << "OUT " << m_Output;
            return out;
        }


        LinearNeuron::~LinearNeuron(){}

void    LinearNeuron::createConnection(int sourceIndex, double weightVal){
            if(weightVal == 0) weightVal = generateWeightVal();
            Connect connection(sourceIndex, weightVal);
            m_Connections->push_back( connection );
        }

void    LinearNeuron::takeThisSignal(Response fromPrevLayer){
            agregateThisSignal(fromPrevLayer);
            if(possibleToSendSignal())
                pushSignal();
        }

void    LinearNeuron::pushSignal(){
            m_Output = tranFun(m_tmpAgregatedSignal);
            prepareNeuronForNextSignals();
        }

double  LinearNeuron::generateWeightVal(){
            double randVal = (((rand() % 1000000L) / 1700.0) - 9.8) * 0.05;
            return (randVal != 0 ? randVal : generateWeightVal());
        }

void    LinearNeuron::agregateThisSignal(Response fromPrvLayer){
            int     sIndex  = fromPrvLayer.first;
            double  sOutput = fromPrvLayer.second;

            for(Connect &givenConnection : *m_Connections)
                if(givenConnection.m_NeuronIndex == sIndex){
                    m_tmpAgregatedSignal            +=  sOutput * givenConnection.m_Weight;
                    givenConnection.m_SignalGate    =   CLOSED;
                }
            if(m_IsBiasAdded == false){
                double biasWeight   = m_Connections->front().m_Weight;
                bool & biasGate     = m_Connections->front().m_SignalGate;
                m_tmpAgregatedSignal+= BIAS_VAL * biasWeight;
                biasGate            = CLOSED;
                m_IsBiasAdded       = true;
            }
        }

bool    LinearNeuron::possibleToSendSignal(){
            for(Connect &givenConnection : *m_Connections){
                if(givenConnection.m_SignalGate == OPEN){
                    return false;
                }
            }
            return true;
        }

void    LinearNeuron::prepareNeuronForNextSignals(){
            m_tmpAgregatedSignal = 0;
            for(Connect &givenConnection : *m_Connections){
                givenConnection.m_SignalGate = OPEN;
                m_IsBiasAdded = false;
            }
        }



void    LinInputNeuron::createConnection(int sourceIndex){
            int dontChangeInputSignal = 1;
            Connect connection(sourceIndex, dontChangeInputSignal);
            m_Connections->push_back( connection );
        }

void    LinInputNeuron::pushSignal(){
            m_Output = m_tmpAgregatedSignal;
            prepareNeuronForNextSignals();
        }






