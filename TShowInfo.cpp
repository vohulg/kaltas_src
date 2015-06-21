#include "TShowInfo.h"

extern bool glStopIndicator;
extern off_t glStartTime;
extern off_t glWritedByte;
extern int glEndCopy ;

TShowInfo::TShowInfo()
{
}

void TShowInfo::setParam(off_t _offset, off_t _srcDiskSize)
{
    offset = _offset;
    srcDiskSize = _srcDiskSize;

}

void TShowInfo::run()
{

    // 1. Размер объектового диска srcDiskSize
    // 2. Начальное время копирования off_t glStartTime
    // 3. Объем скопированного        off_t glWritedByte

    showTime = new TShowTime();

    time_t nextTime = 0;
    time_t workTime = 0;

    QString resStr;
    QString currTime;

    glWritedByte = 0;
     int proc = 0; // количество скопированных байтов в процентах



    while(1)
    {

        if (glStopIndicator == true)
            return;

        if (glEndCopy == 1)
            {
                resStr = "Left: 0 sec \nTotal time of imaging: " + currTime;
                emit changeVal(100,resStr );
                return;
            }



        try
        {
            // ********get procent value*********

             proc = (glWritedByte+offset )*100  / srcDiskSize ;

            // ******** get time value ************

            nextTime = time (&nextTime);

            workTime = nextTime - glStartTime;
            if (workTime == 0 || workTime < 0 ) workTime = 1 ;

            off_t copiedByte = glWritedByte;

            if (copiedByte == 0) copiedByte = 1;
            off_t lostByte = srcDiskSize - copiedByte - offset;
            off_t speed = copiedByte / workTime;
            if (speed == 0)
                speed = 1;
            time_t valLostTime = lostByte / speed ; // ostavsheesa vrema copirovania v secundah

            cout << "vallosttime: " << valLostTime << endl;

            currTime = showTime->getTimestr(workTime); // obshee vrema kopirovania

            resStr = "Left: " + showTime->getTimestr(valLostTime) + " \nTotal time of imaging: " + currTime;

        }

        catch(...)
        {
            resStr = "Time calculation.........";

        }



        emit changeVal(proc, resStr);
        sleep(1);
    }


}





