# -------------------------------------------------
# Project created by QtCreator 2010-01-16T16:32:46
# -------------------------------------------------
TARGET = testProject
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
INCLUDEPATH += ../
LIBS += -L../../bin/ \
    -lEksCore
SOURCES += main.cpp \
    testSignalsMain.cpp
HEADERS += timeNow.h \
    TestClasses.h

include("../GeneralOptions.pri")
