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
 | Filename   : csConnect.cpp
 | Sourcefile :
 | Language   : C++
 |
 +-------------------------------------------------------------------------------+ */

#include "csConnect.h"

#include "dbclient.h"

#include <string.h>
#include <algorithm>
#include <vector>

void oid_gen(csConnect::OID& object_id)
{
    
}

namespace csConnect
{
    class Session::Impl
    {
        mongo::DBClientConnection *mongoConnection;
        
    };
}

csConnect::Session::Session(
                            csConnect::SessionInfo& info, 
                            const std::string& database_server, 
                            const std::string& Session_name)
{
    oid_gen(m_info.Session_id);
    memcpy(info.Session_id.data, m_info.Session_id.data, 12);
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


bool csConnect::Session_utils::createImageSource(
                                                 csConnect::ImageSource& source,
                                                 csConnect::Session& db_serv)
{
    oid_gen(source.object_id);
    db_serv.m_info.sources.push_back(source);
    return true;
}

bool csConnect::Session_utils::createImageView(
                                               csConnect::ImageView& view,
                                               csConnect::Session& db_serv)
{
    oid_gen(view.object_id);
    db_serv.m_info.views.push_back(view);
    return true;
}
