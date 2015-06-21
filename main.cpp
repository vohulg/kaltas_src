/*------------- version 0.2 -------------*/


#include "ddmainwindow.h"
#include "dialog.h"
#include <QApplication>

bool glStopIndicator;
off_t glStartTime;
off_t glWritedByte;
int glEndCopy;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    glStopIndicator = false;
    glStartTime = 0;
    glWritedByte = 0;
    glEndCopy = 0;

    ddMainWindow w;
    w.show();

    //Dialog d;
    //d.show();

    return a.exec();
}
