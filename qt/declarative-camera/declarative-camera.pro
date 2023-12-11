TEMPLATE=app
TARGET=fltest_declarative-camera

QT += quick qml multimedia

SOURCES += qmlcamera.cpp
RESOURCES += declarative-camera.qrc

# install
target.path = $$[QT_INSTALL_BINS/get]
INSTALLS += target

winrt {
    WINRT_MANIFEST.capabilities_device += webcam microphone
}
