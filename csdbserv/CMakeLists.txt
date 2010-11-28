
set(SOURCES src/main.cpp)
set(HEADERS include/main.h)

include_directories(include)
include_directories(${VRCONNECT_COMMON_DIR})
include_directories(${VRCONNECT_CSCONNECT_INTERFACE_DIR})
include_directories(${CGLX_INCLUDE_DIRS})

add_executable(csdbserv ${SOURCES} ${HEADERS})

target_link_libraries(csdbserv
					  common
					  csconnect
					  ${CGLX_LIBRARIES}
					  ${MONGODB_LIBRARIES}
					  ${Boost_LIBRARIES}
					 )


