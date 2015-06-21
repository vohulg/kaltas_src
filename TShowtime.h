#ifndef TSHOWTIME_H
#define TSHOWTIME_H

#include <QTextStream>
#include <sys/time.h>
#include <QString>
#include <iostream>
#include "QThread"



using namespace std ;

class TShowTime : QThread
{
public:
    TShowTime();
    QString getTimestr(time_t lostT);


private:

    QString strLostTime;

    time_t daySecond ;  //86400
    char daySecondChar[15];
    QString daySecondQ;
    QString day ;

    time_t hourSecond ;  //3600
    char hourSecondChar[15];
    QString hourSecondQ;
    QString hour ;

    time_t minutSecond ; //60
    char minutSecondChar[15];
    QString minutSecondQ;
    QString minut ;

    time_t second;
    char secondChar[15];
    QString secondQ;
    QString Second ;


    time_t lostClock;

    time_t temp;

};

#endif // TSHOWTIME_H
