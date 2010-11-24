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
 | @END_COPYRIGHT								 |
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
#include "cJSON.h"

using namespace csConnect;
using std::string;

struct DBClientTest
{
   DBClientTest() : servername("109.171.139.4"){}

   Session session;
   string servername;
};

TEST_FIXTURE(DBClientTest, Sesssion_connect)
{
    CHECK(session.connect(servername));
}


TEST_FIXTURE(DBClientTest, Object_create)
{
    CHECK(session.connect(servername));
    cJSON *json_obj = cJSON_CreateObject();
    CHECK(json_obj);
    cJSON_AddStringToObject(json_obj, "TEST", "DATA");
    cJSON *reply;
    CHECK(session.create(json_obj, reply));
}

int main(int, char const *[])
{
    return UnitTest::RunAllTests();
}

