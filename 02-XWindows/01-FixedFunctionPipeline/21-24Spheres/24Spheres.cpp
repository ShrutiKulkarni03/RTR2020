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

GLfloat angle = 0.0f; 

bool bLight = false;

GLfloat Light_Model_Ambient[] = {0.2f, 0.2f, 0.2f, 1.0f};
GLfloat Light_Model_Local_Viewer[] = { 0.0f };

GLfloat LightAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat LightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat LightPosition[] = { 0.0f, 3.0f, 3.0f, 0.0f };   //directional light

GLfloat AngleForXRotation = 0.0f;
GLfloat AngleForYRotation = 0.0f;
GLfloat AngleForZRotation = 0.0f;

GLint KeyPressed = 0;


GLUquadric* quadric[24];



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
                            
                        case 'X':
                        case 'x':
                            KeyPressed = 1;
                            AngleForXRotation = 0.0f;
                            break;
                            
                            
                        case 'Y':
                        case 'y':
                            KeyPressed = 2;
                            AngleForYRotation = 0.0f;
                            break;
                            
                            
                        case 'Z':
                        case 'z':
                            KeyPressed = 3;
                            AngleForZRotation = 0.0f;
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
    
    
    glEnable(GL_AUTO_NORMAL);
    glEnable(GL_NORMALIZE);
    
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, Light_Model_Ambient);
    glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, Light_Model_Local_Viewer);
    
    //Light Initialization
    
    glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, LightPosition);
    glEnable(GL_LIGHT0);
    
    
    //QUADRIC
    
    for (int i = 0; i < 24; i++)
    {
        quadric[i] = gluNewQuadric();
    }
    
    //SetClearColor
    glClearColor(0.25f, 0.25f, 0.25f, 1.0f);    //dark grey
    
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
    
    //gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
    
    if (width <= height)
	{
		glOrtho(0.0f,	//field of view 'y' FOVY
			    15.5f,		//field of view 'x' FOVX
			    0.0f,
			    15.5f * ((GLfloat)height / (GLfloat)width),
			    -10.0f,	//near
			    10.0f);	//far
	}
	else
	{
		glOrtho(0.0f,
			    15.5f * ((GLfloat)width / (GLfloat)height),
			    0.0f,
			    15.5f,
			    -10.0f,
			    10.0f);
	}
}


void Draw(void)
{
    //Function Prototype
	void Draw24Spheres(void);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	//gluLookAt(0.0f, 0.0f, 20.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
		
	if (KeyPressed == 1)
	{
		glRotatef(AngleForXRotation, 1.0f, 0.0f, 0.0f);
		LightPosition[1] = AngleForXRotation;
		
	}
	else if (KeyPressed == 2)
	{
		glRotatef(AngleForYRotation, 0.0f, 1.0f, 0.0f);
		LightPosition[2] = AngleForYRotation;
		
	}
	else if (KeyPressed == 3)
	{
		glRotatef(AngleForZRotation, 0.0f, 0.0f, 1.0f);
		LightPosition[0] = AngleForZRotation;
		
	}
	

	glLightfv(GL_LIGHT0, GL_POSITION, LightPosition);


	Draw24Spheres();
    
    glXSwapBuffers(gpDisplay, gWindow);
}


void Draw24Spheres(void)
{
    GLfloat MaterialAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat MaterialDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat MaterialSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat MaterialShininess = 50.0f;
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    
    // ***** 1st sphere on 1st row, emerald *****
    
    MaterialAmbient[0] = 0.0215f;
    MaterialAmbient[1] = 0.1745f;
    MaterialAmbient[2] = 0.0215f;
    MaterialAmbient[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);
    
    MaterialDiffuse[0] = 0.07568f;
    MaterialDiffuse[1] = 0.61424f;
    MaterialDiffuse[2] = 0.07568f;
    MaterialDiffuse[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);
    
    MaterialSpecular[0] = 0.633f;
    MaterialSpecular[1] = 0.727811f;
    MaterialSpecular[2] = 0.633f;
    MaterialSpecular[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);
    
    MaterialShininess = 0.6 * 128;
    glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(1.5f, 14.0f, 0.0f);
    
    gluSphere(quadric[0], 1.0f, 30, 30);
    
    
    
    
    // ***** 2nd sphere on 1st row, jade *****
    
    MaterialAmbient[0] = 0.135f;
    MaterialAmbient[1] = 0.2225f;
    MaterialAmbient[2] = 0.1575f;
    MaterialAmbient[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);
    
    MaterialDiffuse[0] = 0.54f;
    MaterialDiffuse[1] = 0.89f;
    MaterialDiffuse[2] = 0.63f;
    MaterialDiffuse[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);
    
    MaterialSpecular[0] = 0.316228f;
    MaterialSpecular[1] = 0.316228f;
    MaterialSpecular[2] = 0.316228f;
    MaterialSpecular[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);
    
    MaterialShininess = 0.1 * 128;
    glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(6.4f, 14.0f, 0.0f);
    
    gluSphere(quadric[1], 1.0f, 30, 30);
    
    
    
    // ***** 3rd sphere on 1st row, obsidian *****
    
    MaterialAmbient[0] = 0.05375f;
    MaterialAmbient[1] = 0.05f;
    MaterialAmbient[2] = 0.06625f;
    MaterialAmbient[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);
    
    MaterialDiffuse[0] = 0.18275f;
    MaterialDiffuse[1] = 0.17f;
    MaterialDiffuse[2] = 0.22525f;
    MaterialDiffuse[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);
    
    MaterialSpecular[0] = 0.332741f;
    MaterialSpecular[1] = 0.328634f;
    MaterialSpecular[2] = 0.346435f;
    MaterialSpecular[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);
    
    MaterialShininess = 0.3 * 128;
    glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(11.3f, 14.0f, 0.0f);
    
    gluSphere(quadric[2], 1.0f, 30, 30);
    
    
    
    
    // ***** 4th sphere on 1st row, pearl *****
    
    MaterialAmbient[0] = 0.25f;
    MaterialAmbient[1] = 0.20725f;
    MaterialAmbient[2] = 0.20725f;
    MaterialAmbient[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);
    
    MaterialDiffuse[0] = 1.0f;
    MaterialDiffuse[1] = 0.829f;
    MaterialDiffuse[2] = 0.829f;
    MaterialDiffuse[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);
    
    MaterialSpecular[0] = 0.296648f;
    MaterialSpecular[1] = 0.296648f;
    MaterialSpecular[2] = 0.296648f;
    MaterialSpecular[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);
    
    MaterialShininess = 0.088 * 128;
    glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(16.2f, 14.0f, 0.0f);
    
    gluSphere(quadric[3], 1.0f, 30, 30);
    
    
    
    
    // ***** 5th sphere on 1st row, ruby *****
    
    MaterialAmbient[0] = 0.1745f;
    MaterialAmbient[1] = 0.01175f;
    MaterialAmbient[2] = 0.01175f;
    MaterialAmbient[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);
    
    MaterialDiffuse[0] = 0.61424f;
    MaterialDiffuse[1] = 0.04136f;
    MaterialDiffuse[2] = 0.04136f;
    MaterialDiffuse[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);
    
    MaterialSpecular[0] = 0.727811f;
    MaterialSpecular[1] = 0.626959f;
    MaterialSpecular[2] = 0.626959f;
    MaterialSpecular[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);
    
    MaterialShininess = 0.6 * 128;
    glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(21.1f, 14.0f, 0.0f);
    
    gluSphere(quadric[4], 1.0f, 30, 30);
    
    
    
    // ***** 6th sphere on 1st row, turquoise *****
    
    MaterialAmbient[0] = 0.1f;
    MaterialAmbient[1] = 0.18725f;
    MaterialAmbient[2] = 0.1745f;
    MaterialAmbient[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);
    
    MaterialDiffuse[0] = 0.396f;
    MaterialDiffuse[1] = 0.74151f;
    MaterialDiffuse[2] = 0.69102f;
    MaterialDiffuse[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);
    
    MaterialSpecular[0] = 0.297254f;
    MaterialSpecular[1] = 0.30829f;
    MaterialSpecular[2] = 0.306678f;
    MaterialSpecular[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);
    
    MaterialShininess = 0.1 * 128;
    glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(26.0f, 14.0f, 0.0f);
    
    gluSphere(quadric[5], 1.0f, 30, 30);
    
    
    
    // ***** 1st sphere on 2nd row, brass *****
    
    MaterialAmbient[0] = 0.329412f;
    MaterialAmbient[1] = 0.223529f;
    MaterialAmbient[2] = 0.027451f;
    MaterialAmbient[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);
    
    MaterialDiffuse[0] = 0.780392f;
    MaterialDiffuse[1] = 0.568627f;
    MaterialDiffuse[2] = 0.113725f;
    MaterialDiffuse[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);
    
    MaterialSpecular[0] = 0.992157f;
    MaterialSpecular[1] = 0.941176f;
    MaterialSpecular[2] = 0.807843f;
    MaterialSpecular[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);
    
    MaterialShininess = 0.21794872 * 128;
    glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(1.5f, 9.84f, 0.0f);
    gluSphere(quadric[6], 1.0f, 30, 30);
    
    
    
    
    // ***** 2nd sphere on 2nd row, bronze *****
    
    MaterialAmbient[0] = 0.0215f;
    MaterialAmbient[1] = 0.1275f;
    MaterialAmbient[2] = 0.054f;
    MaterialAmbient[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);
    
    MaterialDiffuse[0] = 0.714f;
    MaterialDiffuse[1] = 0.4284f;
    MaterialDiffuse[2] = 0.18144f;
    MaterialDiffuse[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);
    
    MaterialSpecular[0] = 0.393548f;
    MaterialSpecular[1] = 0.271906f;
    MaterialSpecular[2] = 0.166721f;
    MaterialSpecular[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);
    
    MaterialShininess = 0.2 * 128;
    glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(6.4f, 9.84f, 0.0f);
    
    gluSphere(quadric[7], 1.0f, 30, 30);
    
    
    
    
    // ***** 3rd sphere on 2nd row, chrome *****
    
    MaterialAmbient[0] = 0.25f;
    MaterialAmbient[1] = 0.25f;
    MaterialAmbient[2] = 0.25f;
    MaterialAmbient[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);
    
    MaterialDiffuse[0] = 0.4f;
    MaterialDiffuse[1] = 0.4f;
    MaterialDiffuse[2] = 0.4f;
    MaterialDiffuse[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);
    
    MaterialSpecular[0] = 0.774597f;
    MaterialSpecular[1] = 0.774597f;
    MaterialSpecular[2] = 0.774597f;
    MaterialSpecular[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);
    
    MaterialShininess = 0.6 * 128;
    glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(11.3f, 9.84f, 0.0f);
    
    gluSphere(quadric[8], 1.0f, 30, 30);
    
    
    
    
    // ***** 4th sphere on 2nd row, copper *****
    
    MaterialAmbient[0] = 0.19125f;
    MaterialAmbient[1] = 0.0735f;
    MaterialAmbient[2] = 0.0225f;
    MaterialAmbient[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);
    
    MaterialDiffuse[0] = 0.7038f;
    MaterialDiffuse[1] = 0.27048f;
    MaterialDiffuse[2] = 0.0828f;
    MaterialDiffuse[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);
    
    MaterialSpecular[0] = 0.256777f;
    MaterialSpecular[1] = 0.137622f;
    MaterialSpecular[2] = 0.086014f;
    MaterialSpecular[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);
    
    MaterialShininess = 0.1 * 128;
    glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(16.2f, 9.84f, 0.0f);
    
    gluSphere(quadric[9], 1.0f, 30, 30);
    
    
    
    
    // ***** 5th sphere on 2nd row, gold *****
    
    MaterialAmbient[0] = 0.24725f;
    MaterialAmbient[1] = 0.1995f;
    MaterialAmbient[2] = 0.0745f;
    MaterialAmbient[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);
    
    MaterialDiffuse[0] = 0.75164f;
    MaterialDiffuse[1] = 0.60648f;
    MaterialDiffuse[2] = 0.22648f;
    MaterialDiffuse[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);
    
    MaterialSpecular[0] = 0.628281f;
    MaterialSpecular[1] = 0.555802f;
    MaterialSpecular[2] = 0.366065f;
    MaterialSpecular[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);
    
    MaterialShininess = 0.4 * 128;
    glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(21.1f, 9.84f, 0.0f);
    
    gluSphere(quadric[10], 1.0f, 30, 30);
    
    
    
    
    // ***** 6th sphere on 2nd row, silver *****
    
    MaterialAmbient[0] = 0.19225f;
    MaterialAmbient[1] = 0.19225f;
    MaterialAmbient[2] = 0.19225f;
    MaterialAmbient[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);
    
    MaterialDiffuse[0] = 0.50754f;
    MaterialDiffuse[1] = 0.50754f;
    MaterialDiffuse[2] = 0.50754f;
    MaterialDiffuse[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);
    
    MaterialSpecular[0] = 0.508273f;
    MaterialSpecular[1] = 0.508273f;
    MaterialSpecular[2] = 0.508273f;
    MaterialSpecular[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);
    
    MaterialShininess = 0.4 * 128;
    glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(26.0f, 9.84f, 0.0f);
    
    gluSphere(quadric[11], 1.0f, 30, 30);
    
    
    
    // ***** 1st sphere on 3rd row, black *****
    
    MaterialAmbient[0] = 0.0f;
    MaterialAmbient[1] = 0.0f;
    MaterialAmbient[2] = 0.0f;
    MaterialAmbient[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);
    
    MaterialDiffuse[0] = 0.01f;
    MaterialDiffuse[1] = 0.01f;
    MaterialDiffuse[2] = 0.01f;
    MaterialDiffuse[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);
    
    MaterialSpecular[0] = 0.5f;
    MaterialSpecular[1] = 0.5f;
    MaterialSpecular[2] = 0.5f;
    MaterialSpecular[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);
    
    MaterialShininess = 0.25 * 128;
    glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(1.5f, 5.68f, 0.0f);
    
    gluSphere(quadric[12], 1.0f, 30, 30);
    
    
    
    // ***** 2nd sphere on 3rd row, cyan *****
    
    MaterialAmbient[0] = 0.0f;
    MaterialAmbient[1] = 0.1f;
    MaterialAmbient[2] = 0.06f;
    MaterialAmbient[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);
    
    MaterialDiffuse[0] = 0.0f;
    MaterialDiffuse[1] = 0.50980392f;
    MaterialDiffuse[2] = 0.50980392f;
    MaterialDiffuse[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);
    
    MaterialSpecular[0] = 0.50196078f;
    MaterialSpecular[1] = 0.50196078f;
    MaterialSpecular[2] = 0.50196078f;
    MaterialSpecular[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);
    
    MaterialShininess = 0.25 * 128;
    glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(6.4f, 5.68f, 0.0f);
    
    gluSphere(quadric[13], 1.0f, 30, 30);
    
    
    
    
    // ***** 3rd sphere on 3rd row, green *****
    
    MaterialAmbient[0] = 0.0f;
    MaterialAmbient[1] = 0.0f;
    MaterialAmbient[2] = 0.0f;
    MaterialAmbient[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);
    
    MaterialDiffuse[0] = 0.1f;
    MaterialDiffuse[1] = 0.35f;
    MaterialDiffuse[2] = 0.1f;
    MaterialDiffuse[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);
    
    MaterialSpecular[0] = 0.45f;
    MaterialSpecular[1] = 0.55f;
    MaterialSpecular[2] = 0.45f;
    MaterialSpecular[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);
    
    MaterialShininess = 0.25 * 128;
    glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(11.3f, 5.68f, 0.0f);
    
    gluSphere(quadric[14], 1.0f, 30, 30);
    
    
    
    // ***** 4th sphere on 3rd row, red *****
    
    MaterialAmbient[0] = 0.0f;
    MaterialAmbient[1] = 0.0f;
    MaterialAmbient[2] = 0.0f;
    MaterialAmbient[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);
    
    MaterialDiffuse[0] = 0.5f;
    MaterialDiffuse[1] = 0.0f;
    MaterialDiffuse[2] = 0.0f;
    MaterialDiffuse[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);
    
    MaterialSpecular[0] = 0.7f;
    MaterialSpecular[1] = 0.6f;
    MaterialSpecular[2] = 0.6f;
    MaterialSpecular[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);
    
    MaterialShininess = 0.25 * 128;
    glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(16.2f, 5.68f, 0.0f);
    
    gluSphere(quadric[15], 1.0f, 30, 30);
    
    
    
    // ***** 5th sphere on 3rd row, white *****
    
    MaterialAmbient[0] = 0.0f;
    MaterialAmbient[1] = 0.0f;
    MaterialAmbient[2] = 0.0f;
    MaterialAmbient[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);
    
    MaterialDiffuse[0] = 0.55f;
    MaterialDiffuse[1] = 0.55f;
    MaterialDiffuse[2] = 0.55f;
    MaterialDiffuse[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);
    
    MaterialSpecular[0] = 0.7f;
    MaterialSpecular[1] = 0.7f;
    MaterialSpecular[2] = 0.7f;
    MaterialSpecular[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);
    
    MaterialShininess = 0.25 * 128;
    glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(21.1f, 5.68f, 0.0f);
    
    gluSphere(quadric[16], 1.0f, 30, 30);
    
    
    
    // ***** 6th sphere on 3rd row, yellow plastic *****
    
    MaterialAmbient[0] = 0.0f;
    MaterialAmbient[1] = 0.0f;
    MaterialAmbient[2] = 0.0f;
    MaterialAmbient[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);
    
    MaterialDiffuse[0] = 0.5f;
    MaterialDiffuse[1] = 0.5f;
    MaterialDiffuse[2] = 0.0f;
    MaterialDiffuse[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);
    
    MaterialSpecular[0] = 0.6f;
    MaterialSpecular[1] = 0.6f;
    MaterialSpecular[2] = 0.5f;
    MaterialSpecular[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);
    
    MaterialShininess = 0.25 * 128;
    glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(26.0f, 5.68f, 0.0f);
    
    gluSphere(quadric[17], 1.0f, 30, 30);
    
    
    
    // ***** 1st sphere on 4th row, black *****
    
    MaterialAmbient[0] = 0.02f;
    MaterialAmbient[1] = 0.02f;
    MaterialAmbient[2] = 0.02f;
    MaterialAmbient[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);
    
    MaterialDiffuse[0] = 0.01f;
    MaterialDiffuse[1] = 0.01f;
    MaterialDiffuse[2] = 0.01f;
    MaterialDiffuse[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);
    
    MaterialSpecular[0] = 0.4f;
    MaterialSpecular[1] = 0.4f;
    MaterialSpecular[2] = 0.4f;
    MaterialSpecular[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);
    
    MaterialShininess = 0.078125 * 128;
    glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(1.5f, 1.5f, 0.0f);
    
    gluSphere(quadric[18], 1.0f, 30, 30);
    
    
    
    // ***** 2nd sphere on 4th row, cyan *****
    
    MaterialAmbient[0] = 0.0f;
    MaterialAmbient[1] = 0.05f;
    MaterialAmbient[2] = 0.05f;
    MaterialAmbient[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);
    
    MaterialDiffuse[0] = 0.4f;
    MaterialDiffuse[1] = 0.5f;
    MaterialDiffuse[2] = 0.5f;
    MaterialDiffuse[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);
    
    MaterialSpecular[0] = 0.04f;
    MaterialSpecular[1] = 0.7f;
    MaterialSpecular[2] = 0.7f;
    MaterialSpecular[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);
    
    MaterialShininess = 0.078125 * 128;
    glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(6.4f, 1.5f, 0.0f);
    
    gluSphere(quadric[19], 1.0f, 30, 30);
    
    
    
    // ***** 3rd sphere on 4th row, green *****
    
    MaterialAmbient[0] = 0.0f;
    MaterialAmbient[1] = 0.05f;
    MaterialAmbient[2] = 0.0f;
    MaterialAmbient[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);
    
    MaterialDiffuse[0] = 0.4f;
    MaterialDiffuse[1] = 0.5f;
    MaterialDiffuse[2] = 0.4f;
    MaterialDiffuse[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);
    
    MaterialSpecular[0] = 0.04f;
    MaterialSpecular[1] = 0.7f;
    MaterialSpecular[2] = 0.04f;
    MaterialSpecular[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);
    
    MaterialShininess = 0.078125 * 128;
    glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(11.3f, 1.5f, 0.0f);
    
    gluSphere(quadric[20], 1.0f, 30, 30);
    
    
    
    // ***** 4th sphere on 4th row, red *****
    
    MaterialAmbient[0] = 0.05f;
    MaterialAmbient[1] = 0.0f;
    MaterialAmbient[2] = 0.0f;
    MaterialAmbient[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);
    
    MaterialDiffuse[0] = 0.5f;
    MaterialDiffuse[1] = 0.4f;
    MaterialDiffuse[2] = 0.4f;
    MaterialDiffuse[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);
    
    MaterialSpecular[0] = 0.7f;
    MaterialSpecular[1] = 0.04f;
    MaterialSpecular[2] = 0.04f;
    MaterialSpecular[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);
    
    MaterialShininess = 0.078125 * 128;
    glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(16.2f, 1.5f, 0.0f);
    
    gluSphere(quadric[21], 1.0f, 30, 30);
    
    
    
    // ***** 5th sphere on 4th row, white *****
    
    MaterialAmbient[0] = 0.05f;
    MaterialAmbient[1] = 0.05f;
    MaterialAmbient[2] = 0.05f;
    MaterialAmbient[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);
    
    MaterialDiffuse[0] = 0.5f;
    MaterialDiffuse[1] = 0.5f;
    MaterialDiffuse[2] = 0.5f;
    MaterialDiffuse[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);
    
    MaterialSpecular[0] = 0.7f;
    MaterialSpecular[1] = 0.7f;
    MaterialSpecular[2] = 0.7f;
    MaterialSpecular[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);
    
    MaterialShininess = 0.078125 * 128;
    glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(21.1f, 1.5f, 0.0f);
    
    gluSphere(quadric[22], 1.0f, 30, 30);
    
    
    
    // ***** 6th sphere on 4th row, yellow rubber *****
    
    MaterialAmbient[0] = 0.05f;
    MaterialAmbient[1] = 0.05f;
    MaterialAmbient[2] = 0.0f;
    MaterialAmbient[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);
    
    MaterialDiffuse[0] = 0.5f;
    MaterialDiffuse[1] = 0.5f;
    MaterialDiffuse[2] = 0.0f;
    MaterialDiffuse[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);
    
    MaterialSpecular[0] = 0.7f;
    MaterialSpecular[1] = 0.7f;
    MaterialSpecular[2] = 0.04f;
    MaterialSpecular[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);
    
    MaterialShininess = 0.078125 * 128;
    glMaterialf(GL_FRONT, GL_SHININESS, MaterialShininess);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(26.0f, 1.5f, 0.0f);
    
    gluSphere(quadric[23], 1.0f, 30, 30);
    
    
}


void Update(void)
{
    //code
   if (KeyPressed == 1)
	{
		AngleForXRotation += 1.0f;
	}
	else if (KeyPressed == 2)
	{
		AngleForYRotation += 1.0f;
	}
	else if (KeyPressed == 3)
	{
		AngleForZRotation += 1.0f;
	}
}


void Uninitialize(void)
{
    //variable declarations
    GLXContext currentGLXContext;
    
    
    for (int i = 0; i < 24; i++)
    {
        gluDeleteQuadric(quadric[i]);
        quadric[i] = NULL;
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

