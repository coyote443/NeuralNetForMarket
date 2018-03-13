#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "headermain.h"
#include "teacher.h"

namespace Ui {
class MainWindow;
}

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
    void on_pushButtonGenerateNetwork_clicked();
    void on_pushButtonStartNetworkLearning_clicked();
    void on_pushButtonTestNetwork_clicked();
    void setEpochOnStatusBar();
    void setEpochProgress();
    void setLrndProgress();
    void on_radioButtonFeedForward_clicked();
    void on_radioButtonGeneticAlg_clicked();
    void on_pushButtonPauseLearn_clicked();

private:
    void createSpecifViaForm();
    void createTopologyViaForm();
    void setClassesNamesInGui(const QStringList &classes);
    void setInOutSizesInGui(const QStringList &topology);
    void makeClassNamesMap(QStringList classes);
    void resetEpochProgress();
    void resetLrndProgress();
    void resetErrProgress();
    void resetEpochOnStatusBar();
    void resetAllProgAndStatus();

    Ui::MainWindow *ui;
    QVector<LinearNetwork*> m_Networks;
    Topology                m_Topology;
    Specification           m_GeneralSpecifi;
    LearnVect               m_LearnVect;
    QMap<QString, int>      m_LearnClasses;
    Teacher                *m_Teacher;
    QStringList             m_Classes;
    ErrorMatrix             m_ErrorMatrix;

    int                     m_EpochCounter      = 0;
    int                     m_EpochProgress     = 0;
    double                  m_LrndNetsProgress  = 0;
    int                     m_LrndCounter       = 0;
    int                     m_NeuronType        = 0;
    int                     m_TeachingSplitType = 0;
    int                     m_NumOfClasses      = 0;
    bool                    m_StopStartSwitch   = true;

    void setNetSpecify(QStringList &NetChar);
    void setNetTopologyForm(QStringList &topology);
    void deleteAndClearNetworks();
    void createLogAndErrMatrix();
};

#endif // MAINWINDOW_H
