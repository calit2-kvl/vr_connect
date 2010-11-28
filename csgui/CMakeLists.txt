
set(SOURCES src/main.cpp)
set(HEADERS include/main.h)

include_directories(${QT_QTCORE_INCLUDE_DIR})
include_directories(${QT_QTGUI_INCLUDE_DIR})
include_directories(${QT_QTWEBKIT_INCLUDE_DIR})
include_directories(${QT_QTNETWORK_INCLUDE_DIR})
include_directories(${QT_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/common)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CGLX_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

QT4_WRAP_UI(UI_SOURCES .ui/csgoogle.ui)
QT4_WRAP_CPP(MOC_SOURCES include/main.h)
add_executable(csgui ${HEADERS} ${SOURCES} ${UI_SOURCES} ${MOC_SOURCES})
target_link_libraries(csgui
					  common
					  ${CGLX_LIBRARIES}
					  ${QT_QTCORE_LIBRARY}
					  ${QT_QTGUI_LIBRARY}
					  ${QT_QTNETWORK_LIBRARY}
					  ${QT_QTWEBKIT_LIBRARY}
					 )
