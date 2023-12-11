TEMPLATE = app
TARGET = fltest_audiorecorder

QT += multimedia

win32:INCLUDEPATH += $$PWD

HEADERS = \
    audiorecorder.h \
    qaudiolevel.h

SOURCES = \
    main.cpp \
    audiorecorder.cpp \
    qaudiolevel.cpp

FORMS += audiorecorder.ui

# install
target.path = $$[QT_INSTALL_BINS/get]
INSTALLS += target

QT+=widgets
