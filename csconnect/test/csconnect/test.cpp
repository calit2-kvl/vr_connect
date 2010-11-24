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

#include <string>
#include <iostream>

#include "UnitTest++.h"
#include "TestReporterStdout.h"


#include "csconnect.h"

TEST(Sesssion_connect)
{
    csConnect::SessionInfo session_info;
    std::string servername = "127.0.0.1";
    std::string sessionname = "tutorial";
    csConnect::Session session;
    CHECK(session.connect(session_info, servername, sessionname));
    std::cout << session_info.sources << "\n";
}

int main(int, char const *[])
{
    return UnitTest::RunAllTests();
}
