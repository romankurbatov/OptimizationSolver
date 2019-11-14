QT       -= gui

TARGET = compact
TEMPLATE = lib

DESTDIR = ../../lib
MOC_DIR = ../../.build/$$TARGET/moc
OBJECTS_DIR = ../../.build/$$TARGET/obj

DEFINES += DLL_LIBRARY

INCLUDEPATH += ../../include

SOURCES += \
    Compact.cpp

LIBS += -L../../lib -lroman -llog
