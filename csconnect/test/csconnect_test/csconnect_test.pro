TEMPLATE = app
SOURCES = test.cpp
INCLUDEPATH += ../../interface ../UnitTest
LIBS += -L../lib -lUnitTest
QT -= core gui
macx:CONFIG -= app_bundle x86
macx:CONFIG += x86_64
