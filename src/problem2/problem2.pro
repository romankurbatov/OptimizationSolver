QT       -= gui

TARGET = problem2
TEMPLATE = lib

DESTDIR = ../../lib
MOC_DIR = ../../.build/$$TARGET/moc
OBJECTS_DIR = ../../.build/$$TARGET/obj

DEFINES += DLL_LIBRARY

INCLUDEPATH += ../../include

SOURCES += \
    Problem2.cpp \
    Problem2Brocker.cpp

HEADERS += \
    Problem2.h
