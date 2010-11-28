set (SOURCES
	 cJSON.c)

set (HEADERS
	cJSON.h)

add_library(common STATIC ${SOURCES} ${HEADERS})
