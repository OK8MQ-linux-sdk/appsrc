######################################################################
# Automatically generated by qmake (2.01a) Sun Sep 29 14:23:13 2013
######################################################################
QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app
TARGET = fltest_network
DEPENDPATH += .
INCLUDEPATH += .

# Input
HEADERS += interface.h \
           mainwindow.h \
           qipaddressedit.h \
           qipaddressedititem.h
FORMS += mainwindow.ui
SOURCES += interface.cpp \
           main.cpp \
           mainwindow.cpp \
           qipaddressedit.cpp \
           qipaddressedititem.cpp
# install
target.path = $$[QT_INSTALL_BINS/get]
INSTALLS += target

