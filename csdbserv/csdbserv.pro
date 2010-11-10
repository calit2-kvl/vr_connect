# ---------------------------------------------------------------------------
# get some configurations
# ---------------------------------------------------------------------------

# ---------------------------------------------------------------------------
# project settings
# ---------------------------------------------------------------------------
TARGET = csdbserv
PROJECT = cglX
VERSION = 1
TEMPLATE = app
LANGUAGE = C++
CONFIG  =  warn_on   release
QT  =

# ---------------------------------------------------------------------------
# common settings  !! Use just the release
# ---------------------------------------------------------------------------
DESTDIR = ./bin
MY_INCDIR = ./include
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
    

# ---------------------------------------------------------------------------
# linux
# -g++ 32bit
# ---------------------------------------------------------------------------
linux-g++ { 
    LIBS += -L /usr/local/cglX/lib -lcglXnet
    INCLUDEPATH += $${MY_INCDIR} /usr/local/cglX/include
    
    # clean it
    CLEAN_FILES += $$DESTDIR/$${TARGET}
}

# ---------------------------------------------------------------------------
# linux
# -g++ 64bit
# ---------------------------------------------------------------------------
linux-g++-64 { 
    LIBS += -L /usr/local/cglX/lib -lcglXnet
    INCLUDEPATH += $${MY_INCDIR} /usr/local/cglX/include
    
    # clean it
    CLEAN_FILES += $$DESTDIR/$${TARGET}
}

# ---------------------------------------------------------------------------
# MAC OS Native AGL
# -g++
# ---------------------------------------------------------------------------
macx-g++ { 
    
    LIBS += -framework cglXnet
    INCLUDEPATH += $${MY_INCDIR} 
    QMAKE_LFLAGS += -F../cglXlib/lib/
    
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
    ICON = .qrc/icons/cglxmac.icns
    LIBS += -framework \
        cglX
    INCLUDEPATH += $${MY_INCDIR} \
        ../cglXlib/include \
        ../addonlibs/
    QMAKE_LFLAGS += -F../cglXlib/lib/
    
    # install binary
    target.path = /Applications
    
    # post innstall -> not nice but it works
    postinst.path = /Applications
    postinst.extra = cd \
        /Applications/$${TARGET}.app/Contents/MacOS;install_name_tool \
        -change \
        cglX.framework/Versions/1/cglX \
        /Library/Frameworks/cglX.framework/Versions/1/cglX \
        $${TARGET}
    
    # "make install" configuration options
    INSTALLS += target \
        postinst
    
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

