TEMPLATE = app
SOURCES = test.cpp
INCLUDEPATH += ../../csconnect/interface ../UnitTest++ ../../../common
QT -= core gui
macx:CONFIG -= app_bundle x86
macx:CONFIG += x86_64
LIBS += -L../lib \
        -L../../lib \
        -L../../../common \
        -lcommon \
        -lcsconnect \
        -lUnitTest++ \
        -lboost_thread \
        -lmongoclient


