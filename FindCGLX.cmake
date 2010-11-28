# todo create components

find_path(CGLX_INCLUDE_DIR cglX.h HINTS /usr/local/cglX/include)
find_library(CGLX_LIBRARY cglX HINTS /usr/local/cglX/lib)

find_path(CGLXNET_INCLUDE_DIR cglXNet.h HINTS /usr/local/cglX/include)
find_library(CGLXNET_LIBRARY cglXnet HINTS /usr/local/cglX/lib)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(CGLX DEFAULT_MSG 
	CGLX_LIBRARY 
	CGLX_INCLUDE_DIR
	CGLXNET_LIBRARY
	CGLXNET_INCLUDE_DIR
)

if(CGLX_FOUND)
	set(CGLX_LIBRARIES ${CGLX_LIBRARY} ${CGLXNET_LIBRARY})
	set(CGLX_INCLUDE_DIRS ${CGLX_INCLUDE_DIR} ${CGLXNET_INCLUDE_DIR})
endif(CGLX_FOUND)

mark_as_advanced(CGLX_LIBRARY CGLX_INCLUDE_DIR)

