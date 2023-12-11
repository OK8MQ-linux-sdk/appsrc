#-------------------------------------------------
#
# Project created by QtCreator 2013-11-26T14:43:58
#
#-------------------------------------------------

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = fltest_watchdog_qt
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui
# install
target.path = $$[QT_INSTALL_BINS/get]
INSTALLS += target

