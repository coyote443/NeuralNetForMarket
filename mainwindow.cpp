#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QInputDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->frameLoadGenerate->setEnabled(false);
    ui->groupBoxTopology->setEnabled(false);
    ui->groupBoxSpecifi->setEnabled(false);
    ui->groupBoxButtons->setEnabled(false);
    ui->groupBoxWayOfNetConstruct->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createSpecifViaForm(){
    m_Specifi.clear();
    m_Specifi.push_back(ui->doubleSpinBoxBETA->value());
    m_Specifi.push_back(ui->doubleSpinBoxETA->value());
    m_Specifi.push_back(ui->doubleSpinBoxALPHA->value());
    m_Specifi.push_back(ui->spinBoxBLURR->value());
    m_Specifi.push_back(ui->checkBoxBIAS->isChecked());
    m_MinError = ui->doubleSpinBoxMIN_ERR->value();
}

// dodaj regex validator dla lineEdit
void MainWindow::createTopolViaForm(){
    m_Topology.clear();
    m_Topology.push_back(ui->spinBoxInputLayerSize->value());

    QStringList hiddLay = ui->lineEditNumOfHiddLays->text().split(" ");

    for(QString size : hiddLay)
        m_Topology.push_back(size.toInt());

    m_Topology.push_back(ui->spinBoxOutputLayerSize->value());
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

}

void MainWindow::on_actionNowa_Sie_triggered(){
    ui->groupBoxWayOfNetConstruct->setEnabled(true);
}

void MainWindow::on_pushButtonApplyConstruct_clicked(){
    ui->radioButtonNeuronSigmoid->isChecked() ? m_NeuronType = SIGMOID : m_NeuronType = RBF;
    ui->radioButtonOneNetworkForAllClases->isChecked() ? m_TeachingSplitType = ONE_NET : m_TeachingSplitType = NET_PER_SIG;
    ui->groupBoxWayOfNetConstruct->setEnabled(false);
    ui->groupBoxSpecifi->setEnabled(true);
    ui->frameLoadGenerate->setEnabled(true);
    ui->groupBoxTopology->setEnabled(true);
}

void MainWindow::on_pushButtonLoadDataset_clicked(){
    // jeśli dataset z powodzeniem załadowany, to
    ui->pushButtonGenerateNetwork->setEnabled(true);
}









void MainWindow::on_pushButton_2_clicked() // button do testów
{
    createSpecifViaForm();
    //createTopolViaForm();
    for(int dd : m_Topology)
        qDebug() << dd;
    for(double ddd : m_Specifi)
        qDebug() << ddd;
}
