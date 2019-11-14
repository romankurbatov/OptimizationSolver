QT       -= gui

TARGET = vector
TEMPLATE = lib

DESTDIR = ../../lib
MOC_DIR = ../../.build/$$TARGET/moc
OBJECTS_DIR = ../../.build/$$TARGET/obj

DEFINES += DLL_LIBRARY

INCLUDEPATH += ../../include

SOURCES += \
    Vector.cpp

LIBS += -L../../lib -llog
