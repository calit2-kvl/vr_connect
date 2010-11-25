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
 *
 *  In this version all communication is carried out by passing around
 *  strings which contain json documents. In a future version I hope
 *  to replace this with bson documents
 */

/* forward declarations */
struct cJSON;
namespace mongo {class DBClientConnection;}
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
    
    namespace OIDOps
    {
        std::string toString(const OID& oid);
        
    }

    class Session
    {
    public:
        Session();
        ~Session();

        /** connect to the database - disconnection will happen in dtor. */
        bool connect(const std::string& database_server);
        /** return a comma separated list of available sessions. */
        bool getSessionNames(std::string& sessions);

        /* not happy with the use of ns here - too mongo like - this will likely change */
        bool create(cJSON *id_object, const std::string& ns, cJSON *create_obj);
        bool update(const std::string& ns, const cJSON *update_obj);
        bool destroy(const std::string& ns, const cJSON *destroy_obj);
        bool load(cJSON *session, const std::string& session_name);
        bool save(const std::string& session_name);

    private:
        mongo::DBClientConnection *db;
    };
}


#endif /* __cplusplus */

#endif
