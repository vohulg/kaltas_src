#ifndef TMOBILECOPY_H
#define TMOBILECOPY_H

#include <QThread>
#include <QProcess>
#include <QTime>
#include <QDebug>
#include <TShowtime.h>

#include <stdio.h>

class TMobileCopyInfo: public QThread
{
    Q_OBJECT
public:
    explicit TMobileCopyInfo(QObject *parent = 0);
    
signals:
    void sigShowMsgBoxMobile(const QString &);
    void sigChangeProgressBarValMobile (int currentValue);
    void sigSetServiceInfo(const QString &strInfo);
    
public slots:
    bool initialize(const QString & destFolder, const qlonglong mobileTotalSize, off_t startTime);
    void run();
    void setStopLoop();
    void breakCopy();


private slots:
    bool startLoopInfo();
    qlonglong getSizeDestFolder();
    QString getStringInfo();

private:
     QString DestDirMobile;
     qlonglong MobileTotalSize;
     off_t StartTime;
     bool stopLoop;
     TShowTime* ShowTime;
     bool interruptCopy;



    
};

#endif // TMOBILECOPY_H