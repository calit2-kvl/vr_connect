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
#include <QWebElement>
//#include <QWebElementCollection>

#include <QMenu>
#include <QFileDialog>
#include "main.h"


int contact_port    = 5510;

// -----------------------------------------------------------------------------+
// CLASS IMPLEMENTATION
// -----------------------------------------------------------------------------+
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Create your own server class
// implementation (Sub-Classing method)
// -----------------------------------------------------------------------------
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

// ------------------------------------------
//  csPAGE
// ------------------------------------------
csPage::csPage()
{

}
// ------------------------------------------
//  csWebView
// ------------------------------------------
csWebView::csWebView(QWidget *parent)
    :QWebView(parent)
{
    // ------------------------------
    // define additional actions
    // ------------------------------
    sendToClientAct = new QAction(tr("&Send to CGLX"), this);

    // ------------------------------
    // reproduce default context menu
    // for images on the web
    // ------------------------------
    connect(pageAction ( QWebPage::DownloadImageToDisk ),   SIGNAL(triggered()), this, SLOT(DownloadImg()));
    connect(pageAction ( QWebPage::OpenImageInNewWindow ),  SIGNAL(triggered()), this, SLOT(OpenImg()));
    connect(pageAction ( QWebPage::CopyImageToClipboard ),  SIGNAL(triggered()), this, SLOT(CopyImg()));
    connect(sendToClientAct,                                SIGNAL(triggered()), this, SLOT(SendToCGLX()));
}
void csWebView::SendToCGLX()
{
    //printf("Hello at x: %d y: %d\n", hit_pos.x(),  hit_pos.y());
    QWebFrame *tframe=page()->currentFrame ();
    QWebHitTestResult	hitresult = tframe->hitTestContent ( hit_pos );
    if(!hitresult.isNull())
    {
        QPixmap img = hitresult.pixmap();
        if(!img.isNull())
        {
            //printf("Width: %d Height: %d Link: %s\n",img.width(), img.height(),hitresult.imageUrl().toString().toStdString().c_str());
            emit send_image(img.width(), img.height() , hitresult.imageUrl());
        }
    }
}
void csWebView::DownloadImg()
{
    printf("DownloadImg()\n");
}
void csWebView::OpenImg()
{
    printf("OpenImg()\n");
}
void csWebView::CopyImg()
{
    printf("CopyImg()\n");
}
void csWebView::contextMenuEvent(QContextMenuEvent *event)
{
    hit_pos.setX(event->x());
    hit_pos.setY(event->y());

    QMenu *editContextMenu=page()->createStandardContextMenu();
    editContextMenu->addAction(sendToClientAct);
    editContextMenu->exec(event->globalPos());
}

// ------------------------------------------
//  csImgGoogle MainWindow
// ------------------------------------------
csImgGoogle::csImgGoogle( QWidget *parent)
 : QMainWindow( parent)
{
    // ---------------------------
    // Setup the MainGUI
    // ---------------------------
    setupUi(this);

    // page attributes
    page.settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);

    myWebView = new csWebView(centralwidget);
    myWebView->setObjectName(QString::fromUtf8("myWebView"));
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(myWebView->sizePolicy().hasHeightForWidth());
    myWebView->setSizePolicy(sizePolicy);
    myWebView->setMinimumSize(QSize(500, 0));
    myWebView->setUrl(QUrl("http://www.google.com/imghp"));
    gridLayout->addWidget(myWebView, 1, 0, 1, 1);

    // ---------------------------
    // create the cglx server
    // ---------------------------
    connected   = false;

    sm	= new myserver(CS_HCI_CUST_SERV,contact_port, CS_SERV_ACTIVE);

    // ---------------------------
    // connect slots and signals
    // ---------------------------
    connect(actionExit, SIGNAL(triggered()), this, SLOT(fileExit()));
    connect(actionConnect, SIGNAL(triggered()), this, SLOT(cs_connect()));
    connect(actionDisconnect, SIGNAL(triggered()), this, SLOT(cs_disconnect()));
    connect(actionLoad_Image, SIGNAL(triggered()), this, SLOT(cs_loadImages()));


    connect(myWebView, SIGNAL(send_image(int,int,const QUrl &)), this, SLOT(sendImgURIToClient(int,int,const QUrl &)));

    sm->setGoogler(this);

    // web inspector
    inspector   = new QWebInspector();
    inspector->setPage(myWebView->page());
}
csImgGoogle::~csImgGoogle()
{
    // ---------------------------
    // delete the cglx server
    // ---------------------------
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

void csImgGoogle::cs_loadImages()
{
    QStringList fl;
    QFileDialog	filedialog(this);
    filedialog.setDirectory(getenv("HOME"));
    QStringList filters;
    filters << "Image files (*.png *.xpm *.jpg *.tif *.gif)"
            << "Any files (*)";
    filedialog.setNameFilters(filters);
    filedialog.setAcceptMode ( QFileDialog::AcceptOpen );
    filedialog.setFileMode(QFileDialog::ExistingFiles);
    filedialog.setWindowTitle ( QString("Open Image Files") );
    if(filedialog.exec())
    {
        fl  = filedialog.selectedFiles();
        for (int i = 0; i < fl.size(); ++i)
        {
            QPixmap pix(fl.at(i));
            //printf("W: %d H: %d File: %s\n",pix.width(),pix.height(),fl.at(i).toStdString().c_str());
            if(connected)
            {
                char *out;
                cJSON *json_send;
                json_send=cJSON_CreateObject();
                cJSON_AddStringToObject(json_send,"CMD",     "CREATE");
                cJSON_AddStringToObject(json_send,"TYPE",    "Image");
                cJSON_AddNumberToObject(json_send,"OID", -1);
                cJSON_AddStringToObject(json_send,"URI",     fl.at(i).toStdString().c_str());
                cJSON_AddNumberToObject(json_send,"WIDTH",   pix.width());
                cJSON_AddNumberToObject(json_send,"HEIGHT",  pix.height());


                float pos[2];
                pos[0]  = 0.0f;
                pos[1]  = 0.0f;
                cJSON_AddItemToObject(json_send,"POS", cJSON_CreateFloatArray(pos,2));

                pos[0]  = 1.0f;
                pos[1]  = 1.0f;
                cJSON_AddItemToObject(json_send,"SCALE", cJSON_CreateFloatArray(pos,2));

                out=cJSON_PrintUnformatted(json_send);
                cJSON_Delete(json_send);

                pte_Url->setPlainText(QString(out));
                sm->sendData((char *)out);
                free(out);
            }
        }
    }
}

void csImgGoogle::sendImgURIToClient(int _width, int _height, const QUrl & url )
{
    // add with and hight and make a jason message
    char *out;
    cJSON *json_send;
    json_send=cJSON_CreateObject();
    cJSON_AddStringToObject(json_send,"CMD",     "CREATE");
    cJSON_AddStringToObject(json_send,"TYPE",    "Image");
    cJSON_AddNumberToObject(json_send,"OID", -1);
    cJSON_AddStringToObject(json_send,"URI",     url.toString().toStdString().c_str());
    cJSON_AddNumberToObject(json_send,"WIDTH",   _width);
    cJSON_AddNumberToObject(json_send,"HEIGHT",  _height);


    float pos[2];
    pos[0]  = 0.0f;
    pos[1]  = 0.0f;

    cJSON_AddItemToObject(json_send,"POS", cJSON_CreateFloatArray(pos,2));
    pos[0]  = 1.0f;
    pos[1]  = 1.0f;
    cJSON_AddItemToObject(json_send,"SCALE", cJSON_CreateFloatArray(pos,2));

    out=cJSON_PrintUnformatted(json_send);
    cJSON_Delete(json_send);

    pte_Url->setPlainText(QString(out));
    sm->sendData((char *)out);
    free(out);
}
void csImgGoogle::on_pb_Home_pressed()
{
    myWebView->load (QUrl(QString("http://www.google.com/imghp")));
}
void csImgGoogle::on_pb_Back_pressed()
{
    myWebView->triggerPageAction(QWebPage::Back);
}
void csImgGoogle::on_pb_Forward_pressed()
{
    myWebView->triggerPageAction(QWebPage::Forward);
}

// -----------------------------------------------------------------------------+
// MAIN
// -----------------------------------------------------------------------------+
int main( int argc, char ** argv )
{

    // --------------------------------------------
    // parse the arguments
    // --------------------------------------------
    for (int i=1;i<argc;i++) {
        if (strcmp(argv[i],"-p") == 0)
        {
            if(argv[i+1] !=NULL)
            {
                contact_port = atoi(argv[i+1]);
                i++;
            }
        }
    }

    // ---------------------------
    // start the GUI
    // ---------------------------
    QApplication a( argc, argv );
    csImgGoogle w( NULL);
    w.show();
    a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );
    return a.exec();
    return 0;
}
