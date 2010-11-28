

set(SOURCES
	csconnect.cpp
)

set(HEADERS
	interface/csconnect.h
)


include_directories(${CMAKE_SOURCE_DIR}/common)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/interface)
include_directories(${MONGODB_INCLUDE_DIRS})

add_library(csconnect STATIC ${SOURCES} ${HEADERS})
target_link_libraries(csconnect
					  common
)
