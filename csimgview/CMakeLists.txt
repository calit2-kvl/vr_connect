
set (SOURCES
	main.cpp)

set (HEADERS
	main.h
	all_icons.h)

include_directories(${CMAKE_SOURCE_DIR}/common)
include_directories(${CGLX_INCLUDE_DIRS})
include_directories(${QT_INCLUDES})
include_directories(${ImageMagick_INCLUDE_DIRS})

add_executable(csimgview ${SOURCES} ${HEADERS})

target_link_libraries(csimgview 
					  common
					  ${CGLX_LIBRARIES} 
					  ${QT_QTCORE_LIBRARY}
					  ${OpenGL_LIBRARIES} 
					  ${ImageMagick_Magick++_LIBRARY}
					 )

