######################################################################
# Automatically generated by qmake (2.01a) Thu Sep 26 14:53:08 2013
######################################################################

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app
TARGET = fltest_wifi
DEPENDPATH += .
INCLUDEPATH += .

# Input
HEADERS += mainwindow.h
FORMS += mainwindow.ui
SOURCES += main.cpp mainwindow.cpp
# install
target.path = $$[QT_INSTALL_BINS/get]
INSTALLS += target

