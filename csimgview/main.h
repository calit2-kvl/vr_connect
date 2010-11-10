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
#include "../common/cJSON.h"

// -------------------------------------------------------------------------------------+
// Prototype
// -------------------------------------------------------------------------------------+
class csWindow
{

public:
    /*! \fn csWindow::csWindow();
     *  \brief Default Constructor.
     */
    csWindow()
    {
        //container information
        UID         =   -1;
        OID         =   0;
        session_obj =   false;
        TID         =   0;
        width       =   1;
        height      =   1;
        posx        =   0.0f;
        posy        =   0.0f;
        scalex      =   1.0f;
        scaley      =   1.0;
        hit_posx    =   -1;
        hit_posy    =   -1;
        button      =   -1;
        state       =   -1;
        //content information
        c_width     =   1;
        c_height    =   1;
        c_aspect    =   1;
    }

    /*!	\fn csWindow::~csWindow();
     *  \brief Default Destructor.
     */
    virtual ~csWindow()
    {
        glDeleteTextures(1, &TID);
    }

    uint64_t    getOID(void)            {return OID;}
    int         getUID(void)            {return UID;}
    int         getTID(void)            {return TID;}
    float       getPosX(void)           {return posx;}
    float       getPosY(void)           {return posy;}
    float       getWidth(void)          {return width;}
    float       getHeight(void)         {return height;}
    int         getButton(void)         {return button;}

    void        setOID(uint64_t _OID)   {OID    = _OID;}
    void        setUID(int _UID)        {UID    = _UID;}
    void        setTID(GLuint _TID)     {TID    = _TID;}
    void        setPosX(float x)        {posx   = x;}
    void        setPosY(float y)        {posy   = y;}
    void        setWidth(float w)       {width  = w;}
    void        setHeight(float h)      {height = h;}

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
        if(TID==0)return;
        glBindTexture(GL_TEXTURE_2D, TID);
        glEnable(GL_TEXTURE_2D);

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

        glDisable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // iterator
    typedef std::list<csWindow*>::iterator list_iter;
    typedef std::list<csWindow*>::reverse_iterator list_riter;

    // interaction map iterator
    typedef std::map<int, csWindow*>::iterator im_iter;
    typedef std::map<int, csWindow*>::reverse_iterator im_riter;

    // object map iterator
    typedef std::map<uint64_t, csWindow*>::iterator om_iter;
    typedef std::map<uint64_t, csWindow*>::reverse_iterator om_riter;

private:
    // container information
    int         UID;
    uint64_t    OID;            // object ID
    bool        session_obj;    // true if object is in database
    GLuint      TID;
    float       posx;
    float       posy;
    float       scalex;
    float       scaley;
    float       width;
    float       height;
    float       aspect;

    // picking and state
    float       hit_posx;
    float       hit_posy;
    int         button;
    int         state;

    // content information
    float       c_width;
    float       c_height;
    float       c_aspect;
};

