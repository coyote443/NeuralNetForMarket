#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_Teacher = new Teacher(this);
    connect(m_Teacher, SIGNAL(nextEpoch()), this, SLOT(setEpochOnStatusBar()));
    connect(m_Teacher, SIGNAL(nextOnePercentOfEpoch()), this, SLOT(setEpochProgress()));
    connect(m_Teacher, SIGNAL(netTrained()), this, SLOT(setLrndProgress()));

    m_ProgBar = ui->progressBarError;

    ui->groupBoxAllNetsControls->setEnabled(false);
    ui->groupBoxButtonsStartAndTest->setEnabled(false);
    ui->groupBoxWayOfNetConstruct->setEnabled(false);
}

MainWindow::~MainWindow(){
    deleteAndClearNetworks();
    delete ui;
}

void MainWindow::deleteAndClearNetworks(){
    for(LinearNetwork *  neur : m_Networks )
        delete neur;
    m_Networks.clear();
}

void MainWindow::on_actionZapisz_Sie_triggered(){
    /// numberOfNetworks << endl
    /// << specif..1 << SEP << specif..n << endl
    /// << topol..1 << SEP << topol..n << endl
    /// << index..1 << SEP << weight..1 << SEP << index..n << SEP << weight..n << endl

    if(m_Networks.empty())
        return;

    QString fileName = QFileDialog::getSaveFileName(this, "Zapisz Sieć / Sieci","", tr("ANN (*.ann);;All Files (*)"));
    QFile file(fileName);
    file.open(QIODevice::WriteOnly | QIODevice::Truncate);

    QTextStream stream(&file);

    stream << m_Networks.size() << endl;

    for(NeuralNetwork * neuralNetwork : m_Networks){
        stream << neuralNetwork->toQString("[::]");
    }
    file.close();
}




void MainWindow::on_actionWczytaj_Sie_triggered(){
    QString fileName = QFileDialog::getOpenFileName(this, "Wczytaj sieć/sieci", "", tr("ANN (*.ann);;All Files (*)"));
    QFile inputFile(fileName);
    if (inputFile.open(QIODevice::ReadOnly)){
        QString SEP = "[::]";
        QTextStream in(&inputFile);

        deleteAndClearNetworks();
        int networksToGnerate = in.readLine().toInt();
        QStringList netChar  = in.readLine().split(SEP);
        setNetSpecify(netChar);
        createSpecifViaForm();      // czy to nie będzie przeszkadzało przy uczeniu?

        QStringList topology = in.readLine().split(SEP);
        setNetTopology(topology);
        createTopologyViaForm();



        // po wczytaniu sieci powinna być mozliwość zmiany jej parametrów. Nie możemy zmieniać ilości warstw ukrytych
        // ponadto po wygenerowaniu nowej sieci, ma być usuwana z pamięci stara, zaś okno parametrów zerowane do stanu bazowego
        // po wczytaniu sieci też ma być wszystko usuwane

        while (!in.atEnd()){
            for(int netNr = 0; netNr < networksToGnerate; netNr++){
                LinearNetwork * tmpNet = new LinearNetwork(m_Topology, m_Specifi);
                m_Networks.push_back(tmpNet);
                for(int size : m_Topology){
                    QStringList neuronConnAndWeights = in.readLine().split(SEP);
                    // dodaj funkcje change connections vals i każdemu neuronowi wysyłaj vektor odpowiedni.



                }
            }


        }
        inputFile.close();
        ui->groupBoxAllNetsControls->setEnabled(true);
        ui->groupBoxTopology->setEnabled(false);
    }
}

void MainWindow::on_actionNowa_Sie_triggered(){
    deleteAndClearNetworks();
    ui->groupBoxAllNetsControls->setEnabled(false);
    ui->groupBoxButtonsStartAndTest->setEnabled(false);
    ui->groupBoxWayOfNetConstruct->setEnabled(true);
}

void MainWindow::on_pushButtonApplyConstruct_clicked(){
    ui->radioButtonNeuronSigmoid->isChecked() ? m_NeuronType = SIGMOID : m_NeuronType = RBF;
    ui->radioButtonOneNetworkForAllClases->isChecked() ? m_TeachingSplitType = ONE_NET : m_TeachingSplitType = NET_PER_SIG;
    ui->groupBoxWayOfNetConstruct->setEnabled(false);
    ui->groupBoxAllNetsControls->setEnabled(true);
}

void MainWindow::setClassesNamesInGui(const QStringList &classes){
    QString lernCls;
    for(QString signClass : classes)
        lernCls += QString("'%1' ").arg(signClass);
    ui->textBrowserSignalClasses->setText(lernCls);
}

void MainWindow::setInOutSizesInGui(const QStringList &topology){
    ui->spinBoxInputLayerSize->setValue( topology[0].toInt() );
    m_NumOfClasses = topology[1].toInt();
    if(m_TeachingSplitType == OneNetwork)
        ui->spinBoxOutputLayerSize->setValue(m_NumOfClasses);
}

void MainWindow::makeClassNamesMap(QStringList classes){
    for(int pos = 0; pos < classes.size(); pos++)
        m_LearnClasses.insert(classes[pos], pos);
}


void MainWindow::on_pushButtonLoadDataset_clicked(){
    resetAllProgAndStatus();

    QString fileName = QFileDialog::getOpenFileName(this, "Otwórz plik z wygenerowaną bazą wektorów uczących",
                                                    "",tr("Signal (*.signal);; All Files (*)"));
    QFile inputFile(fileName);

    if (inputFile.open(QIODevice::ReadOnly)){
        QString SEP = "[::]";

        m_LearnVect.clear();
        m_LearnClasses.clear();

        QTextStream in(&inputFile);
        QStringList topology    = in.readLine().split(SEP);
        QStringList classes     = in.readLine().split(SEP);
        setClassesNamesInGui(classes);
        makeClassNamesMap(classes);
        setInOutSizesInGui(topology);

        while (!in.atEnd()){
            QString       stringClas = in.readLine();
            QStringList   stringVals = in.readLine().split(SEP);

            QVector<double> Vals;
            for(QString v : stringVals){
                Vals.push_back(v.toDouble());
            }

            LearnSig newSig = {stringClas, Vals};
            m_LearnVect.push_back(newSig);
        }
        inputFile.close();
    }
    ui->pushButtonGenerateNetwork->setEnabled(true);
}

void MainWindow::on_pushButtonGenerateNetwork_clicked(){
    resetAllProgAndStatus();
    createSpecifViaForm();
    createTopologyViaForm();
    deleteAndClearNetworks();

    int numberOfNetworks = 0;
    m_TeachingSplitType == 0 ? numberOfNetworks = 1 : numberOfNetworks = m_NumOfClasses;

    for(int net = 0; net < numberOfNetworks; net++){
        LinearNetwork * wsk = new LinearNetwork(m_Topology, m_Specifi);
        m_Networks.push_back(wsk);
    }
    QMessageBox::information(this, "Powodzenie", "Proces generowania zakończony powodzeniem");
    ui->groupBoxButtonsStartAndTest->setEnabled(true);
}

void MainWindow::on_pushButtonStartNetworkLearning_clicked(){
    resetAllProgAndStatus();
    createSpecifViaForm();
    for(LinearNetwork * net : m_Networks){
        net->changeNetSpecification(m_Specifi);
    }

    m_Teacher->teachThoseNetworks(m_Networks, m_LearnVect, m_LearnClasses, m_MinError, *m_ProgBar);
    ui->progressBarNetworkTrained->setValue(100);
}

void MainWindow::on_pushButtonTestNetwork_clicked(){
    resetAllProgAndStatus();
    createSpecifViaForm();
    for(LinearNetwork * net : m_Networks){
        net->changeNetSpecification(m_Specifi);
    }
}


///     TOPOLOGY AND NET SPECIFICATION FORMS


void MainWindow::createSpecifViaForm(){
    m_Specifi.clear();
    m_Specifi.push_back(ui->doubleSpinBoxBETA->value());
    m_Specifi.push_back(ui->doubleSpinBoxETA->value());
    m_Specifi.push_back(ui->doubleSpinBoxALPHA->value());
    m_Specifi.push_back(ui->spinBoxBLURR->value());
    m_Specifi.push_back(ui->checkBoxBIAS->isChecked());
    m_MinError = ui->doubleSpinBoxMIN_ERR->value();
}

void MainWindow::setNetSpecify(QStringList &NetChar){
    ui->doubleSpinBoxBETA->setValue(NetChar[0].toDouble());
    ui->doubleSpinBoxETA->setValue(NetChar[1].toDouble());
    ui->doubleSpinBoxALPHA->setValue(NetChar[2].toDouble());
    ui->spinBoxBLURR->setValue(NetChar[3].toDouble());
    ui->checkBoxBIAS->setChecked(NetChar[4].toInt());
}

// dodaj regex validator dla lineEdit
void MainWindow::createTopologyViaForm(){
    m_Topology.clear();
    m_Topology.push_back(ui->spinBoxInputLayerSize->value());

    QStringList hiddLay = ui->lineEditNumOfHiddLays->text().split(" ");

    for(QString size : hiddLay)
        m_Topology.push_back(size.toInt());

    m_Topology.push_back(ui->spinBoxOutputLayerSize->value());
}

void MainWindow::setNetTopology(QStringList &topology){
    /// If bias is added, we should remove one neuron from input, and from each hidd layer
    /// Those bias neuron will be created during network creating process
    int bias = 0;
    if(ui->checkBoxBIAS->isChecked())
        bias = 1;

    ui->spinBoxOutputLayerSize->setValue( topology.back().toInt() );
    ui->spinBoxInputLayerSize->setValue( topology.first().toInt() - bias);

    QStringList Vals;
    for(int pos = 1; pos < topology.size() - 1; pos++){
        int tmp = topology[pos].toInt() - bias;
        Vals.push_back( QString("%1").arg(tmp) );
    }
    ui->lineEditNumOfHiddLays->setText(Vals.join(" "));
}

///     END TOPOLOGY AND NET SPECIFICATION FORMS



///     PROGRESS AND STATUS BARS

void MainWindow::resetAllProgAndStatus(){
    resetEpochOnStatusBar();
    resetEpochProgress();
    resetLrndProgress();
    resetErrProgress();
}

void MainWindow::resetEpochOnStatusBar(){
    ui->statusBar->showMessage("");
    m_EpochCounter = 1;
}

void MainWindow::setEpochOnStatusBar(){
    m_EpochCounter++;
    QString status = QString("Aktualnie uczona sieć nr. %1\t\t Epoka nr. %2").arg(m_LrndCounter).arg(m_EpochCounter);
    ui->statusBar->showMessage(status);
    resetEpochProgress();
}

void MainWindow::resetErrProgress(){
    ui->progressBarError->setValue(0);
}

void MainWindow::resetEpochProgress(){
    m_EpochProgress = 0;
    ui->progressBarEpoch->setValue(m_EpochProgress);
}

void MainWindow::setEpochProgress(){
    m_EpochProgress++;
    ui->progressBarEpoch->setValue(m_EpochProgress);
}

void MainWindow::resetLrndProgress(){
    m_LrndCounter      = 0;
    m_LrndNetsProgress = 0;
    ui->progressBarNetworkTrained->setValue(m_LrndNetsProgress);
}

void MainWindow::setLrndProgress(){
    m_LrndCounter++;
    m_LrndNetsProgress += 1.0 / m_Networks.size() * 100;
    ui->progressBarNetworkTrained->setValue(m_LrndNetsProgress);
    resetEpochOnStatusBar();
}

///     END PROGRESS AND STATUS BARS
