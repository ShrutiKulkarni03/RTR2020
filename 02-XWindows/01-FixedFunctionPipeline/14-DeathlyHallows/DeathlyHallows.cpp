#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<memory.h>
#include<GL/gl.h>
#include<GL/glu.h>
#include<GL/glx.h>   //bridging API
#include<math.h>


#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include<X11/XKBlib.h>
#include<X11/keysym.h>

#define PI 3.1415

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

GLfloat cloak_x = -2.5f;
GLfloat cloak_y = -2.5f;
GLfloat cloak_rot = 0.0f;

GLfloat stone_x = 2.5f;
GLfloat stone_y = -2.5f;
GLfloat stone_rot = 0.0f;

GLfloat wand_y = 2.0;


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
    
    glShadeModel(GL_SMOOTH);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);    
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    
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
    
    GLfloat x1 = 0.0f;
    GLfloat x2 = 0.5f;
    GLfloat x3 = -0.5f;
    
    GLfloat y1 = 0.5f;
    GLfloat y2 = -0.5f;
    GLfloat y3 = -0.5f;
    
    GLfloat in_x, in_y;
    
    GLfloat da, db, dc, a, b, c, r, value;
    GLfloat s;
    GLfloat angle;
    
    
    //code
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glMatrixMode(GL_MODELVIEW);
    
    
    //The Invisibility Cloak


	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	glTranslatef(cloak_x, cloak_y, 0.0f);
	glRotatef(cloak_rot, 0.0f, 1.0f, 0.0f);

	glColor3f(1.0f, 1.0f, 1.0f);
	glLineWidth(2.0f);

	glBegin(GL_LINES);	

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(-0.5f, -0.5f, 0.0f);

	glVertex3f(-0.5f, -0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.0f);

	glVertex3f(0.5f, -0.5f, 0.0f);
	glVertex3f(0.0f, 0.5f, 0.0f);

	glEnd();


	//The Elder Wand

	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	glTranslatef(0.0f, wand_y, 0.0f);

	glBegin(GL_LINES);

	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.0f, -0.5f, 0.0f);


	glEnd();

	//Resurrection Stone

	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	glTranslatef(stone_x, stone_y, 0.0f);
	glRotatef(stone_rot, 0.0f, 1.0f, 0.0f);

	glBegin(GL_LINE_LOOP);

	da = ((x2 - x3) * (x2 - x3)) + ((y2 - y3) * (y2 - y3));
	a = sqrt(da);

	db = ((x3 - x1) * (x3 - x1)) + ((y3 - y1) * (y3 - y1));
	b = sqrt(db);

	dc = ((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2));
	c = sqrt(dc);

	in_x = ((a * x1) + (b * x2) + (c * x3)) / (a + b + c);
	in_y = ((a * y1) + (b * y2) + (c * y3)) / (a + b + c);

	s = (a + b + c) / 2;

	value = s * (s - a) * (s - b) * (s - c);

	r = (sqrt(value))/s;

	for (angle = 0.0f; angle <= 2 * PI; angle += 0.01f)
	{
		glVertex3f(r * cos(angle) + in_x, r * sin(angle) + in_y, 0.0);
	}

	glEnd();

    
    glXSwapBuffers(gpDisplay, gWindow);
}


void Update(void)
{
   //code
    
    //cloak
	cloak_x += 0.005f;
	cloak_y += 0.005f;

	if (cloak_x >= 0.0f, cloak_y >= 0.0f)
	{
		cloak_x = 0.0f;
		cloak_y = 0.0f;
	}

	cloak_rot += 0.7f;

    if (cloak_x >= 0.0f, cloak_y >= 0.0f)
		cloak_rot = 540.0f;

	//stone

	stone_x -= 0.005f;
	stone_y += 0.005f;

	if (stone_x <= 0.0f, stone_y >= 0.0f)
	{
		stone_x = 0.0f;
		stone_y = 0.0f;
	}

	stone_rot += 0.7f;

    if (stone_x <= 0.0f, stone_y >= 0.0f)
		stone_rot = 540.0f;

	//wand

	wand_y -= 0.004f;

	if (wand_y <= 0.0f)
	{
		wand_y = 0.0f;
	}
}


void Uninitialize(void)
{
    //variable declarations
    GLXContext currentGLXContext;
    
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

