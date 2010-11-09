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

#ifndef CV_CS_GOOGLE_GUI_
#define CV_CS_GOOGLE_GUI_

#include <QtGui>
#include "cglXNet.h"
#include "ui_csgoogle.h"
/*! \file main.h
 *
 */
// -----------------------------------------------------------------------------+
// FORWARD DECLARATION
// -----------------------------------------------------------------------------+
class csImgGoogle;

// -----------------------------------------------------------------------------+
// CLASS PROTOTYPE
// -----------------------------------------------------------------------------+
// Create your own server class
// prototype (Sub-Classing method)
// -----------------------------------------------------------------------------+
/*! \class myserver
 *  \brief Image loader server class.
 *
 *   Image loader server base class. (cglX).
 *
 *  \author Kai-U. Doerr
 *  \date   2010
 *
 */
class myserver: public cglXServer
{

public:
  /*! \fn myserver::myserver(cs_hci_type_E type = CS_HCI_UNDEF, int port=-1, cs_serv_mode_E mode = CS_SERV_PASSIVE);
   *  \brief Default Constructor.
   */
  myserver(cs_hci_type_E type = CS_HCI_UNDEF, int port=-1, cs_serv_mode_E mode = CS_SERV_PASSIVE);
  /*! \fn myserver::~myserver();
   *  \brief Default Destructor.
   */
  ~myserver();

  /*! \fn void signal_connected(const int PID = -1, const char *IP=NULL, const int World=-1, const int UID=-1 );
   *  \brief Called when server connects to a client.
   */
  void signal_connected(const int PID = -1, const char *IP=NULL, const int World=-1, const int UID=-1 );
  /*! \fn void signal_disconnected(const int PID = -1, const char *IP=NULL, const int World=-1, const int UID=-1);
   *  \brief Called when server disconnects from a client.
   */
  void signal_disconnected(const int PID = -1, const char *IP=NULL, const int World=-1, const int UID=-1);
  /*! \fn void setGoogler(csImgGoogle* _googler)
   *  \brief Set the googler pointer.
   */
  void setGoogler(csImgGoogle* _googler){googler=_googler;}
protected:
    csImgGoogle *googler;
private:

};

/*! \class csPage
 *  \brief Base page Class for image google.
 *
 *   The basic page class for image google.
 *
 *  \author Kai-U. Doerr
 *  \date    2010
 *
 */
class csPage : public QWebPage
{
    Q_OBJECT
public:
    csPage();
    void triggerAction ( WebAction _action, bool checked = false );
    WebAction getCurrentAction(void){return current_action;}
protected:

private:
    WebAction current_action;
};

/*! \class csImgGoogle
 *  \brief Base Class for image google.
 *
 *   The basic class for image google.
 *
 *  \author Kai-U. Doerr
 *  \date    2010
 *
 */
class csImgGoogle: public QMainWindow, Ui::MainWindow
{
    Q_OBJECT
public:
    /*!	\fn csImgGoogle::csImgGoogle( QWidget *parent, const char* name=0, QApplication * qapp=NULL);
     *	\brief Default Constructor.
     */
    csImgGoogle( QWidget *parent);
    /*!	\fn csImgGoogle::~csImgGoogle();
     *	\brief Default Destructor.
     */
    ~csImgGoogle();

public slots:
    // gui control
    void fileExit();
    void cs_connect();
    void cs_disconnect();
    void on_pb_ConnectDisconnect_pressed();

    // browser control
    void on_pb_Home_pressed();
    void on_pb_Back_pressed();
    void on_pb_Forward_pressed();
    //
    void ImgDownload(const QNetworkRequest & request);

protected:
    csPage      page;
    myserver    *sm;
    bool        connected;
};

#endif
