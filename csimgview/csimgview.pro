# ---------------------------------------------------------------------------
#               This .pro file was generated by CGLX
#      (It should help you to get started with CGLX programming)
# ---------------------------------------------------------------------------
# ---------------------------------------------------------------------------
#                     >> INSTRUCTIONS <<
#
#  ! If you want to create a Makefile for your OS type in:
#   > "qmake"
#   > and the compile with "make" or "gmake"
#
#  This will compile the simple program "t0". Your task would be to change
#  the HEADER and SOURCE file to the once you want to use.
#  It does not get any easier.
#  ! If you remove the DEF_USE_CGLX define you will get a normal GLUT program
#
#                      >> TROUBLESHOOT <<
#
#  ! If "qmake" is not found on your system, check where QT is installed and
#  add the QT-bin directory to your PATH environment. Try again :-).
#  It may also help if you read the documentation of qmake, to expand this
#  .pro file according to your needs.
# ---------------------------------------------------------------------------
# ---------------------------------------------------------------------------
# some basic configurations
# ---------------------------------------------------------------------------

TARGET                = csimgview
PROJECT               = TUTORIAL
VERSION               = 1

TEMPLATE              = app
LANGUAGE              = C++
CONFIG               += warn_off release

DEFINES              += DEF_USE_CGLX

# ---------------------------------------------------------------------------
# common settings
# ---------------------------------------------------------------------------

DESTDIR               = ./
ADD_INCDIR            = ../csconnect/interface ../common
OBJECTS_DIR           = .obj

# ---------------------------------------------------------------------------
# The sources
# ---------------------------------------------------------------------------

HEADERS              += main.h \
                        all_icons.h

SOURCES              += main.cpp

LIBS += -L../common -lcommon

# ---------------------------------------------------------------------------
# linux
# -g++
# ---------------------------------------------------------------------------
linux-g++ {
 QT                   =
 LIBS                += -L/usr/lib -L/usr/local/cglX/lib/ -lcglX
 INCLUDEPATH         += $${ADD_INCDIR} /usr/local/cglX/include
 # clean it
 CLEAN_FILES          = ./$${DESTDIR}$${TARGET}
}
# ---------------------------------------------------------------------------
# linux
# -g++-64
# ---------------------------------------------------------------------------
linux-g++-64 {
 QT                   =
 LIBS                += -L/usr/lib64 -L/usr/local/lib/ -lMagick++ -L/usr/local/cglX/lib/ -lcglX -L/opt/cglx/lib64
 INCLUDEPATH         += /usr/local/include/ImageMagick $${ADD_INCDIR} /usr/local/cglX/include /opt/cglx/include
 # clean it
 CLEAN_FILES          = ./$${DESTDIR}$${TARGET}
}
# ---------------------------------------------------------------------------
# MAC OS Native AGL
# -g++
# ---------------------------------------------------------------------------
macx-g++{
 QT                   =
 LIBS                += -framework cglX -lpthread -framework Carbon -framework AGL -framework OpenGL
 # clean it
 CLEAN_FILES          = -rf ./$${DESTDIR}$${TARGET}.app
}
# ---------------------------------------------------------------------------
# MAC OS Native AGL
# -macx-xcode
# ---------------------------------------------------------------------------
macx-xcode{
 QT                   =
 LIBS                += -framework cglX -lpthread -framework Carbon -framework AGL -framework OpenGL
 # clean it
 CLEAN_FILES          = -rf ./$${DESTDIR}$${TARGET}.app
}

# ---------------------------------------------------------------------------
# CLEAN UP THE REST THAT IS COMMON
# ---------------------------------------------------------------------------
CLEAN_FILES	+= ./*~ $${OBJECTS_DIR}/* ./Makefile
