#ifndef TCOPYTHREAD_H
#define TCOPYTHREAD_H

#include <QThread>

#include <sys/stat.h>
#include "sys/types.h"
#include <unistd.h>
#include  <fcntl.h>
#include <string>
#include <stdio.h>
#include <QMessageBox>
#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <QString>
#include <errno.h>
#include <QTextStream>
#include <QFile>

class TCopyThread : public QThread
{
    Q_OBJECT
public:
    explicit TCopyThread(QObject *parent = 0);
    
signals:

    
public slots:
    
};

#endif // TCOPYTHREAD_H
