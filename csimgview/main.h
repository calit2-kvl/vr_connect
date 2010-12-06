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
| $RCSfile: main.h,v $
+---------------------------------------------------------------------------------------+
*/
// -------------------------------------------------------------------------------------+
// Includes
// -------------------------------------------------------------------------------------+
#include <stdlib.h>
#include <stdio.h>
#include <QtCore/QUuid>


#ifdef __APPLE__
        #include <cglX/cglX.h>
#else
        #include "cglX.h"
#endif
using namespace cglx;

#include <map>
#include <stdint.h>
#include <pthread.h>
#include <Magick++.h>
#include "cJSON.h"
#include "all_icons.h"

// -------------------------------------------------------------------------------------+
// defines
// -------------------------------------------------------------------------------------+
#define DEF_MENU_Y          -0.85f
#define DEF_MENU_ITEM_Y     -0.925
#define DEF_MENU_X          0.5f
// -------------------------------------------------------------------------------------+
// ENUM
// -------------------------------------------------------------------------------------+
/*! \enum deco_type_E;
 * 	Defines the deco type.
 */
typedef enum{DECO_NDEF=0, DECO_DELETE=1}deco_type_E;

// -------------------------------------------------------------------------------------+
// Prototype
// -------------------------------------------------------------------------------------+
class csWidget
{

public:
    /*! \fn csWidget::csWidget();
     *  \brief Default Constructor.
     */
    csWidget()
    {
        // widget type
        wtype           =   WDG_NDEF;
        //container information
        TID             =   0;
        width           =   .1;
        height          =   .1;
        posx            =   0.0f;
        posy            =   0.0f;
        hit_posx        =   -1;
        hit_posy        =   -1;
        button          =   -1;
        state           =   -1;

        //content information
        c_width         =   1;
        c_height        =   1;
        c_aspect        =   1;
    }
    /*!	\fn csWindow::~csWindow();
     *  \brief Default Destructor.
     */
    virtual ~csWidget()
    {
        glDeleteTextures(1, &TID);
    }
    void        draw(bool _menu_flg, float _w2)
    {
        // --------------------------
        // position the icons
        // --------------------------
        // if we want to move them
        // --------------------------
        switch (wtype)
        {
        case WDG_CLEAR:
            {
                if(!_menu_flg)return;
            }
            break;
        case WDG_LOAD:
            {
                if(!_menu_flg)return;
            }
            break;
        case WDG_SAVE:
            {
                if(!_menu_flg)return;
            }
            break;
        case WDG_DELETE:
            {
                //if(!_menu_flg)return;
            }
            break;
        case WDG_DUMP:
            {
                if(isSoloAPP())setPosX(_w2*0.85f);
                else setPosX(DEF_MENU_X*3.85f);
                setPosY(DEF_MENU_ITEM_Y);
            }
            break;
        default:
            {

            }
        }

        if(TID==0)
        {
            glPushMatrix();
            glTranslatef(posx,posy,0);
            glColor3f(0.0f,0.0f,0.0f);
            glBegin(GL_LINE_LOOP);
                glVertex2f(-width*.5, -height*.5);
                glVertex2f(-width*.5, height*.5);
                glVertex2f(width*.5, height*.5);
                glVertex2f(width*.5, -height*.5);
            glEnd();
            glPopMatrix();
        }
        else
        {
            glBindTexture(GL_TEXTURE_2D, TID);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

            glPushMatrix();
            glTranslatef(posx,posy,0);
            if(state == CGLX_DOWN)glScalef(1.1,1.1,1.0);

            glBegin(GL_QUADS);
                glTexCoord2f(0, 1);
                glVertex2f(-width*.5, -height*.5);
                glTexCoord2f(0, 0);
                glVertex2f(-width*.5, height*.5);
                glTexCoord2f(1, 0);
                glVertex2f(width*.5, height*.5);
                glTexCoord2f(1, 1);
                glVertex2f(width*.5, -height*.5);
            glEnd();
            glPopMatrix();
            glColor3f(1.0,1.0,1.0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }
    // iterator
    typedef     std::list<csWidget*>::iterator list_iter;
    typedef     std::list<csWidget*>::reverse_iterator list_riter;

    // widget type
    typedef enum{WDG_NDEF=0, WDG_CLEAR=1, WDG_LOAD=2, WDG_SAVE=3, WDG_DELETE=4, WDG_DUMP=5}widget_type_E;

    float       getPosX(void)           {return posx;}
    float       getPosY(void)           {return posy;}
    void        setPosX(float x)        {posx   = x;}
    void        setPosY(float y)        {posy   = y;}

    float       getWidth(void)          {return width;}
    float       getHeight(void)         {return height;}
    void        setWidth(float w)       {width  = w;}
    void        setHeight(float h)      {height = h;}

    int         getButton(void)         {return button;}
    int         getState(void)          {return state;}
    void        setState(float _hit_posx, float _hit_posy, int _button, int _state)
    {
        hit_posx    = _hit_posx;
        hit_posy    = _hit_posy;
        button      = _button;
        state       = _state;
    }
    void        setType(csWidget::widget_type_E _wtype)
    {
        wtype = _wtype;
        createTexture();
    }

    // widget type
    widget_type_E   wtype;

private:

    // container information
    GLuint          TID;
    float           posx;
    float           posy;
    float           width;
    float           height;
    float           aspect;

    // picking and state
    float           hit_posx;
    float           hit_posy;
    int             button;
    int             state;

    // content information
    float           c_width;
    float           c_height;
    float           c_aspect;

    // create OpenGL texture
    void createTexture(void)
    {
        // create the temporary Texture
        glGenTextures(1, &TID);
        // bind the texture to the next thing we make
        glBindTexture(GL_TEXTURE_2D, TID);

        switch (wtype)
        {
        case WDG_CLEAR:
            {
                glTexImage2D(GL_TEXTURE_2D, 0, CGLX_IMAGE_BYTES_PER_PIXEL, CGLX_IMAGE_WIDTH, CGLX_IMAGE_HEIGHT, 0, GL_RGBA,
                                    GL_UNSIGNED_BYTE, CGLX_IMAGE_CLEAR_DATA);
            }
            break;
        case WDG_LOAD:
            {
                glTexImage2D(GL_TEXTURE_2D, 0, CGLX_IMAGE_BYTES_PER_PIXEL, CGLX_IMAGE_WIDTH, CGLX_IMAGE_HEIGHT, 0, GL_RGBA,
                                    GL_UNSIGNED_BYTE, CGLX_IMAGE_LOAD_DATA);
            }
            break;
        case WDG_SAVE:
            {
                glTexImage2D(GL_TEXTURE_2D, 0, CGLX_IMAGE_BYTES_PER_PIXEL, CGLX_IMAGE_WIDTH, CGLX_IMAGE_HEIGHT, 0, GL_RGBA,
                                    GL_UNSIGNED_BYTE, CGLX_IMAGE_SAVE_DATA);
            }
            break;
        case WDG_DELETE:
            {
                glTexImage2D(GL_TEXTURE_2D, 0, CGLX_IMAGE_BYTES_PER_PIXEL, CGLX_IMAGE_WIDTH, CGLX_IMAGE_HEIGHT, 0, GL_RGBA,
                                    GL_UNSIGNED_BYTE, CGLX_IMAGE_LOAD_DATA);
            }
            break;
        case WDG_DUMP:
            {
                glTexImage2D(GL_TEXTURE_2D, 0, CGLX_IMAGE_BYTES_PER_PIXEL, CGLX_IMAGE_WIDTH_B, CGLX_IMAGE_HEIGHT_B, 0, GL_RGBA,
                                    GL_UNSIGNED_BYTE, CGLX_IMAGE_DUMPSTER_DATA);
            }
            break;
        default:
            {
                glTexImage2D(GL_TEXTURE_2D, 0, CGLX_IMAGE_BYTES_PER_PIXEL, CGLX_IMAGE_WIDTH, CGLX_IMAGE_HEIGHT, 0, GL_RGBA,
                                    GL_UNSIGNED_BYTE, CGLX_IMAGE_LOAD_DATA);
            }
        }

        // unbind
        glDisable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
};

class csWindow
{

public:
    /*! \fn csWindow::csWindow();
     *  \brief Default Constructor.
     */
    csWindow()
    {
        //container information
        UID             =   -1;
        OID             =   0;
        session_obj     =   false;
        deco_type       =   DECO_NDEF;
        TID             =   0;
        width           =   1;
        height          =   1;
        posx            =   0.0f;
        posy            =   0.0f;
        scalex          =   1.0f;
        scaley          =   1.0;
        hit_posx        =   -1;
        hit_posy        =   -1;
        button          =   -1;
        state           =   -1;
        //content information
        c_width         =   1;
        c_height        =   1;
        c_aspect        =   1;

        pthread_mutex_init(&blob_mutex, NULL);
        blob            = new Magick::Blob();
        blob_update     = false;
        c_uri.assign("NONE");
    }
    /*!	\fn csWindow::~csWindow();
     *  \brief Default Destructor.
     */
    virtual ~csWindow()
    {
        glDeleteTextures(1, &TID);
        pthread_mutex_destroy(&blob_mutex);
        if(blob!=0)delete blob;
    }

    QUuid       getOID(void)            {return OID;}
    int         getUID(void)            {return UID;}
    const char *getURI(void)            {return c_uri.c_str();}
    int         getTID(void)            {return TID;}
    float       getPosX(void)           {return posx;}
    float       getPosY(void)           {return posy;}
    float       getWidth(void)          {return width;}
    float       getHeight(void)         {return height;}
    int         getButton(void)         {return button;}
    int         getState(void)          {return state;}
    float       getScaleX(void)         {return scalex;}
    float       getScaleY(void)         {return scaley;}

    void        setOID(QUuid _OID)   {OID    = _OID;}
    void        setUID(int _UID)        {UID    = _UID;}
    void        setURI(const char* _uri){c_uri.assign(_uri);}
    void        setTID(GLuint _TID)     {TID    = _TID;}
    void        setPosX(float x)        {posx   = x;}
    void        setPosY(float y)        {posy   = y;}
    void        setWidth(float w)       {width  = w;}
    void        setHeight(float h)      {height = h;}
    void        setScale(float _sx, float _sy)
    {
        scalex = _sx;scaley=_sy;
        // limit the minimum size
        width 		= width*scalex;
        height 		= height*scalex;
        float aspect    = width/(float)height;
        if(width<0.2)
        {
            width   = 0.2;
            height  = width/aspect;
        }
    }

    // object manipulation
    void        setState(float _hit_posx, float _hit_posy, int _button, int _state)
    {
        hit_posx    = _hit_posx;
        hit_posy    = _hit_posy;
        button      = _button;
        state       = _state;
    }
    void        resetState(void)
    {
        hit_posx    =   -1;
        hit_posy    =   -1;
        button      =   -1;
        state       =   -1;
    }
    void        move(float x, float y)
    {
        posx	+= x-hit_posx;
        posy	+= y-hit_posy;
        hit_posx = x;
        hit_posy = y;
        //printf("Pos x:%f y:%f\n",posx,posy);
    }
    void        scale(float x, float y)
    {
        float delta	=  1+(x-hit_posx);
        scalex=delta;
        scaley=delta;

        // limit the minimum size
        width 		= width*scalex;
        height 		= height*scalex;
        float aspect    = width/(float)height;
        if(width<0.2)
        {
            width   = 0.2;
            height  = width/aspect;
        }

        // update hit pos
        hit_posx 	= x;
        hit_posy 	= y;
    }

    // content
    void        setImageDim(float _w, float _h)
    {
        c_width     =   _w;
        c_height    =   _h;
        c_aspect    =   c_width/(float)c_height;

        // adjust the container with and height
        // height is always 1
        height      =   1;
        width       =   c_aspect;
    }

    /*!	\fn void draw(void)
     *  \brief Draw a textured rectangle
     */
    void        draw(void)
    {
        // -------------------------------
        // this will work for now
        // -------------------------------
        pthread_mutex_lock (&blob_mutex);
        if(blob_update)createTexture();
        pthread_mutex_unlock (&blob_mutex);

        if(TID==0)
        {
            glPushMatrix();
            glTranslatef(posx,posy,0);
            glBegin(GL_LINE_LOOP);
                glVertex2f(-width*.5, -height*.5);
                glVertex2f(-width*.5, height*.5);
                glVertex2f(width*.5, height*.5);
                glVertex2f(width*.5, -height*.5);
            glEnd();
            glPopMatrix();
        }
        else
        {
            //glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, TID);

            float a=1;
            float b=1;

            aspect  = width/(float)height;
            //wider than high
            if (aspect > 1)
            {
                    a*=float(aspect);
            }
            else
            {
                    b*=float(aspect);
            }
            /*
            glBegin( GL_QUADS );
                glNormal3f( 0.0, 0.0, -1.0);
                glTexCoord2f(0, 0);
                glVertex2f(-a,-b);
                glTexCoord2f(0, 1);
                glVertex2f(-a,b);
                glTexCoord2f(1, 1);
                glVertex2f(a,b);
                glTexCoord2f(1, 0);
                glVertex2f(a,-b);
            glEnd();
            */

            glPushMatrix();
            glTranslatef(posx,posy,0);
            glBegin(GL_QUADS);
                glTexCoord2f(0, 1);
                glVertex2f(-width*.5, -height*.5);
                glTexCoord2f(0, 0);
                glVertex2f(-width*.5, height*.5);
                glTexCoord2f(1, 0);
                glVertex2f(width*.5, height*.5);
                glTexCoord2f(1, 1);
                glVertex2f(width*.5, -height*.5);
            glEnd();
            glPopMatrix();
            glBindTexture(GL_TEXTURE_2D, 0);

            //glDisable(GL_TEXTURE_2D);
        }

        if(deco_type == DECO_DELETE)
        {
            glColor3f(1.0,0.5,0.5);
            glPushMatrix();
            glTranslatef(posx,posy,0);
            glBegin(GL_LINE_LOOP);
                glVertex2f(-width*.5, -height*.5);
                glVertex2f(-width*.5, height*.5);
                glVertex2f(width*.5, height*.5);
                glVertex2f(width*.5, -height*.5);
            glEnd();
            glPopMatrix();
            glColor3f(1.0,1.0,1.0);
        }
    }

    // iterator
    typedef     std::list<csWindow*>::iterator list_iter;
    typedef     std::list<csWindow*>::reverse_iterator list_riter;

    // interaction map iterator
    typedef     std::map<int, csWindow*>::iterator im_iter;
    typedef     std::map<int, csWindow*>::reverse_iterator im_riter;

    // object map iterator
    typedef     std::map<QUuid, csWindow*>::iterator om_iter;
    typedef     std::map<uint64_t, csWindow*>::reverse_iterator om_riter;

    // mutex to protect the raw data
    pthread_mutex_t     blob_mutex;
    Magick::Blob        *blob;
    bool                blob_update;

    bool                session_obj;    // true if object is in database
    deco_type_E         deco_type;      // type of decoration for now just colored  lines

private:
    // container information
    int                 UID;            // user ID
    QUuid               OID;            // object ID
    GLuint              TID;            // texture ID
    float               posx;
    float               posy;
    float               scalex;
    float               scaley;
    float               width;
    float               height;
    float               aspect;

    // picking and state
    float               hit_posx;
    float               hit_posy;
    int                 button;
    int                 state;

    // content information
    float               c_width;
    float               c_height;
    float               c_aspect;
    std::string         c_uri;

    // create OpenGL texture
    void createTexture(void)
    {
        // create the temporary Texture
        glGenTextures(1, &TID);
        // bind the texture to the next thing we make
        glBindTexture(GL_TEXTURE_2D, TID);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        gluBuild2DMipmaps(GL_TEXTURE_2D, 4, c_width, c_height, GL_RGBA, GL_UNSIGNED_BYTE, blob->data());

        // unbind
        glBindTexture(GL_TEXTURE_2D, 0);
        blob_update = false;
        if(blob!=0)
        {
            delete blob;
            blob = 0;
        }
    }
};

