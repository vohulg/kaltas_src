#ifndef TDDTHREAD_H
#define TDDTHREAD_H

#include <QThread>
#include <QProcess>
#include <iostream>
#include <QString>

using namespace std;

extern bool StopIndicator;



class TDdThread : public QThread
{
public:
    TDdThread();
    void run();
    qint64 ddPid;
    int ddEnd;

    void setSrcDest( QString ,  QString );

    QString ThreadSrc;
    QString ThreadDest;
private:

};

#endif // TDDTHREAD_H
