TARGET = csconnect
LANGUAGE = C++
TEMPLATE = lib
DESTDIR = ./lib


macx:MONGOPATH = ../../../dependencies/mongo


macx:QMAKE_CXXFLAGS += -fno-visibility-inlines-hidden

INCLUDEPATH += interface
INCLUDEPATH += $$MONGOPATH/client

LIBS += -L/usr/local/lib \
        -L$$MONGOPATH  \
        -lboost_system-mt \
        -lboost_thread-mt \
        -lmongoclient

PRECOMPILED_HEADER = source/csccommon.h
HEADERS = interface/csconnect.h \
          source/csccommon.h
SOURCES = source/csconnect.cpp


QT -= core gui

CONFIG += x86_64
CONFIG -= x86

SUBDIRS = test
