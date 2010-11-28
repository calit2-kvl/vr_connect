project(VRCONNECT)
cmake_minimum_required(VERSION 2.8)

# get custom modules
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR} ${CMAKE_MODULE_PATH})

# search here for all packages we need - but include them only when required
find_package(Qt4 4.7.0 COMPONENTS QtCore QtGui QtWebKit QtNetwork REQUIRED)
find_package(ImageMagick COMPONENTS Magick++ REQUIRED)
find_package(OpenGL REQUIRED)
find_package(CGLX REQUIRED)
find_package(MongoDB REQUIRED)
find_package(Boost COMPONENTS thread system REQUIRED)

# set the output directories
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# set project variables
set(VRCONNECT_COMMON_DIR ${CMAKE_SOURCE_DIR}/common)
set(VRCONNECT_CSCONNECT_INTERFACE_DIR ${CMAKE_SOURCE_DIR}/csconnect/csconnect/interface)

# default to debug build while developing
set(CMAKE_BUILD_TYPE Debug)

# finally enter the directories for each project
add_subdirectory(common)
add_subdirectory(csconnect)
add_subdirectory(csimgview)
add_subdirectory(csgui)
add_subdirectory(csdbserv)
