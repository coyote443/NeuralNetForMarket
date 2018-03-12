#include "teacher.h"
#include <algorithm>
#include <QCoreApplication>

Teacher::Teacher(QObject *parent): QObject(parent){
    qsrand(QTime::currentTime().msec());
}


void Teacher::setSpecification(double err, double mut, double surv, int popSize){
    MIN_ERROR       = err,
    MUTATION_RATE   = mut,
    SURVIVE_RATE    = surv;
    POPULATION_SIZE = popSize;
}

void Teacher::setTopolAndGeneralSpecif(const Topology &topol, const Specification &specif){
    m_Topol    = &topol;
    m_Specif   = &specif;
}

void Teacher::linkProgBarrs(QProgressBar *progBar, QProgressBar *epochBarr){
    m_ProgBar = progBar;
    m_EpochBar= epochBarr;
}


void Teacher::teachOneNetworkFF(LinearNetwork &nets, int netNr, int netSize, const LearnVect &sig, SigClasses sigClasses){
    int howManyOuts = (netSize > 1 ? 1 : sigClasses.size());
    Signals targetVals(howManyOuts);

    LearnVect learnVect = sig;
    double currentError = 1;
    std::random_shuffle(learnVect.begin(), learnVect.end());

    do{ /// Teach those network until it reaches destinated error rate
        std::random_shuffle(learnVect.begin(), learnVect.end());

        int progressCounter = 0,
            sigSize         = learnVect.size();

        for(LearnSig lernSig : learnVect){
            /// Make target vals Vector
            QString takenClass = lernSig.first;                       /// Take signal class

            for(double &val : targetVals)
                val = 0;

            switch (netSize) {
            case 1:
                targetVals[ sigClasses[takenClass] ]  = 1;            /// Only output related with takenClass have value '1'
                break;
            default:
                if(netNr == sigClasses[takenClass])                   /// Only network related with takenClass
                    targetVals[0] = 1;                                /// have one output with '1' value
                break;
            }

            /// FeedForward
            const Signals &givenSignal = lernSig.second;
            nets.feedForward(givenSignal);

            /// BackPropagation
            currentError = nets.backPropagation(targetVals);

            /// Set Epoch Bar
            double progtmp = (double)progressCounter / (double)sigSize * 100;
            m_EpochBar->setValue(progtmp);
            progressCounter++;

            /// Set Error Bar
            m_ProgBar->setValue(100 - currentError * 100);
            QCoreApplication::processEvents();                       /// GUI don`t frezee due to using this instruction
        }
        emit nextEpoch();
    }while(currentError > MIN_ERROR);
}

void Teacher::teachThoseNetworksFF(AllNets &nets, const LearnVect &sig, SigClasses sigClasses){
    if(nets.size() == 1){
        teachOneNetworkFF(*nets.first(), 0, 1, sig, sigClasses);
        emit netTrained();
    }
    else{
        for(int pos = 0; pos < nets.size(); pos++){
            LinearNetwork * linearNetwork = nets[pos];
            teachOneNetworkFF(*linearNetwork, pos, nets.size(), sig, sigClasses);
            emit netTrained();
        }
    }
}


void Teacher::calculateAvarageError(Population &population, int netSize, const LearnVect &AllSignals, SigClasses sigClasses, int netNr){
    for(NetAndCharacter &netandfit : population){
        int howMany = (netSize > 1 ? 1 : sigClasses.size());
        Signals targetVals(howMany);

        LinearNetwork * takenNetwork = netandfit.network;
        double & networkAvaErr       = netandfit.errorRate;

        for(LearnSig learnSig : AllSignals){
            /// Make target vals Vector
            QString takenClass = learnSig.first;                       /// Take signal class

            for(double &val : targetVals)
                val = 0;

            switch (netSize) {
            case 1:
                targetVals[ sigClasses[takenClass] ]  = 1;            /// Only output related with takenClass have value '1'
                break;
            default:
                if(netNr == sigClasses[takenClass])                   /// Only network related with takenClass
                    targetVals[0] = 1;                                /// have one output with '1' value
                break;
            }

            /// FeedForward
            const Signals &givenSignal = learnSig.second;
            takenNetwork->feedForward(givenSignal);

            /// Error sum, and divider optimization
            networkAvaErr = takenNetwork->backPropagationOnlyError(targetVals);
            QCoreApplication::processEvents();                       /// GUI don`t frezee due to using this instruction
        }
    }
}

void Teacher::killGivenPercOfPopulation(Population &population){
    int toDelSize = population.size() - SURVIVE_RATE * population.size();
    for(int toKill = 0; toKill < toDelSize; toKill++){
        delete population.back().network;
        population.pop_back();
    }
}

void Teacher::makeBreedRate(Population &population){
    double divider = 0;
    for(NetAndCharacter &subject : population){
        if(subject.errorRate >= 1.0){
            subject.breeadingRate = 0;
            continue;
        }
        subject.breeadingRate = (1.0 - subject.errorRate) * 100.0;
        divider += subject.breeadingRate;
    }

    for(NetAndCharacter &subject : population){
        subject.breeadingRate = subject.breeadingRate / divider;
    }
}

NetAndCharacter &Teacher::findParent(Population &population){
    double tmpFit = 0.0;
    double val = qrand()%100 / 100.0,
        pos = 0;

    //qDebug() << "val             " << val;

    for(NetAndCharacter network : population){
        tmpFit += network.breeadingRate;
        if(val <= tmpFit){
            //qDebug() << "Pos             " << pos;
            return population[pos];
        }
        pos++;
    }
}

//  DO REGULACJI JEST TUTAJ ILOŚĆ ZMIAN W PĘTLI A`LA CROSSING OVER
NetAndCharacter & Teacher::makeChildren(Population &population){
    // Losuj 2 rodziców, prawdopodobieństwo wylosowania jest proporcjonalne do fitness
    NetAndCharacter *parentOne = 0, *parentTwo = 0;
    parentOne = &findParent(population);
    do{
        parentTwo = &findParent(population);
    }while(parentOne == parentTwo);

    // Zrób dziecko identyczne jak rodzicOne - przepisz wartości;
    NetAndCharacter *offspring;

    offspring                 = new NetAndCharacter;
    offspring->breeadingRate  = 0;
    offspring->errorRate      = 0;
    offspring->network        = new LinearNetwork(*m_Topol, *m_Specif);
    *offspring->network       = *parentOne->network;

    // Tutaj jest pętla zamian a`la crossing over
    for(int zamianaNr = 0; zamianaNr < 150; zamianaNr++){
        int randNum     = qrand() % 100;
        int randLayer   = qrand() % (parentOne->network->size() - 1) + 1;

        Layer & parentTwoLayer          = parentTwo->network->m_Net[randLayer];

        int randNeuron                  = qrand()%parentTwoLayer.size();
        Neuron & parentTwoNeuron        = *parentTwoLayer[randNeuron];
        Connections & parentTwoConns    = parentTwoNeuron.getConnections();
        int randConn                    = qrand()%parentTwoConns.size();

        if(randNum == 0)            // 1%  na zamianę warstw
            offspring->network->swapLayer(parentTwoLayer, randLayer);
        else if(randNum < 30)       // 29% na zamiane neuronów
            offspring->network->swapNeuron(parentTwoNeuron, randLayer, randNeuron);
        else if(randNum < 100)      // 70% na zamianę wag pomiędzy dziećmi
            offspring->network->swapConn(parentTwoConns[randConn], randLayer, randNeuron, randConn);
    }
    return *offspring;
}


void Teacher::makeMutation(NetAndCharacter &individual){
    int mutRate = 100 - 100 * MUTATION_RATE;
    int rndVal  = qrand()%100;

    if(rndVal >= mutRate){
        LinearNetwork & takenNet = *individual.network;

        int randNum     = qrand()%100;

        int randLayer   = qrand()%(takenNet.size() - 1) + 1;
        Layer &takenLayer = takenNet.m_Net[randLayer];

        int randNeuron = qrand()%takenLayer.size();
        Neuron &takenNeuron = *takenLayer[randNeuron];

        Connections &takenConns = takenNeuron.getConnections();
        int randConnOne = qrand()%takenConns.size();
        int randConnTwo = qrand()%takenConns.size();

        if(takenConns.size() > 1)
            while(randConnOne == randConnTwo)
                randConnTwo = qrand()%takenConns.size();

        Connection &takenConOne = takenConns[randConnOne];
        Connection &takenConTwo = takenConns[randConnTwo];

        // ale to okropnie wygląda
        if(randNum < 7){
            // zastąp losową wagę zupełnie nową wartością losową - 7%
            takenConOne.m_Weight = LinearNeuron::generateWeightVal();
        }
        else if(randNum < 14){
            // zmień znak wagi - 7%
            takenConOne.m_Weight *= -1;
        }
        else if(randNum < 21){
            // zamień ze sobą wartości dwóch wag w neuronie - 7%
            double tmpWeightVal     = takenConOne.m_Weight;
            takenConOne.m_Weight    = takenConTwo.m_Weight;
            takenConTwo.m_Weight    = tmpWeightVal;
        }
        else if(randNum < 61){
            // pomnóż wagę przez liczbę losową z przedziału od 0.1 do 2.0 - 39.5%
            double multipler = 0;
            while(multipler == 0)
                multipler = (qrand()%2000 + 1) / 1000.0;

            takenConOne.m_Weight *= multipler;
        }
        else if(randNum < 100){
            // dodaj lub odejmij liczbę losową z zakresu 0 - 1 od wagi - 39.5%
            double val = (qrand()%1000) / 1000;
            if(qrand()%2 != 0)
                val *= -1;
            takenConOne.m_Weight += val;
        }
    }
}


void Teacher::teachOneNetworkGen(LinearNetwork &receivedNet, int netNr, int netSize, const LearnVect &AllSignals, SigClasses sigClasses){

    /// Net nr. - dla której z kolei sieci przeprowadzamy naukę. netSize - Ile łacznie sieci mamy do nauki. netSize wyznacza sposób
    /// konstrukcji sygnału testowego dla sieci

    /// Wygeneruj losową populację sieci dla zadanego problemu.
    Population population;
    for(int indivd = 0; indivd < POPULATION_SIZE - 1; indivd++){
        LinearNetwork * tmp = new LinearNetwork(*m_Topol, *m_Specif);
        population.push_back({0, 0, tmp});
    }

    /// Włączamy otrzymaną sieć do populacji
    population.push_back({0, 0, &receivedNet});

    /// Skonstruuj wektor prawidłowych odpowiedzi sieci. Licz usredniony fitness(Error) dla każdej sieci.
    qDebug() << "Prosze chwile poczekac";
    calculateAvarageError(population, netSize, AllSignals, sigClasses, netNr);


    qSort(population.begin(), population.end(), [](NetAndCharacter a,  NetAndCharacter b)->bool{return a.errorRate < b.errorRate;}); // to del

    qDebug() <<"Posortowany wedle errRate ";
    for(NetAndCharacter simple : population)
        qDebug() << simple.errorRate;
    qDebug() <<"END " << endl << endl;

    double currAvError = 1.0;

    /// Zacznij ewoulcję
    do{
        qDebug() << "Pop size " << population.size();
        qSort(population.begin(), population.end(), [](NetAndCharacter a,  NetAndCharacter b)->bool{return a.errorRate < b.errorRate;});

        qDebug() <<"Przed Usmierceniem";
        for(NetAndCharacter simple : population)
            qDebug() << simple.errorRate;
        qDebug() <<"END " << endl << endl;

        // usmierć wszystkich rodziców, i zrób tyle dzieci aby to sensownie działało
        /// Uśmiercam pewien % najgorzej przystosowanych
        killGivenPercOfPopulation(population);

        qDebug() <<"Po usmierceniu";
        for(NetAndCharacter simple : population)
            qDebug() << simple.errorRate;
        qDebug() <<"END " << endl << endl;

        /// Obliczamy szanse na rozmnożenie się poszczególnych osobników
        makeBreedRate(population);


        /// Usuń duplikaty u rodziców

        /// Stwórz kolejną generację o wielkości == ilości uśmierconych osobników;
        /// Nie możemy tworzyc duplikatów, bo proces ewolucji jest skupiony w pewnym momencie tylko na jednym genotypie;
        Population Offspring;
        for(int x = population.size(); x < POPULATION_SIZE; x++){
            while(true){
                Population oneIndiv = {makeChildren(population)};
                NetAndCharacter & myChildren = oneIndiv.back();

                /// Szczęśliwcy otrzymują mutację
                makeMutation(myChildren);

                calculateAvarageError(oneIndiv, netSize, AllSignals, sigClasses, netNr);
                m_EpochBar->setValue(33);

                if( !Offspring.isEmpty() ){
                    bool isEqualToAny = false;
                    for(NetAndCharacter &takenNet : Offspring){     /// Nie może urodzić się identyczny jak jego brat
                        if(takenNet.errorRate == myChildren.errorRate){
                            isEqualToAny = true;
                        }
                    }
                    for(NetAndCharacter &takenNet : population){    /// Nie może urodzić się identyczny jak rodzic
                        if(takenNet.errorRate == myChildren.errorRate){
                            isEqualToAny = true;
                        }
                    }

                    if(isEqualToAny == false){
                        Offspring.push_back(myChildren);
                        break;
                    }
                    else{
                        qDebug() << "niechciane dziecko";
                        qDebug() << myChildren.errorRate;
                        delete myChildren.network;
                    }
                }
                else{
                    Offspring.push_back(myChildren);
                    break;
                }
            }



            makeChildren(population);
        }

        calculateAvarageError(Offspring, netSize, AllSignals, sigClasses, netNr);
        m_EpochBar->setValue(66);

        qDebug() <<"Offspring";
        for(NetAndCharacter simple : Offspring)
            qDebug() << simple.errorRate;
        qDebug() <<"END " << endl << endl;

        /// Dzieci dołączają do populacji rodzicielskiej
        for(NetAndCharacter &child : Offspring)
            population.push_back(child);

        /// Skonstruuj wektor prawidłowych odpowiedzi sieci. Licz usredniony fitness(Error) dla każdej sieci.
        calculateAvarageError(population, netSize, AllSignals, sigClasses, netNr);
        m_EpochBar->setValue(100);

        qSort(population.begin(), population.end(), [](NetAndCharacter a,  NetAndCharacter b)->bool{return a.errorRate < b.errorRate;});

        currAvError = population.front().errorRate;

        /// Tutaj określamy wartość poprawności odpowiedzi dla najlepszej sieci
        m_ProgBar->setValue(100 - currAvError * 100);
        emit nextEpoch();
    }while(currAvError > MIN_ERROR);

    /// Po okresie nauki, przekształcamy otrzymaną referencję do sieci tak, aby odpowiadała najlepszemu osobnikowi.
    receivedNet = *population.front().network;
}

void Teacher::teachThoseNetworksGen(AllNets &nets, const LearnVect &sig, SigClasses sigClasses){
    if(nets.size() == 1){
        teachOneNetworkGen(*nets.first(), 0, 1, sig, sigClasses);
        emit netTrained();
    }
    else{
        for(int pos = 0; pos < nets.size(); pos++){
            LinearNetwork * linearNetwork = nets[pos];
            teachOneNetworkGen(*linearNetwork, pos, nets.size(), sig, sigClasses);
            emit netTrained();
        }
    }
}
