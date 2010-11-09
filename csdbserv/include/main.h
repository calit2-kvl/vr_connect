/*
+-------------------------------------------------------------------------------+
| @BEGIN_COPYRIGHT                                                             	|
|        									|
| Copyright (C) 2010-2015, The Regents of the University of California &       	|
| King Abdula University of Science and Technology                              |
| All Rights Reserved.                                                          |
|                                                                               |
|                                                                               |
| Prototyped and developed By:                                                  |
|    Kai-Uwe Doerr       {kdoerr@usd.edu}                                       |
|    Christopher Knox  	 {christopher.knox@kaust.edu.sa}                        |
|        									|
| @END_COPYRIGHT                                                           	|
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

#ifndef CV_CS_DB_SERV_
#define CV_CS_DB_SERV_


#include "cglXNet.h"
#include <signal.h>

// -----------------------------------------------------------------------------+
// CLASS PROTOTYPE
// -----------------------------------------------------------------------------+
// Create your own server class
// prototype (Sub-Classing method)
// ------------------------------------------------------------------------------------------
class dbserver: public cglXServer
{

public:
  // constructor
  dbserver(cs_hci_type_E type = CS_HCI_UNDEF, int port=-1, cs_serv_mode_E mode = CS_SERV_PASSIVE);
  // destructor
  ~dbserver();

  // signal connect
  void signal_connected(const int PID = -1, const char *IP=NULL, const int World=-1, const int UID=-1 );

  // signal disconnect
  void signal_disconnected(const int PID = -1, const char *IP=NULL, const int World=-1, const int UID=-1);

  // signal message
  void signal_clientdata (const int PID = -1, const char *IP=NULL, const int World=-1, const int UID=-1, unsigned char *data = NULL, unsigned int size = 0);
protected:

private:

};

#endif
