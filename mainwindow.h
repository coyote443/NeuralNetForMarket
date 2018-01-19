#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "neuralnetwork.h"

namespace Ui {
class MainWindow;
}

enum nType{SIGMOID = 0, RBF = 1};
enum lern{ONE_NET = 0, NET_PER_SIG = 1};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionZapisz_Sie_triggered();
    void on_actionWczytaj_Sie_triggered();
    void on_actionNowa_Sie_triggered();
    void on_pushButtonApplyConstruct_clicked();
    void on_pushButtonLoadDataset_clicked();

    void on_pushButton_2_clicked();

private:
    void createSpecifViaForm();
    void createTopolViaForm();
    Ui::MainWindow *ui;
    QVector<NeuralNetwork*> m_Networks;
    Topology                m_Topology;
    Specification           m_Specifi;
    int                     m_NeuronType        = 0;
    int                     m_TeachingSplitType = 0;
    double                  m_MinError          = 0;
};

#endif // MAINWINDOW_H
