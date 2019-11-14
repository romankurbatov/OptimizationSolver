QT       -= gui

TARGET = roman
TEMPLATE = lib

DESTDIR = ../../lib
MOC_DIR = ../../.build/$$TARGET/moc
OBJECTS_DIR = ../../.build/$$TARGET/obj

DEFINES += DLL_LIBRARY

INCLUDEPATH += ../../include

SOURCES += \
    MyVector.cpp \
    MySet.cpp \

LIBS += -L../../lib -llog
