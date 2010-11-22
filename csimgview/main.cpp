/*
+---------------------------------------------------------------------------------------+
| @BEGIN_COPYRIGHT                                                                      |
|                                                                                       |
| Copyright (C) 2010-2015, The Regents of the University of California &                |
| King Abdula University of Science and Technology                                      |
| All Rights Reserved.                                                                  |
|                                                                                       |
|                                                                                       |
| Prototyped and developed By:                                                          |
|    Kai-Uwe Doerr       {kdoerr@usd.edu}                                               |
|    Christopher Knox  	 {christopher.knox@kaust.edu.sa}                                |
|                                                                                       |
| @END_COPYRIGHT                                                                        |
+---------------------------------------------------------------------------------------+
|
| Component  :
| Filename   :
| Sourcefile :
| Language   : C++
|
+----------------------------- Revision Information-------------------------------------+
| $Author: kdoerr $
| $Revision: 1.1.1.1 $
| $Date: 2010/10/22 19:14:22 $
| $RCSfile: main.cpp,v $
+---------------------------------------------------------------------------------------+
*/
// -------------------------------------------------------------------------------------+
// Includes
// -------------------------------------------------------------------------------------+
#include "main.h"

// -------------------------------------------------------------------------------------+
// Global
// -------------------------------------------------------------------------------------+
int         db_device_ID    = -1;
bool        background_flg  = false;
bool        menu_flg        = false;
float       h2              = 1;
float       w2              = 1;

// loader mutex
pthread_mutex_t obj_list_mutex;

// -------------------------------------------------------------------------------------+
// This is our window list (later on we have objects and then subclass a window)
// -------------------------------------------------------------------------------------+
std::list<csWindow*> m_windowList;                  // list of objects
std::map<int,csWindow*> m_InteractionMap;           // use user id to find object
std::map<uint64_t,csWindow*> m_DBObjectMap;         // use db object id to find object

std::list<csWidget*> m_widgetList;                  // list of widgets

// -------------------------------------------------------------------------------------+
// CLASS IMPLEMENTATION
// -------------------------------------------------------------------------------------+
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
IDPool  my_id_pool;

// this is the fake object id pool
IDPool  fake_oid_pool(0);

// -------------------------------------------------------------------------------------+
// LOADER THREAD
// -------------------------------------------------------------------------------------+
void *loaderTH(void *arg)
{
    if(arg==NULL)
    {
        printf("arg==NULL\n");
        return NULL;
    }

    csWindow  *winobj   = (csWindow  *)(arg);

    // ----------------------------------------
    // Get the image
    // ----------------------------------------
    std::string uri;
    uri.assign(winobj->getURI());

    // ----------------------------------------
    // make sure we don't have white spaces.
    // They are actually invalid for an uri's
    // ----------------------------------------
    // Fortunately magic is using curl and curl
    // will accept a white space replacement
    // with "%20".
    // If we still can't read an image we
    // through an exception.
    // ----------------------------------------
    size_t found        = 0;
    bool    replaced    = false;
    do
    {
        found   = uri.find (" ", found );
        if(found!=std::string::npos)
        {
            uri.replace ( found, 1,   std::string("\%20") );
            found = found+1;
        }
        else replaced = true;
    }
    while(!replaced);

    Magick::Image image;
    try
    {
        // -------------------------
        // ping will get us the size
        // without waiting for the
        // whole image to download
        // -------------------------
        /*
        image.ping (uri.c_str());
        winobj->setImageDim(image.baseColumns(), image.baseRows());
        if(!cglxEM::invokeTriggerLocal(0,0))
        {
            printf("Local Trigger Failed\n");
            fflush(stdout);
        }
        */

        // ---------------------------
        // now we really read the file
        // ---------------------------
        image.read(uri.c_str());
    }
    catch( Magick::Exception &error_ )
    {
       printf("Caught exception: %s\n", error_.what());
       pthread_exit(NULL);
    }

    //printf("Image %s W: %d H: %d\n",image.fileName().c_str(),(int)image.baseColumns(),(int)image.baseRows());
    // set RGBA output format
    image.magick("RGBA");

    // write to BLOB in RGBA format
    image.write(winobj->blob);

    pthread_mutex_lock (&winobj->blob_mutex);
    winobj->blob_update = true;
    pthread_mutex_unlock (&winobj->blob_mutex);

    // ---------------------------------------
    // the simplest way to guarantee a redraw
    // when an images has loaded is with
    // a local event that triggers a redraw.
    // ---------------------------------------
    // !! this is a non synchronized event
    // > still we have not solved for the
    // picking problem whn windim is unknown ...
    // ---------------------------------------
    if(!cglxEM::invokeTriggerLocal(0,0))
    {
        printf("Local Trigger Failed\n");
        fflush(stdout);
    }
    pthread_exit(NULL);
}

// -------------------------------------------------------------------------------------+
// FUNCTION IMPLEMENTATION
// -------------------------------------------------------------------------------------+
void jsonToConsol(cJSON *json_console)
{
    char *read_out;
    read_out=cJSON_Print(json_console);
    printf("%s\n",read_out);
    free (read_out);
}
void transPointerPos(vec2f_P vec)
{
    float 	transx		= 0.0f;
    float 	transy		= 0.0f;
    float	ratio		= cglx::getHeadRatio();
    float	canv_width	= cglx::getHeadWidth();
    float	canv_height	= cglx::getHeadHeight();

    //printf("In-> X: %f Y: %f\n",vec->x,vec->y);
    if(ratio>1)	// wider hight is 1
    {
       vec->y   =  1-(vec->y/(float)canv_height);
       vec->x   =  vec->x/(float)canv_height;
       transy   = -0.5;
       transx   = -0.5*ratio;
    }
    else	// higher with is 1					// higher
    {
        vec->y   =  1-((vec->y-.5*canv_height)/(float)canv_width);
        vec->x   =  vec->x/(float)canv_width;
        transy   = -1;
        transx   = -0.5f;
    }

    vec->x	= 2*(vec->x+transx);
    vec->y	= 2*(vec->y+transy);

    //printf("Out-> X: %f Y: %f\n\n",vec->x,vec->y);
}
void loadImage(csWindow  *_newWin)
{
    // ------------------------------------
    // Init default threads attributes
    // ------------------------------------
    pthread_attr_t pthread_mevent_attr;
    pthread_attr_init(&pthread_mevent_attr);
    pthread_attr_setdetachstate(&pthread_mevent_attr,PTHREAD_CREATE_DETACHED);  // a detatached thread

    // ------------------------------------
    // create the thread
    // ------------------------------------
    pthread_t	loader_thread;
    if(pthread_create(&loader_thread, &pthread_mevent_attr, loaderTH, (void *)_newWin)!=0)
    {
        printf("Failed to create thread\n");
    }
    //printf("Add window with ID: %d \n",newWindow->getOID());
}

int removeObject(uint64_t _oid)
{
    // erase from object map
    csWindow::om_iter map_iter = m_DBObjectMap.find(_oid);
    if(map_iter!=m_DBObjectMap.end())
    {
        m_DBObjectMap.erase(map_iter);
    }

    // erase from object list and delete
    csWindow::list_iter iter2 = m_windowList.begin();
    bool removed  = false;
    int _uid    = -1;
    while ( iter2 != m_windowList.end() && !removed)
    {
        if((*iter2)->getOID()==_oid)
        {
            delete *iter2;
            m_windowList.erase(iter2);
            removed = true;
        }
        iter2++;
    }
    return _uid;
}

bool checkWidgetHit(vec2f_P vec=NULL, int UserID=-1, int button=0, int state=-1, csWindow* _window=NULL)
{
    bool hit  = false;
    csWidget::list_iter w_iter;
    for(w_iter=m_widgetList.begin(); w_iter!=m_widgetList.end(); w_iter++)
    {
        hit = false;
        csWidget* widget= *w_iter;
        if((widget->getPosX()+widget->getWidth()*.5)>vec->x && (widget->getPosX()-widget->getWidth()*.5)<vec->x)
        {
            hit = true;
            if((widget->getPosY()+widget->getHeight()*.5)>vec->y && (widget->getPosY()-widget->getHeight()*.5)<vec->y)
                hit = true;
            else
                hit = false;
        }

        // ----------------------------------
        // for now we have only the dumpster
        // widget. Later on we need to assign
        // actions to the widgets
        // ----------------------------------
        if(hit && state==CGLX_UP)
        {
            // delete the
            if(widget->wtype == csWidget::WDG_DUMP)
            {
                csWindow* window    = NULL;
                int UserID  = cglxEM::getUID();
                csWindow::im_iter iter = m_InteractionMap.find(UserID);
                if(iter!=m_InteractionMap.end())
                {
                    window  = iter->second;
                    uint64_t _oid   = window->getOID();

                    if(window->session_obj)
                    {
                        if(isHead())
                        {
                            // -------------------------------
                            // send a  message to the database
                            // -------------------------------
                            char *send_out;
                            cJSON *json_send;
                            json_send=cJSON_CreateObject();
                            cJSON_AddStringToObject(json_send,"CMD",     "DELETE");
                            cJSON_AddNumberToObject(json_send,"OID",     window->getOID());
                            send_out=cJSON_PrintUnformatted(json_send);
                            cJSON_Delete(json_send);

                            if(!cglxDM::sendData(db_device_ID, (unsigned char*) send_out, strlen(send_out)))
                            {
                                printf("Send Failed\n");
                            }
                            free(send_out);
                        }
                    }

                    // erase from object map
                    m_InteractionMap.erase(iter);
                    // erase from object map and list
                    removeObject(_oid);

                    cglXPostRedisplay();
                }
            }

            if(menu_flg)
            {
                // set widget state
                widget->setState(vec->x, vec->y, button, state);

                switch (widget->wtype)
                {
                case csWidget::WDG_CLEAR:
                    {
                        //printf("CGLX_UP - WDG_LOAD\n");
                    }
                    break;
                case csWidget::WDG_LOAD:
                    {
                        //printf("CGLX_UP - WDG_LOAD\n");
                    }
                    break;
                case csWidget::WDG_SAVE:
                    {
                        //printf("CGLX_UP - WDG_SAVE\n");
                    }
                    break;
                default:
                    {

                    }
                }
            }
        }
        else if(hit && state==CGLX_DOWN)
        {
            if(menu_flg)
            {
                // set widget state
                widget->setState(vec->x, vec->y, button, state);

                switch (widget->wtype)
                {
                case csWidget::WDG_CLEAR:
                    {
                        //printf("CGLX_DOWN - WDG_CLEAR\n");
                        // ----------------------------------
                        // clean up interaction map
                        // ----------------------------------
                        m_InteractionMap.clear();

                        // ----------------------------------
                        // clean up object map
                        // ----------------------------------
                        m_DBObjectMap.clear();

                        // ----------------------------------
                        // clean up our windows
                        // ----------------------------------
                        for(csWindow::list_iter it = m_windowList.begin(); it != m_windowList.end();it++)
                        {
                            delete *it;
                        }
                        m_windowList.clear();
                    }
                    break;
                case csWidget::WDG_LOAD:
                    {
                        //printf("CGLX_DOWN - WDG_LOAD\n");
                        if(isHead())
                        {
                            // -------------------------------
                            // send a  message to the database
                            // -------------------------------
                            char *send_out;
                            cJSON *json_send;
                            json_send=cJSON_CreateObject();
                            cJSON_AddStringToObject(json_send,"CMD",     "LOAD");
                            cJSON_AddStringToObject(json_send,"SESSION",  "dummy session");
                            send_out=cJSON_PrintUnformatted(json_send);
                            cJSON_Delete(json_send);

                            if(!cglxDM::sendData(db_device_ID, (unsigned char*) send_out, strlen(send_out)))
                            {
                                printf("Send Failed\n");
                            }
                            free(send_out);
                        }
                    }
                    break;
                case csWidget::WDG_SAVE:
                    {
                        //printf("CGLX_DOWN - WDG_SAVE\n");
                        if(isHead())
                        {
                            // -------------------------------
                            // send a  message to the database
                            // -------------------------------
                            char *send_out;
                            cJSON *json_send;
                            json_send=cJSON_CreateObject();
                            cJSON_AddStringToObject(json_send,"CMD",     "SAVE");
                            cJSON_AddStringToObject(json_send,"SESSION",  "dummy session");
                            send_out=cJSON_PrintUnformatted(json_send);
                            cJSON_Delete(json_send);

                            if(!cglxDM::sendData(db_device_ID, (unsigned char*) send_out, strlen(send_out)))
                            {
                                printf("Send Failed\n");
                            }
                            free(send_out);
                        }
                    }
                    break;
                default:
                    {

                    }
                }
            }
        }
        else
        {   // mark it red when we hover over the dumpster
            if(_window!=NULL && widget->wtype == csWidget::WDG_DUMP)
            {
                if(hit) _window->deco_type   = DECO_DELETE;
                else    _window->deco_type   = DECO_NDEF;
            }
        }
    }
    return hit;
}

bool checkWinHit(vec2f_P vec, int UserID, int button, int state)
{
    bool hit  = false;
    // ------------------------------------
    // check widget list
    // ------------------------------------

    if(checkWidgetHit(vec, UserID, button, state))
    {
        return true;
    }


    // ------------------------------------
    // check object list
    // ------------------------------------
    hit  = false;
    csWindow::list_iter iter;
    for(iter=m_windowList.begin(); iter!=m_windowList.end(); iter++)
    {
        csWindow* window= *iter;
        if((window->getPosX()+window->getWidth()*.5)>vec->x && (window->getPosX()-window->getWidth()*.5)<vec->x)
        {
            hit = true;
            if((window->getPosY()+window->getHeight()*.5)>vec->y && (window->getPosY()-window->getHeight()*.5)<vec->y)
                hit = true;
            else
                hit = false;
        }

        if(hit)
        {
            if(state==CGLX_DOWN)
            {
                if(window->getUID()==-1)
                {
                    window->setState(vec->x, vec->y, button, state);
                    window->setUID(UserID);
                    m_windowList.erase(iter);
                    m_windowList.push_front (window);

                    // add object to interaction map
                    m_InteractionMap.insert(std::pair<int,csWindow*>(UserID, window));

                    //printf("Set State\n");
                    // -----------------------------------------
                    // we might want to announce this hit to the
                    // database server to lock the object
                    // -----------------------------------------
                    if(db_device_ID!=-1)
                    {

                    }

                    // we return the first match for a user id
                    return(true);
                }
            }
            else
            {
                if(window->getUID() == UserID)
                {
                    // remove object from interaction map
                    m_InteractionMap.erase(UserID);

                    window->resetState();
                    window->setUID(-1);

                    //printf("Reset\n");
                    // -----------------------------------------
                    // we might want to announce this release to
                    // the database server to lock the object
                    // -----------------------------------------
                    if(db_device_ID!=-1)
                    {

                    }

                    // we return the first match for a user id
                    return(true);
                }
            }
        }
        else
        {
            if(window->getUID() == UserID)
            {
                window->resetState();
                window->setUID(-1);

                //printf("Reset\n");
                // -----------------------------------------
                // we might want to announce this release to
                // the database server to lock the object
                // -----------------------------------------
                if(db_device_ID!=-1)
                {

                }
            }
        }
    }
    return false;
}

// -------------------------------------------------------------------------------------+
// CGLX Implementation
// -------------------------------------------------------------------------------------+
void displayfunc(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // ----------------------
    // draw background
    // ----------------------
    if(background_flg)
    {
        glBegin(GL_QUADS);
            glColor3f(0.1,0.5,1.0);
            glVertex3f (-w2, -h2, 0.0);	// bottom left
            glVertex3f (-w2, h2, 0.0);	// top left
            glColor3f(0.0,0.0,0.1);
            glVertex3f (w2, h2, 0.0);	// top right
            glVertex3f (w2, -h2, 0.0);	// bottom right
        glEnd();
    }

    // ----------------------
    // draw axes
    // ----------------------
    //cglXUnitAxes(.1);

    // ----------------------
    // draw the window list
    // ----------------------
    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f,1.0f,1.0f);
    for (csWindow::list_riter riter = m_windowList.rbegin(); riter!=m_windowList.rend(); riter++)
    {
        (*riter)->draw();
    }
    glDisable(GL_TEXTURE_2D);

    // ----------------------
    // draw menu background
    // ----------------------
    if(menu_flg)
    {
        float t_d   = 1.2f;
        glColor3f(0.0f,0.0f,0.0f);
        glBegin(GL_QUADS);
            glVertex2f(-DEF_MENU_X*t_d,-1.0f);
            glVertex2f(-DEF_MENU_X, DEF_MENU_Y);
            glVertex2f(DEF_MENU_X, DEF_MENU_Y);
            glVertex2f(DEF_MENU_X*t_d, -1.0f);
        glEnd();
        if(!background_flg)
        {
            glColor3f(0.6f,0.6f,1.0f);
            glBegin(GL_LINES);
                glVertex2f(-DEF_MENU_X*t_d,-1.0f);
                glVertex2f(-DEF_MENU_X, DEF_MENU_Y);
                glVertex2f(-DEF_MENU_X, DEF_MENU_Y);
                glVertex2f(DEF_MENU_X, DEF_MENU_Y);
                glVertex2f(DEF_MENU_X, DEF_MENU_Y);
                glVertex2f(DEF_MENU_X*t_d, -1.0f);
            glEnd();
        }
    }

    // ----------------------
    // draw the widget on top
    // ----------------------
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glColor3f(0.0f,0.0f,0.0f);
    glEnable(GL_TEXTURE_2D);

    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glEnable(GL_BLEND);
    glDisable(GL_TEXTURE_GEN_S);		// keep
    glDisable(GL_TEXTURE_GEN_T);		// keep
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for (csWidget::list_riter riter = m_widgetList.rbegin(); riter!=m_widgetList.rend(); riter++)
    {
        (*riter)->draw(menu_flg, w2);
    }
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
    glPopAttrib();
}
void init(void)
{ 
    glClearColor (0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_SMOOTH); 							// The Default coloring style 
    //glEnable(GL_DEPTH_TEST);
    GLint params;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &params);
    printf("Texture size limit : %d bytes \n",params);
}
void reshapefunc(int w, int h)
{  
    glViewport (0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    GLdouble aspect	= (GLdouble)w/(GLdouble)h;

    GLdouble left	= -1.0f;
    GLdouble right	= 1.0f;
    GLdouble bottom	= -1.0f;
    GLdouble top	= 1.0f;

    if(aspect < 1.0)
    {
            bottom 	/= aspect;
            top	/= aspect;
    }
    else
    {
            left	*= aspect;
            right	*= aspect;
    }
    cglXOrtho(left, right, bottom, top, -1.0, 1.0);

    cglXPostRedisplay();

    // ---------------------
    // Canvas size
    // ---------------------
    if(isSoloAPP())
    {
        w2	= getHeadRatio();
    }
    else
    {
        w2	= getWallRatio();
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
void normakKeysfunc(unsigned char key, int x, int y)
{
    switch (key)
    {
        case  27:	// the ESC
        {
            cglx::exit(0);
        }
        break;
    }
    if(cglXUpdateFirst())
    {
        switch (key)
        {
            case  'b':	// toggle background
            {
                if(background_flg)background_flg=false;
                else background_flg = true;
            }
            break;
        }
    }cglXUpdateDone();
    cglXPostRedisplay();
}
void customMSGfunc(int len, unsigned char *msg)
{
    // --------------------------------------------------------
    // update the simulation
    // --------------------------------------------------------
    if(cglXUpdateShared())
    {
        // ----------------------------------------------------
        // We talk json
        // ----------------------------------------------------
        cJSON *json_read, *item;
        char *out, *read_out;
        // ----------------------------------------------------
        // check if this message comes from our database server
        // ----------------------------------------------------
        // This is not completely clean, the servers need to
        // identify them because all server types can send
        // custom messages. For now we start only servers
        // that we know ......
        // ----------------------------------------------------
        if(db_device_ID!=-1 && cglxEM::getUID()==db_device_ID)
        {
            // ------------------------------------------
            // This is a message from the database server
            //
            // We need to define the protocol to talk
            // with the database server and add it to
            // the message.
            // ------------------------------------------
            // Possible message types are:
            //
            //  - CREATE:
            //  - UPDATE:
            //  - DELETE:
            //  - CLONE:
            //  - SAVE:
            //  - LOAD:
            //
            //  (Chris and Kai will define the protocol)
            //
            // ------------------------------------------

            // ------------------------------------------
            // read a json message
            // ------------------------------------------
            json_read   = cJSON_Parse((const char*)msg);
            item        = cJSON_GetObjectItem(json_read,"CMD");

            // -------------------------------------
            // Command type interpretation
            // -------------------------------------
            // This is very primitive and unflexible
            // but since we know what we are sending
            // it will do for now.
            // -------------------------------------
            csWindow* window = NULL;
            uint64_t    _oid = -1;
            if(strcmp(item->valuestring,        "UPDATE"    )==0)
            {
                item    = cJSON_GetObjectItem(json_read,"ATTR");
                if(strcmp(item->valuestring,"OID")==0)              // update object ID
                {
                    int   _cid      = cJSON_GetObjectItem(json_read,"CID")->valueint;
                    _oid            = cJSON_GetObjectItem(json_read,"OID")->valueint;

                    csWindow::om_iter iter = m_DBObjectMap.find(_cid);
                    if(iter!=m_DBObjectMap.end())
                    {
                        window = m_DBObjectMap[_cid];
                        window->setOID(_oid);
                        window->session_obj = true;
                        // remove object  from map and add it with a new oid
                        m_DBObjectMap.erase (iter);
                        m_DBObjectMap.insert(std::pair<uint64_t,csWindow*>(window->getOID(), window));
                    }
                }
                else if(strcmp(item->valuestring,"POS")==0)         // update object position
                {
                    _oid            = cJSON_GetObjectItem(json_read,"OID")->valueint;

                    csWindow::om_iter iter = m_DBObjectMap.find(_oid);
                    if(iter!=m_DBObjectMap.end())
                    {
                        window      = iter->second;
                        item        = cJSON_GetObjectItem(json_read,"POS");
                        int	  num   = cJSON_GetArraySize(item);
                        float *pos;
                        pos = new float[num];
                        for(int i=0;i<num;i++)
                        {
                            pos[i]=cJSON_GetArrayItem(item, i)->valuedouble;
                        }
                        window->setPosX(pos[0]);
                        window->setPosY(pos[1]);
                    }
                }
                else if(strcmp(item->valuestring,"SCALE")==0)     // update object scale
                {
                    _oid            = cJSON_GetObjectItem(json_read,"OID")->valueint;

                    //window      = m_DBObjectMap[_oid];        // if we know the object is in map !!

                    csWindow::om_iter iter = m_DBObjectMap.find(_oid);
                    if(iter!=m_DBObjectMap.end())
                    { 
                        window      = iter->second;
                        item        = cJSON_GetObjectItem(json_read,"SCALE");
                        int	  num   = cJSON_GetArraySize(item);
                        float *scale;
                        scale = new float[num];
                        for(int i=0;i<num;i++)
                        {
                            scale[i]=cJSON_GetArrayItem(item, i)->valuedouble;
                        }
                        window->setScale(scale[0],scale[1]);
                    }
                }
                // ---------------------------------
                // make sure that the updated object
                // will be on top.
                // ---------------------------------
                // not very efficient becuse we need
                // to go through the list again
                // ---------------------------------
                if(window!=NULL)
                {
                    csWindow::list_iter iter = m_windowList.begin();
                    bool swaped  = false;
                    while ( iter != m_windowList.end() && !swaped)
                    {
                        if((*iter)->getOID()==_oid)
                        {
                            m_windowList.erase(iter);
                            m_windowList.push_front (window);
                            swaped = true;
                        }
                        iter++;
                    }
                }
            }
            else if(strcmp(item->valuestring,   "CREATE"    )==0)
            {
                item    = cJSON_GetObjectItem(json_read,"TYPE");
                if(strcmp(item->valuestring,"Image")==0)
                {
                    // ------------------------------------
                    // create new object
                    // ------------------------------------
                    window  = new csWindow();
                    window->setOID(cJSON_GetObjectItem(json_read,"OID")->valueint);
                    window->setURI(cJSON_GetObjectItem(json_read,"URI")->valuestring);
                    window->session_obj  = true;     // object is already in database
                    window->setImageDim(cJSON_GetObjectItem(json_read,"WIDTH")->valueint, cJSON_GetObjectItem(json_read,"HEIGHT")->valueint);

                    // ------------------------------------
                    // if we have a position
                    // ------------------------------------
                    item        = cJSON_GetObjectItem(json_read,"POS");
                    if(item!=NULL)
                    {
                        window->setPosX(cJSON_GetArrayItem(item, 0)->valuedouble);
                        window->setPosY(cJSON_GetArrayItem(item, 1)->valuedouble);
                    }
                    // ------------------------------------
                    // if we have a scale
                    // ------------------------------------
                    item        = cJSON_GetObjectItem(json_read,"SCALE");
                    if(item!=NULL)
                    {
                        window->setScale(cJSON_GetArrayItem(item, 0)->valuedouble,cJSON_GetArrayItem(item, 1)->valuedouble);
                    }

                    // ------------------------------------
                    // add object to list and map
                    // ------------------------------------
                    m_windowList.push_front (window);
                    m_DBObjectMap.insert(std::pair<uint64_t,csWindow*>(window->getOID(), window));

                    // ------------------------------------------
                    // start image loading
                    // ------------------------------------------
                    loadImage(window);
                }
            }
            else if(strcmp(item->valuestring,   "DELETE"    )==0)
            {
                //jsonToConsol(json_read);

                uint64_t _oid     = cJSON_GetObjectItem(json_read,"OID")->valueint;

                // erase from object map and list
                int uid=removeObject(_oid);
                // erase from interaction map
                if(uid!=-1)
                {
                    csWindow::im_iter iter=m_InteractionMap.find(window->getUID());
                    if(iter!=m_InteractionMap.end())
                    {
                       m_InteractionMap.erase(iter);
                    }
                }
            }
            else
            {

            }
            cJSON_Delete(json_read);
        }
        else
        {
            // ------------------------------------------
            // This is a message from another server type
            // in our case a custom server (csgui).
            // If we have a database server we send him
            // the url...
            //
            // If not we just load an image...
            // ------------------------------------------

            // ------------------------------------------
            // read a json message
            // ------------------------------------------
            json_read   = cJSON_Parse((const char*)msg);

            //out=cJSON_Print(json_read);
            //printf("%s\n",out);

            item        = cJSON_GetObjectItem(json_read,"CMD");
            if(strcmp(item->valuestring,   "CREATE"    )==0)
            {
                item    = cJSON_GetObjectItem(json_read,"TYPE");
                if(strcmp(item->valuestring,"Image")==0)
                {
                    // ------------------------------------
                    // create new object
                    // ------------------------------------
                    csWindow    *newWindow  = new csWindow();
                    uint64_t    _oid    = cJSON_GetObjectItem(json_read,"OID")->valueint;
                    newWindow->setURI(cJSON_GetObjectItem(json_read,"URI")->valuestring);
                    if(_oid!=-1)
                    {
                        newWindow->setOID(_oid);
                        newWindow->session_obj  = true;     // object is already in database
                    }
                    else
                    {
                        newWindow->setOID(my_id_pool.getID());
                        newWindow->session_obj  = false;    // object is still local
                    }
                    // ------------------------------------
                    // if we have a dimensions
                    // ------------------------------------
                    int _w  = cJSON_GetObjectItem(json_read,"WIDTH")->valueint;
                    int _h  = cJSON_GetObjectItem(json_read,"HEIGHT")->valueint;
                    if(_w ==-1 || _h == -1)
                    {
                        // keep default size (need to set a flag that we don't have the right size yet)
                        // as soon as the client head has the right size it needs to update the
                        // database ... tbd
                    }
                    else
                    {
                        // set the size
                        newWindow->setImageDim(_w,_h);
                    }
                    // ------------------------------------
                    // if we have a position
                    // ------------------------------------
                    item        = cJSON_GetObjectItem(json_read,"POS");
                    bool has_pos    = false;
                    float pos[2];
                    if(item!=NULL)
                    {
                        pos[0]  = cJSON_GetArrayItem(item, 0)->valuedouble;
                        pos[1]  = cJSON_GetArrayItem(item, 1)->valuedouble;
                        newWindow->setPosX(pos[0]);
                        newWindow->setPosY(pos[1]);
                        has_pos    = true;
                    }

                    // ------------------------------------
                    // if we have a scale
                    // ------------------------------------
                    item        = cJSON_GetObjectItem(json_read,"SCALE");
                    bool has_scale  = false;
                    float scale[2];
                    if(item!=NULL)
                    {
                        scale[0]    = cJSON_GetArrayItem(item, 0)->valuedouble;
                        scale[1]    = cJSON_GetArrayItem(item, 1)->valuedouble;
                        newWindow->setScale(scale[0],scale[1]);
                        has_scale  = true;
                    }

                    //printf("New Window Width: %d Height: %d\n",newWindow->getWidth(),newWindow->getHeight());
                    // ------------------------------------
                    // add object to list and map
                    // ------------------------------------
                    m_windowList.push_front (newWindow);
                    m_DBObjectMap.insert(std::pair<uint64_t,csWindow*>(newWindow->getOID(), newWindow));

                    // ----------------------------------------
                    // create a json message to database
                    // ----------------------------------------
                    if(isHead())
                    {
                        char *send_out;
                        cJSON *json_send;
                        json_send=cJSON_CreateObject();
                        cJSON_AddStringToObject(json_send,"CMD",     "CREATE");
                        cJSON_AddNumberToObject(json_send,"CID",     newWindow->getOID());
                        cJSON_AddStringToObject(json_send,"TYPE",    "Image");
                        cJSON_AddStringToObject(json_send,"URI",     newWindow->getURI());
                        cJSON_AddNumberToObject(json_send,"WIDTH",   _w);
                        cJSON_AddNumberToObject(json_send,"HEIGHT",  _h);
                        if(has_pos)cJSON_AddItemToObject    (json_send,"POS",       cJSON_CreateFloatArray(pos,2));
                        if(has_scale)cJSON_AddItemToObject  (json_send,"SCALE",     cJSON_CreateFloatArray(scale,2));

                        send_out=cJSON_PrintUnformatted(json_send);
                        cJSON_Delete(json_send);

                        if(!cglxDM::sendData(db_device_ID, (unsigned char*) send_out, strlen(send_out)))
                        {
                            printf("Send Failed\n");
                        }
                        free(send_out);
                    }
                    // ------------------------------------------
                    // start image loading
                    // ------------------------------------------
                    loadImage(newWindow);
                }
            }
            cJSON_Delete(json_read);
        }
    }cglXUpdateDone();

    // --------------------------------------------------------
    // update the scene
    // --------------------------------------------------------
    // ! not all updates would probably require an update
    // --------------------------------------------------------
    cglXPostRedisplay();
}
void mousefunc(int button, int state, int x, int y)
{
    static vec2f_S vec;
    vec.x   = x;
    vec.y   = y;
    transPointerPos(&vec);

    if(cglXUpdateFirst())
    {
        if(checkWinHit(&vec, cglxEM::getUID(), button, state))
        {
            // put it in front of the list
        }
        else
        {
            //printf("No hit\n");
        }

        switch (button)
        {
            case CGLX_LEFT_BUTTON:
            {
            }
            break;
            case CGLX_MIDDLE_BUTTON:
            {
            }
            break;
            case CGLX_RIGHT_BUTTON:
            {
            }
            break;
            default:
            {
            }
        }
    }cglXUpdateDone();
    cglXPostRedisplay();
}
void motionfunc(int x,int y)
{
    static vec2f_S vec;
    vec.x   = x;
    vec.y   = y;
    transPointerPos(&vec);

    if(cglXUpdateFirst())
    {
        csWindow* window    = NULL;
        int UserID  = cglxEM::getUID();
        csWindow::im_iter iter = m_InteractionMap.find(UserID);
        if(iter!=m_InteractionMap.end())
        {
            window  = iter->second;
        }

        if(window!=NULL)
        {
            if(checkWidgetHit(&vec, UserID, -1, -1, window))
            {
                //window->deco_type   = DECO_DELETE;
            }
            else
            {
                //window->deco_type   = DECO_NDEF;
            }

            int button  = window->getButton();
            int state   = window->getState();

            bool action = false;
            switch (button)
            {
                case CGLX_LEFT_BUTTON:
                {
                    window->move(vec.x,vec.y);
                    action  = true;
                }
                break;
                case CGLX_MIDDLE_BUTTON:
                {

                }
                break;
                case CGLX_RIGHT_BUTTON:
                {
                    window->scale(vec.x,vec.y);
                    action  = true;
                }
                break;
                default:
                {

                }
            }
            // ----------------------------
            // send update to database
            // ----------------------------
            if(isHead())
            {
                if(db_device_ID!=-1 && action )
                {
                    // ----------------------------------------
                    // create a json message
                    // ----------------------------------------
                    float pos[2];

                    char *send_out;
                    cJSON *json_send=NULL;
                    json_send=cJSON_CreateObject();
                    cJSON_AddStringToObject(json_send,"CMD",     "UPDATE");
                    cJSON_AddNumberToObject(json_send,"OID",     window->getOID());

                    if(button==CGLX_LEFT_BUTTON)
                    {
                        pos[0]  = window->getPosX();
                        pos[1]  = window->getPosY();
                        cJSON_AddStringToObject(json_send,"ATTR",    "POS");
                        cJSON_AddItemToObject(json_send,"POS", cJSON_CreateFloatArray(pos,2));
                    }
                    else if(button==CGLX_RIGHT_BUTTON)
                    {
                        pos[0]  = window->getScaleX();
                        pos[1]  = window->getScaleY();
                        cJSON_AddStringToObject(json_send,"ATTR",    "SCALE");
                        cJSON_AddItemToObject(json_send,"SCALE", cJSON_CreateFloatArray(pos,2));
                    }
                    send_out=cJSON_PrintUnformatted(json_send);
                    cJSON_Delete(json_send);
                    json_send=NULL;

                    // ----------------------------------------
                    // we send this only to the creating client
                    // to update his object map
                    // ----------------------------------------
                    if(!cglxDM::sendData(db_device_ID, (unsigned char*) send_out, strlen(send_out)))
                    {
                        printf("Send Failed\n");
                    }
                    free(send_out);
                }
            }
        }
    }cglXUpdateDone();

    cglXPostRedisplay();
}
void passivemotionfunc(int x, int y)
{
    // ---------------------------------
    // Check first if we need to show
    // the menu bar
    // ---------------------------------
    if(cglXUpdateFirst())
    {
        static int uid_lock=-1;
        static vec2f_S vec;
        vec.x   = x;
        vec.y   = y;
        transPointerPos(&vec);
        // -----------------------------------
        // That can be a problem for multiuser
        // -----------------------------------
        // therefore we bind it to a UID
        // -----------------------------------

        if(vec.y<DEF_MENU_Y && fabs(vec.x)<DEF_MENU_X)
        {
            if(uid_lock ==-1 && cglxEM::getUID()!=-1)
            {
                uid_lock    = cglxEM::getUID();
                menu_flg    = true;
            }
        }
        else
        {
            if(uid_lock !=-1 && cglxEM::getUID()==uid_lock)
            {
               menu_flg = false;
               uid_lock =-1;
            }
        }
    }cglXUpdateDone();
    cglXPostRedisplay();
}
void devnotifyfunc(CS_DEVICE_P dev)
{
    if(dev->state == CGLX_DEV_ON)
    {
        if(dev->type == CS_HCI_APPL_SERV)
        {
            //printf("Our Database server connected from IP: %s DID: %d\n", dev->hostname.c_str(), dev->ID);
            db_device_ID    = dev->ID;
        }
    }
    else if(dev->state == CGLX_DEV_OFF)
    {
        if(dev->type == CS_HCI_APPL_SERV)
        {
            //printf("Our Database server has disconnected from IP: %s DID: %d\n", dev->hostname.c_str(), dev->ID);
            db_device_ID    = -1;
        }
    }
}
void specialKeysfunc(int key, int x, int y)
{
    if(cglXUpdateFirst ())
    {
        switch (key)
        {
        case  GLUT_KEY_F1:
        {
            if(cglxOL::isEnable(OL_WALL_AREA))
                cglxOL::Disable(OL_WALL_AREA);
            else
                cglxOL::Enable(OL_WALL_AREA);
            }
            break;
            case  GLUT_KEY_F2:
            {
                if(cglxOL::isEnable(OL_POINTER))
                    cglxOL::Disable(OL_POINTER);
                else
                    cglxOL::Enable(OL_POINTER);
            }
            break;
            case  GLUT_KEY_F3:
            {
                if(cglxOL::isEnable(OL_STREAMS))
                    cglxOL::Disable(OL_STREAMS);
                else
                    cglxOL::Enable(OL_STREAMS);
            }
            break;
            default:
            break;
        }
    }cglXUpdateDone();
    // redraw
    cglXPostRedisplay();
}
void loc_triggfunc(int ID, int code)
{
    if(cglXUpdateFirst ())
    {
        //printf("loc_triggfunc\n");
    }cglXUpdateDone();
    cglXPostRedisplay();
}
int main(int argc, char** argv)
{  
    // ----------------------------------
    // Initialize CGLX
    // ----------------------------------
    cglXInit(&argc, argv);
    cglXInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);

    if(cglx::isSoloAPP())
        cglXInitWindowSize( 800, 600 );
    else
    {
        cglXInitWindowSize( 800, (int)800/cglx::getWallRatio() );
    }

    cglXCreateWindow ("csObjectView");
    cglXInitWindowPosition (600, 100);
    init ();

    // ----------------------------------
    // Add some widgets
    // ----------------------------------
    typedef enum{WDG_NDEF=0, WDG_CLEAR=1, WDG_LOAD=2, WDG_SAVE=3, WDG_DELETE=4, WDG_DUMP=5}widget_type_E;
    // -- clear
    csWidget    *newWidget1  = new csWidget();
    newWidget1->setType(csWidget::WDG_CLEAR);
    newWidget1->setPosX(-DEF_MENU_X * 0.75f);
    newWidget1->setPosY(DEF_MENU_ITEM_Y);
    m_widgetList.push_front (newWidget1);
    // -- load
    csWidget    *newWidget2  = new csWidget();
    newWidget2->setType(csWidget::WDG_LOAD);
    newWidget2->setPosX(0.0f);
    newWidget2->setPosY(DEF_MENU_ITEM_Y);
    m_widgetList.push_front (newWidget2);
    // -- save
    csWidget    *newWidget3  = new csWidget();
    newWidget3->setType(csWidget::WDG_SAVE);
    newWidget3->setPosX(DEF_MENU_X * 0.75f);
    newWidget3->setPosY(DEF_MENU_ITEM_Y);
    m_widgetList.push_front (newWidget3);
    // -- delete
    //csWidget    *newWidget4  = new csWidget();
    //newWidget4->setType(csWidget::WDG_DELETE);
    //m_widgetList.push_front (newWidget4);
    // -- dump
    csWidget    *newWidget5  = new csWidget();
    newWidget5->setType(csWidget::WDG_DUMP);
    m_widgetList.push_front (newWidget5);

    // ----------------------------------
    // create mutex that protects our
    // maps and lists
    // ----------------------------------
    pthread_mutex_init(&obj_list_mutex, NULL);

    // ----------------------------------
    // register callbacks
    // ----------------------------------
    cglXDisplayFunc         (displayfunc);
    cglXReshapeFunc         (reshapefunc);
    cglXKeyboardFunc        (normakKeysfunc);
    cglXSpecialFunc         (specialKeysfunc);
    cglXMouseFunc           (mousefunc);
    cglXMotionFunc          (motionfunc);
    cglXPassiveMotionFunc   (passivemotionfunc);
    cglXCustomMsgFunc       (customMSGfunc);
    cglXLocalTriggerFunc    (loc_triggfunc);        // local trigger function
    cglXDeviceNotifyFunc    (devnotifyfunc);        // device notification function

    // ----------------------------------
    // enable pointers by default
    // ----------------------------------
    cglxOL::Enable(OL_POINTER);

    cglXMainLoop();

    // ----------------------------------
    // clean up interaction map
    // ----------------------------------
    m_InteractionMap.clear();

    // ----------------------------------
    // clean up object map
    // ----------------------------------
    m_DBObjectMap.clear();

    // ----------------------------------
    // clean up our windows
    // ----------------------------------
    for(csWindow::list_iter it = m_windowList.begin(); it != m_windowList.end();it++)
    {
        delete *it;
    }
    m_windowList.clear();

    // ----------------------------------
    // clean up our widgets
    // ----------------------------------
    for(csWidget::list_iter it = m_widgetList.begin(); it != m_widgetList.end();it++)
    {
        delete *it;
    }
    m_widgetList.clear();

    // ----------------------------------
    // destroy mutex
    // ----------------------------------
    pthread_mutex_destroy(&obj_list_mutex);

    return 0;   								// ANSI C requires main to return int.
}  
