#-------------------------------------------------
#
# Project created by QtCreator 2017-06-28T06:25:53
#
#-------------------------------------------------

QT       += core gui
QT       += xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
                                  QT += network

TARGET = ecatui
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    osal/linux/osal.c \
    oshw/linux/nicdrv.c \
    oshw/linux/oshw.c \
    soem/ethercatbase.c \
    soem/ethercatcoe.c \
    soem/ethercatconfig.c \
    soem/ethercatdc.c \
    soem/ethercatfoe.c \
    soem/ethercatmain.c \
    soem/ethercatprint.c \
    soem/ethercatsoe.c \
    initxml.cpp

HEADERS += \
        mainwindow.h \
    osal/linux/osal_defs.h \
    osal/osal.h \
    oshw/linux/nicdrv.h \
    oshw/linux/oshw.h \
    soem/ethercatbase.h \
    soem/ethercatcoe.h \
    soem/ethercatconfig.h \
    soem/ethercatconfiglist.h \
    soem/ethercatdc.h \
    soem/ethercatfoe.h \
    soem/ethercatmain.h \
    soem/ethercatprint.h \
    soem/ethercatsoe.h \
    soem/ethercattype.h \
    initxml.h

FORMS += \
        mainwindow.ui \
    initxml.ui
