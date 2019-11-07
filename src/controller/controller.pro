QT       += core gui sql printsupport declarative

QMAKE_CXXFLAGS += -std=gnu++0x

TARGET = optimization-solver
TEMPLATE = app

DESTDIR = ../..
MOC_DIR = ../../.build/controller/moc
OBJECTS_DIR = ../../.build/controller/obj
UI_DIR = ../../.build/controller/ui

INCLUDEPATH += ../../include

#LIBS += -L../../lib -lvector -lset -lcompact -llog
LIBS += -L../../lib -lroman -lcompact -llog

SOURCES += main.cpp \
    controller.cpp \
    sqlconnectiondialog.cpp \
    insertsolverdialog.cpp \
    qcustomplot.cpp \
    solverdialog.cpp

HEADERS  += \
    ../../include/SHARED_EXPORT.h \
    ../../include/IVector.h \
    ../../include/ISolver.h \
    ../../include/ISet.h \
    ../../include/IProblem.h \
    ../../include/ILog.h \
    ../../include/ICompact.h \
    ../../include/IBrocker.h \
    ../../include/error.h \
    controller.h \
    sqlconnectiondialog.h \
    insertsolverdialog.h \
    qcustomplot.h \
    solverdialog.h

FORMS    += \
    controller.ui \
    sqlconnectiondialog.ui \
    insertsolverdialog.ui \
    solverdialog.ui
