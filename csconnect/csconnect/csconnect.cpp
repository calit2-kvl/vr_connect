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

bool csConnect::Session::connect(csConnect::SessionInfo &info, 
                                 std::string const &database_server,  
                                 std::string const &session_name)
{
    std::string errorstr;
    mongo::HostAndPort host(database_server);
    try
    {
        if (!pimpl->mongoConnection.connect(host, errorstr))
            return false;
        
        /* ideally I would like to query the list of database names here.
         * but I cannot work out how to do it at the moment so I am just 
         * going to create a collection with some metadata in it which 
         * will always be present in a csconnect data base as a way of 
         * verifiying whether or not a database exists.
         *
         * actually this may not be necessary as I will probably just 
         * have a object and a position collection so I can just query them.
         */
        auto_ptr<mongo::DBClientCursor> cursor;
        
        cursor = pimpl->mongoConnection.query("tutorial.persons", mongo::BSONObj());
        while( cursor->more() ) 
        { 
            cout << cursor->next().jsonString() << endl;
        }
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

bool csConnect::Session::getSessionUpdates(csConnect::SessionInfo& info)
{
    if (info.Session_id != m_info.Session_id)
        return false;
    
    std::copy(m_info.sources.begin(), m_info.sources.end(), info.sources.begin());
    std::copy(m_info.views.begin(), m_info.views.end(), info.views.begin());
    
    m_info.sources.clear();
    m_info.views.clear();
    return true;
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
