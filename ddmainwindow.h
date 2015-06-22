#ifndef DDMAINWINDOW_H
#define DDMAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QFile>
#include <unistd.h>
#include  <fcntl.h>
#include <string>
#include <stdio.h>
#include "QThread"
#include <QMessageBox>
#include <QTextStream>
#include <cstdlib>
#include <iostream>
#include "sys/types.h"
#include <sys/stat.h>
#include  <fcntl.h>
#include <sys/statvfs.h>
#include <TShowInfo.h>
#include <TMythread.h>
#include <QTimer>
#include <QEventLoop>
#include <TMount.h>
#include <QProcess>
#include <dialog.h>
#include <QDebug>
#include <QTime>
#include <TMobilecopyInfo.h>

using namespace std;

extern bool StopIndicator;

enum ENV_TYPE {eSDCARD, eDATA, eSDCARD_EXT };

namespace Ui {
class ddMainWindow;
}

class ddMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    void fillTable();
    void getLocalDisks();
    void getDisksSize();
    void parseLine(QString line);
    explicit ddMainWindow(QWidget *parent = 0);

    off_t getFreeSpace(const QString& sDirPath);
    QString digitToQStr (off_t );

    ~ddMainWindow();

private slots:
    void on_ddStartButton_clicked();
    void slotFillTable();
    void getMountPoint();
    void SaveDestFile();
    void ExitApp();
    void ddStop();
    void funcShowInfo(int, const QString &);
    void ShowMsgBox(const QString &);
    void ShowMsgBox2(const QString &);
    void ShowMsgBoxMount(const QString &);
    void ChangeButton();
    void updateDisks();
    void showDiskInCombobox(int, int);
    void getVenorDisks();
    void on_ButtonMountSrc_clicked();
    void on_ButtonMountDest_clicked();
    void on_ButtonMount_clicked();
    void on_pushButton_clicked();
    bool auto_mounting();
    bool initializing();
    void on_widButUpdateMobileDevice_clicked();
    void on_widButChooseDir_clicked();
    void on_widButStartMobileImage_clicked();
    qlonglong getSizeMobileFolder(const QString &folder);
    qlonglong getSizeWithDU(const QString &folder);
    qlonglong getSizeWithDF(const QString &folder);


    void on_widButStopMobileImage_clicked();

    void dataCopyFinished (int code, QProcess::ExitStatus exitStatus);
    void dataCopyStarted();
    void sdcardCopyFinished (int code, QProcess::ExitStatus exitStatus);
     bool startMobileFolderCopy(const QString &folderSrc, const QString &folderDest, QProcess* proc);

     QString getPathSdcardFromLink();
    QString getSdcardPath();
    QString getEnvPath(int type);
    void getMemoryFolderNames();


     void slotChangeProgressBarValMobile (int currentValue);
     bool initialBeforStartMobile();
     bool initialAfterFinishMobile();

     void slotSetServiceInfo(const QString &strInfo);

     void on_destDialog_clicked();

     void on_fillTable_clicked();

     void on_ExitBut_clicked();

     void on_ddStopButton_clicked();

private:

    QStringList localDisksList;
    QStringList localDisksSize;
    QStringList localDiskMountPointList;
    QStringList localDiskVendor;
    bool localDiskOffset;
    Ui::ddMainWindow *ui;
    TMyThread* mythread;
    TShowInfo *showInfo;
    TMount *mnt;

    QTimer timer;
    QEventLoop loop;
    int prevRow;
    int prevCol;
    Dialog helpWin;

    QString destDirMobile;
    qlonglong totalMobileSize;


    TMobileCopyInfo* mobileCopyInfo;

    QString dataFolder ;
    QString sdcardFolder ;

    QProcess* dataCopyProc;
    QProcess* sdcardCopyProc;

    bool stopMobileCopy;





};

Q_DECLARE_METATYPE(off_t)

#endif // DDMAINWINDOW_H
