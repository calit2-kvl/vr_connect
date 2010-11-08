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
#include <QApplication>
#include <QAction>
#include <QNetworkRequest>
#include <QPushButton>
#include <QString>
#include "main.h"

// -----------------------------------------------------------------------------+
// CLASS IMPLEMENTATION
// -----------------------------------------------------------------------------+
// ------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------
// Create your own server class
// implementation (Sub-Classing method)
// ------------------------------------------------------------------------------------------
myserver::myserver( cs_hci_type_E type, int port, cs_serv_mode_E mode)
 : cglXServer( type, port, mode)

{

}

myserver::~myserver()
{
    //printf("Delete Core Server\n");
}
// signal connect
void myserver::signal_connected(const int PID, const char *IP, const int World, const int UID )
{
   printf("My Server PID: %d CONNECTED to client: %s UID: %d in World %d \n", PID, IP, UID, World );
}
// signal disconnect
void myserver::signal_disconnected(const int PID, const char *IP, const int World, const int UID)
{
   printf("My Server PID: %d DISCONNECTED from client: %s UID: %d in World %d \n", PID, IP, UID, World );
   googler->cs_disconnect();
}

csPage::csPage()
{

}
void csPage::triggerAction ( WebAction _action, bool checked)
{
    //bool	isEnabled () const
    //void	setEnabled ( bool )

    if(action(_action)->isEnabled())
    {
         printf("Action is valid\n");
         current_action  = _action;
    }

    // --------------------------------------
    // we are only intercepting
    // the browser will not be effected
    // --------------------------------------
    QWebPage::triggerAction(_action, checked);
}

csImgGoogle::csImgGoogle( QWidget *parent)
 : QMainWindow( parent)
{
    // =================
    // Setup the MainGUI
    // =================


    setupUi(this);
    //global_googler  = this;

    webView->setPage ( &page);
    webView->load (QUrl(QString("http://www.google.com/imghp")));

    connected   = false;

    // create the cglx server
    sm	= new myserver(CS_HCI_CUST_SERV,-1, CS_SERV_ACTIVE);


    connect(actionExit, SIGNAL(triggered()), this, SLOT(fileExit()));
    connect(actionConnect, SIGNAL(triggered()), this, SLOT(cs_connect()));
    connect(actionDisconnect, SIGNAL(triggered()), this, SLOT(cs_disconnect()));

    // download request from page
    connect(webView->page(), SIGNAL(downloadRequested ( const QNetworkRequest & )), this, SLOT(ImgDownload(const QNetworkRequest &)));

    sm->setGoogler(this);
}
csImgGoogle::~csImgGoogle()
{
    // delete the cglx server
    if(sm!=NULL)delete sm;
}
void csImgGoogle::fileExit()
{
    QApplication::exit( 0 );
}


void csImgGoogle::cs_connect()
{
    if(connected)return;
    connected = true;
    pb_ConnectDisconnect->setText ( QString("Disconnect"));
    le_IP->setDisabled (true);
    sb_World->setDisabled (true);
}

void csImgGoogle::cs_disconnect()
{

    if(!connected)return;
    connected = false;
    pb_ConnectDisconnect->setText ( QString("Connect"));
    le_IP->setDisabled (false);
    sb_World->setDisabled (false);
}
void csImgGoogle::on_pb_ConnectDisconnect_pressed()
{
    if(connected)
    {
        cs_disconnect();
        sm->disconnectRequest((char *)le_IP->text().toStdString().c_str(), sb_World->value());
    }
    else
    {
        cs_connect();
        sm->connectRequest((char *)le_IP->text().toStdString().c_str(), sb_World->value());
    }
}

void csImgGoogle::ImgDownload(const QNetworkRequest & request)
{
    const QUrl& url = request.url();

    if (!url.isValid())
    {
         return;
    }

    pte_Url->clear();
    QString mimeType = url.toString();

    switch (page.getCurrentAction())
    {
    case QWebPage::DownloadImageToDisk:
        {
            pte_Url->setPlainText ( mimeType );
            printf("Download URL: %s\n",mimeType.toStdString().c_str());
            sm->sendData((char *)mimeType.toStdString().c_str(),strlen(mimeType.toStdString().c_str()));
        }
        break;
    case QWebPage::DownloadLinkToDisk:
        {
            printf("Download Link: %s\n",mimeType.toStdString().c_str());
        }
        break;
    default:
        {

        }
    }
}

void csImgGoogle::on_pb_Home_pressed()
{
    webView->load (QUrl(QString("http://www.google.com/imghp")));
}

void csImgGoogle::on_pb_Back_pressed()
{
    webView->triggerPageAction(QWebPage::Back);
}
void csImgGoogle::on_pb_Forward_pressed()
{
    webView->triggerPageAction(QWebPage::Forward);
}
// -----------------------------------------------------------------------------+
// MAIN
// -----------------------------------------------------------------------------+
int main( int argc, char ** argv )
{
    // ===============================
    // start the GUI
    // ===============================
    QApplication a( argc, argv );
    csImgGoogle w( NULL);
    w.show();
    a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );
    return a.exec();
    return 0;
}
