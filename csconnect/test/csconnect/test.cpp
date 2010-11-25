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
#include <vector>

#include "UnitTest++.h"
#include "TestReporterStdout.h"

#include "csconnect.h"
#include "cJSON.h"

#include <QtCore/QUuid>
#include <QtCore/QString>

using namespace csConnect;
using std::string;
using std::vector;

//string servername = "109.171.139.4";
QUuid *s_oid = NULL;

struct DBClient
{
    
    DBClient() : servername("109.171.139.4"), ns("devel.test"), json_obj(NULL), reply_obj(NULL), oid(NULL) 
    { 
        CHECK(init()); 
    }
    
    ~DBClient() 
    { 
        if (json_obj) 
            cJSON_Delete(json_obj); 
        if (reply_obj) 
            cJSON_Delete(reply_obj); 
        if (oid)
            delete oid;
    }
    
    bool init() 
    { 
        return session.connect(servername); 
    }
    
    void create_json() 
    {
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
    QUuid *oid;
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
    cJSON *json_oid = cJSON_GetObjectItem(reply_obj, "OID");
    CHECK(json_oid);
    oid = new QUuid(QString(json_oid->valuestring));
    CHECK(oid);
}

TEST_FIXTURE(DBClient, Object_Update)
{
    vector<cJSON *> objects;
    CHECK(session.read(objects, ns, NULL));
    
    CHECK(objects.size() > 0);
    if (objects.size() > 0)
    {
        cJSON_AddStringToObject(objects[0], "FOO", "BAR");
        CHECK(session.update(ns, objects[0]));
    }
}

#if 0
TEST_FIXTURE(DBClient, Object_remove_oid)
{
    /* this test must be called after Object_create */
    CHECK(oid);
    cJSON* oid_obj = cJSON_CreateObject();
    cJSON_AddStringToObject(oid_obj, "OID", oid->toString().toStdString().c_str());
    CHECK(session.destroy(ns, oid_obj));
}
#endif


TEST_FIXTURE(DBClient, Session_Load)
{
    std::vector<cJSON *> objects;
    CHECK(session.load(objects, ns));
}

TEST_FIXTURE(DBClient, Object_remove)
{
    CHECK(session.destroy(ns, json_obj));
    
}

int main(int, char const *[])
{
    return UnitTest::RunAllTests();
}

