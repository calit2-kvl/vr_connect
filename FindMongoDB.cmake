
find_path(MongoDB_INCLUDE_DIR mongo/client/dbclient.h)
find_library(MongoDB_LIBRARY mongoclient)
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(MONGODB DEFAULT_MSG  MongoDB_LIBRARY  MongoDB_INCLUDE_DIR)

if(MONGODB_FOUND)
	set(MongoDB_LIBRARIES ${MongoDB_LIBRARY})
	set(MongoDB_INCLUDE_DIRS ${MongoDB_INCLUDE_DIR})
endif(MONGODB_FOUND)

mark_as_advanced(MongoDB_LIBRARY MongoDB_INCLUDE_DIR)

