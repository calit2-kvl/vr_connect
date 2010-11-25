/*
+-------------------------------------------------------------------------------+
| @BEGIN_COPYRIGHT                                                             	|
|                                                                               |
| Copyright (C) 2010-2015, The Regents of the University of California &       	|
| King Abdula University of Science and Technology                              |
| All Rights Reserved.                                                          |
|                                                                               |
|                                                                               |
| Prototyped and developed By:                                                  |
|    Kai-Uwe Doerr       {kdoerr@usd.edu}                                       |
|    Christopher Knox  	 {christopher.knox@kaust.edu.sa}                        |
|                                                                               |
| @END_COPYRIGHT                                                                |
+-------------------------------------------------------------------------------+
|
| Component  :
| Filename   :
| Sourcefile :
| Language   : C++
|
+----------------------------- Revision Information-----------------------------+
| $Author: kdoerr $
| $Revision: 1.1.1.1 $
| $Date: 2010/07/22 19:14:22 $
| $RCSfile: main.h,v $
+-------------------------------------------------------------------------------+
*/
// -----------------------------------------------------------------------------+
// Includes
// -----------------------------------------------------------------------------+
#include "main.h"
#include <list>
#include <string>
#include <iostream>
#include <assert.h>
#include "csconnect.h"

// -----------------------------------------------------------------------------+
// GLOBALS
// -----------------------------------------------------------------------------+
dbserver   *session_server  = NULL;
class IDPool
{
public:
    IDPool(int start_id=0){base_id_range = start_id;}
    ~IDPool(){id_pool.clear();}

    int getID(void)
    {
        // --------------------------------
        // if pool is empty
        // we return the first available id
        // which is base_id_range.
        // --------------------------------
        if(id_pool.size()==0)
        {
            id_pool.push_back (base_id_range);
            return (base_id_range);
        }

        int freeID 	= -1;
        int count	= 0;

        // check if we can re-use an id
        std::list<int>::iterator it;
        for ( it=id_pool.begin() ; it != id_pool.end(); it++ )
        {
            if(*it == -1)
            {
                freeID	= count + base_id_range;
                *it		= freeID;
                return freeID;
            }
            count++;
        }
        freeID	= count+base_id_range;

        // we add an id if all are taken
        id_pool.push_back (freeID);
        return freeID;
    }
    void returnID(int id)
    {
        std::list<int>::iterator it;
        for ( it=id_pool.begin() ; it != id_pool.end(); it++ )
        {
            if(*it == id)
            {
                *it=-1;
            }
        }
    }
protected:
private:
    int		base_id_range;
    std::list<int> id_pool;
};
IDPool  obj_id_pool(100); // start with 100 as the first object ID

// -----------------------------------------------------------------------------+
// CLASS IMPLEMENTATION
// -----------------------------------------------------------------------------+
dbserver::dbserver( cs_hci_type_E type, int port, cs_serv_mode_E mode)
 : cglXServer( type, port, mode)

{
    _dbconnect = new csConnect::Session;
    _dbconnect->connect("109.171.139.4");

    _sessionname = "cglX.default";
}
dbserver::~dbserver()
{
    delete _dbconnect;
}
// signal connect
void dbserver::signal_connected(const int PID, const char *IP, const int World, const int UID )
{
   printf("My Server PID: %d CONNECTED to client: %s UID: %d in World %d \n", PID, IP, UID, World );
}
// signal disconnect
void dbserver::signal_disconnected(const int PID, const char *IP, const int World, const int UID)
{
   printf("My Server PID: %d DISCONNECTED from client: %s UID: %d in World %d \n", PID, IP, UID, World );
}
// signal message
void dbserver::signal_clientdata (const int PID, const char *IP, const int World, const int UID, unsigned char *data, unsigned int size)
{
    // very simple
    std::string message;
    message.assign((const char*)data,size);
    //printf("My Server PID: %d Client Message: %s UID: %d msg-> %s \n",PID ,IP , UID, message.c_str());

    // ---------------------------------------
    // We need to define the protocoll to talk
    // with the database server and add it to
    // the message.
    // ---------------------------------------
    // Possible message types are:
    //
    //  - CREATE:
    //  - UPDATE: (OID,POS,...)
    //  - DELETE:
    //  - SAVE:
    //
    //  (Chris and Kai will define the protocol)
    //
    // ---------------------------------------
    cJSON *json_read, *item;
    char *out, *send_out;
    json_read   = cJSON_Parse(message.c_str());
    item        = cJSON_GetObjectItem(json_read,"CMD");

    // -------------------------------------
    // Command type reaction
    // -------------------------------------
    if(strcmp(item->valuestring,"CREATE")==0)
    {
#if 0
        // ----------------------------------------
        // read client object id
        // ----------------------------------------
        int _cid=cJSON_GetObjectItem(json_read,"CID")->valueint;

        // ----------------------------------------
        // create a unique object ID
        // (Don't want to create Chris connect instance)
        // for now we create our own id the CGLX way.
        // ----------------------------------------
        uint64_t    uoid    = (uint64_t)obj_id_pool.getID();

        // ----------------------------------------
        // create a json message
        // ----------------------------------------
        cJSON *json_send=NULL;
        json_send=cJSON_CreateObject();
        cJSON_AddStringToObject(json_send,"CMD",     "UPDATE");
        cJSON_AddStringToObject(json_send,"ATTR",    "OID");
        cJSON_AddNumberToObject(json_send,"CID",     _cid);
        cJSON_AddNumberToObject(json_send,"OID",     uoid);
        send_out=cJSON_PrintUnformatted(json_send);
        cJSON_Delete(json_send);
        json_send=NULL;
#else
        cJSON *json_send=NULL;
        json_send=cJSON_CreateObject();
	bool success = _dbconnect->create(json_send, _sessionname, json_read);
        send_out=cJSON_PrintUnformatted(json_send);
#endif
        // ----------------------------------------
        // we send this only to the creating client
        // to update his object map
        // ----------------------------------------
        setWaitTime (0);
        sendData((char*) send_out, strlen(send_out), PID);
        free(send_out);

        // ----------------------------------------
        //  Send a CREATE to all other clients
        // ----------------------------------------
#if 0
        json_send=cJSON_CreateObject();
        cJSON_AddStringToObject(json_send,"CMD",     "CREATE");
        cJSON_AddNumberToObject(json_send,"OID",     uoid);
        cJSON_AddStringToObject(json_send,"TYPE",    "Image");
        cJSON_AddStringToObject(json_send,"URI",     cJSON_GetObjectItem(json_read,"URI")->valuestring);
        cJSON_AddNumberToObject(json_send,"WIDTH",   cJSON_GetObjectItem(json_read,"WIDTH")->valueint);
        cJSON_AddNumberToObject(json_send,"HEIGHT",  cJSON_GetObjectItem(json_read,"HEIGHT")->valueint);

        // ------------------------------------
        // if we have a position
        // ------------------------------------
        item        = cJSON_GetObjectItem(json_read,"POS");
        if(item!=NULL)
        {
            float pos[2];
            pos[0]  = cJSON_GetArrayItem(item, 0)->valuedouble;
            pos[1]  = cJSON_GetArrayItem(item, 1)->valuedouble;
            cJSON_AddItemToObject(json_send,"POS", cJSON_CreateFloatArray(pos,2));
        }
        // ------------------------------------
        // if we have a scale
        // ------------------------------------
        item        = cJSON_GetObjectItem(json_read,"SCALE");
        if(item!=NULL)
        {
            float scale[2];
            scale[0]    = cJSON_GetArrayItem(item, 0)->valuedouble;
            scale[1]    = cJSON_GetArrayItem(item, 1)->valuedouble;
            cJSON_AddItemToObject(json_send,"SCALE", cJSON_CreateFloatArray(scale,2));
        }
#else
	std::vector<cJSON *> objs;	
	_dbconnect->read(objs, _sessionname, json_send);
	assert(objs.size() == 1);
#endif

        send_out=cJSON_PrintUnformatted(objs[0]);
        //printf(send_out);
        //fflush(stdout);
        cJSON_Delete(json_send);

        setWaitTime (0);
        sendData((char*) send_out, strlen(send_out), PID, true);
        free(send_out);
    }
    else if(strcmp(item->valuestring,"UPDATE")==0)
    {
        // ----------------------------------------
        //  Send an UPDATE to all other clients
        // ----------------------------------------
        send_out=cJSON_PrintUnformatted(json_read);
	_dbconnect->update(_sessionname, json_read);
        setWaitTime (0);
        sendData((char*) send_out, strlen(send_out), PID, true);
        free(send_out);
    }
    else if(strcmp(item->valuestring,"DELETE")==0)
    {
        uint64_t _oid     = cJSON_GetObjectItem(json_read,"OID")->valueint;
        // ----------------------------------------
        //  Send an UPDATE to all other clients
        // ----------------------------------------
	_dbconnect->destroy(_sessionname, json_read);
        send_out=cJSON_PrintUnformatted(json_read);
        setWaitTime (0);
        sendData((char*) send_out, strlen(send_out), PID, true);
        free(send_out);
    }
    else if(strcmp(item->valuestring,"SAVE")==0)
    {
        std::string session("SAVE: ");
        session.append(cJSON_GetObjectItem(json_read,"SESSION")->valuestring);
        std::cout << session.c_str() << std::endl;
    }
    else if(strcmp(item->valuestring,"LOAD")==0)
    {
        std::string session("LOAD: ");
        session.append(cJSON_GetObjectItem(json_read,"SESSION")->valuestring);
        std::cout << session.c_str() << std::endl;
    }
    out=cJSON_Print(json_read);
    cJSON_Delete(json_read);
    //printf("%s\n",out);
    free(out);
}
// ------------------------------------------------------------------------------------------
//   Basic signal handling
// ------------------------------------------------------------------------------------------
void GiveUsage(char *cmd)
{
    fprintf(stderr,"\nUsage: %s [-h] [-p] <port> \n",cmd);
    fprintf(stderr,"          -h    this text\n");
    fprintf(stderr,"          -p    <num> port number (CGLX command port. Use this parameter only\n");
    fprintf(stderr,"                in case you want to run multiple servers on one workstation)\n");
    fprintf(stderr,"                Please see the documentation on this feature.\n\n");
    fprintf(stderr,"Default values are: -p <5510>\n\n");
    fprintf(stderr,"Examples: \n");
    fprintf(stderr,"%s                      Starts server with default contact port 5510\n",cmd);
    fprintf(stderr,"%s -p 5511              Starts server with contact port 5511\n\n",cmd);
}
void startServer(int _contact_port=-1)
{
    session_server	= new dbserver(CS_HCI_APPL_SERV, _contact_port);
}
void stopServer(void)
{
    if(session_server!=NULL) delete  session_server;
}
void signal_handler(int sig) {
    switch(sig)
    {
        case SIGABRT:	{printf("**SIGNAL: Abnormal termination**\n");}break;
        case SIGFPE:	{printf("**SIGNAL: Erroneous arithmetic operation**\n");}break;
        case SIGILL:	{printf("**SIGNAL: An invalid object program has been detected**\n");}break;
        case SIGINT:	{/*printf("**SIGNAL: Interactive attention signal**\n")*/;}break;           // CTLX+C exit
        case SIGSEGV:	{printf("**SIGNAL: Invalid storage access**\n");}break;
        case SIGTERM:	{printf("**SIGNAL: Termination request made to the program**\n");}break;
        case SIGKILL:	{printf("**SIGNAL: KILLED**\n");}break;
        case SIGHUP:	{printf("**SIGNAL: HUP**\n");}break;
        default:		{;}
    }
    // --------------------------------------------
    // stop the server and exit
    // --------------------------------------------
    stopServer();
    exit(0);
}

// -----------------------------------------------------------------------------+
// MAIN
// -----------------------------------------------------------------------------+
int main( int argc, char ** argv )
{
    // --------------------------------------------
    // initialize signal handler
    // --------------------------------------------
    (void) signal(SIGABRT,	signal_handler);
    (void) signal(SIGFPE,	signal_handler);
    (void) signal(SIGILL,	signal_handler);
    (void) signal(SIGINT,	signal_handler);
    (void) signal(SIGSEGV,	signal_handler);
    (void) signal(SIGTERM,	signal_handler);
    (void) signal(SIGKILL,	signal_handler);
    (void) signal(SIGHUP,	signal_handler);

    // --------------------------------------------
    //	Initialize
    // --------------------------------------------
    int 			contact_port 		= -1;			// cglx will use the default port

    // --------------------------------------------
    // parse the arguments
    // --------------------------------------------
    for (int i=1;i<argc;i++) {
        if (strcmp(argv[i],"-h") == 0)
        {
            GiveUsage(argv[0]);
            exit(0);
        }
        if (strcmp(argv[i],"-p") == 0)
        {
            if(argv[i+1] !=NULL)
            {
                contact_port = atoi(argv[i+1]);
                i++;
            }
        }
    }

    // --------------------------------------------
    // start the server
    // --------------------------------------------
    startServer(contact_port);

    // --------------------------------------------
    // enter a loop
    // --------------------------------------------
    while (session_server->inUse())
    {
        // ----------------------------------------
        // some database management
        // ----------------------------------------
        usleep(100);
    }

    // --------------------------------------------
    // stop the server
    // --------------------------------------------
    stopServer();

    return 0;
}
