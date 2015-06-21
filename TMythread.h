#ifndef TMYTHREAD_H
#define TMYTHREAD_H

#include <QThread>
#include <sys/stat.h>
#include "sys/types.h"
#include <unistd.h>
#include  <fcntl.h>
#include <string>
#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <QString>
#include <errno.h>
#include <QTextStream>
#include <QFile>
#include <QStringList>



#define BUFSIZE 512*4


class TMyThread: public QThread
{
   Q_OBJECT

public:
    explicit TMyThread();
    void setParam(off_t , std::string, std::string );
    void run();
    time_t getStartTime();

    void setEndCopy(int);
    int checkDiskConnect();
    void parseLine(QString);

signals:
    void sigShowMsgBox2(const QString &);
    void sigChangeButton();



private:
    off_t offset;
    std::string destFile;
    std::string srcFile;
    time_t startTime;
    off_t writed ;
    QString disk;


};

#endif // TMYTHREAD_H
