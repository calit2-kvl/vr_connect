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
    DBClient() : servername("127.0.0.1"), ns("devel.test"), json_obj(NULL), reply_obj(NULL) { CHECK(init()); }
    ~DBClient() { if (json_obj) cJSON_Delete(json_obj); if (reply_obj) cJSON_Delete(reply_obj); }
    bool init() { return session.connect(servername); }
    void create_json() {
        json_obj = cJSON_CreateObject();
        CHECK(json_obj);
        cJSON_AddStringToObject(json_obj, "TEST", "DATA");
        cJSON_AddNumberToObject(json_obj, "CID", 2334345);
        reply_obj = cJSON_CreateObject();
        CHECK(reply_obj);
    }

    Session session;
    string servername;
    string ns;
    cJSON *json_obj;
    cJSON *reply_obj;
};

TEST_FIXTURE(DBClient, Sesssion_connect)
{
    /* this test is not empty - it exists to check the connection to the database
     * which is called by the constructor. */
}

TEST_FIXTURE(DBClient, Object_create)
{
    /* erase the entire namespace */
    CHECK(session.destroy(ns));
    create_json();
    CHECK(session.create(reply_obj, ns, json_obj));
}

TEST_FIXTURE(DBClient, Object_remove)
{
    /* this test must be called after Object_create */
    
}

TEST_FIXTURE(DBClient, Object_Update)
{
    CHECK(session.update(ns, NULL));
}

TEST_FIXTURE(DBClient, Session_Load)
{
    CHECK(session.load(NULL, "blah"));
}

int main(int, char const *[])
{
    return UnitTest::RunAllTests();
}

