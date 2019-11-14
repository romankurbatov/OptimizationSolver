QT       -= gui

TARGET = set
TEMPLATE = lib

DESTDIR = ../../lib
MOC_DIR = ../../.build/$$TARGET/moc
OBJECTS_DIR = ../../.build/$$TARGET/obj

DEFINES += DLL_LIBRARY

INCLUDEPATH += ../../include

SOURCES += \
    ISetImpl.cpp

LIBS += -L../../lib -lvector -llog
