#ifndef TTEST_H
#define TTEST_H



extern bool StopIndicator;

#include <QThread>
#include <QString>
#include <QFile>

#include <unistd.h>
#include  <fcntl.h>
#include <stdio.h>
#include "QThread"
#include <QTextStream>
#include <cstdlib>
#include <iostream>
#include "sys/types.h"
#include <unistd.h>
#include <sys/stat.h>
#include  <fcntl.h>
#include <string>
#include "TShowtime.h"
using namespace std;

class TShowInfo : public QThread
{
Q_OBJECT

public:
     TShowInfo();
    void run();
     int fd;
    QString currentDiskName;
    void setParam(off_t, off_t);
    
signals:

    void changeVal(int, const QString &);
    void sigShowMsgBox(const QString &);

    
public slots:

private:
    off_t offset;
    off_t srcDiskSize;
    TShowTime* showTime;
    
};


#endif // TTEST_H
