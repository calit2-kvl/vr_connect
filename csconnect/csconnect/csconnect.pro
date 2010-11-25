TARGET = csconnect
LANGUAGE = C++
TEMPLATE = lib
DESTDIR = ../lib
CONFIG = staticlib

macx:MONGOPATH = ../../../../dependencies


macx:QMAKE_CXXFLAGS += -fno-visibility-inlines-hidden

INCLUDEPATH += interface
INCLUDEPATH += ../../common
macx:INCLUDEPATH += $$MONGOPATH

macx:LIBS += -L$$MONGOPATH/mongo -lmongoclient -lboost_system-mt -lboost_thread-mt -L../../common -lcommon

HEADERS = interface/csconnect.h

SOURCES = csconnect.cpp


QT -= core gui

macx:CONFIG += x86_64
macx:CONFIG -= x86
