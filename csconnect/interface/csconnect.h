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
 | Component  : csConnect interface
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
 *  @namespace csConnect
 *  @brief wraps the functionality need to prototype vr_connect ideas.
 * 
 *  This namespace contains the classes/interfaces/structs used to prototype
 *  a simple version of the vr_connect protocol for a basic web aware image 
 *  viewer.
 */
namespace csConnect
{
    struct OID
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
        OID() : a(0), b(0) {}
    };
    
    inline bool operator==(const OID& l, const OID& r) { return l.a == r.a && l.b == r.b; }
    inline bool operator!=(const OID& l, const OID& r) { return l.a != r.a || l.b != r.b; }
    
    /* simple vec3 wrapper */
    struct Vec3
    { 
        float x, y, z;
    };
    
    struct ObjectData
    {
        ObjectData() : client_id(-1), user_id(-1) {}
        OID Session_id;
        OID object_id;
        int client_id;
        int user_id;
        int local_id;
    };
    
    /** data required to describe an image source */
    struct ImageSource : ObjectData
    {
        ImageSource() : width(-1), height(-1) {}
        
        std::string uri;
        int width;
        int height;    
    };
    
    /** data required to describe an image view */
    struct ImageView : ObjectData
    {
        ImageView() : source(NULL) {}
        Vec3 position;
        Vec3 scale;
        ImageSource *source;
    };
    
    /**
     *  @struct SessionInfo
     *  
     *  wraps the information needed to describe a Session.
     */
    struct SessionInfo
    {
        SessionInfo() {}
        
        OID Session_id;
        std::vector<ImageSource> sources;
        std::vector<ImageView> views;
    };
    
    
    /** 
     *  @class Session
     *  @brief prototype interface to the vr_connect interface for images.
     *
     *  @author C. Knox
     *  @date 2010
     */
    class Session
    {
    public:
        /* connect to or create a Session - info struct is filled with the current state of the Session */
        Session(SessionInfo& info, const std::string& database_server, const std::string& Session_name);
        ~Session();
        
        bool getSessionUpdates(SessionInfo& info);

        SessionInfo m_info;
    private:
        struct Impl;
        Impl *pimpl;
    };
    
    namespace Session_utils
    {
        /** provides a comma separated list of the available Sessions. */
        bool serverSessions(std::string& Session_list, const std::string& server_name);
        
        /** create an image */
        bool createImageSource(ImageSource& source, Session& db_serv);
        /** add image metadata */
        bool updateImageSource(ImageSource& source, Session& db_serv);
        /** create an image view */
        bool createImageView(ImageView& view, Session& db_serv);
        /** update an ImageView */
        bool updateImageView(ImageView& view, Session& db_serv);
    }
}




#endif /* __cplusplus */

#endif
