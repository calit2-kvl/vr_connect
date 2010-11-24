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

using namespace csConnect;

TEST(Sesssion_connect)
{
    Session session;
    std::string servername = "109.171.139.4";
    CHECK(session.connect(servername));
}

int main(int, char const *[])
{
    return UnitTest::RunAllTests();
}
