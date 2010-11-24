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
 | Component  : csConnect implementation
 | Filename   : csconnect.cpp
 | Sourcefile :
 | Language   : C++
 |
 +-------------------------------------------------------------------------------+ */

#include "csconnect.h"

#include <mongo/client/dbclient.h>
#include "cJSON.h"

#include <string>
#include <algorithm>
#include <vector>

csConnect::Session::Session()
{
    db = new mongo::DBClientConnection;
}

void collectionAsJson(std::string& output,
                      mongo::DBClientConnection& conn,
                      const std::string& database,
                      const std::string& collection)
{
    std::string path(database);
    path.append("." + collection);

    auto_ptr<mongo::DBClientCursor> cursor = conn.query(path, mongo::BSONObj());

    output = "{ \"" + collection + "\" : ";
    if (!cursor->more())
        output.append("null");
    else
        output.append("[");
    while(cursor->more())
    {
        output.append(cursor->next().jsonString());
        if (cursor->more())
            output.append(",");
        else
            output.append("]");
    }
    output.append("}");
}

bool csConnect::Session::connect(std::string const &database_server)
{
    std::string errorstr;
    mongo::HostAndPort host(database_server);

    try
    {
        if (!db->connect(host, errorstr))
            return false;
    }
    catch (mongo::DBException &e)
    /* not sure from mongo docs as to whether I need to catch exceptions
     * and check for false returns from the functions or not. */
    {
        return false;
    }
    return true;
}

csConnect::Session::~Session()
{
    delete db;
}

bool csConnect::Session::create(cJSON *id_object, cJSON *create_obj)
{
    try
    {
        mongo::BSONObj create_b = mongo::BSONObj(cJSON_PrintUnformatted(create_obj));
        db->insert("devel_test", create_b);
    }
    catch (...)
    {
        return false;
    }
    return true;
    
    
    return false;
}
