#include "ddmainwindow.h"
#include "ui_ddmainwindow.h"

#define ENG 1

extern bool glStopIndicator; // индикатор для определения нажатия кнопки стоп - прерывания процесса копирования
extern off_t glStartTime; // хранит общее время копирования
extern off_t glWritedByte; // хранит объем записанных байтов
extern int glEndCopy ;   // индикация окончания копирования

// сообщения тексты

#ifdef ENG

QString sFileForWrite = "Choose file for writing";
QString sError = "Error";
QString sSysytemPart = "Sysytem part is not imaging";
QString sOpenDisk = "Not avaible open disk";
QString sCreateFile = "Not avaible create file";
QString sOpenFile = "Not avaible open file";
QString sProlongCopy = "Prolong copy ?";
QString sChooseFile = "Choose existing file !";
QString sUpdated = "Updated";
QString sUpdate = "Update";
QString sNotChooseFile = "Not choosed file or directory";
QString sNotMountImage = "Image not mounted";
QString sNotFindFileManager = "Image mounted but fileManager not finded";
QString sImageFolder = "Image mounted in folder ";
#else

QString sFileForWrite = "Выберите файл для записи";
QString sError = "Ошибка";
QString sSysytemPart = "Невозможно копировать системный раздел";
QString sOpenDisk = "Невозможно открыть диск источник";
QString sCreateFile = "Невозможно создать файл для запис";
QString sOpenFile = "Невозможно открыть файл для записи";
QString sProlongCopy = "Продолжить копирование ?";
QString sChooseFile = "Выберите существующий файл";
QString sUpdated = "Обновлено";
QString sUpdate = "Обновить";
QString sNotChooseFile = "Не выбран образ или директория назначения";
QString sNotMountImage = "Не удалось примонтировать образ";
QString sNotFindFileManager = "Образ примонтирован однако файловый менеджер для просмотра файлов не удалось запустить";
QString sImageFolder = "Образ примонтирован в папку ";
#endif


ddMainWindow::ddMainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::ddMainWindow)
{
    ui->setupUi(this);
    initializing();


}


// первоначальная инициализация
 bool ddMainWindow::initializing()
 {
     // даем значения папкам мобильного телефона
     dataFolder = "/data";
      sdcardFolder = "/sdcard";

      mobileCopyInfo = NULL;

      stopMobileCopy = false;

     auto_mounting(); // автоматическое монтирование
     slotFillTable(); // заполнение таблицы дисков

     showInfo = new TShowInfo();
     mythread = new TMyThread();
     mnt = new TMount();

     QObject::connect(showInfo, SIGNAL(changeVal(int, const QString &)), SLOT(funcShowInfo(int, const QString &)), Qt::QueuedConnection);
     QObject::connect(showInfo, SIGNAL(sigShowMsgBox(QString)), SLOT(ShowMsgBox(QString)), Qt::QueuedConnection);
     QObject::connect(mythread, SIGNAL(sigShowMsgBox2(const QString &)), SLOT(ShowMsgBox2(const QString &)), Qt::QueuedConnection);
     QObject::connect(mythread, SIGNAL(sigChangeButton()), SLOT(ChangeButton()), Qt::QueuedConnection);
     QObject::connect(ui->tableWidget, SIGNAL(cellClicked(int,int)), SLOT(showDiskInCombobox(int, int)));

     connect (&timer, SIGNAL (timeout()), &loop, SLOT(quit()));

     prevCol = -1;
     prevRow = -1;

     return true;
 }

bool ddMainWindow::auto_mounting() //-- run automat mounting
{


    QFile udevfile("/etc/udev/rules.d/automount.rules");
        if (!udevfile.exists())
        {
            udevfile.open(QIODevice::WriteOnly | QIODevice::Text);
            QTextStream out(&udevfile);
            out << "ACTION==\"add\" KERNEL==\"sd[b-z][0-9]\" RUN+=\"/bin/mkdir -p /mnt/%k\"\n";
            out << "ACTION==\"add\" KERNEL==\"sd[b-z][0-9]\" RUN+=\"/bin/mount -o uid=1000 /dev/%k /mnt/%k\"\n";
            out << "ACTION==\"remove\" KERNEL==\"sd[b-z][0-9]\" RUN+=\"/bin/rmdir /mnt/%k\"";
            udevfile.close();

            int res = system("udevadm control --reload-rules");
            if(res != 0)
              QMessageBox::information(this, tr("Warning") ,tr("Autoount of falsh card and extern HDD is not avalible"));


        }


    return true;
}

ddMainWindow::~ddMainWindow()
{
    delete ui;
}

void ddMainWindow::slotFillTable()
{
    localDisksList.clear();
    localDisksSize.clear();
    localDiskMountPointList.clear();
    localDiskVendor.clear();

    ui->tableWidget->setRowCount(0);
    ui->comboBoxSrc->clear();

    getLocalDisks();
    getMountPoint();
    getVenorDisks();

    ui->tableWidget->setRowCount(localDisksList.count());

    QStringList list;
    list << "Disk name" << "Size(Gb)" << "Mount point" << "Vendor";

    ui->tableWidget->setHorizontalHeaderLabels(list);

    for(int i=0; i < 4; i++)
        ui->tableWidget->setColumnWidth(i,170);




    for (int row=0; row < localDisksList.count(); row++)
    {
        QTableWidgetItem *itemDisk = new QTableWidgetItem(localDisksList[row].toUpper());
        itemDisk->setTextAlignment(Qt::AlignCenter);
        //itemDisk->setBackgroundColor(QColor(Qt::red));
        ui->tableWidget->setItem(row,0,itemDisk);
        itemDisk->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);

        QTableWidgetItem *itemSize = new QTableWidgetItem((localDisksSize[row]+" Gb "));
        itemSize->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(row,1,itemSize);
        itemSize->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);


        QTableWidgetItem *itemMountPoint = new QTableWidgetItem(localDiskMountPointList[row]);
        itemMountPoint->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(row,2,itemMountPoint);
         itemMountPoint->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);

         QTableWidgetItem *itemVendor = new QTableWidgetItem(localDiskVendor[row]);
         itemVendor->setTextAlignment(Qt::AlignCenter);
         ui->tableWidget->setItem(row,3,itemVendor);
          itemVendor->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);



    }

    ui->comboBoxSrc->addItems(localDisksList);

}


void ddMainWindow::on_ddStartButton_clicked()
{

    glEndCopy = 0 ;
    glStopIndicator = false;

    string src;
    string dest;
    src = "/dev/" + ui->comboBoxSrc->currentText().toStdString();
    dest = ui->lineEditdest->text().toStdString();

    //-----------------------------------------



    if( ui->lineEditdest->text().isEmpty() )
    {
        QMessageBox::warning(this, sError , sFileForWrite);
        return;

    }

    //--------check not copy SYSTEM disks------------

    if ( localDiskMountPointList[ui->comboBoxSrc->currentIndex()] == "SYSTEM")
        {
           QMessageBox::warning(this, sError ,sSysytemPart);
           return;
        }

    //********* get local disk size

    int fd = open(src.c_str(), O_RDONLY | O_NOATIME );
    off_t localDiskSize = 0;
    if (fd == -1)
        localDiskSize = 0;
    else
    {
     localDiskSize = lseek (fd, 0, SEEK_END);

     if (localDiskSize == 0)
     {
         QMessageBox::warning(this, sError , sOpenDisk);
         return;
     }

     ::close(fd);
    }

    //*********************** check free space
/*
    if (getFreeSpace(ui->lineEditdest->text()) < localDiskSize)
    {

        QString freeSpace = "Недостаточно свободного места на диске";
        off_t fFree = getFreeSpace(ui->lineEditdest->text());

        if (fFree > 1024*1024*1024)
                   freeSpace = "Недостаточно свободного места на диске.Свободно " + digitToQStr(fFree/1024/1024/1024) + "Gb";
        else if (fFree > 1024*1024 && fFree < 1024*1024*1024)
            freeSpace = "Недостаточно свободного места на диске.Свободно " + digitToQStr(fFree/1024/1024) + "Mb";
        else if (fFree > 1024 && fFree < 1024*1024)
            freeSpace = "Недостаточно свободного места на диске.Свободно " + digitToQStr(fFree/1024) + "Kb";
        else if (fFree > 0 && fFree < 1024)
            freeSpace = "Недостаточно свободного места на диске.Свободно " + digitToQStr(fFree) + "byte";
        else
            QMessageBox::warning(this, tr("Ошибка"), freeSpace);
            //return;

    }

    */

    //********** check and set offset for local disk****************

      int destFd; //
      off_t offset = 0;//

      if (localDiskOffset == false)
      {
          destFd = open(dest.c_str(), O_WRONLY | O_NOATIME  );
          if (destFd == -1)
          {
              QMessageBox::warning(this, sError ,sCreateFile);
              return;

          }
          ::close (destFd);
      }

      else
      {
          destFd = open(dest.c_str(), O_WRONLY | O_NOATIME);
          if (destFd == -1)
          {
              QMessageBox::warning(this, sError ,sOpenFile);
              return;

          }
          offset = lseek(destFd, 0, SEEK_END);
          ::close (destFd);
      }


      //******************************************************************

      showInfo->currentDiskName = ui->comboBoxSrc->currentText();
      ui->ddStartButton->setEnabled(false);
      ui->ddStopButton->setEnabled(true);

       // запускаем процесс копирования
      mythread->setParam(offset, dest, src);
       mythread->start();


       // вычисляем время старнта процесса копирования
      time_t startTime;
      startTime = time (&startTime);
      glStartTime = startTime;


      // запускаем процесс для вычисления скорости копирования, оставшегося объема и времени
      showInfo->setParam(offset, localDiskSize);
      showInfo->start();

     return;

}

void ddMainWindow::getLocalDisks()
{

    localDisksList.clear();
    QFile file("/proc/partitions");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

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

    getDisksSize();

 }

 void ddMainWindow::getDisksSize()
 {

    //ui->comboBoxSrc->addItem(tr("Выберите локальный диск"));
    localDisksSize.clear();

    // создаем и обнуляем значения для всех дисков
    for (int i=0; i < localDisksList.count();i++)
            localDisksSize << "0";


    for (int i=0; i < localDisksList.count();i++)
     {

        QFile file("/dev/"+localDisksList[i]);

        if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
            continue;

         off_t diskSize = 0;
         int fd;
         const char* disk = ("/dev/" + localDisksList[i]).toStdString().c_str();
         fd = open(disk, O_RDONLY | O_NOATIME);
         if (fd == -1)
             diskSize = 0;
         else
         {
          diskSize = lseek (fd, 0, SEEK_END);
          ::close(fd);
         }

        double dSize = (double)diskSize/1000/1000/1000;
        char* szStr = new char[33];

        snprintf(szStr, sizeof(szStr),"%.1f",dSize);

        localDisksSize[i] = (QString)szStr;
     }

     //ui->comboBoxSrc->addItems(localDisksList);
 }

void ddMainWindow::parseLine(QString line)
{

    QStringList list;
    list = line.split(QRegExp("\\s+"));
    QString localDiskItem = list[list.count()-1];

    localDisksList << localDiskItem;

    return;

}

//-------------------------------------------------------

void ddMainWindow::getVenorDisks()
{
    for (int j=0; j < localDisksList.count(); j++)
        localDiskVendor << "";


    QString tmpFile = "/tmp/out";

    for (int j=0; j < localDisksList.count(); j++)
    {
        QString cmd = "udisks --show-info /dev/" + localDisksList[j] + " | sed -n '/vendor/,/model/p' > " + tmpFile;


        int res = system (cmd.toStdString().c_str());
        if(res != 0 )
        {
            cout << "vendor not finded" << endl;
            localDiskVendor[j] = "not defined";
            continue;

        }


        QFile file(tmpFile);
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return;

        QStringList tmpList;
        QString tmpStr;

        QTextStream in(&file);

        tmpStr = in.readLine();


        if(tmpStr.isNull())
        {
            localDiskVendor[j] = " ";
            continue;
        }

        tmpList = tmpStr.split(QRegExp("\\s+"));
        localDiskVendor[j] = tmpList[2];




        }



        //---------------------------------------------


    }
         //QString str = "udisks --show-info " + (QString)srcFile.c_str() + " | sed -n '/drive/,/ATA SMART/p' > " + (QString)destFile.c_str() + ".info";

void ddMainWindow::getMountPoint()
{
    for (int j=0; j < localDisksList.count(); j++)
        localDiskMountPointList << "";

    QFile file("/etc/mtab");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    QString line;
    QStringList list;

    QString localDiskItem;
    line = in.readLine();

    while (!line.isNull())
    {

        list = line.split(QRegExp("\\s+"));
        localDiskItem = list[0];
        for (int i = 0; i < localDisksList.count(); i++)
        {
            if (localDiskItem == ("/dev/"+localDisksList[i]))
            {
                if (list[1]=="/")
                    list[1] = "SYSTEM";
                localDiskMountPointList[i] = list[1];
            }

        }
        line = in.readLine();


    }


}

//------------------------------------

void ddMainWindow::SaveDestFile()
{

    QString filter = "Any files (*)";
    QString* selecFilter = &filter;
    QString source = QFileDialog::getSaveFileName(this, tr("Save file"), tr("/home"), filter, selecFilter, QFileDialog::DontConfirmOverwrite );
    int res = QMessageBox::warning(this, tr("Reading"), sProlongCopy, QMessageBox::No | QMessageBox::Default,
                                 QMessageBox::Yes, QMessageBox::Cancel | QMessageBox::Escape);
    QFile file(source);

        switch (res)
        {
            case QMessageBox::No:
                localDiskOffset = false;
                ui->lineEditdest->setText(source);
                if (file.exists())
                    file.remove();
                    file.open(QIODevice::WriteOnly);
                    file.close();
                break;

            case QMessageBox::Yes:
                if (!file.exists())
                {
                    QMessageBox::information(this, sError ,sChooseFile);
                    break;

                }
                localDiskOffset = true;
                ui->lineEditdest->setText(source);
                break;

            default:
                break;
        }


        //ddThread->setSrcDest(ui->comboBoxSrc->currentText(), ui->lineEditdest->text());

     return;
 }

//-------------

void ddMainWindow::ExitApp()
{
    QCoreApplication::exit();
}

//----------------

void ddMainWindow::ddStop()
{
    ui->ddStartButton->setEnabled(true);
    ui->ddStopButton->setEnabled(false);
    ui->lineEditdest->setText("");
    glStopIndicator = true;
    glWritedByte = 0;
    glEndCopy = 0 ;



}

void ddMainWindow::funcShowInfo(int progressVal, const QString &time)
{
    ui->progressBar->setValue(progressVal);
    ui->labelInfo->setText(time);


}

void ddMainWindow::ShowMsgBox(const QString &str)
{

    QMessageBox::information(this, tr("Info") ,str);

}

void ddMainWindow::ShowMsgBox2(const QString &str)
{

    QMessageBox::information(this, tr("Info") ,str);

}

void ddMainWindow::ShowMsgBoxMount(const QString &str)
{

    QMessageBox::information(this, tr("Error") ,str);

}

//****** get freee space of disk

off_t ddMainWindow::getFreeSpace(const QString& sDirPath)
{

    //int byte = sizeof(off_t);

    struct statvfs stat;

    int res = statvfs(sDirPath.toStdString().c_str(), &stat);

    if (res == 0 && stat.f_fsid != 0)
            return stat.f_bfree * stat.f_bsize; //svobodnoe mesto v byte

    else return -1;

}

QString ddMainWindow::digitToQStr (off_t digit)
{
    char valChar[15];
    sprintf (valChar, "%ld", digit);
    return (QString)valChar;

}


void ddMainWindow::updateDisks()
{
    slotFillTable();

    timer.start(3000);
    ui->fillTable->setText(sUpdated);
    ui->fillTable->setEnabled(false);
    loop.exec();

    timer.start(3000);
    ui->fillTable->setText(sUpdate);
    ui->fillTable->setEnabled(true);
    loop.exec();

}

void ddMainWindow::ChangeButton()
{
    ui->ddStartButton->setEnabled(true);
    ui->ddStopButton->setEnabled(false);

}

void ddMainWindow::showDiskInCombobox(int row, int col)
{
    if (col != 0)
        return;

    ui->comboBoxSrc->setCurrentIndex(row);
    QColor color(Qt::gray);
    QColor defaultColor(Qt::white);

    for (int i=0; i < 4; i++)
        ui->tableWidget->item(row,col+i)->setBackgroundColor(color);

    if(prevRow != -1)
    {
        for (int i=0; i < 4; i++)
            ui->tableWidget->item(prevRow,prevCol+i)->setBackgroundColor(defaultColor);

    }

    prevCol = col;
    prevRow = row;



}



void ddMainWindow::on_ButtonMountSrc_clicked()
{
    QString filter = "Any files (*)";
    QString* selecFilter = &filter;
    QString source = QFileDialog::getOpenFileName(this, tr("Open image file"), tr("/"), filter, selecFilter, QFileDialog::DontConfirmOverwrite );
    ui->lineEditMountSrc->setText(source);

}

void ddMainWindow::on_ButtonMountDest_clicked()
{

    QString destDir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                        "/home",
                                                        QFileDialog::ShowDirsOnly
                                                        | QFileDialog::DontResolveSymlinks);
    ui->lineEditMountDest->setText(destDir);

}

void ddMainWindow::on_ButtonMount_clicked()
{

    if (ui->lineEditMountSrc->text() == "" || ui->lineEditMountDest->text() == "")
    {

      QMessageBox::information(this, tr("Error") ,sNotChooseFile);
      return;
    }



    std::string iso = ui->lineEditMountSrc->text().toStdString();
    std::string destDir = ui->lineEditMountDest->text().toStdString();

    timer.start(8000);
    ui->ButtonMount->setText("Wait.....");
    ui->ButtonMount->setEnabled(false);
    loop.exec();




    int res = mnt->info(iso, destDir);

    if (mnt)
        delete mnt;


    switch (res) {

    case -1:
        QMessageBox::information(this, sError ,sNotMountImage);
        break;

    case -2:
        QMessageBox::information(this, sError , sNotFindFileManager);
        break;
    default:
        QString message = sImageFolder + ui->lineEditMountDest->text();
        QMessageBox::information(this, tr("Info") , message);



        break;
    }

    timer.start(3000);
    ui->ButtonMount->setText("Mount");
    ui->ButtonMount->setEnabled(true);
    loop.exec();

    return;

}

void ddMainWindow::on_pushButton_clicked()
{

    helpWin.show();
    //Dialog dialog;
     // dialog.show();

    //QWidget widget = new QWidget(this);
    //widget.setWindowFlags(Qt::Window);
    //QMessageBox::information(widget, tr("Info") ,tr("Инструкция по использованию Калтас"));
}


// получение списка устройств подключенных к компьютеру
void ddMainWindow::on_widButUpdateMobileDevice_clicked()
{
    ui->widComboMobileDevice->clear();

    QProcess* adb = new QProcess(this);
    QString cmd = "adb";
    QStringList argAdb;
    argAdb << "devices";


    adb->start(cmd, argAdb);

    if (!adb->waitForFinished())
        return ;

    QByteArray devices = adb->readLine();
    devices = adb->readLine();

    qDebug() << "devices:" << devices;

    if (devices.startsWith("* daemon started successfully"))
    {
        adb->close();
        delete adb;
        on_widButUpdateMobileDevice_clicked();
        return;

    }

    QString strDevices = devices;

    QStringList list = strDevices.split("\t");

    if (list.at(0).toLatin1().startsWith("\n"))
    {
      // устройство не подключено
        QMessageBox::information(this, tr("Info") ,tr("Mobile device not connected"));
        return;
    }

    int lastIndex = list.size() - 1;
    if (list.at(lastIndex).toLatin1().startsWith("offline\n"))
    {
      // устройство подключено но находится в offline режиме
        QMessageBox::information(this, tr("Info") ,tr("Mobile device connectrd in offline mode. Reconnect to PC manual"));
        return;
    }

   ui->widComboMobileDevice->clear();
    ui->widComboMobileDevice->addItem(list.at(0));

    // плдучаем байтах размер папки /data
    qlonglong dataSize = getSizeMobileFolder(dataFolder);

    // получаем в байтах размер папки sdcard

    qlonglong sdcardSize = getSizeMobileFolder(sdcardFolder);

    // выводим информацию пользователю о размерах файлов

    totalMobileSize = sdcardSize + dataSize;

    double doublTotalSize =  (double)totalMobileSize / 1024 /1024;

    if (doublTotalSize > 0) // выводим размер папок только если удалось определить размер с помощью утилит du или df
    {
         QString info = "Mobile size " + QString::number(doublTotalSize) + " Gbyte";
         ui->widLabelSizeMobileInfo->setText(info );
    }

}
// выбор директории куда будет сохраняться данные с мобильного телефона

void ddMainWindow::on_widButChooseDir_clicked()
{

    destDirMobile = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                        "/home",
                                                        QFileDialog::ShowDirsOnly
                                                        | QFileDialog::DontResolveSymlinks);
    ui->widLineDestDirMobile->setText(destDirMobile);

}


// Запуск процесса копирования папок мобильного телефона /data и /sdcard в папку назначения
void ddMainWindow::on_widButStartMobileImage_clicked()
{
    // инициализация данных
    initialBeforStartMobile();

    // создаем процесс для запуска копирования папки /data
    dataCopyProc = new QProcess(this);
    QObject::connect(dataCopyProc, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(dataCopyFinished(int, QProcess::ExitStatus)));
    QObject::connect(dataCopyProc, SIGNAL(started()), SLOT(dataCopyStarted()));
    startMobileFolderCopy(dataFolder, (ui->widLineDestDirMobile->text() + dataFolder), dataCopyProc );

}

qlonglong ddMainWindow::getSizeMobileFolder(const QString &folder)
{
  qlonglong folderSize = 0;

  QProcess* adb = new QProcess(this);
  QString cmd = "adb";
  QStringList argAdb;
  argAdb << "shell" << "du" <<"-Lcs" << folder;
  adb->start(cmd, argAdb);
  if (!adb->waitForFinished())
      return 0;

  QByteArray res = adb->readLine();
  while (!res.isNull())
  {
      QString strLine = res;
      if (strLine.contains("total"))
      {
          QStringList list = strLine.split("\t");
          folderSize = list.at(0).toLongLong();
          break;
      }

          res = adb->readLine();
  }

  return folderSize;

}



void ddMainWindow::on_widButStopMobileImage_clicked()
{
    stopMobileCopy = true;

    QProcess* adb = new QProcess(this);
    QString cmd = "killall";
    QStringList argAdb;
    argAdb << "adb"  ;
    adb->start(cmd, argAdb);
    if (!adb->waitForFinished())
        return ;

    if (mobileCopyInfo != NULL)
    {
        //mobileCopyInfo->setStopLoop();
        mobileCopyInfo->breakCopy();
    }




}

 void ddMainWindow::dataCopyFinished (int code, QProcess::ExitStatus exitStatus)
 {
     qDebug() << "coping of DATA FOLDER pid process" << dataCopyProc->pid() <<"  finished;"  << "exitStatus=" << exitStatus << "exitCode=" << code ;
     dataCopyProc->close();

       if (dataCopyProc)
           delete dataCopyProc;


       // проверяем была ли нажата кнопка STOP.
       // если кнопка нажата то второй процесс не запускаем

       if (!stopMobileCopy)
       {
           //============запуск второго процесса копирования папки /sdcard====================
           sdcardCopyProc = new QProcess (this);
           QObject::connect(sdcardCopyProc, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(sdcardCopyFinished(int, QProcess::ExitStatus)));

            // получение полного пути для sdcard
           // if (!getPathSdcardFromLink().isEmpty())
              //  sdcardFolder = getPathSdcardFromLink();

            // получение директории с внешней карточки
            if (!getSdcardPath().isEmpty())
                sdcardFolder = getSdcardPath();


            startMobileFolderCopy(sdcardFolder, (ui->widLineDestDirMobile->text() + sdcardFolder), sdcardCopyProc );

       }

       else // значит копирование полностью завершено
          initialAfterFinishMobile();

 }

 void ddMainWindow::sdcardCopyFinished (int code, QProcess::ExitStatus exitStatus)
 {
     // приводим кнопки и переменные к состоянию до старта процесса копирования
     initialAfterFinishMobile();

     qDebug() << "sdcard coping  process finished" << "exitStatus=" << exitStatus << "exitCode=" << code ;

     sdcardCopyProc->close();

       if (sdcardCopyProc)
           delete sdcardCopyProc;

       // останавливаем вывод информации и убиваем объект отвечающий за вывод информации
       if (mobileCopyInfo != NULL)
       {
           mobileCopyInfo->setStopLoop();
           //mobileCopyInfo->exit();
           //mobileCopyInfo = NULL;
           qDebug() << "mobileCopyInfo is Running =" << mobileCopyInfo->isRunning();
       }
 }


 // запуск процесса копирования папки мобильного телефона
  bool ddMainWindow::startMobileFolderCopy(const QString &folderSrc, const QString &folderDest, QProcess* proc )
  {
      QString cmd = "adb";
      QStringList argAdb;
      argAdb << "pull" << folderSrc << folderDest ;
      proc->start(cmd, argAdb);
      qDebug() <<  " pid =" << proc->pid() << " process started"  ;
      if (!proc->waitForFinished())
          return false;

   return true;
  }

  QString ddMainWindow::getEnvPath(int type)
  {
      QString env = "";

      switch (type)
      {
        case eSDCARD: env = "$EXTERNAL_STORAGE";
          break;
        case eDATA: env = "$ANDROID_DATA";
          break;

      case eSDCARD_EXT: env = "$EXTERNAL_STORAGE2";
        break;


      }


      QProcess proc;
      QString cmd = "adb";
      QStringList argAdb;
      argAdb << "shell" << "echo" << env ;
      proc.start(cmd, argAdb);
      qDebug() <<  " pid =" << proc.pid() << " process started"  ;
      if (!proc.waitForFinished())
          return false;

      QByteArray res = proc.readLine();

      return res.trimmed();

  }



  QString ddMainWindow::getSdcardPath()
  {
      QProcess proc;
      QString cmd = "adb";
      QStringList argAdb;
      argAdb << "shell" << "echo" << "$EXTERNAL_STORAGE" ;
      proc.start(cmd, argAdb);
      qDebug() <<  " pid =" << proc.pid() << " process started"  ;
      if (!proc.waitForFinished())
          return false;

      QByteArray res = proc.readLine();

      return res.trimmed();

  }

// получаем путь к точке монтирования sdcard, так как на многоих моделях в корне лежит ссылка на точку монтирования
  // карты памяти,
  QString ddMainWindow::getPathSdcardFromLink()
  {
      QString sdcardPath = "";
      sdcardPath.clear();

      QProcess proc;
      QString cmd = "adb";
      QStringList argAdb;
      argAdb << "shell" << "ls" << "-l" << sdcardFolder  ;
      proc.start(cmd, argAdb);
      qDebug() <<  " pid =" << proc.pid() << " process started"  ;
      if (!proc.waitForFinished())
          return false;

      QByteArray res = proc.readLine();
      if (res.isEmpty())
          return sdcardPath;

     // если не начинается на 'l' значит это не ссылка и возвращаем пустую строку
      if (!res.startsWith("l"))
          return sdcardPath;

          sdcardPath = res;

          QStringList list = sdcardPath.split(" ");

          int indexPath = list.size() - 1;
          sdcardPath = list.at(indexPath).trimmed();

    return sdcardPath;

  }

  // функция вызывается когда запущен первый процесс по копированию папки /data
  // также здесь запускается процесс вывода информации о копировании
  void ddMainWindow::dataCopyStarted()
  {



      time_t startTime;
      startTime = time (&startTime);

      mobileCopyInfo = new TMobileCopyInfo();

      QObject::connect(mobileCopyInfo, SIGNAL(sigChangeProgressBarValMobile(int)), SLOT(slotChangeProgressBarValMobile(int)), Qt::QueuedConnection);
      QObject::connect(mobileCopyInfo, SIGNAL(sigSetServiceInfo(QString)), SLOT(slotSetServiceInfo(QString)), Qt::QueuedConnection);

      mobileCopyInfo->initialize(ui->widLineDestDirMobile->text(),totalMobileSize ,startTime);
      mobileCopyInfo->start();

  }

  void ddMainWindow::slotChangeProgressBarValMobile (int currentValue)
  {
    ui->widProgressBarMobile->setValue(currentValue);

  }

  // инициализация перед началом копирования телефона
  bool ddMainWindow::initialBeforStartMobile()
  {

    stopMobileCopy = false;
   ui->widProgressBarMobile->setValue(0);
   ui->widButStartMobileImage->setEnabled(false);
   ui->widButStopMobileImage->setEnabled(true);
   ui->widLabelServiceInfo->setText("Coping process is started......");
   return true;

  }


  // инициализация после завершения всех процессов копирования
  bool ddMainWindow::initialAfterFinishMobile()
  {
      if (!stopMobileCopy)
          ui->widProgressBarMobile->setValue(100);
   ui->widLabelServiceInfo->setText("Coping process is finished");
   ui->widButStartMobileImage->setEnabled(true);
   ui->widButStopMobileImage->setEnabled(false);
   stopMobileCopy = false;
   return true;


  }

  void ddMainWindow::slotSetServiceInfo(const QString &strInfo)
  {
     ui->widLabelServiceInfo->setText(strInfo);

  }

void ddMainWindow::on_destDialog_clicked()
{
    SaveDestFile();
}

void ddMainWindow::on_fillTable_clicked()
{
    updateDisks();
}

void ddMainWindow::on_ExitBut_clicked()
{
    ExitApp();
}

void ddMainWindow::on_ddStopButton_clicked()
{
    ddStop();
}
