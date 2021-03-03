#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<memory.h>
#include<GL/gl.h>
#include<GL/glu.h>
#include<GL/glx.h>   //bridging API


#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include<X11/XKBlib.h>
#include<X11/keysym.h>

//namespaces
using namespace std;

//global variable declarations
bool bFullscreen = false;

Display *gpDisplay=NULL;
XVisualInfo *gpXVisualInfo=NULL;
Colormap gColormap;
Window gWindow;
GLXContext gGLXContext;

int giWindowWidth=800;
int giWindowHeight=600;

GLfloat angle = 0.0f;  //initialization

bool bLight = false;

GLfloat LightAmbientZero[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat LightDiffuseZero[] = { 1.0f, 0.0f, 0.0f, 1.0f };
GLfloat LightSpecularZero[] = { 1.0f, 0.0f, 0.0f, 1.0f };
GLfloat LightPositionZero[] = { 0.0f, 0.0f, 0.0f, 1.0f };    //positional light

GLfloat LightAmbientOne[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat LightDiffuseOne[] = { 0.0f, 1.0f, 0.0f, 1.0f };
GLfloat LightSpecularOne[] = { 0.0f, 1.0f, 0.0f, 1.0f };
GLfloat LightPositionOne[] = { 0.0f, 0.0f, 0.0f, 1.0f };    //positional light

GLfloat LightAmbientTwo[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat LightDiffuseTwo[] = { 0.0f, 0.0f, 1.0f, 1.0f };
GLfloat LightSpecularTwo[] = { 0.0f, 0.0f, 1.0f, 1.0f };
GLfloat LightPositionTwo[] = { 0.0f, 0.0f, 0.0f, 1.0f };    //positional light

GLfloat MaterialAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat MaterialDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat MaterialSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat MaterialShininess = 50.0f;

GLfloat LightAngle0 = 0.0f;
GLfloat LightAngle1 = 0.0f;
GLfloat LightAngle2 = 0.0f;

GLUquadric* quadric = NULL;



//entry-point function
int main(void)
{
    //function prototypes
    void CreateWindow(void);
    void ToggleFullscreen(void);
    void Uninitialize(void);
    void Initialize(void);
    void Resize(int, int);
    void Draw(void);
    void Update(void);
    
    //variable declarations
    int winWidth=giWindowWidth;
    int winHeight=giWindowHeight;
    bool bDone = false;
    
    //code
    CreateWindow();
    Initialize();
    
    
    //game loop
    XEvent event;
    KeySym keysym;
    
    while(bDone==false)
    {
        while(XPending(gpDisplay))
        {
            XNextEvent(gpDisplay, &event);
            switch(event.type)
            {
                case MapNotify:
                    break;
                    
                case KeyPress:
                    keysym=XkbKeycodeToKeysym(gpDisplay, event.xkey.keycode, 0, 0);
                    switch(keysym)
                    {
                        case XK_Escape:
                            bDone=true;
                            break;
                            
                        case XK_F:
                        case XK_f:
                            if(bFullscreen==false)
                            {
                                ToggleFullscreen();
                                bFullscreen=true;
                            }
                            else
                            {
                                ToggleFullscreen();
                                bFullscreen=false;
                            }
                            break;
                            
                        case 'L':
                        case 'l':
                            if (bLight == true)
                            {
                                glDisable(GL_LIGHTING);
                                bLight = false;
                                
                            }
                            else
                            {
                                glEnable(GL_LIGHTING);
                                bLight = true;
                            }
                            break;
                        
                        default:
                            break; 
                            
                    }
                    break;
                    
                case ButtonPress:
                    switch(event.xbutton.button)
                    {
                        case 1:
                            break;
                            
                        case 2:
                            break;
                            
                        case 3:
                            break;
                            
                        default:
                            break;
                    }
                    break;
                    
                case MotionNotify:
                    break;
                    
                case ConfigureNotify:
                    winWidth=event.xconfigure.width;
                    winHeight=event.xconfigure.height;
                    Resize(winWidth, winHeight);
                    break;
                    
                case Expose:
                    break;
                    
                case DestroyNotify:
                    break;
                    
                case 33:
                    bDone=true;
                    break;
                    
                default:
                    break;
                    
            }
        }
        
        Draw();
        Update();
    }
    
    Uninitialize();
        
    return(0);
    
}


void CreateWindow(void)
{
    //function prototypes
    void Uninitialize(void);
    
    //variable declarations
    XSetWindowAttributes winAttribs;
    int defaultScreen;
    int styleMask;
    static int frameBufferAttributes[] = {GLX_DOUBLEBUFFER, True,
                                          GLX_RGBA,              //static is conventional
                                          GLX_RED_SIZE, 8,
                                          GLX_GREEN_SIZE, 8,
                                          GLX_BLUE_SIZE, 8,
                                          GLX_ALPHA_SIZE, 8,
                                          GLX_DEPTH_SIZE, 24,      //V4L (Video for Linux) recommends 24bit not 32bit
                                          None};                   //when only 5 members out of many are to be initialized use '0' or 'None'               
    
    //code
    gpDisplay=XOpenDisplay(NULL);
    if(gpDisplay==NULL)
    {
        printf("Error : Unable to open X Display.\nExiting Now!\n\n");
        Uninitialize();
        exit(1);
    }
    
    defaultScreen=XDefaultScreen(gpDisplay);
    
    gpXVisualInfo=(XVisualInfo *)malloc(sizeof(XVisualInfo));
        
    gpXVisualInfo = glXChooseVisual(gpDisplay, defaultScreen, frameBufferAttributes);
        
   
    if(gpXVisualInfo==NULL)
    {
        printf("Error : Unable to allocate memory for Visual Info.\nExiting Now!\n\n");
        Uninitialize();
        exit(1);
    }
    
    //XMatchVisualInfo(gpDisplay, defaultScreen, defaultDepth, TrueColor, gpXVisualInfo);
    if(gpXVisualInfo==NULL)
    {
        printf("Error : Unable to get a Visual.\nExiting Now!\n\n");
        Uninitialize();
        exit(1);
    }
    
   
    
    winAttribs.border_pixel=0;
    winAttribs.background_pixmap=0;
    winAttribs.colormap=XCreateColormap(gpDisplay, RootWindow(gpDisplay, gpXVisualInfo->screen), gpXVisualInfo->visual, AllocNone);
    gColormap=winAttribs.colormap;
    winAttribs.background_pixel=BlackPixel(gpDisplay, defaultScreen);
    winAttribs.event_mask=ExposureMask | VisibilityChangeMask | ButtonPressMask | KeyPressMask | PointerMotionMask | StructureNotifyMask;
    styleMask=CWBorderPixel | CWBackPixel | CWEventMask | CWColormap;
    
    gWindow=XCreateWindow(gpDisplay, 
                          RootWindow(gpDisplay, gpXVisualInfo->screen), 
                          0,
                          0, 
                          giWindowWidth,
                          giWindowHeight,
                          0, 
                          gpXVisualInfo->depth,
                          InputOutput,
                          gpXVisualInfo->visual,
                          styleMask,
                          &winAttribs);
    
    if(!gWindow)
    {
        printf("Error : Failed to create Main Window.\nExiting Now!\n\n");
        Uninitialize();
        exit(1);
    }
    
    XStoreName(gpDisplay, gWindow, "My XWindow Assignment - Shruti Kulkarni");
        
    Atom windowManagerDelete=XInternAtom(gpDisplay, "WM_DELETE_WINDOW", True);
    XSetWMProtocols(gpDisplay, gWindow, &windowManagerDelete, 1);
        
    XMapWindow(gpDisplay, gWindow);
    
}


void ToggleFullscreen(void)
{
    //variable declarations
    Atom wm_state;
    Atom fullscreen;
    XEvent xev = {0};
    
    //code
    wm_state=XInternAtom(gpDisplay, "_NET_WM_STATE", False);
    memset(&xev, 0, sizeof(xev));
    
    xev.type=ClientMessage;
    xev.xclient.window=gWindow;
    xev.xclient.message_type=wm_state;
    xev.xclient.format=32;
    xev.xclient.data.l[0] = bFullscreen ? 0:1;	
    fullscreen = XInternAtom(gpDisplay, "_NET_WM_STATE_FULLSCREEN", False);
    xev.xclient.data.l[1]=fullscreen;
    
    XSendEvent(gpDisplay,
               RootWindow(gpDisplay, gpXVisualInfo->screen),
               False,
               StructureNotifyMask,
               &xev);
    
}


void Initialize(void)
{
    //function prototype
    void Resize(int, int);   //for warm-up call
    
    gGLXContext = glXCreateContext(gpDisplay, gpXVisualInfo, NULL, GL_TRUE);
    
    glXMakeCurrent(gpDisplay, gWindow, gGLXContext);
    
    //depth
    
    glShadeModel(GL_SMOOTH);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);    
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    
    
    //Light Initialization
    
    glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbientZero);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuseZero);
    glLightfv(GL_LIGHT0, GL_SPECULAR, LightSpecularZero);
    glEnable(GL_LIGHT0);
    
    glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbientOne);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuseOne);
    glLightfv(GL_LIGHT1, GL_SPECULAR, LightSpecularOne);
    glEnable(GL_LIGHT1);
    
    glLightfv(GL_LIGHT2, GL_AMBIENT, LightAmbientTwo);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, LightDiffuseTwo);
    glLightfv(GL_LIGHT2, GL_SPECULAR, LightSpecularTwo);
    glEnable(GL_LIGHT2);
    
    
    //Material Initialization
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);
    glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);
    
    //SetClearColor
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //Black
    
    Resize(giWindowWidth, giWindowHeight);
    
    
}


void Resize(int width, int height)
{
    if(height==0)
    {
        height=1;
    }
    
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}


void Draw(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glPushMatrix();
    
    gluLookAt(0.0f, 0.0f, 2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    
    glPushMatrix();
    
    
    
    glRotatef(LightAngle0, 1.0f, 0.0f, 0.0f);
    
    LightPositionZero[1] = LightAngle0;
    
    glLightfv(GL_LIGHT0, GL_POSITION, LightPositionZero);
    
    glPopMatrix();
    glPushMatrix();
    
    
    
    glRotatef(LightAngle1, 0.0f, 1.0f, 0.0f);
    
    LightPositionOne[0] = LightAngle1;
    
    glLightfv(GL_LIGHT1, GL_POSITION, LightPositionOne);
    
    glPopMatrix();
    glPushMatrix();
    
    
    
    glRotatef(LightAngle2, 0.0f, 0.0f, 1.0f);
    
    LightPositionTwo[0] = LightAngle2;
    
    glLightfv(GL_LIGHT2, GL_POSITION, LightPositionTwo);
    
    glPopMatrix();
    //glPushMatrix();
    
    
    glTranslatef(0.0f, 0.0f, -1.0f);
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    quadric = gluNewQuadric();
    
    glColor3f(1.0f, 1.0f, 1.0f);
    gluSphere(quadric, 0.5f, 50, 50);
    
    glPopMatrix();
    
    
    glXSwapBuffers(gpDisplay, gWindow);
}


void Update(void)
{
    //code
    
    LightAngle0 += 0.2f;
    if (LightAngle0 >= 360.0f)
	{
		LightAngle0 = 0.0f;
	}

	LightAngle1 += 0.2f;
	if (LightAngle1 >= 360.0f)
	{
		LightAngle1 = 0.0f;
	}

	LightAngle2 += 0.2f;
	if (LightAngle2 >= 360.0f)
	{
		LightAngle2 = 0.0f;
	}
}


void Uninitialize(void)
{
    //variable declarations
    GLXContext currentGLXContext;
    
    
    if (quadric)
    {
        gluDeleteQuadric(quadric);
        quadric = NULL;
    }
    
    currentGLXContext = glXGetCurrentContext();
    
    if(currentGLXContext == gGLXContext)
    {
        glXMakeCurrent(gpDisplay, 0, 0);
    }
    
    if(gGLXContext)
    {
        glXDestroyContext(gpDisplay, gGLXContext);
    }
    
    if(gWindow)
    {
        XDestroyWindow(gpDisplay, gWindow);
    }
    
    if(gColormap)
    {
        XFreeColormap(gpDisplay, gColormap);
    }
 
    if(gpXVisualInfo)
    {
        free(gpXVisualInfo);
        gpXVisualInfo = NULL;
    }
                    
    if(gpDisplay)
    {
        XCloseDisplay(gpDisplay);
        gpDisplay=NULL;
    }
    
}

