TARGET = csconnect
LANGUAGE = C++
TEMPLATE = lib
DESTDIR = ./lib

INCLUDEPATH += interface
HEADERS = interface/csconnect.h
SOURCES = source/csconnect.cpp
QT -= core gui

SUBDIRS = test
