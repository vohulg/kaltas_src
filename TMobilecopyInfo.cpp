#include "TMobilecopyInfo.h"

TMobileCopyInfo::TMobileCopyInfo(QObject *parent)

{
}

bool TMobileCopyInfo::initialize(const QString & destFolder, const qlonglong mobileTotalSize, off_t startTime)
{
    DestDirMobile = destFolder;
    MobileTotalSize = mobileTotalSize;
    StartTime = startTime;
    stopLoop = false;
    interruptCopy = false;
    return true;

}

void  TMobileCopyInfo::run()
{
  if (startLoopInfo())
      return;
}


bool TMobileCopyInfo::startLoopInfo()
{
    while (!stopLoop)
    {
        // вычисляем проценты
        int procent = 0;
        qlonglong currentSize = 0;
        try
        {
        currentSize = getSizeDestFolder();

        if (MobileTotalSize == 0) // не получилось узнать размер памяти, поэтому копируем выводя количество скопированных килобайт
        {

           double size = (double)currentSize / 1024 /1024;
            QString str = trUtf8("Скопировано:  ") + QString::number(size) + " Mbyte ";
            emit sigSetServiceInfo(str);
            sleep(1);
            qDebug() << "size of destination folder = " << currentSize ;
            continue;

        }


        procent = currentSize * 100 / MobileTotalSize;
        emit sigChangeProgressBarValMobile(procent);

        // вычисляем оставшееся время
        QString serviceInfo = getStringInfo();
        emit sigSetServiceInfo(serviceInfo);



        }
        catch(...)
        {
            qDebug() << "При вычислении процентов и времени возникло исключение";
            break;
        }


        sleep(1);
        qDebug() << "size of folder = " << currentSize << "procent = " << procent;


    }

    // end of copying
    if (interruptCopy == false)
    {

        double size = (double)getSizeDestFolder() / 1024 /1024;
        QString str = trUtf8("Копирование завершено. Скопировано:  ") + QString::number(size) + " Mbyte ";


     emit sigChangeProgressBarValMobile(100);
     emit sigSetServiceInfo(str);
    }

    //qDebug() << "stopLoop=" << stopLoop;

    interruptCopy = false;

return true;
}

qlonglong TMobileCopyInfo::getSizeDestFolder()
{
 qlonglong folderSize = 0;

 QProcess proc;
 QString cmd = "du";
 QStringList argProc;
 argProc << "-Lcsb" << DestDirMobile  ;
 proc.start(cmd, argProc);

 if (!proc.waitForFinished())
     return false;

 QByteArray res = proc.readLine();
 while (!res.isNull())
 {
     QString strLine = res;
     if (strLine.contains("total"))
     {
         QStringList list = strLine.split("\t");
         folderSize = list.at(0).toLongLong();
         break;
     }

         res = proc.readLine();
 }

return folderSize;
}

void TMobileCopyInfo::setStopLoop()
{
    stopLoop = true;
}

void TMobileCopyInfo::breakCopy()
{
   interruptCopy = true;
   setStopLoop();

}

// вычисление служебной информации: оставшееся время, общее время копирования, сколько скопировано
QString TMobileCopyInfo::getStringInfo()
{

    ShowTime = new TShowTime();
    time_t nextTime = 0;
    time_t workTime = 0;
    QString resultStr = "Идет вычисление";

    nextTime = time (&nextTime);
    workTime = nextTime - StartTime;
    if (workTime == 0)
        workTime = 1;
    qlonglong copiedByte = getSizeDestFolder(); // в килобайтах
    if (copiedByte == 0) copiedByte = 1;
    qlonglong lostKiloByte = MobileTotalSize - copiedByte;

    off_t speed = copiedByte / workTime;
    if (speed <= 0)
        speed = 1;

    time_t valLostTimeMobile = lostKiloByte / speed;
    cout << "valLostTimeMobile=" << valLostTimeMobile ;

    QString currTimeStr = ShowTime->getTimestr(workTime);

    resultStr = "Left: " + ShowTime->getTimestr(valLostTimeMobile) + " \nTotal time of imaging: " + currTimeStr;

    return resultStr;


}

