TEMPLATE = app
TARGET = mlitenotificationtool
INCLUDEPATH += ../..
DEPENDPATH += $$INCLUDEPATH

QMAKE_LIBDIR = ../..
LIBS = -lmlite

SOURCES += mlitenotificationtool.cpp
INSTALLS += target
QT += dbus

target.path = /usr/bin
