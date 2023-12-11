TEMPLATE = app
TARGET = fltest_musicplayer

QT += widgets multimedia

HEADERS = \
    musicplayer.h \
    volumebutton.h

SOURCES = \
    main.cpp \
    musicplayer.cpp \
    volumebutton.cpp

RC_ICONS = images/qt-logo.ico

# install
target.path = $$[QT_INSTALL_BINS/get]
INSTALLS += target

