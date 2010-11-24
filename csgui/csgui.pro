# ---------------------------------------------------------------------------
# get some configurations
# ---------------------------------------------------------------------------

# ---------------------------------------------------------------------------
# project settings
# ---------------------------------------------------------------------------
TARGET = csgui
PROJECT = cglX
VERSION = 1
TEMPLATE = app
LANGUAGE = C++
CONFIG += qt \
    warn_on \
    release \
    assistant \
    core \
    help
QT += opengl webkit network

# ---------------------------------------------------------------------------
# common settings  !! Use just the release
# ---------------------------------------------------------------------------
DESTDIR = ./bin
MY_INCDIR = ./include
ADD_INCDIR            = ../common
MY_SRCDIR = ./src
UI_DIR = .ui
RCC_DIR = .qrc
MOC_DIR = .moc
OBJECTS_DIR = .obj
MY_ADD_DIR = ../addonlibs

# ---------------------------------------------------------------------------
# The sources
# ---------------------------------------------------------------------------
HEADERS = $${MY_INCDIR}/main.h

SOURCES = $${MY_SRCDIR}/main.cpp

LIBS += -L../common -lcommon
FORMS = $${UI_DIR}/csgoogle.ui


#RESOURCES = $${RCC_DIR}/csgoogle.qrc

# ---------------------------------------------------------------------------
# linux
# -g++ 32bit
# ---------------------------------------------------------------------------
linux-g++ {
    LIBS += -L /usr/local/cglX/lib -lcglXnet
    INCLUDEPATH += $${MY_INCDIR} $${ADD_INCDIR} /usr/local/cglX/include

    # clean it
    CLEAN_FILES += $$DESTDIR/$${TARGET}
}

# ---------------------------------------------------------------------------
# linux
# -g++ 64bit
# ---------------------------------------------------------------------------
linux-g++-64 {
    LIBS += -L /usr/local/cglX/lib -lcglXnet
    INCLUDEPATH += $${MY_INCDIR} $${ADD_INCDIR} /usr/local/cglX/include

    # clean it
    CLEAN_FILES += $$DESTDIR/$${TARGET}
}

# ---------------------------------------------------------------------------
# MAC OS Native AGL
# -g++
# ---------------------------------------------------------------------------
macx-g++ {

    LIBS += -framework cglXnet
    INCLUDEPATH += $${MY_INCDIR} $${ADD_INCDIR}
    QMAKE_LFLAGS +=

    # clean it
    CLEAN_FILES += -r $$DESTDIR/$${TARGET}.app
}

# ---------------------------------------------------------------------------
# MAC OS Native AGL
# -macx-xcode
# ---------------------------------------------------------------------------
# The installation does not really work ->> need to fix that !!
# This mode is more for better debugging under MAC since we can use XTools
# ---------------------------------------------------------------------------
macx-xcode {
    # ICON = .qrc/icons/cglxmac.icns
    LIBS += -framework \
        cglXnet
    INCLUDEPATH += $${MY_INCDIR} $${ADD_INCDIR}

    QMAKE_LFLAGS +=

    # install binary
    # target.path = /Applications

    # post innstall -> not nice but it works
    # postinst.path = /Applications
    # postinst.extra = cd \
    #    /Applications/$${TARGET}.app/Contents/MacOS;install_name_tool \
    #    -change \
    #    cglX.framework/Versions/1/cglX \
    #    /Library/Frameworks/cglX.framework/Versions/1/cglX \
    #    $${TARGET}
    #
    # "make install" configuration options
    #INSTALLS += target \
    #    postinst

    # clean it
    CLEAN_FILES += -r \
        ./build
}

# ---------------------------------------------------------------------------
# CLEAN UP THE REST THAT IS COMMON
# ---------------------------------------------------------------------------
CLEAN_FILES += $${MY_INCDIR}/*~ \
    $${MY_SRCDIR}/*~ \
    $${OBJECTS_DIR}/* \
    $${MOC_DIR}/* \
    ./Makefile

