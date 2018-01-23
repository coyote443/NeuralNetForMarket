#ifndef TEACHER_H
#define TEACHER_H

#include "headermain.h"

class Teacher : public QObject
{
    Q_OBJECT
public:
    Teacher(QObject * parent = 0);
    void teachThoseNetworks(QVector<LinearNetwork*> &nets, const LearnVect &sig,
                            const QMap<QString, int> sigClasses, double targetError, QProgressBar &progBar);
signals:
    void nextEpoch();
    void nextOnePercentOfEpoch();
};

#endif // TEACHER_H
