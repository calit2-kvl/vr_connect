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
| $Revision: 1.0.0.0 $
| $Date: 2010/11/08 19:14:22 $
| $RCSfile: main.h,v $
+-------------------------------------------------------------------------------+
*/
// -----------------------------------------------------------------------------+
// Includes
// -----------------------------------------------------------------------------+

#ifndef CV_CS_DB_SERV_
#define CV_CS_DB_SERV_

#include "cglXNet.h"
#include <signal.h>
#include <stdint.h>
#include "cJSON.h"
#include "csconnect.h"
/*! \file main.h
 *
 */

// -----------------------------------------------------------------------------+
// CLASS PROTOTYPE
// -----------------------------------------------------------------------------+
// Create your own server class
// prototype (Sub-Classing method)
// -----------------------------------------------------------------------------+
/*! \class dbserver
 *  \brief Database server base class.
 *
 *   Database server base class. (cglX).
 *
 *
 *  \author Kai-U. Doerr
 *  \date   2010
 *
 */
class dbserver: public cglXServer
{

public:
  /*! \fn dbserver::dbserver(cs_hci_type_E type = CS_HCI_UNDEF, int port=-1, cs_serv_mode_E mode = CS_SERV_PASSIVE);
   *  \brief Default Constructor.
   */
  dbserver(cs_hci_type_E type = CS_HCI_UNDEF, int port=-1, cs_serv_mode_E mode = CS_SERV_PASSIVE);
  /*! \fn dbserver::~dbserver();
   *  \brief Default Destructor.
   */
  ~dbserver();
  /*! \fn void signal_connected(const int PID = -1, const char *IP=NULL, const int World=-1, const int UID=-1 );
   *  \brief Called when server connects to a client.
   */
  void signal_connected(const int PID = -1, const char *IP=NULL, const int World=-1, const int UID=-1 );
  /*! \fn void signal_disconnected(const int PID = -1, const char *IP=NULL, const int World=-1, const int UID=-1);
   *  \brief Called when server disconnects from a client.
   */
  void signal_disconnected(const int PID = -1, const char *IP=NULL, const int World=-1, const int UID=-1);
  /*! \fn void signal_clientdata (const int PID = -1, const char *IP=NULL, const int World=-1, const int UID=-1, unsigned char *data = NULL, unsigned int size = 0);
   *  \brief Called when server gets a message from a client.
   */
  void signal_clientdata (const int PID = -1, const char *IP=NULL, const int World=-1, const int UID=-1, unsigned char *data = NULL, unsigned int size = 0);
protected:

private:

};

#endif
