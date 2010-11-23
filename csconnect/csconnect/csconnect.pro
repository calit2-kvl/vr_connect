TARGET = csconnect
LANGUAGE = C++
TEMPLATE = lib
DESTDIR = ../lib


macx:MONGOPATH = ../../../../dependencies/mongo


macx:QMAKE_CXXFLAGS += -fno-visibility-inlines-hidden

INCLUDEPATH += ../interface
INCLUDEPATH += $$MONGOPATH/client

LIBS += -L/usr/local/lib \
        -L$$MONGOPATH  \
        -lboost_system-mt \
        -lboost_thread-mt \
        -lmongoclient

PRECOMPILED_HEADER = csccommon.h

HEADERS = ../interface/csconnect.h \
          csccommon.h
          
SOURCES = csconnect.cpp


QT -= core gui

macx:CONFIG += x86_64
macx:CONFIG -= x86
