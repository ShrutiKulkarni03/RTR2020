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

bool bLight = false;

//LIGHT1 - ambient and spot

GLfloat Light_Model_Ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
GLfloat Light_Model_Local_Viewer[] = { 0.0f };
GLfloat sk_lightAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f }; 
GLfloat sk_lightDiffuse[] = { 0.0f, 1.0f, 1.0f, 1.0f };  
GLfloat sk_lightPosition[] = { -2.0f, 2.0f, 1.0f, 1.0f };
//GLfloat sk_lightPosition[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat sk_lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat sk_spotDirection[] = { 0.6f, -1.0f, -0.5f };

GLfloat sk_materialAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat sk_materialDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat sk_materialSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f }; 
GLfloat sk_materialShininess = 128.0f;

int keyPressed = 0;

GLfloat lightPosX = 0.6f;
GLfloat lightPosZ = -0.5f;

GLUquadric* sk_quadric = NULL;

char keys[26];

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
                            
                        case XK_Up:
                            keyPressed=1;
                            break;
                            
                        case XK_Down:
                            keyPressed=2;
                            break;
                            
                        case XK_Left:
                            keyPressed=3;
                            break;
                            
                        case XK_Right:
                            keyPressed=4;
                            break;
                            
                        default:
                            keyPressed=0;
                            break;
                            
                    }
                    
                    XLookupString((XKeyEvent*)&event, keys, sizeof(keys), NULL, NULL);
                    
                    switch(keys[0])
                    {
                            
                        case 'F':
                        case 'f':
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
    
    
   
    
   //SetClearColor
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);    //black

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, Light_Model_Ambient);
	glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, Light_Model_Local_Viewer);

	glLightfv(GL_LIGHT1, GL_AMBIENT, sk_lightAmbient);  //gl..Light..float..vector
	glLightfv(GL_LIGHT1, GL_DIFFUSE, sk_lightDiffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, sk_lightSpecular);
	glLightfv(GL_LIGHT1, GL_POSITION, sk_lightPosition);

	glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.5f);
	glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.5f);
	glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.2f);

	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 15.0f);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, sk_spotDirection);
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 2.0f);


	//glMaterialfv(GL_FRONT, GL_AMBIENT, sk_materialAmbient);
	//glMaterialfv(GL_FRONT, GL_DIFFUSE, sk_materialDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, sk_materialSpecular);
	glMaterialf(GL_FRONT, GL_SHININESS, sk_materialShininess);

	glEnable(GL_LIGHT1);
    
    
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
   //code
	
	//declaration
	void drawSpheres(void);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	
	glPushMatrix();

	//up

	if (keyPressed == 1)
	{
		glTranslatef(lightPosX, 0.0f, lightPosZ);
		sk_spotDirection[3] = lightPosZ;
	}


	//down

	if (keyPressed == 2)
	{
		glTranslatef(lightPosX, 0.0f, lightPosZ);
		sk_spotDirection[3] = lightPosZ;
	}


	//left

	if (keyPressed == 3)
	{
		glTranslatef(lightPosX, 0.0f, lightPosZ);
		sk_spotDirection[1] = lightPosX;
	}


	//right

	if (keyPressed == 4)
	{
		glTranslatef(lightPosX, 0.0f, lightPosZ);
		sk_spotDirection[1] = lightPosX;
	}


	glLightfv(GL_LIGHT1, GL_POSITION, sk_lightPosition);
	//glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, sk_spotDirection);
	
	glColor3f(1.0f, 1.0f, 1.0f);

	drawSpheres();
    
    
    glXSwapBuffers(gpDisplay, gWindow);
}


void Update(void)
{
    //code

	if (keyPressed == 1)
	{
		lightPosZ -= 0.03f;

		if (lightPosZ <= -2.5f)
		{
			lightPosZ = -2.5f;
		}
	}

	if (keyPressed == 2)
	{
		lightPosZ += 0.03f;

		if (lightPosZ >= 3.0f)
		{
			lightPosZ = 3.0f;
		}
	}


	if (keyPressed == 3)
	{
		lightPosX -= 0.03f;

		if (lightPosX <= -3.0f)
		{
			lightPosX = -3.0f;
		}
	}

	if (keyPressed == 4)
	{
		lightPosX += 0.03f;

		if (lightPosX >= 3.0f)
		{
			lightPosX = 3.0f;
		}
	}

	if (keyPressed == 0)
	{
		lightPosX = 0.6f;
		lightPosZ = -0.5f;
	}


}


void Uninitialize(void)
{
    //variable declarations
    GLXContext currentGLXContext;
    
    
    if (sk_quadric)
	{
		gluDeleteQuadric(sk_quadric);
		sk_quadric = NULL;
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


void drawSpheres(void)
{

	GLfloat r = 0.23f;

	for (GLfloat x = -3.0f; x <= 3.0f; x += 0.5f)
	{
		glTranslatef(x, -1.3f, 2.0f);
		sk_quadric = gluNewQuadric();
		gluSphere(sk_quadric, r, 30, 30);

		glPopMatrix();
		glPushMatrix();
	}

	for (GLfloat x = -3.0f; x <= 3.0f; x += 0.5f)
	{
		glTranslatef(x, -1.3f, 1.5f);
		sk_quadric = gluNewQuadric();
		gluSphere(sk_quadric, r, 30, 30);

		glPopMatrix();
		glPushMatrix();
	}

	for (GLfloat x = -3.0f; x <= 3.0f; x += 0.5f)
	{
		glTranslatef(x, -1.3f, 1.0f);
		sk_quadric = gluNewQuadric();
		gluSphere(sk_quadric, r, 30, 30);

		glPopMatrix();
		glPushMatrix();
	}


	for (GLfloat x = -3.5f; x <= 3.5f; x += 0.5f)
	{
		glTranslatef(x, -1.3f, 0.5f);
		sk_quadric = gluNewQuadric();
		gluSphere(sk_quadric, r, 30, 30);

		glPopMatrix();
		glPushMatrix();
	}


	for (GLfloat x = -3.5f; x <= 3.5f; x += 0.5f)
	{
		glTranslatef(x, -1.3f, 0.0f);
		sk_quadric = gluNewQuadric();
		gluSphere(sk_quadric, r, 30, 30);

		glPopMatrix();
		glPushMatrix();
	}


	for (GLfloat x = -4.0f; x <= 4.0f; x += 0.5f)
	{
		glTranslatef(x, -1.3f, -0.5f);
		sk_quadric = gluNewQuadric();
		gluSphere(sk_quadric, r, 30, 30);

		glPopMatrix();
		glPushMatrix();
	}


	for (GLfloat x = -4.5f; x <= 4.5f; x += 0.5f)
	{
		glTranslatef(x, -1.3f, -1.0f);
		sk_quadric = gluNewQuadric();
		gluSphere(sk_quadric, r, 30, 30);

		glPopMatrix();
		glPushMatrix();
	}


	for (GLfloat x = -4.5f; x <= 4.5f; x += 0.5f)
	{
		glTranslatef(x, -1.3f, -1.5f);
		sk_quadric = gluNewQuadric();
		gluSphere(sk_quadric, r, 30, 30);

		glPopMatrix();
		glPushMatrix();
	}


	for (GLfloat x = -5.0f; x <= 5.0f; x += 0.5f)
	{
		glTranslatef(x, -1.3f, -2.0f);
		sk_quadric = gluNewQuadric();
		gluSphere(sk_quadric, r, 30, 30);

		glPopMatrix();
		glPushMatrix();
	}


	for (GLfloat x = -5.5f; x <= 5.5f; x += 0.5f)
	{
		glTranslatef(x, -1.3f, -2.5f);
		sk_quadric = gluNewQuadric();
		gluSphere(sk_quadric, r, 30, 30);

		glPopMatrix();
		glPushMatrix();
	}


	for (GLfloat x = -6.0f; x <= 6.0f; x += 0.5f)
	{
		glTranslatef(x, -1.3f, -3.0f);
		sk_quadric = gluNewQuadric();
		gluSphere(sk_quadric, r, 30, 30);

		glPopMatrix();
		glPushMatrix();
	}


	for (GLfloat x = -6.5f; x <= 6.5f; x += 0.5f)
	{
		glTranslatef(x, -1.3f, -3.5f);
		sk_quadric = gluNewQuadric();
		gluSphere(sk_quadric, r, 30, 30);

		glPopMatrix();
		glPushMatrix();
	}


	for (GLfloat x = -7.0f; x <= 7.0f; x += 0.5f)
	{
		glTranslatef(x, -1.3f, -4.0f);
		sk_quadric = gluNewQuadric();
		gluSphere(sk_quadric, r, 30, 30);

		glPopMatrix();
		glPushMatrix();
	}

}



