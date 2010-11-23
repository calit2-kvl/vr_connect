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

#include "dbclient.h"

#include <string>
#include <algorithm>
#include <vector>

namespace csConnect
{
    struct Session::Impl
    {
        mongo::DBClientConnection mongoConnection;
        
    };
}

csConnect::Session::Session()
{
    pimpl = new Impl;
}

//namespace SessionUtils
//{
//    bool 
//    
//}

void collectionAsJson(std::string& output, 
                      mongo::DBClientConnection& conn,
                      const std::string& collection)
{
    auto_ptr<mongo::DBClientCursor> cursor = conn.query(collection, mongo::BSONObj());
    while(cursor->more()) 
        output.append(cursor->next().jsonString());
}

bool csConnect::Session::connect(csConnect::SessionInfo &info, 
                                 std::string const &database_server,  
                                 std::string const &session_name)
{
    std::string errorstr;
    std::string collection;
    
    mongo::HostAndPort host(database_server);

    try
    {
        if (!pimpl->mongoConnection.connect(host, errorstr))
            return false;
        
        collection = session_name + ".sources";
        collectionAsJson(info.sources, pimpl->mongoConnection, collection);

        collection = session_name + ".views";
        collectionAsJson(info.views, pimpl->mongoConnection, collection);

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
    delete pimpl;
}

bool csConnect::SessionUtils::createImageSource(
                                                 csConnect::ImageSource& source,
                                                 csConnect::Session& db_serv)
{
    return true;
}

bool csConnect::SessionUtils::createImageView(
                                               csConnect::ImageView& view,
                                               csConnect::Session& db_serv)
{
    return true;
}
