#include "TMythread.h"

extern bool glStopIndicator;
extern off_t glStartTime;
extern off_t glWritedByte;
extern int glEndCopy ;


TMyThread::TMyThread()
{
    writed = 0;
    glStopIndicator = false;
    startTime = 0;

}



void TMyThread::setParam(off_t _offset, std::string _destFile, std::string _srcFile)
{
    offset = _offset;
    srcFile = _srcFile;
    destFile = _destFile;
}

void TMyThread::setEndCopy(int _endCopy)
{
    glStopIndicator = _endCopy;

}

void TMyThread::run()
{
     glEndCopy = 0;

    int srcFd = open(srcFile.c_str(), O_RDONLY | O_NOATIME);
    if (srcFd == -1)
    {
        emit sigShowMsgBox2("Невозможно открыть диск источник");
        return;

    }

    int destFd = open(destFile.c_str(), O_WRONLY | O_NOATIME);
    if (destFd == -1)
    {
        emit sigShowMsgBox2("Невозможно создать файл для записи");
        return;

    }

    if (offset != 0)
    {
        lseek(destFd, offset, SEEK_SET);
        lseek (srcFd, offset, SEEK_SET);
    }


    char buffer[BUFSIZE];
    size_t bytesRead = 0;

    char nullBuf[BUFSIZE] = {0};

    off_t wrSize = 0;
    writed = 0;


//-----------osnovnoy zikl copirovania---------------------------


    while(true)
    {
        //-----check press button Stop----------------
        if (glStopIndicator == true)
            {
                emit sigShowMsgBox2("Imaging interrupt");
                return;
            }
        //---------------

        bytesRead = read(srcFd, buffer, sizeof(buffer));

        if (bytesRead == 0)
            {
                ::close(srcFd);
                ::close(destFd);
                glEndCopy = 1; // coping secuessfull ended
                emit sigChangeButton();
                emit sigShowMsgBox2("Imaging secuessfull ended");

                // ------получение серийного номера диска-------

                QString str = "udisks --show-info " + (QString)srcFile.c_str() + " | sed -n '/drive/,/ATA SMART/p' > " + (QString)destFile.c_str() + ".info";
                int res = system(str.toStdString().c_str());
                if (res > 0)
                   emit sigShowMsgBox2("Не удалось получить серийный номер диска");
                 // ------серийный номер диска----------


                return;

            }

        //-----------проверка на ошибку чтения---------

        if (bytesRead == -1 && errno == EIO)
        {

           // -----check disk connect---------
            if (checkDiskConnect() == -1)
            {

                ::close(srcFd);
                ::close(destFd);
                emit sigShowMsgBox2("Диск не вставлен в компьютер. Копирование прервано.");
                glStopIndicator = true;

                return;

            }
            //--------------------------

            wrSize = write(destFd, nullBuf, sizeof(nullBuf));
            continue;

        }

        //-----------проверка на друие ошибки при чтении----------------------

       if (bytesRead == -1)
       {

           glStopIndicator = true;
           ::close(srcFd);
           ::close(destFd);
           emit sigShowMsgBox2("Возникла ошибка при чтении. Копирование прервано");
           return;

       }

       //-------------------------------------------------


        wrSize = write(destFd, buffer, sizeof(buffer));


        writed += wrSize;
        glWritedByte = writed;

    }




    //---close descriptor of files of destination ans source

        ::close(srcFd);
        ::close(destFd);

    return;

}

time_t TMyThread::getStartTime()
{
    return startTime;

}



int TMyThread::checkDiskConnect()
{

    disk = "nodisk";

        QFile file("/proc/partitions");
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return -1;

        QTextStream in(&file);
        QString line = in.readLine();
        line = in.readLine();
        line = in.readLine();
        int cnt = 0;
        while (!line.isNull())
        {
            parseLine(line);
            line = in.readLine();
            cnt++;
        }

        if (disk == "nodisk")
           return -1; // no disk insert in computer

        return 0; // disk insert in computer ant have bad sector



}

void TMyThread::parseLine(QString line)
{
    QStringList list;
    list = line.split(QRegExp("\\s+"));
    QString localDiskItem = list[list.count()-1];

    if (srcFile == localDiskItem.toStdString())
        disk = localDiskItem;

    return;

}





