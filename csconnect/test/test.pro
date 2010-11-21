TEMPLATE = app
SOURCES = test.cpp
INCLUDEPATH += ../interface
LIBS += -L../lib -lcsconnect -luuid
QT -= core gui
