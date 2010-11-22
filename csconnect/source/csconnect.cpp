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

csConnect::Session::Session(
                            csConnect::SessionInfo& info, 
                            const std::string& database_server, 
                            const std::string& Session_name)
{
    pimpl = new Impl;
    std::string errorstr;
    mongo::HostAndPort host(database_server);
    bool success = pimpl->mongoConnection.connect(host, errorstr);
    if (!success)
    {
        std::error << errorstr << "\n";
    }
    
    
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
    return true;
}

bool csConnect::Session_utils::createImageView(
                                               csConnect::ImageView& view,
                                               csConnect::Session& db_serv)
{
    return true;
}
