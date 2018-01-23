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


    void on_pushButton_2_clicked();

private:
    void createSpecifViaForm();
    void createTopolViaForm();
    Ui::MainWindow *ui;
    QVector<LinearNetwork*> m_Networks;
    Topology                m_Topology;
    Specification           m_Specifi;
    LearnVect               m_LearnVect;
    QMap<QString, int>      m_LearnClasses;
    Teacher                *m_Teacher;
    int                     m_Epoch             = 0;
    int                     m_EpochProgress     = 0;
    QProgressBar *          m_ProgBar;
    int                     m_NeuronType        = 0;
    int                     m_TeachingSplitType = 0;
    double                  m_MinError          = 0;
    int                     m_NumOfClasses      = 0;
    void setClassesNamesInGui(const QStringList &classes);
    void setInOutSizesInGui(const QStringList &topology);
    void makeClassNamesMap(QStringList classes);
    void resetEpochProgress();
};

#endif // MAINWINDOW_H
