TEMPLATE = app
TARGET = fltest_qmlsink
QT += qml quick widgets

QT_CONFIG -= no-pkg-config
CONFIG += link_pkgconfig debug
PKGCONFIG = \
    gstreamer-1.0 \
    gstreamer-video-1.0

DEFINES += GST_USE_UNSTABLE_API

INCLUDEPATH += ../lib

SOURCES += main.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =