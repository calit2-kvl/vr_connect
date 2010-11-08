/*
+---------------------------------------------------------------------------------------+
| @BEGIN_COPYRIGHT                                               			|
|                                       						|
| Copyright (C) XXXX-XXXX, Wo ever                                    			|
| All Rights Reserved for ...                                         			|
|                                                                      			|
| @END_COPYRIGHT			                                                |
+---------------------------------------------------------------------------------------+
|                                               
| Component  : 
| Filename   : main.cpp
| Sourcefile : 
| Language   : C++
|                                               
+----------------------------- Revision Information-------------------------------------+
| $Author: XXX $
| $Revision: 0.0.0.0 $
| $Date: 0 $
| $RCSfile: 0 $
+---------------------------------------------------------------------------------------+
|                        This is a simple example program.
|              (It should help you to get started with CGLX programming) 
|
+---------------------------------------------------------------------------------------+
*/
// -------------------------------------------------------------------------------------+
// Includes
// -------------------------------------------------------------------------------------+


#include "main.h"


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


IDPool my_id_pool;

// -------------------------------------------------------------------------------------+
// This is our window list
// -------------------------------------------------------------------------------------+
std::list<csWindow*> m_windowList;

std::map<int,csWindow*> m_InteractionMap;


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


// -------------------------------------------------------------------------------------+
// Implementation
// -------------------------------------------------------------------------------------+

void displayfunc(void)
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    cglXUnitAxes(.2);

    // ----------------------
    // draw the window list
    // ----------------------

    /*for (csWindow::list_iter iter = m_windowList.begin(); iter!=m_windowList.end(); iter++)
    {
        (*iter)->draw();
    }*/

    for (csWindow::list_riter riter = m_windowList.rbegin(); riter!=m_windowList.rend(); riter++)
    {
        (*riter)->draw();
    }
    //cglXPostRedisplay();
}

void init(void)
{ 
    glClearColor (0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_SMOOTH); 							// The Default coloring style 
    //glEnable(GL_DEPTH_TEST);
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
        //printf("UID: %d\nMSG len: %d\nMSG: %s\n",cglxDM::getDID(),len,msg);

        Magick::Image image((const char*)msg);
        printf("Image %s W: %d H: %d\n",image.fileName().c_str(),(int)image.baseColumns(),(int)image.baseRows());

        // ------------------------------------
        // add a window
        // ------------------------------------
        csWindow    *newWindow  = new csWindow();
        newWindow->setImageDim(image.baseColumns(), image.baseRows());

        // ------------------------------------
        // create a texture
        // ------------------------------------
        /*
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
        */


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
        //glTexImage2D(GL_TEXTURE_2D, 0, 4, image.columns(), image.rows(), 0, GL_RGB, GL_UNSIGNED_BYTE, blob.data());

        glDisable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);

        newWindow->setTID(GLtexture);

        // ----------------------------------
        // add the window
        // ----------------------------------
        newWindow->setWID(my_id_pool.getID());
        //m_windowList.push_back (newWindow);
        m_windowList.push_front (newWindow);

        //iter_swap(cii,m_windowList.begin());
        /*
        // ------------------------------------
        // write it to file if necessary
        // we want to try it without
        // ------------------------------------
        //std::string filename(getenv("HOME"));
        //filename.append("/data/images/out.png");
        //image.magick("png");
        //image.write(filename);

        // ------------------------------------
        // create a texture
        // ------------------------------------
        // make ImageMagick object / read file into memory
        Magick::Blob blob;
        // set RGBA output format
        image.magick("RGBA");
        // write to BLOB in RGBA format
        image.write(&blob);
        // pointer to temporary Texture
        GLuint GLtexture;
        // create the temporary Texture
        glGenTextures(1, &GLtexture);
        // bind the texture to the next thing we make
        glBindTexture(GL_TEXTURE_2D, GLtexture);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        gluBuild2DMipmaps(GL_TEXTURE_2D, 4, image.columns(), image.rows(), GL_RGBA, GL_UNSIGNED_BYTE, blob.data());
        // draw sphere
        //gluSphere(quadratic, ObjectRadius, 32, 32);
        */
        printf("Add window with ID: %d\n",newWindow->getWID());
    }cglXUpdateDone();


    // --------------------------------------------------------------
    // need to send info about the new  object to the DataBase Server
    // --------------------------------------------------------------
    // once we get a valid ID for this object we can add it with
    // a trigger event.
    // --------------------------------------------------------------
    cglXPostRedisplay();
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
                    printf("Set State\n");

                    // we return the first match
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
                    printf("Reset\n");

                    // we return the first match
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
                printf("Reset\n");
            }
        }
    }
    return false;
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
            printf("No hit\n");
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
        csWindow::iter iter = m_InteractionMap.find(cglxEM::getUID());
        if(iter!=m_InteractionMap.end())
        {
            csWindow* window = m_InteractionMap[cglxEM::getUID()];
            /*if(window->getButton()==CGLX_LEFT_BUTTON)
                window->move(vec.x,vec.y);
            if(window->getButton()==CGLX_RIGHT_BUTTON)
                window->scale(vec.x,vec.y);
            */
            switch (window->getButton())
            {
                case CGLX_LEFT_BUTTON:
                {
                    window->move(vec.x,vec.y);
                }
                break;
                case CGLX_MIDDLE_BUTTON:
                {

                }
                break;
                case CGLX_RIGHT_BUTTON:
                {
                    window->scale(vec.x,vec.y);
                }
                break;
                default:
                {

                }
            }
        }

    }cglXUpdateDone();

    cglXPostRedisplay();
}

int main(int argc, char** argv)
{  
    cglXInit(&argc, argv);
    cglXInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
    cglXInitWindowSize (640, 480);

    cglXCreateWindow ("csimgview");
    cglXInitWindowPosition (100, 100);
    init ();

    // register callbacks
    cglXDisplayFunc     (displayfunc);
    cglXReshapeFunc     (reshapefunc);
    cglXKeyboardFunc    (normakKeysfunc);
    cglXMouseFunc       (mousefunc);
    cglXMotionFunc      (motionfunc);
    cglXCustomMsgFunc   (customMSGfunc);
    cglXMainLoop();

    // ----------------------------------
    // clean up our windows
    // ----------------------------------
    /*for(csWindow::iter it = m_windowMap.begin(); it != m_windowMap.end();it++)    // returns immediately if map is empty
    {
        delete (*it).second;
        m_windowMap.erase(it);
    }*/

    // ----------------------------------
    // clean up interaction map
    // ----------------------------------
    m_InteractionMap.clear();
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
