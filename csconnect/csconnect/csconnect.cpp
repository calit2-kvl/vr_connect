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
#include <iostream>
#include <set>

using mongo::BSONObj;
using mongo::BSONElement;
using mongo::Query;

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

bool csConnect::Session::create(cJSON *id_object, const std::string& ns, const cJSON *create_obj)
{
    try
    {
        /* insert the object into the database */
        cJSON *cid_obj = cJSON_GetObjectItem(const_cast<cJSON *>(create_obj), "CID");
        if (!cid_obj)
            return false;
        int cid = cid_obj->valueint;
        std::string json_str = cJSON_PrintUnformatted(const_cast<cJSON *>(create_obj));
        BSONObj create_b = mongo::fromjson(json_str);
        db->insert(ns, create_b);
        
        /* find out what its ID is */
        BSONObj query_obj = BSON("CID" << cid);
        auto_ptr<mongo::DBClientCursor> cursor = db->query(ns, BSONObj());
        
        if (!cursor->more())
            return false;
        
        BSONObj result_b = cursor->next();

        
        BSONElement new_id;
        result_b.getObjectID(new_id);

        cJSON_AddStringToObject(id_object,"CMD", "UPDATE");
        cJSON_AddStringToObject(id_object,"ATTR", "OID");
        cJSON_AddNumberToObject(id_object, "CID", cid);
        cJSON_AddStringToObject(id_object, "OID", new_id.OID().toString().c_str());
        /* we don't want there to be anymore */
        if (cursor->more())
            return false;
    }
    catch (mongo::DBException &e)
    {
        return false;
    }
    return true;
}

bool csConnect::Session::destroy(const std::string& ns, const cJSON *destroy_obj)
{
    /* passing a NULL destroy object will erase the whole collection */
    
    if (!destroy_obj)
        db->remove(ns, BSONObj());
    else 
        return false; /* not implemented yet */
    return true;
}

bool csConnect::Session::update(const std::string& ns, const cJSON *update_obj)
{
    if (!update_obj)
        return false;
    
    char *json_str = cJSON_PrintUnformatted(const_cast<cJSON *>(update_obj));
    BSONObj bson_obj = mongo::fromjson(json_str);
    free(json_str);

    BSONElement id_elem;
    if (bson_obj.getObjectID(id_elem))
        return false; /* should never pass in an object with a bson id */

    BSONElement oid_elem = bson_obj["OID"];
    if (oid_elem.eoo())
        return false;

    std::string oid_str = oid_elem.String();
    mongo::OID new_oid;
    new_oid.init(oid_str);
    BSONObj new_obj = BSON("_id" << new_oid);

    std::vector<BSONElement> elem_list;
    bson_obj.elems(elem_list);

    mongo::BSONObjBuilder builder;
    builder.appendElements(new_obj);
    for (std::vector<BSONElement>::iterator it = elem_list.begin(); it != elem_list.end(); ++it)
    {
        if (strcmp("OID", it->fieldName()) != 0)
            builder.append(*it);
    }
    
    BSONObj final = builder.obj();
    
    Query query = QUERY("_id" << new_oid);
    try
    {
        db->update(ns, query, final);
    }
    catch(mongo::AssertionException& e)
    {
        return false;
    }
                         
    return true;
}

bool csConnect::Session::read(std::vector<cJSON *>& objects, const std::string& ns, const cJSON *query_obj)
{
    if (query_obj)
        return false; /* not implemented yet */
    auto_ptr<mongo::DBClientCursor> cursor = db->query(ns, mongo::BSONObj());
    while (cursor->more())
    {
        BSONObj current = cursor->next();

        BSONElement id_elem;
        current.getObjectID(id_elem);
        BSONObj oid_obj = BSON("OID" << id_elem.OID().toString());

        mongo::BSONObjBuilder builder;
        builder.appendElements(oid_obj);
        builder.appendElements(current);
        
        BSONObj new_obj = builder.obj();

        std::string json_str = new_obj.jsonString();
        cJSON *json_obj = cJSON_Parse(json_str.c_str());
        cJSON_DeleteItemFromObject(json_obj, "_id");
        objects.push_back(json_obj);
    }
    
    return true;
}

bool csConnect::Session::load(std::vector<cJSON *>& session, const std::string& session_name)
{
    
    return read(session, session_name, NULL);
}


