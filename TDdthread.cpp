#include "TDdthread.h"

TDdThread::TDdThread()
{
    ddEnd = 0;
}

void TDdThread::run ()
{
    QProcess* gzip = new QProcess(this);
    QString pGzip = "gzip";
    QStringList argGzip;
    gzip->setStandardOutputFile(ThreadDest);
    argGzip << "-c" ;

    QProcess *ddProc = new QProcess(this);
    QString program = "dd";
    QStringList arguments;
    QString ifArg = "if=/dev/"+ ThreadSrc;
    arguments << ifArg << "bs=8M";
    ddProc->setStandardOutputProcess(gzip);
    ddProc->setStandardErrorFile("/home/pipe");

     ddProc->start(program, arguments);
     if (!ddProc->waitForStarted())
         cout << "dd is not started" << endl;

     ddPid = ddProc->pid();

     gzip->start(pGzip, argGzip);
     if (!gzip->waitForStarted())
         cout << "gzip is not started" << endl;

     cout << "ddProc->pid()=" << ddProc->pid() << endl ;

     if (!gzip->waitForFinished(-1))
         cout << "gzip is not correct finished" << endl;

     ddEnd = 1;
     //glStopIndicator = true;
     return ;

}

void TDdThread::setSrcDest(QString _src ,  QString _dest)
{
        ThreadSrc = _src;
        ThreadDest = _dest;

}


