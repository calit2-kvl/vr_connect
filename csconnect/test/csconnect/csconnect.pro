TEMPLATE = app
SOURCES = test.cpp
INCLUDEPATH += ../../csconnect/interface ../UnitTest++ ../../../common
QT -= gui
macx:CONFIG -= app_bundle x86
macx:CONFIG += x86_64
LIBS += -L../lib \
        -L../../lib \
        -L../../../common \
        -lcommon \
        -lcsconnect \
        -lUnitTest++ \
        -lmongoclient
        
# unix:LIBS += -lboost_thread 

macx:LIBS += -L../../../../../dependencies/mongo -lboost_thread-mt -lboost_system-mt



