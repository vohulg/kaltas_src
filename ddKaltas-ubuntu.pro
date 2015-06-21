#-------------------------------------------------
#
# Project created by QtCreator 2014-08-20T04:35:22
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ddKaltas-ubuntu
TEMPLATE = app



SOURCES += main.cpp\
        ddmainwindow.cpp \
    TShowInfo.cpp \
    TMythread.cpp \
    TShowtime.cpp \
    TMount.cpp \
    dialog.cpp \
    TMobilecopyInfo.cpp

HEADERS  += ddmainwindow.h \
    TShowInfo.h \
    TMythread.h \
    TShowtime.h \
    TMount.h \
    dialog.h \
    TMobilecopyInfo.h

FORMS    += ddmainwindow.ui \
    dialog.ui
