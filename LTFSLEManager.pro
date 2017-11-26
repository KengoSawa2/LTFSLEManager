#-------------------------------------------------
#
# Project created by QtCreator 2015-10-25T14:12:12
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LTFSLE_MANAGER
TEMPLATE = app

RC_ICONS = LTFSLEMANAGER.ico

TRANSLATIONS += LTFSLEManager.ts

SOURCES += main.cpp \
        mainwindow.cpp

HEADERS  += mainwindow.h \
    ltfsdefine.h

FORMS    += mainwindow.ui

RESOURCES += \
    resource.qrc
