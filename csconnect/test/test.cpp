/*
 +-------------------------------------------------------------------------------+
 | @BEGIN_COPYRIGHT                                                              |
 |                                                                               |
 | Copyright (C) 2010-2015, The Regents of the University of California &        |
 | King Abdullah University of Science and Technology                            |
 | All Rights Reserved.                                                          |
 |                                                                               |
 |                                                                               |
 | Prototyped and developed By:                                                  |
 |    Kai-Uwe Doerr       {kdoerr@usd.edu}                                       |
 |    Christopher Knox    {christopher.knox@kaust.edu.sa}                        |
 |                                                                               |
 | @END_COPYRIGHT                                                           	 |
 +-------------------------------------------------------------------------------+
 |
 | Component  : csconnect test
 | Filename   : test.cpp
 | Sourcefile :
 | Language   : C++
 |
 +-------------------------------------------------------------------------------+ */

#include "csconnect.h"
#include <stdio.h>
#include <vector>

using namespace csconnect;

int main(int argc, char **argv)
{
	session_info info;
	session dummy_session = session(info, "foo", "bar");

	if (info.session_id.b)
	printf("session creation successful\n");

	image_source img_src;
	img_src.uri = "foo_image.png";

	int local_id = 5;
	img_src.local_id = local_id;
	
	session_utils::create_image_source(img_src, dummy_session);

	dummy_session.get_session_updates(info);

	if (info.sources.size() > 0 && info.sources[0].object_id.b)
		printf("object creation successful\n");


    return 0;
}

