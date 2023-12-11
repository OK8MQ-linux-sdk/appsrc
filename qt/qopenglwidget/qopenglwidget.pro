QT += widgets
TARGET = fltest_qopenglwidget
SOURCES += main.cpp \
           glwidget.cpp \
           mainwindow.cpp \
           bubble.cpp

HEADERS += glwidget.h \
           mainwindow.h \
           bubble.h

RESOURCES += texture.qrc

# install
target.path = $$[QT_INSTALL_BINS/get]
INSTALLS += target

