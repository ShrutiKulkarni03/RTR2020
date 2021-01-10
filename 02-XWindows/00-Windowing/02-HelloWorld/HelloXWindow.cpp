#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<memory.h>

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
int giWindowWidth=800;
int giWindowHeight=600;

//entry-point function
int main(void)
{
	//function prototypes
	void CreateWindow(void);
	void ToggleFullscreen(void);
	void uninitialize(void);
	
	//variable declarations
	int winWidth=giWindowWidth;
	int winHeight=giWindowHeight;
	
	static XFontStruct *pXFontStruct = NULL;
	static GC gc;
	XGCValues gcValues;
	XColor greenColor;
	char str[] = "Hello XWindow! - Shruti Kulkarni";
	int stringLength;
	int stringWidth;
	int fontHeight;
	
	//code
	CreateWindow();
	
	//message loop
	XEvent event;
	KeySym keysym;
	
	while(1)
    {
		 XNextEvent(gpDisplay, &event);
		 switch(event.type)
	     {
			 case MapNotify:
				 pXFontStruct = XLoadQueryFont(gpDisplay, "fixed");
				 break;
				 
			 case KeyPress:
				 keysym=XkbKeycodeToKeysym(gpDisplay, event.xkey.keycode, 0, 0);
				 switch(keysym)
				{
					 case XK_Escape:
						 XUnloadFont(gpDisplay, pXFontStruct->fid);
						 XFreeGC(gpDisplay, gc);
						 uninitialize();
						 exit(0);
						 
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
				 break;
				 
			 case Expose:
				 gc=XCreateGC(gpDisplay, gWindow, 0, &gcValues);
				 XSetFont(gpDisplay, gc, pXFontStruct->fid);
				 XAllocNamedColor(gpDisplay, gColormap, "green", &greenColor, &greenColor);
				 XSetForeground(gpDisplay, gc, greenColor.pixel);
				 
				 stringLength = strlen(str);
				 stringWidth = XTextWidth(pXFontStruct, str, stringLength);
				 fontHeight = pXFontStruct->ascent + pXFontStruct->descent;
				 XDrawString(gpDisplay, gWindow, gc, (winWidth/2 - stringWidth/2), (winHeight/2-fontHeight/2), str, stringLength);
				 break;
				 
			 case DestroyNotify:
				 break;
				 
			 case 33:
				 XUnloadFont(gpDisplay, pXFontStruct->fid);
				 XFreeGC(gpDisplay, gc);
				 uninitialize();
				 exit(0);
				 
			 default:
				 break;
						 
			
				 
	     }
    }
    
    uninitialize();
	
	return(0);
}


void CreateWindow(void)
{
	//function prototypes
	void uninitialize(void);
	
	//variable declarations
	XSetWindowAttributes winAttribs;
	int defaultScreen;
	int defaultDepth;
	int styleMask;
	
	//code
	gpDisplay=XOpenDisplay(NULL);
	if(gpDisplay==NULL)
    {
		 printf("Error : Unable to open X Display.\nExiting Now!\n\n");
		 uninitialize();
		 exit(1);
	}
	
	defaultScreen=XDefaultScreen(gpDisplay);
	
	defaultDepth=DefaultDepth(gpDisplay,defaultScreen);
	
	gpXVisualInfo=(XVisualInfo *)malloc(sizeof(XVisualInfo));
	if(gpXVisualInfo==NULL)
    {
		 printf("Error : Unable to allocate memory for Visual Info.\nExiting Now!\n\n");
		 uninitialize();
		 exit(1);
    }
    
    XMatchVisualInfo(gpDisplay, defaultScreen, defaultDepth, TrueColor, gpXVisualInfo);
	if(gpXVisualInfo==NULL)
    {
		 printf("Error : Unable to get a Visual.\nExiting Now!\n\n");
		 uninitialize();
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
		uninitialize();
		exit(1);
    }
    
    XStoreName(gpDisplay, gWindow, "First XWindow - Shruti Kulkarni");
	
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


void uninitialize(void)
{
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





