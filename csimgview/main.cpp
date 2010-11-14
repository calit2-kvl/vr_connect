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
int     db_device_ID    = -1;

// loader mutex
pthread_mutex_t obj_list_mutex;

// -------------------------------------------------------------------------------------+
// This is our window list (later on we have objects and then subclass a window)
// -------------------------------------------------------------------------------------+
std::list<csWindow*> m_windowList;          // list of objects
std::map<int,csWindow*> m_InteractionMap;   // use user id to find object
std::map<uint64_t,csWindow*> m_DBObjectMap; // use db object id to find object

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
// FUNCTION IMPLEMENTATION
// -------------------------------------------------------------------------------------+
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
csWindow    *loadImage( const char* msg, int _oid=-1)
{

    Magick::Image image((const char*)msg);
    printf("Image %s W: %d H: %d\n",image.fileName().c_str(),(int)image.baseColumns(),(int)image.baseRows());

    // ------------------------------------
    // create a texture
    // ------------------------------------

    // make ImageMagick object / read file into memory
    Magick::Blob blob;
    // set RGBA output format
    image.magick("RGBA");
    // write to BLOB in RGBA format
    image.write(&blob);

    // create the temporary Texture
    GLuint GLtexture;
    glGenTextures(1, &GLtexture);
    // bind the texture to the next thing we make
    glBindTexture(GL_TEXTURE_2D, GLtexture);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    gluBuild2DMipmaps(GL_TEXTURE_2D, 4, image.columns(), image.rows(), GL_RGBA, GL_UNSIGNED_BYTE, blob.data());

    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    // error check
    if(glGetError()!=GL_NO_ERROR)return NULL;

    // ------------------------------------
    // add a window
    // ------------------------------------
    csWindow    *newWindow  = new csWindow();
    newWindow->setImageDim(image.baseColumns(), image.baseRows());
    newWindow->setTID(GLtexture);
    if(_oid!=-1)
    {
        newWindow->setOID(_oid);
    }
    else
    {
        newWindow->setOID(my_id_pool.getID());
    }
    // add object to list
    m_windowList.push_front (newWindow);
    // add object to map
    m_DBObjectMap.insert(std::pair<uint64_t,csWindow*>(newWindow->getOID(), newWindow));

    printf("Add window with ID: %d \n",newWindow->getOID());
    return newWindow;
}
bool checkWinHit(vec2f_P vec, int UserID, int button, int state)
{
    bool hit  = false;

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
                    iter_swap(iter,m_windowList.begin());

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

    cglXUnitAxes(.2);

    // ----------------------
    // draw the window list
    // ----------------------
    for (csWindow::list_riter riter = m_windowList.rbegin(); riter!=m_windowList.rend(); riter++)
    {
        (*riter)->draw();
    }
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
}
void customMSGfunc(int len, unsigned char *msg)
{
    if(cglXUpdateShared())
    {
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
            // for now the database server bounces back
            // the url.
            // ------------------------------------------
            //loadImage(( const char*) msg);

            // update the object ID -> this means that
            // this object is known to the database
            // we find the object via the id that we gave
            // it when we created the object on the client
            // side. Once we found it we exchange the OID
            // with the one we get from the database and
            // update the object map.

            // ----------------------------------------
            // read a json message
            // ----------------------------------------
            cJSON *json_read, *item;
            char *out;
            json_read   = cJSON_Parse((const char*)msg);
            item        = cJSON_GetObjectItem(json_read,"CMD");
            // -------------------------------------
            // Command type interpretation
            // -------------------------------------
            csWindow* window = NULL;
            if(strcmp(item->valuestring,"UPDATE")==0)
            {
                item    = cJSON_GetObjectItem(json_read,"ATTR");
                if(strcmp(item->valuestring,"OID")==0)          // update object ID
                {
                    int         _cid  = cJSON_GetObjectItem(json_read,"CID")->valueint;
                    uint64_t    _oid  = cJSON_GetObjectItem(json_read,"OID")->valueint;

                    // lock object map
                    // -> change oid
                    csWindow::om_iter iter = m_DBObjectMap.find(_cid);

                    if(iter!=m_DBObjectMap.end())
                    {
                        window = m_DBObjectMap[_cid];
                        window->setOID(_oid);
                        // remove object  from map and add it with a new oid
                        m_DBObjectMap.erase (iter);
                        m_DBObjectMap.insert(std::pair<uint64_t,csWindow*>(window->getOID(), window));
                        printf("Old oid: %d new: %u\n",_cid,(unsigned  int)_oid);
                    }
                    // unlock object map
                }
                else if(strcmp(item->valuestring,"POS")==0)         // update object position
                {
                    //printf("Got update POS\n");
                    uint64_t    _oid  = cJSON_GetObjectItem(json_read,"OID")->valueint;

                    csWindow::om_iter iter = m_DBObjectMap.find(_oid);
                    if(iter!=m_DBObjectMap.end())
                    {
                        window = m_DBObjectMap[_oid];
                        item    = cJSON_GetObjectItem(json_read,"POS");
                        int	  num=cJSON_GetArraySize(item);
                        //printf("Num Item: %d\n",num);
                        float *pos;
                        pos = new float[num];
                        for(int i=0;i<num;i++)
                        {
                            pos[i]=cJSON_GetArrayItem(item, i)->valuedouble;
                        }
                        //printf("X: %f Y: %f\n",pos[0],pos[1]);
                        window->setPosX(pos[0]);
                        window->setPosY(pos[1]);
                    }
                }
                else if(strcmp(item->valuestring,"SCALE")==0)     // update object scale
                {
                    //printf("Got update SCALE\n");
                    uint64_t    _oid  = cJSON_GetObjectItem(json_read,"OID")->valueint;

                    csWindow::om_iter iter = m_DBObjectMap.find(_oid);
                    if(iter!=m_DBObjectMap.end())
                    {
                        window = m_DBObjectMap[_oid];
                        item    = cJSON_GetObjectItem(json_read,"SCALE");
                        int	  num=cJSON_GetArraySize(item);
                        //printf("Num Item: %d\n",num);
                        float *scale;
                        scale = new float[num];
                        for(int i=0;i<num;i++)
                        {
                            scale[i]=cJSON_GetArrayItem(item, i)->valuedouble;
                        }
                        window->setScale(scale[0],scale[1]);
                    }
                }
            }
            else if(strcmp(item->valuestring,"CREATE")==0)
            {
                item    = cJSON_GetObjectItem(json_read,"TYPE");
                if(strcmp(item->valuestring,"Image")==0)
                {
                    loadImage(( const char*) cJSON_GetObjectItem(json_read,"URI")->valuestring, cJSON_GetObjectItem(json_read,"OID")->valueint);
                }
                //printf("Should load: %s\n",cJSON_GetObjectItem(json_read,"URI")->valuestring);

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

            // ---------------------------------------
            // start image loading
            // ---------------------------------------
            csWindow *newobj=loadImage(( const char*) msg);

            if(db_device_ID!=-1 && newobj!=NULL)
            {
                // ---------------------------------------
                // We need to define the protocoll to talk
                // with the database server and add it to
                // the message.
                // ---------------------------------------
                // Possible message types are:
                //
                //  - CREATE:
                //  - DELETE:
                //  - CLONE:
                //  - SAVE:
                //
                //  (Chris and Kai will define the protocol)
                //
                // ---------------------------------------
                // for now we just send the URL
                // ---------------------------------------

                // ----------------------------------------
                // create a json message
                // ----------------------------------------
                char *out;
                cJSON *json_send;
                json_send=cJSON_CreateObject();
                cJSON_AddStringToObject(json_send,"CMD",     "CREATE");
                cJSON_AddNumberToObject(json_send,"CID",     newobj->getOID());
                cJSON_AddStringToObject(json_send,"TYPE",    "Image");
                cJSON_AddStringToObject(json_send,"URI",     (const char*)msg);
                out=cJSON_PrintUnformatted(json_send);
                cJSON_Delete(json_send);

                if(!cglxDM::sendData(db_device_ID, (unsigned char*) out, strlen(out)))
                {
                    printf("Send Failed\n");
                }
                free(out);
            }
        }
    }cglXUpdateDone();

    // --------------------------------------------------------------
    // need to send info about the new  object to the DataBase Server
    // --------------------------------------------------------------
    // once we get a valid ID for this object we can add it with
    // a trigger event.
    // --------------------------------------------------------------
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
        csWindow::im_iter iter = m_InteractionMap.find(cglxEM::getUID());
        if(iter!=m_InteractionMap.end())
        {
            csWindow* window = m_InteractionMap[cglxEM::getUID()];
            int button  = window->getButton();
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
            if(db_device_ID!=-1 && action )
            {
                // ----------------------------------------
                // create a json message
                // ----------------------------------------
                float pos[2];
                // pos[0]  = vec.x;
                //pos[1]  = vec.y;

                char *send_out;
                cJSON *json_send=NULL;
                json_send=cJSON_CreateObject();
                cJSON_AddStringToObject(json_send,"CMD",     "UPDATE");
                cJSON_AddNumberToObject(json_send,"OID",     window->getOID());
                //cJSON_AddStringToObject(json_send,"ATTR",    "MATRIX");
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
                    if(isHead())printf("Send Failed\n");
                }

                free(send_out);
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
            printf("Our Database server connected from IP: %s DID: %d\n", dev->hostname.c_str(), dev->ID);
            db_device_ID    = dev->ID;
        }
    }
    else if(dev->state == CGLX_DEV_OFF)
    {
        if(dev->type == CS_HCI_APPL_SERV)
        {
            printf("Our Database server has disconnected from IP: %s DID: %d\n", dev->hostname.c_str(), dev->ID);
            db_device_ID    = -1;
        }
    }
}
int main(int argc, char** argv)
{  
    cglXInit(&argc, argv);
    cglXInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
    cglXInitWindowSize (640, 480);

    cglXCreateWindow ("csimgview");
    cglXInitWindowPosition (600, 100);
    init ();

    // ----------------------------------
    // register callbacks
    // ----------------------------------
    cglXDisplayFunc     (displayfunc);
    cglXReshapeFunc     (reshapefunc);
    cglXKeyboardFunc    (normakKeysfunc);
    cglXMouseFunc       (mousefunc);
    cglXMotionFunc      (motionfunc);
    cglXCustomMsgFunc   (customMSGfunc);
    cglXDeviceNotifyFunc(devnotifyfunc);// device notification function

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
    for(csWindow::list_iter it = m_windowList.begin(); it != m_windowList.end();it++)    // returns immediately if map is empty
    {
        delete *it;
    }
    m_windowList.clear();

    return 0;   								// ANSI C requires main to return int.
}  
