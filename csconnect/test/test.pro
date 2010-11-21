TEMPLATE = app
SOURCES = test.cpp
INCLUDEPATH += ../interface
LIBS += -L../lib -lcsconnect -luuid
QT -= core gui
CONFIG -= app_bundle x86
CONFIG += x86_64
