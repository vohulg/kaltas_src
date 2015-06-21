#include "TCount.h"

TCount::TCount(QObject *parent) :
    QThread(parent)
{
}

TCount::TCount(time_t _startTime)

{
    StartTime = _startTime;

}

void TCount::run()
{
    time_t nextTime;
    time_t workTime;

    while(1)
    {
        /*

        nextTime = time (&nextTime);

        workTime = nextTime - StartTime;
        if (workTime == 0 || workTime < 0 ) workTime = 1 ;

        off_t copiedByte = mythread->getWrited();

        if (copiedByte == 0) copiedByte = 1;
        off_t lostByte = localDiskSize - copiedByte - offset;
        time_t valLostTime = lostByte / (copiedByte / workTime) ; // ostavsheesa vrema copirovania v secundah

        receiver->setLostTime(valLostTime);



        //***** set persentage


        pt = (mythread->getWrited()+offset)* 100 / localDiskSize;

        ui->progressBar->setValue(pt);


        if ( mythread->getEndCopy() == 2)
        {

            cout << "endcopy mythread->isRunning()  = " << mythread->isRunning() << endl;

            if(mythread->isRunning())
            {

                mythread->wait();
                delete mythread;

            }

            QMessageBox::information(this, tr("Information") ,tr("Копирование успешно завершено"));
            ui->startButton->setEnabled(true);
            ui->stopButton->setEnabled(false);

            // получение серийного номера диска-------

            QString str = "udisks --show-info /dev/" + ui->comboBoxSrc->currentText() + " | sed -n '/drive/,/ATA SMART/p' > " + ui->lineEditdest->text() + ".info";
            int res = system(str.toStdString().c_str());
            if (res > 0)
               QMessageBox::information(this, tr("Information") ,tr("Не удалось получить серийный номер диска"));
             //****** ------серийный номер диска


            return;

        }

        QString res = showTime->getTimestr(receiver->getLostTime());
        QString currTime = showTime->getTimestr(nextTime - startTime);

            sleep(1);
           // ui->labelTime->setText("Осталось: " + res + " \nОбщее время копирования: " + digitToQStr(nextTime - startTime) + " сек");
            ui->labelTime->setText("Осталось: " + res + " \nОбщее время копирования: " + currTime);


*/
    }


}
