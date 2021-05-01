#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<memory.h>
#include<GL/gl.h>
#include<GL/glu.h>
#include<GL/glx.h>   //bridging API
#include<AL/al.h>
#include<AL/alc.h>
#include<math.h>

#include "wavhelper.h"
#include"DrawObjects.h"


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

//OpenAL
ALCdevice* device = NULL;
ALCcontext* context = NULL;
unsigned int buffer;
unsigned int source;
ALsizei alSize, alFrequency;
ALenum alFormat;
ALvoid* alData;
ALboolean alLoop = AL_FALSE;


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
    void alInitialize(void);
    
    gGLXContext = glXCreateContext(gpDisplay, gpXVisualInfo, NULL, GL_TRUE);
    
    glXMakeCurrent(gpDisplay, gWindow, gGLXContext);
    
    glShadeModel(GL_SMOOTH);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);    
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    
    //SetClearColor
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //Black
    
    //OpenAL
	alInitialize();
    
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
    
    GLfloat x, y;
    
    
    //code
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    gluLookAt(0.0f, 0.0f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    
    bgRect();


	//INDIA


	//Flag_A

	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	glScalef(India_scale_x, India_scale_y, 0.0f);

	Flag_A();


	//I

	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	glTranslatef(I1_x, 0.0f, 0.0f);
	glScalef(India_scale_x, India_scale_y, 0.0f);

	I1();


	//A

	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	glTranslatef(A_x, 0.0f, 0.0f);
	glScalef(India_scale_x, India_scale_y, 0.0f);

	A();


	//N

	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	glTranslatef(0.0f, N_y, 0.0f);
	glScalef(India_scale_x, India_scale_y, 0.0f);

	N();


	//I

	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	glTranslatef(0.0f, I2_y, 0.0f);
	glScalef(India_scale_x, India_scale_y, 0.0f);

	I2();


	//D

	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	glTranslatef(0.0f, 0.0f, 0.0f);
	glScalef(India_scale_x, India_scale_y, 0.0f);

	D();


	//Fighter Jets


	//1

	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	glTranslatef(1.25f * cos(Jet1_angle1) + r1_x, 1.25f * sin(Jet1_angle1) + r1_y, 0.0f);
	glTranslatef(Jet1_x, 0.0f, 0.0f);
	glRotatef(Jet1_rot, 0.0f, 0.0f, 1.0f);
	glScalef(0.8f, 0.8f, 0.8f);

	FighterJet1();


	//3

	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	glTranslatef(1.25f * cos(Jet3_angle1) + r3_x, 1.25f * sin(Jet3_angle1) + r3_y, 0.0f);
	glTranslatef(Jet3_x, 0.0f, 0.0f);
	glRotatef(Jet3_rot, 0.0f, 0.0f, 1.0f);
	glScalef(0.8f, 0.8f, 0.8f);

	FighterJet3();


	//2

	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	glTranslatef(Jet2_x, 0.0f, 0.0f);
	glScalef(0.8f, 0.8f, 0.8f);

	FighterJet2();

	//Smoke Tricolor
	glBegin(GL_QUADS);

	//saffron

	glColor3f(Smoke_light_Saffron_R, Smoke_light_Saffron_G, Smoke_light_Saffron_B);

	glVertex3f(-0.225f, 0.0067f, 0.0f);
	glVertex3f(-0.225f, 0.02f, 0.0f);

	glColor3f(Smoke_dark_Saffron_R, Smoke_dark_Saffron_G, Smoke_dark_Saffron_B);

	glVertex3f(-0.35f, 0.02f, 0.0f);
	glVertex3f(-0.35f, 0.0067f, 0.0f);

	//White

	glColor3f(Smoke_light_White_R, Smoke_light_White_G, Smoke_light_White_B);

	glVertex3f(-0.225f, -0.0067f, 0.0f);
	glVertex3f(-0.225f, 0.0067f, 0.0f);

	glColor3f(Smoke_dark_White_R, Smoke_dark_White_G, Smoke_dark_White_B);

	glVertex3f(-0.35f, 0.0067f, 0.0f);
	glVertex3f(-0.35f, -0.0067f, 0.0f);

	//Green

	glColor3f(Smoke_light_Green_R, Smoke_light_Green_G, Smoke_light_Green_B);

	glVertex3f(-0.225f, -0.02f, 0.0f);
	glVertex3f(-0.225f, -0.0067f, 0.0f);

	glColor3f(Smoke_dark_Green_R, Smoke_dark_Green_G, Smoke_dark_Green_B);

	glVertex3f(-0.35f, -0.0067f, 0.0f);
	glVertex3f(-0.35f, -0.02f, 0.0f);

	//glColor3f(0.0f, 0.0f, 0.082f);	

	glEnd();

    
    glXSwapBuffers(gpDisplay, gWindow);
}


void Update(void)
{
	static int frame = 0;

	switch (frame)
	{

	case 0:

		//bgRect

		if (BG_blue <= 0.14f)
		{
			BG_blue += 0.0006f;
		}
		else
		{
			BG_blue = 0.14f;
		}


		//Flag_A

		if (Flag_A_Saffron_B <= 0.081)
		{
			Flag_A_Saffron_B += 0.0003f;
		}
		else
		{
			Flag_A_Saffron_B = 0.081f;
		}


		if (Flag_A_White_B <= 0.081)
		{
			Flag_A_White_B += 0.0003f;
		}
		else
		{
			Flag_A_White_B = 0.081f;
		}


		if (Flag_A_Green_B <= 0.081)
		{
			Flag_A_Green_B += 0.0003f;
		}
		else
		{
			Flag_A_Green_B = 0.081f;
		}


		//D

		if (D_SaffronB <= 0.081f)
		{
			D_SaffronB += 0.0003f;
		}
		else
		{
			D_SaffronB = 0.081f;
			frame++;
		}



		if (D_GreenB <= 0.064f)
		{
			D_GreenB += 0.0003f;
		}
		else
		{
			D_GreenB = 0.064f;
		}
		break;


		//I

	case 1:
		if (I1_x <= 0.0f)
		{
			I1_x += 0.0055f;
		}
		else
		{
			I1_x = 0.0f;
			frame++;
		}
		break;


		//A

	case 2:
		if (A_x >= 0.0f)
		{
			A_x -= 0.0056f;
		}
		else
		{
			A_x = 0.0f;
			frame++;
		}
		break;


		//N

	case 3:
		if (N_y >= 0.0f)
		{
			N_y -= 0.0053f;
		}
		else
		{
			N_y = 0.0f;
			frame++;
		}
		break;


		//I

	case 4:
		if (I2_y <= 0.0f)
		{
			I2_y += 0.005f;
		}
		else
		{
			I2_y = 0.0f;
			frame++;
		}
		break;


		//D

	case 5:
		if (D_SaffronR <= 1.0f)
		{
			D_SaffronR += 0.0028f;
		}
		else
		{
			D_SaffronR = 1.0f;
			frame++;
		}

		if (D_SaffronG <= 0.6f)
		{
			D_SaffronG += 0.0028f;
		}
		else
		{
			D_SaffronG = 0.6f;
		}

		if (D_SaffronB <= 0.2f)
		{
			D_SaffronB += 0.0028f;
		}
		else
		{
			D_SaffronB = 0.2f;
		}


		if (D_GreenR <= 0.07f)
		{
			D_GreenR += 0.0028f;
		}
		else
		{
			D_GreenR = 0.07f;
		}

		if (D_GreenG <= 0.53f)
		{
			D_GreenG += 0.0028f;
		}
		else
		{
			D_GreenG = 0.53f;
		}

		if (D_GreenB <= 0.03f)
		{
			D_GreenB += 0.0028f;
		}
		else
		{
			D_GreenB = 0.03f;
		}
		break;


		//INDIA Scale-up

	case 6:

		if (India_scale_x < 0.9f)
		{
			India_scale_x += 0.00032f;
		}
		else
		{
			India_scale_x = 0.9f;
		}

		if (India_scale_y < 0.9f)
		{
			India_scale_y += 0.00032f;
		}
		else
		{
			India_scale_y = 0.9f;
			frame++;
		}
		break;


	case 7:

		//Jet2

		if (Jet2_x < -1.0f)
		{
			Jet2_x += 0.0055f;
		}
		else
		{
			Jet2_x = -1.0f;
			//frame++;
		}


		//Jet1

		if (Jet1_angle1 <= 3 * PI / 2)
		{
			Jet1_angle1 += 0.0065f;
		}
		else
		{
			Jet1_angle1 = 3 * PI / 2;
		}

		if (Jet1_rot <= 360.0f)
		{
			Jet1_rot += 0.4f;
		}
		else
		{
			Jet1_rot = 360.0f;
		}



		//Jet3

		if (Jet3_angle1 >= PI / 2)
		{
			Jet3_angle1 -= 0.0065f;
		}
		else
		{
			Jet3_angle1 = PI / 2;
			frame++;
		}

		if (Jet3_rot >= 0.0f)
		{
			Jet3_rot -= 0.4f;
		}
		else
		{
			Jet3_rot = 0.0f;
		}
		break;


	case 8:

		//Smoke Tricolor

		//saffron

		//light

		if (Smoke_light_Saffron_R <= 1.0f)
		{
			Smoke_light_Saffron_R += 0.01f;
		}
		else
		{
			Smoke_light_Saffron_R = 1.0f;

		}

		if (Smoke_light_Saffron_G >= 0.6f)
		{
			Smoke_light_Saffron_G -= 0.01f;
		}
		else
		{
			Smoke_light_Saffron_G = 0.6f;

		}

		if (Smoke_light_Saffron_B >= 0.2f)
		{
			Smoke_light_Saffron_B -= 0.01f;
		}
		else
		{
			Smoke_light_Saffron_B = 0.2f;

		}

		//dark

		if (Smoke_dark_Saffron_R <= 1.0f)
		{
			Smoke_dark_Saffron_R += 0.01f;
		}
		else
		{
			Smoke_dark_Saffron_R = 1.0f;

		}

		if (Smoke_dark_Saffron_G <= 0.6f)
		{
			Smoke_dark_Saffron_G += 0.01f;
		}
		else
		{
			Smoke_dark_Saffron_G = 0.6f;

		}

		if (Smoke_dark_Saffron_B <= 0.2f)
		{
			Smoke_dark_Saffron_B += 0.01f;
		}
		else
		{
			Smoke_dark_Saffron_B = 0.2f;

		}

		//white

		//light

		if (Smoke_light_White_R <= 1.0f)
		{
			Smoke_light_White_R += 0.01f;
		}
		else
		{
			Smoke_light_White_R = 1.0f;

		}

		if (Smoke_light_White_G <= 1.0f)
		{
			Smoke_light_White_G += 0.01f;
		}
		else
		{
			Smoke_light_White_G = 1.0f;

		}

		if (Smoke_light_White_B <= 1.0f)
		{
			Smoke_light_White_B += 0.01f;
		}
		else
		{
			Smoke_light_White_B = 1.0f;

		}

		//dark

		if (Smoke_dark_White_R <= 1.0f)
		{
			Smoke_dark_White_R += 0.01f;
		}
		else
		{
			Smoke_dark_White_R = 1.0f;

		}

		if (Smoke_dark_White_G <= 1.0f)
		{
			Smoke_dark_White_G += 0.01f;
		}
		else
		{
			Smoke_dark_White_G = 1.0f;

		}

		if (Smoke_dark_White_B <= 1.0f)
		{
			Smoke_dark_White_B += 0.01f;
		}
		else
		{
			Smoke_dark_White_B = 1.0f;

		}


		//Green

		//light

		if (Smoke_light_Green_R <= 0.07f)
		{
			Smoke_light_Green_R += 0.01f;
		}
		else
		{
			Smoke_light_Green_R = 0.07f;

		}

		if (Smoke_light_Green_G >= 0.53f)
		{
			Smoke_light_Green_G -= 0.01f;
		}
		else
		{
			Smoke_light_Green_G = 0.53f;

		}

		if (Smoke_light_Green_B >= 0.03f)
		{
			Smoke_light_Green_B -= 0.01f;
		}
		else
		{
			Smoke_light_Green_B = 0.03f;

		}

		//dark

		if (Smoke_dark_Green_R <= 0.07f)
		{
			Smoke_dark_Green_R += 0.01f;
		}
		else
		{
			Smoke_dark_Green_R = 0.07f;

		}

		if (Smoke_dark_Green_G <= 0.53f)
		{
			Smoke_dark_Green_G += 0.01f;
		}
		else
		{
			Smoke_dark_Green_G = 0.53f;

		}

		if (Smoke_dark_Green_B <= 0.03f)
		{
			Smoke_dark_Green_B += 0.01f;
		}
		else
		{
			Smoke_dark_Green_B = 0.03f;

		}


		//Jet1

		if (Jet1_x <= 2.0f)
		{
			Jet1_x += 0.0055f;
		}
		else
		{
			Jet1_x = 2.0f;
			//frame++;
		}


		//Jet2

		if (Jet2_x < 1.0f)
		{
			Jet2_x += 0.0055f;
		}
		else
		{
			Jet2_x = 1.0f;
		}


		//Jet3

		if (Jet3_x < 2.0f)
		{
			Jet3_x += 0.0055f;
		}
		else
		{
			Jet3_x = 2.0f;
			frame++;
		}

		if (Jet2_x >= 0.5f)
		{
			//saffron
			if (Flag_A_Saffron_R <= 1.0f)
			{
				Flag_A_Saffron_R += 0.1f;
			}
			else
			{
				Flag_A_Saffron_R = 1.0f;
			}

			if (Flag_A_Saffron_G <= 0.6f)
			{
				Flag_A_Saffron_G += 0.1f;
			}
			else
			{
				Flag_A_Saffron_G = 0.6f;
			}

			if (Flag_A_Saffron_B <= 0.2f)
			{
				Flag_A_Saffron_B += 0.1f;
			}
			else
			{
				Flag_A_Saffron_B = 0.2f;
			}


			//white
			if (Flag_A_White_R <= 1.0f)
			{
				Flag_A_White_R += 0.1f;
			}
			else
			{
				Flag_A_White_R = 1.0f;
			}

			if (Flag_A_White_G <= 1.0f)
			{
				Flag_A_White_G += 0.1f;
			}
			else
			{
				Flag_A_White_G = 1.0f;
			}

			if (Flag_A_White_B <= 1.0f)
			{
				Flag_A_White_B += 0.1f;
			}
			else
			{
				Flag_A_White_B = 1.0f;
			}


			//green
			if (Flag_A_Green_R <= 0.07f)
			{
				Flag_A_Green_R += 0.1f;
			}
			else
			{
				Flag_A_Green_R = 0.07f;
			}

			if (Flag_A_Green_G <= 0.53f)
			{
				Flag_A_Green_G += 0.1f;
			}
			else
			{
				Flag_A_Green_G = 0.53f;
			}

			if (Flag_A_Green_B >= 0.03f)
			{
				Flag_A_Green_B -= 0.1f;
			}
			else
			{
				Flag_A_Green_B = 0.03f;
			}



			/*GLfloat Flag_A_Saffron_R = 0.0f;
			GLfloat Flag_A_Saffron_G = 0.0f;
			GLfloat Flag_A_Saffron_B = 0.0f;

			GLfloat Flag_A_White_R = 0.0f;
			GLfloat Flag_A_White_G = 0.0f;
			GLfloat Flag_A_White_B = 0.0f;

			GLfloat Flag_A_Green_R = 0.0f;
			GLfloat Flag_A_Green_G = 0.0f;
			GLfloat Flag_A_Green_B = 0.0f;*/
		}
		break;


	case 9:


		//Smoke Tricolor

		//saffron

		//light

		if (Smoke_light_Saffron_R <= 1.0f)
		{
			Smoke_light_Saffron_R += 0.05f;
		}
		else
		{
			Smoke_light_Saffron_R = 1.0f;

		}

		if (Smoke_light_Saffron_G <= 1.0f)
		{
			Smoke_light_Saffron_G += 0.05f;
		}
		else
		{
			Smoke_light_Saffron_G = 1.0f;

		}

		if (Smoke_light_Saffron_B <= 1.0f)
		{
			Smoke_light_Saffron_B += 0.05f;
		}
		else
		{
			Smoke_light_Saffron_B = 1.0f;

		}

		//dark

		if (Smoke_dark_Saffron_R >= 0.0f)
		{
			Smoke_dark_Saffron_R -= 0.05f;
		}
		else
		{
			Smoke_dark_Saffron_R = 0.0f;

		}

		if (Smoke_dark_Saffron_G >= 0.0f)
		{
			Smoke_dark_Saffron_G -= 0.05f;
		}
		else
		{
			Smoke_dark_Saffron_G = 0.0f;

		}

		if (Smoke_dark_Saffron_B >= 0.082f)
		{
			Smoke_dark_Saffron_B -= 0.05f;
		}
		else
		{
			Smoke_dark_Saffron_B = 0.082f;

		}

		//white

		//light

		if (Smoke_light_White_R <= 1.0f)
		{
			Smoke_light_White_R += 0.05f;
		}
		else
		{
			Smoke_light_White_R = 1.0f;

		}

		if (Smoke_light_White_G <= 1.0f)
		{
			Smoke_light_White_G += 0.05f;
		}
		else
		{
			Smoke_light_White_G = 1.0f;

		}

		if (Smoke_light_White_B <= 1.0f)
		{
			Smoke_light_White_B += 0.05f;
		}
		else
		{
			Smoke_light_White_B = 1.0f;
		}

		//dark

		if (Smoke_dark_White_R >= 0.0f)
		{
			Smoke_dark_White_R -= 0.05f;
		}
		else
		{
			Smoke_dark_White_R = 0.0f;

		}

		if (Smoke_dark_White_G >= 0.0f)
		{
			Smoke_dark_White_G -= 0.05f;
		}
		else
		{
			Smoke_dark_White_G = 0.0f;

		}

		if (Smoke_dark_White_B >= 0.082f)
		{
			Smoke_dark_White_B -= 0.05f;
		}
		else
		{
			Smoke_dark_White_B = 0.082f;

		}


		//Green

		//light

		if (Smoke_light_Green_R <= 1.0f)
		{
			Smoke_light_Green_R += 0.05f;
		}
		else
		{
			Smoke_light_Green_R = 1.0f;

		}

		if (Smoke_light_Green_G <= 1.0f)
		{
			Smoke_light_Green_G += 0.05f;
		}
		else
		{
			Smoke_light_Green_G = 1.0f;

		}

		if (Smoke_light_Green_B <= 1.0f)
		{
			Smoke_light_Green_B += 0.05f;
		}
		else
		{
			Smoke_light_Green_B = 1.0f;

		}

		//dark

		if (Smoke_dark_Green_R >= 0.0f)
		{
			Smoke_dark_Green_R -= 0.05f;
		}
		else
		{
			Smoke_dark_Green_R = 0.0f;

		}

		if (Smoke_dark_Green_G >= 0.0f)
		{
			Smoke_dark_Green_G -= 0.05f;
		}
		else
		{
			Smoke_dark_Green_G = 0.0f;

		}

		if (Smoke_dark_Green_B <= 0.082f)
		{
			Smoke_dark_Green_B += 0.05f;
		}
		else
		{
			Smoke_dark_Green_B = 0.082f;

		}


		//Jet1

		if (Jet1_angle1 <= 2.05 * PI)
		{
			Jet1_angle1 += 0.0065f;
		}
		else
		{
			Jet1_angle1 = 2.05 * PI;
		}

		if (Jet1_rot <= 450.0f)
		{
			Jet1_rot += 0.4f;
		}
		else
		{
			Jet1_rot = 450.0f;
		}


		//Jet3

		if (Jet3_angle1 >= -PI / 18)
		{
			Jet3_angle1 -= 0.0065f;
		}
		else
		{
			Jet3_angle1 = -PI / 18;
			frame++;
		}

		if (Jet3_rot >= -270.0f)
		{
			Jet3_rot -= 0.4f;
		}
		else
		{
			Jet3_rot = -270.0f;

		}

		//Jet2

		if (Jet2_x < 2.5f)
		{
			Jet2_x += 0.0055f;
		}
		else
		{
			Jet2_x = 2.5f;
		}
		break;



		//INDIA Scale-up

	case 10:

		if (India_scale_x < 1.0f)
		{
			India_scale_x += 0.00032f;
		}
		else
		{
			India_scale_x = 1.0f;
		}

		if (India_scale_y < 1.0f)
		{
			India_scale_y += 0.00032f;
		}
		else
		{
			India_scale_y = 1.0f;
			frame++;
		}
		break;


	}


}


void alInitialize(void)
{
	int channel, sampleRate, beatsPerSecond, size;
	device = alcOpenDevice(NULL);

	if (device)
	{
		
		context = alcCreateContext(device, NULL);

		

		alcMakeContextCurrent(context);

		alGenBuffers(1, &buffer);
		alGenSources(1, &source);

		

		alData = loadWav("AbTumhareHawaleVatanSathiyon.wav", &channel, &sampleRate, &beatsPerSecond, &size);

		

		if (channel == 1)
		{
			if (beatsPerSecond == 8)
			{
				alFormat = AL_FORMAT_MONO8;
				
			}
			else
			{
				alFormat = AL_FORMAT_MONO16;
				
			}
		}
		else
		{
			if (beatsPerSecond == 8)
			{
				alFormat = AL_FORMAT_STEREO8;
				
			}
			else
			{
				alFormat = AL_FORMAT_STEREO16;
				
			}
		}



		alBufferData(buffer, alFormat, alData, size, sampleRate);

		alSourcei(source, AL_BUFFER, buffer);

		alSourcePlay(source);

		
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
    
    //OpenAL

	alDeleteSources(1, &source);
	alDeleteBuffers(1, &buffer);

	context = alcGetCurrentContext();
	device = alcGetContextsDevice(context);
	alcMakeContextCurrent(NULL);
	alcDestroyContext(context);
	alcCloseDevice(device);
    
}

