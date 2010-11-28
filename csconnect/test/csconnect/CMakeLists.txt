INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/csconnect/csconnect/interface)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/csconnect/test/UnitTest++)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/common)
INCLUDE_DIRECTORIES(${QT_INCLUDE_DIR})

SET(SOURCES test.cpp)


ADD_EXECUTABLE(csconnect_test ${SOURCES})

TARGET_LINK_LIBRARIES(csconnect_test
					  common
					  csconnect
					  UnitTest++ 
					  ${MongoDB_LIBRARY}
					  ${Boost_LIBRARIES}
					  ${QT_QTCORE_LIBRARY}
					 )
