TARGET = csconnect
LANGUAGE = C++
TEMPLATE = lib
DESTDIR = ./lib

INCLUDEPATH += interface
INCLUDEPATH += ../../../dependencies/mongo/client

LIBS += -L/usr/local/lib -lboost_system-mt

# HEADERS = interface/csconnect.h
# SOURCES = source/csconnect.cpp


QT -= core gui

CONFIG += x86_64
CONFIG -= x86

SUBDIRS = test
