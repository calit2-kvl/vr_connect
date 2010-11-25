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

//string servername = "109.171.139.4";

struct DBClient
{
    Session session;
    string servername;
    string ns;
    DBClient() : servername("127.0.0.1"), ns("devel.test") {}
};

TEST_FIXTURE(DBClient, Sesssion_connect)
{
    CHECK(session.connect(servername));
}


TEST_FIXTURE(DBClient, Object_create)
{
    /* connect */
    CHECK(session.connect(servername));
    CHECK(session.destroy(ns, NULL));
    cJSON *json_obj = cJSON_CreateObject();
    CHECK(json_obj);
    cJSON_AddStringToObject(json_obj, "TEST", "DATA");
    cJSON_AddNumberToObject(json_obj, "CID", 2334345);
    cJSON *reply;
    CHECK(session.create(reply, ns, json_obj));
}

int main(int, char const *[])
{
    return UnitTest::RunAllTests();
}

