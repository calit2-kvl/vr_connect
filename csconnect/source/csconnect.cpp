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
 | Component  : csconnect implementation
 | Filename   : csconnect.cpp
 | Sourcefile :
 | Language   : C++
 |
 +-------------------------------------------------------------------------------+ */

#include "csconnect.h"
#include <uuid/uuid.h>

#include <string.h>
#include <algorithm>
#include <vector>

void oid_gen(csconnect::oid& object_id)
{
    uuid_t tmp;
    uuid_generate(tmp);
    
    memcpy(object_id.data, tmp, 12);
}

csconnect::session::session(
                            csconnect::session_info& info, 
                            const std::string& database_server, 
                            const std::string& session_name)
{
    oid_gen(m_info.session_id);
    memcpy(info.session_id.data, m_info.session_id.data, 12);
}

bool csconnect::session::get_session_updates(csconnect::session_info& info)
{
    if (info.session_id != m_info.session_id)
        return false;
        
    std::copy(m_info.sources.begin(), m_info.sources.end(), info.sources.begin());
    std::copy(m_info.views.begin(), m_info.views.end(), info.views.begin());

    m_info.sources.clear();
    m_info.views.clear();
    return true;
}


bool csconnect::session_utils::create_image_source(
                                                   csconnect::image_source& source,
                                                   csconnect::session& db_serv)
{
    oid_gen(source.object_id);
    db_serv.m_info.sources.push_back(source);
    return true;
}

bool csconnect::session_utils::create_image_view(
                                                   csconnect::image_view& view,
                                                   csconnect::session& db_serv)
{
    oid_gen(view.object_id);
    db_serv.m_info.views.push_back(view);
    return true;
}
