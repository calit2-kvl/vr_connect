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
#include <string>

// -----------------------------------------------------------------------------+
// GLOBALS
// -----------------------------------------------------------------------------+
dbserver   *session_server  = NULL;

// -----------------------------------------------------------------------------+
// CLASS IMPLEMENTATION
// -----------------------------------------------------------------------------+
dbserver::dbserver( cs_hci_type_E type, int port, cs_serv_mode_E mode)
 : cglXServer( type, port, mode)

{

}
dbserver::~dbserver()
{

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
    //printf("Got: %s\n",message.c_str());

    // ---------------------------------------
    // We need to define the protocoll to talk
    // with the database server and add it to
    // the message.
    // ---------------------------------------
    // Possible message types are:
    //
    //  - CREATE:
    //  - UPDATE:
    //  - DELETE:
    //  - CLONE:
    //  - SAVE:
    //
    //  (Chris and Kai will define the protocol)
    //
    // ---------------------------------------
    // for now we just send the URL back
    // ---------------------------------------

    cJSON *json_rec, *item;
    char *out, *send_out;
    json_rec    = cJSON_Parse(message.c_str());
    item        = cJSON_GetObjectItem(json_rec,"CMD");

    // -------------------------------------
    // Command type reaction
    // -------------------------------------
    if(strcmp(item->valuestring,"CREATE")==0)
    {
        // ----------------------------------------
        // read client object id
        // ----------------------------------------
        //item=cJSON_GetObjectItem(json_rec,"CID");
        //int _cid=item->valueint;
        int _cid=cJSON_GetObjectItem(json_rec,"CID")->valueint;

        // ----------------------------------------
        // create a unique object ID
        // ----------------------------------------
        uint64_t    uoid    = 0;

        // ----------------------------------------
        // create a json message
        // ----------------------------------------
        cJSON *json_send;
        json_send=cJSON_CreateObject();
        cJSON_AddStringToObject(json_send,"CMD",     "UPDATE");
        cJSON_AddNumberToObject(json_send,"CID",     _cid);
        cJSON_AddNumberToObject(json_send,"OID",     uoid);
        send_out=cJSON_PrintUnformatted(json_send);
        cJSON_Delete(json_send);

        // ----------------------------------------
        // we send this only to the creating client
        // to update his object map
        // ----------------------------------------
        sendData((char*) send_out, strlen(send_out), PID);
        free(send_out);
    }
    else if(strcmp(item->valuestring,"DELETE")==0)
    {

    }
    else if(strcmp(item->valuestring,"CLONE")==0)
    {

    }
    else if(strcmp(item->valuestring,"SAVE")==0)
    {

    }


    out=cJSON_Print(json_rec);
    cJSON_Delete(json_rec);
    printf("%s\n",out);
    free(out);


    sendData((char*)data, size);
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
