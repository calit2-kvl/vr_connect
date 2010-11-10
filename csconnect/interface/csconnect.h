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
 | Component  : csconnect interface
 | Filename   : csconnect.h
 | Sourcefile :
 | Language   : C++
 |
 +-------------------------------------------------------------------------------+ */


#ifndef INCLUDED_CSCONNECT_H
#define INCLUDED_CSCONNECT_H

#ifdef __cplusplus

#include <string>
#include <vector>
#include <stdint.h>

/**
 *  @namespace csconnect
 *  @brief wraps the functionality need to prototype vr_connect ideas.
 * 
 *  This namespace contains the classes/interfaces/structs used to prototype
 *  a simple version of the vr_connect protocol for a basic web aware image 
 *  viewer.
 */
namespace csconnect
{
    struct oid
    {
        union 
        {
            struct 
            {
                uint64_t a;
                uint32_t b;
            };
            uint8_t data[12];
        };
        oid() : a(0), b(0) {}
    };
    
    inline bool operator==(const oid& l, const oid& r) { return l.a == r.a && l.b == r.b; }
    inline bool operator!=(const oid& l, const oid& r) { return l.a != r.a || l.b != r.b; }
    
    /* simple vec3 wrapper */
    struct vec3
    { 
        float x, y, z;
    };
    
    struct object_data
    {
        object_data() : client_id(-1), user_id(-1) {}
        oid session_id;
        oid object_id;
        int client_id;
        int user_id;
    };
    
    /** data required to describe an image source */
    struct image_source : object_data
    {
        image_source() : width(-1), height(-1) {}
        
        std::string uri;
        int width;
        int height;    
    };
    
    /** data required to describe an image view */
    struct image_view : object_data
    {
        image_view() : source(NULL) {}
        vec3 position;
        vec3 scale;
        image_source *source;
    };
    
    /**
     *  @struct session_info
     *  
     *  wraps the information needed to describe a session.
     */
    struct session_info
    {
        session_info() {}
        
        oid session_id;
        std::vector<image_source> sources;
        std::vector<image_view> views;
    };
    
    
    /** 
     *  @class session
     *  @brief prototype interface to the vr_connect interface for images.
     *
     *  @author C. Knox
     *  @date 2010
     */
    class session
    {
    public:
        /* connect to or create a session - info struct is filled with the current state of the session */
        session(session_info& info, const std::string& database_server, const std::string& session_name) {}
        ~session() {}
        
        bool get_session_updates(session_info& info);
    };
    
    namespace session_utils
    {
        /** provides a comma separated list of the available sessions. */
        bool server_sessions(std::string& session_list, const std::string& server_name);
        
        /** create an image */
        bool create_image_source(image_source& source, session& db_serv, uint64_t local_id);
        /** add image metadata */
        bool update_image_source(image_source& source, session& db_serv);
        /** create an image view */
        bool create_image_view(image_view& view, session& db_serv, uint64_t local_id);
        /** update an image_view */
        bool update_image_view(image_view& view, session& db_serv);
        
        
        
    }
}




#endif /* __cplusplus */

#endif
