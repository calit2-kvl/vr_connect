TARGET = csconnect
LANGUAGE = C++
TEMPLATE = lib
DESTDIR = ../lib
CONFIG = staticlib

macx:MONGOPATH = ../../../../dependencies/mongo


macx:QMAKE_CXXFLAGS += -fno-visibility-inlines-hidden

INCLUDEPATH += interface
INCLUDEPATH += ../../common
macx:INCLUDEPATH += $$MONGOPATH/client

macx:LIBS += -L$$MONGOPATH

HEADERS = interface/csconnect.h

SOURCES = csconnect.cpp


QT -= core gui

macx:CONFIG += x86_64
macx:CONFIG -= x86
