# ---------------------------------------------------------------------------
# some basic configurations 
# ---------------------------------------------------------------------------

TARGET                = csconnect
PROJECT               = TUTORIAL
VERSION               = 1

TEMPLATE              = lib
LANGUAGE              = C++
CONFIG               += warn_off release staticlib

DEFINES              += 

QT 										= 
QMAKE_CXXFLAGS 				+= -fno-visibility-inlines-hidden

# ---------------------------------------------------------------------------
# common settings 
# ---------------------------------------------------------------------------

DESTDIR               = ../lib
OBJECTS_DIR           = .obj
INCLUDEPATH 				 += interface
INCLUDEPATH          += ../../common

macx:MONGOPATH = ../../../../dependencies
unix:MONGOPATH = /usr/local/include

# ---------------------------------------------------------------------------
# The sources
# ---------------------------------------------------------------------------

HEADERS              += interface/csconnect.h

SOURCES              += csconnect.cpp

# ---------------------------------------------------------------------------
# linux
# -g++
# ---------------------------------------------------------------------------
linux-g++ {
 LIBS                += -L$$MONGOPATH/mongo -lmongoclient -lboost_system-mt -lboost_thread-mt -L../../common -lcommon

 # clean it
 CLEAN_FILES          = ./$${DESTDIR}/lib$${TARGET}.a
}
# ---------------------------------------------------------------------------
# linux
# -g++-64
# ---------------------------------------------------------------------------
linux-g++-64 {
 LIBS                += -L$$MONGOPATH/mongo -lmongoclient -L/usr/local/lib -lboost_system -lboost_thread -L../../common -lcommon

 # clean it
 CLEAN_FILES          = ./$${DESTDIR}/lib$${TARGET}.a
}
# ---------------------------------------------------------------------------
# MAC OS Native AGL
# -g++
# ---------------------------------------------------------------------------
macx-g++{
 CONFIG += x86_64
 CONFIG -= x86
 LIBS                += -L$$MONGOPATH/mongo -lmongoclient -lboost_system-mt -lboost_thread-mt -L../../common -lcommon

 # clean it
 CLEAN_FILES          = -rf ./$${DESTDIR}$${TARGET}.app
}
# ---------------------------------------------------------------------------
# MAC OS Native AGL
# -macx-xcode
# ---------------------------------------------------------------------------
macx-xcode{
 CONFIG 						+= x86_64
 CONFIG 						 -= x86
 LIBS                += -L$$MONGOPATH/mongo -lmongoclient -lboost_system-mt -lboost_thread-mt -L../../common -lcommon

 # clean it
 CLEAN_FILES          = -rf ./$${DESTDIR}$${TARGET}.app
}

# ---------------------------------------------------------------------------
# CLEAN UP THE REST THAT IS COMMON
# ---------------------------------------------------------------------------
CLEAN_FILES	+= ./*~ $${OBJECTS_DIR}/* ./Makefile
