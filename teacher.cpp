#include "teacher.h"
#include <algorithm>

Teacher::Teacher(QObject *parent): QObject(parent){
    qsrand(QTime::currentTime().msec());
}

void Teacher::teachOneNetworkFF(LinearNetwork &nets, int netNr, int netSize, const LearnVect &sig, SigClasses sigClasses,
                              QProgressBar &progBar, double targetError){
    int howMany = (netSize > 1 ? 1 : sigClasses.size());
    Signals targetVals(howMany);

    LearnVect learnVect = sig;
    double currentError = 1;
    std::random_shuffle(learnVect.begin(), learnVect.end());

    do{ /// Teach those network until it reaches destinated error rate
        std::random_shuffle(learnVect.begin(), learnVect.end());

        QVector<int> progresPoints;
        for(int point = 100; point >= 0; point--)
            progresPoints.push_back(point);

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
           // qDebug() << currentError;

            /// Set Status Bar
            progressCounter++;
            double progtmp = (double)progressCounter / (double)sigSize * 100;
            if((int)progtmp > progresPoints.back()){
                progresPoints.pop_back();
                emit nextOnePercentOfEpoch();
            }
            progBar.setValue(100 - currentError * 100);
        }
        emit nextEpoch();
    }while(currentError > targetError);
}

void Teacher::teachThoseNetworksFF(AllNets &nets, const LearnVect &sig, SigClasses sigClasses,
                                  double targetError, QProgressBar &progBarEpoch){
    if(nets.size() == 1){
        teachOneNetworkFF(*nets.first(), 0, 1, sig, sigClasses, progBarEpoch, targetError);
        emit netTrained();
    }
    else{
        for(int pos = 0; pos < nets.size(); pos++){
            LinearNetwork * linearNetwork = nets[pos];
            teachOneNetworkFF(*linearNetwork, pos, nets.size(), sig, sigClasses, progBarEpoch, targetError);
            emit netTrained();
        }
    }
}


void Teacher::calculateAvarageError(Population &population, int netSize, const LearnVect &AllSignals, const QMap<QString,
                                         int> sigClasses, int netNr){
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
        }
        qDebug() << networkAvaErr;
    }
}

void Teacher::killGivenPercOfPopulation(double SURVIVE_RATE, Population &population){
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


    qDebug() << "val             " << val;
    qDebug() << "population size " << population.size();

    for(NetAndCharacter network : population){
        tmpFit += network.breeadingRate;
        if(val <= tmpFit){
            qDebug() << "Pos             " << pos;
            return population[pos];
        }
        pos++;
    }
}

void Teacher::teachOneNetworkGen(LinearNetwork &receivedNet, int netNr, int netSize, const LearnVect &AllSignals, SigClasses sigClasses, QProgressBar &progBar, double targetError, Topology &topol, Specification &specif){
    /// Net nr. - dla której z kolei sieci przeprowadzamy naukę. netSize - Ile łacznie sieci mamy do nauki. netSize wyznacza sposób
    /// konstrukcji sygnału testowego dla sieci

    int POPULATION_SIZE     = 10;
    double SURVIVE_RATE     = 0.50;
    double MUTATION_RATE    = 0.05;

    // Wygeneruj losową populację sieci dla zadanego problemu.
    Population population;
    for(int indivd = 0; indivd < POPULATION_SIZE - 1; indivd++){
        LinearNetwork * tmp = new LinearNetwork(topol, specif);
        population.push_back({0, 0, tmp});
    }

    // Włączamy otrzymaną sieć do populacji
    population.push_back({0, 0, &receivedNet});

    // Skonstruuj wektor prawidłowych odpowiedzi sieci. Licz usredniony fitness(Error) dla każdej sieci.
    calculateAvarageError(population, netSize, AllSignals, sigClasses, netNr);

    killGivenPercOfPopulation(SURVIVE_RATE, population);
    makeBreedRate(population);
    qSort(population.begin(), population.end(), [](NetAndCharacter a,  NetAndCharacter b)->bool{return a.breeadingRate > b.breeadingRate;});

    qDebug() <<"Posortowany wedle breedRate ";
    for(NetAndCharacter simple : population)
        qDebug() << simple.breeadingRate;
    qDebug() <<"WOLOLO ";

    double currAvError = 1.0;
    /// Zacznij ewoulcję
    do{
        /// Uśmiercam pewien % najgorzej przystosowanych
        killGivenPercOfPopulation(SURVIVE_RATE, population);

        /// Rozmnażanie - domnażamy tyle osobników ile powinno być w populacji
        makeBreedRate(population);

        /// Stwórz kolejną generację
        Population Offspring;
        for(int x = population.size(); x < POPULATION_SIZE; x++){
            // Losuj 2 rodziców, prawdopodobieństwo wylosowania jest proporcjonalne do fitness
            NetAndCharacter * parentOne = 0, * parentTwo = 0;
            parentOne = &findParent(population);
            do{
                parentTwo = &findParent(population);
            }while(parentOne == parentTwo);

            // Zrób dwójke dzieci identyczną jak rodzice - przepisz wartości;
            NetAndCharacter offspringOne(*parentOne), offspringTwo(*parentTwo);

            //Tutaj jest pętla zamian
            for(int zamianaNr = 0; zamianaNr < 100; zamianaNr++){
                // 70% na zamianę wag pomiędzy dziećmi
                    /// W każdej sieci nr tworzonego neuronu nie może odnosić się do wartości static,
                    /// bo wtedy kazda sieć ma inną (kolejną) numeracje


                // 29% na zamiane neuronów
                // 1%  na zamianę warstw
            }

        }
        /// Skonstruuj wektor prawidłowych odpowiedzi sieci. Licz usredniony fitness(Error) dla każdej sieci.
        calculateAvarageError(population, netSize, AllSignals, sigClasses, netNr);
        qSort(population.begin(), population.end(), [](NetAndCharacter a,  NetAndCharacter b)->bool{return a.errorRate < b.errorRate;});
        currAvError = population.front().errorRate;
    }while(currAvError > targetError);

    /// Po okresie nauki, przekształcamy otrzymaną referencję do sieci tak, aby odpowiadała najlepszemu osobnikowi.
    receivedNet = *population.front().network;

    // Rozmnażanie - Operatory krzyżowania
        //      Zamień pojedynczą wagę między neuronem nr x obu rodziców	- 89% szans
        //      Wszystkie wagi neuronu x obu rodziców                       - 10% szans
        //      Wszystkie wagi z danej warstwu L obu rodziców               - 1 % szans


    // Mutacje     - Operatory mutacji [Ustalamy współczynnik mutacji]
        //      zastąp zupełnie nową wartością losową                       - 7%
        //      pomnóż wagę przez liczbę losową z przedziału od 0.1 do 2.0	- 39.5%
        //      dodaj lub odejmij liczbę losową z zakresu 0 - 1 od wagi		- 39.5%
        //      zmień znak wagi                                             - 7%
        //      zamień ze sobą wartości dwóch wag w neuronie                - 7%
}

void Teacher::teachThoseNetworksGen(AllNets &nets, const LearnVect &sig, SigClasses sigClasses,
                                    double targetError, QProgressBar &progBarEpoch, Topology &topol, Specification &specif){
    if(nets.size() == 1){
        teachOneNetworkGen(*nets.first(), 0, 1, sig, sigClasses, progBarEpoch, targetError, topol, specif);
        emit netTrained();
    }
    else{
        for(int pos = 0; pos < nets.size(); pos++){
            LinearNetwork * linearNetwork = nets[pos];
            teachOneNetworkGen(*linearNetwork, pos, nets.size(), sig, sigClasses, progBarEpoch, targetError, topol, specif);
            emit netTrained();
        }
    }
}
