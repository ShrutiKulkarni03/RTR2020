#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<memory.h>
#include<GL/glew.h>
#include<GL/gl.h>
#include<GL/glx.h>   //bridging API
#include"vmath.h"

#include<SOIL/SOIL.h>

#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include<X11/XKBlib.h>
#include<X11/keysym.h>

//namespaces
using namespace std;
using namespace vmath;

enum
{
	SPK_ATTRIBUTE_POSITION = 0,
	SPK_ATTRIBUTE_COLOR,
	SPK_ATTRIBUTE_NORMAL,
	SPK_ATTRIBUTE_TEXCOORD,	
};

//global variable declarations
bool bFullscreen = false;


//function prototypes
void Resize(int, int);
GLuint LoadBitmapAsTexture(const char *);
void Uninitialize(void);

Display *gpDisplay=NULL;
XVisualInfo *gpXVisualInfo=NULL;
Colormap gColormap;
Window gWindow;
GLXContext gGLXContext;
FILE* gpFile = NULL;



typedef GLXContext(* glXCreateContextAttribsARBProc)(Display *, GLXFBConfig, GLXContext, Bool, const int *);
glXCreateContextAttribsARBProc glXCreateContextAttribsARB = NULL;
GLXFBConfig gGLXFBConfig;

int giWindowWidth=800;
int giWindowHeight=600;


//shader variables

GLuint shaderProgramObjectPyramid;
GLuint shaderProgramObjectCube;

//framebuffer variables
GLuint fbo;                         //frame buffer object
GLuint textureRenderBuffer; 
GLuint rbo;                        //render buffer object

int fbWidth;
int fbHeight;

//vao for pyramid
GLuint vao_pyramid;                          //vertex array object
GLuint vbo_position_pyramid;                 //vertex buffer object
GLuint vbo_texture_pyramid;

//vao for cube
GLuint vao_cube;
GLuint vbo_position_cube;
GLuint vbo_texture_cube;

GLuint mvpMatrixUniformPyramid;
GLuint textureSamplerUniformPyramid;

GLuint mvpMatrixUniformCube;
GLuint textureSamplerUniformCube;

mat4 perspectiveProjectionMatrix;   //4x4 matrix

//update variables

GLfloat pyramidAngle = 0.0f;
GLfloat cubeAngle = 0.0f;

//texture variables

GLuint textureID;
GLuint stone_texture;

//entry-point function
int main(void)
{
    //function prototypes
    void CreateWindow(void);
    void ToggleFullscreen(void);
    void Initialize(void);
    void Uninitialize(void);
    void Resize(int, int);
    void Draw(void);
    void Update(void);
    
    //variable declarations
    int winWidth=giWindowWidth;
    int winHeight=giWindowHeight;
    bool bDone = false;
    
    gpFile = fopen("RenderLog.txt", "w");
    
    if(gpFile == NULL)
	{
		exit(0);
	}
	else
	{
		fprintf(gpFile, ("Log File Created Successfully, Program Started Successfully.\n\n"));
	}
    
    
    
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
    
    GLXFBConfig * pGLXFBConfig = NULL;
    GLXFBConfig bestGLXFBConfig;
    XVisualInfo * pTempXVisualInfo = NULL;
    int numFBConfigs = 0;
    
    int defaultScreen;
    int styleMask;
    static int frameBufferAttributes[] = {GLX_DOUBLEBUFFER, True,
                                          GLX_X_RENDERABLE, True,
                                          GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
                                          GLX_RENDER_TYPE, GLX_RGBA_BIT,
                                          GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
                                          GLX_RED_SIZE, 8,
                                          GLX_GREEN_SIZE, 8,
                                          GLX_BLUE_SIZE, 8,
                                          GLX_ALPHA_SIZE, 8,
                                          GLX_STENCIL_SIZE, 8,
                                          GLX_DEPTH_SIZE, 24,      //V4L (Video for Linux) recommends 24bit not 32bit
                                          None};                   //when only 5 members out of many are to be initialized use '0' or 'None'               
    
    //code
    gpDisplay=XOpenDisplay(NULL);
    if(gpDisplay==NULL)
    {
        fprintf(gpFile, "Error : Unable to open X Display.\nExiting Now!\n\n");
        Uninitialize();
        exit(1);
    }
    
    
    defaultScreen=XDefaultScreen(gpDisplay);
    
    pGLXFBConfig = glXChooseFBConfig(gpDisplay, XDefaultScreen(gpDisplay), frameBufferAttributes, &numFBConfigs);
    
    fprintf(gpFile, "Number of FBConfigs = %d\n\n", numFBConfigs);
    
    int bestFrameBufferConfig = -1;
    int worstFrameBufferConfig = -1;
    int bestNumberOfSamples = -1;
    int worstNumberOfSamples = 999;
    
    for(int i = 0; i < numFBConfigs; i++)
    {
        pTempXVisualInfo = glXGetVisualFromFBConfig(gpDisplay, pGLXFBConfig[i]);
        
        if(pTempXVisualInfo != NULL)
        {
            int sampleBuffers, samples;
            
            glXGetFBConfigAttrib(gpDisplay, pGLXFBConfig[i], GLX_SAMPLE_BUFFERS, &sampleBuffers);
            glXGetFBConfigAttrib(gpDisplay, pGLXFBConfig[i], GLX_SAMPLES, &samples);
            
            if(bestFrameBufferConfig < 0 || sampleBuffers && samples > bestNumberOfSamples)
            {
                bestFrameBufferConfig = i;
                bestNumberOfSamples = samples;
            }
            
            if(worstFrameBufferConfig < 0 || !sampleBuffers || samples < 999)
            {
                worstFrameBufferConfig = i;
                worstNumberOfSamples = samples;
            }
            
            fprintf(gpFile, "When i = %d -> samples = %d -> sampleBuffers = %d\n", i, samples, sampleBuffers);
            
        }
        
        XFree(pTempXVisualInfo);
    }
    
    
    bestGLXFBConfig = pGLXFBConfig[bestFrameBufferConfig];
    gGLXFBConfig = bestGLXFBConfig;
    
    XFree(pGLXFBConfig);
    
    gpXVisualInfo = glXGetVisualFromFBConfig(gpDisplay, gGLXFBConfig);
    
    //gpXVisualInfo=(XVisualInfo *)malloc(sizeof(XVisualInfo));
    //gpXVisualInfo = glXChooseVisual(gpDisplay, defaultScreen, frameBufferAttributes);
    
   
    if(gpXVisualInfo==NULL)
    {
        fprintf(gpFile, "Error : Unable to allocate memory for Visual Info.\nExiting Now!\n\n");
        Uninitialize();
        exit(1);
    }
    
    
    if(gpXVisualInfo==NULL)
    {
        fprintf(gpFile, "Error : Unable to get a Visual.\nExiting Now!\n\n");
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
        fprintf(gpFile, "Error : Failed to create Main Window.\nExiting Now!\n\n");
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
    
    GLuint vertexShaderObjectPyramid;
	GLuint fragmentShaderObjectPyramid;
	GLuint vertexShaderObjectCube;
	GLuint fragmentShaderObjectCube;
    
    glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddressARB((GLubyte*)"glXCreateContextAttribsARB");
    
    const int attribs[] = {GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
                           GLX_CONTEXT_MINOR_VERSION_ARB, 5,
                           GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
                           None};
    
    gGLXContext = glXCreateContextAttribsARB(gpDisplay, gGLXFBConfig, 0, True, attribs);
    
    if(!gGLXContext)
    {
        const int attribs[] = {GLX_CONTEXT_MAJOR_VERSION_ARB, 1,
                               GLX_CONTEXT_MINOR_VERSION_ARB, 0,
                               0,
                               None};
                     
        gGLXContext = glXCreateContextAttribsARB(gpDisplay, gGLXFBConfig, 0, True, attribs);
    }
    
    Bool isDirectContext = glXIsDirect(gpDisplay, gGLXContext);
    
    if(isDirectContext == true)
    {
        fprintf(gpFile, "\nRendering Context is Direct Hardware Rendering Context.\n");
    }
    else
    {
        fprintf(gpFile, "\nRendering Context is Software Rendering Context.\n");
    }
                           
    glXMakeCurrent(gpDisplay, gWindow, gGLXContext);
    
    
    GLenum glew_error = glewInit();
	if (glew_error != GLEW_OK)
	{
		Uninitialize();
	}
	
	
	//OpenGL related LOG

	fprintf(gpFile, "OpenGL VENDOR : %s\n", glGetString(GL_VENDOR));
	fprintf(gpFile, "OpenGL RENDERER : %s\n", glGetString(GL_RENDERER));
	fprintf(gpFile, "OpenGL VERSION : %s\n", glGetString(GL_VERSION));
	fprintf(gpFile, "GLSL[Graphics Library Shading Language] VERSION : %s\n\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	fprintf(gpFile, "EXTENTIONS : \n");

	//OpenGL Enabled Extensions

	GLint numExt;

	glGetIntegerv(GL_NUM_EXTENSIONS, &numExt);

	for (int i = 0; i < numExt; i++)
	{
		fprintf(gpFile, "%s\n", glGetStringi(GL_EXTENSIONS, i));
	}
	
	
	
	/********SHADERS********/


	/*****VERTEX SHADER PYRAMID*****/

	//create shader
	vertexShaderObjectPyramid = glCreateShader(GL_VERTEX_SHADER);

	//provide source code to vertex shader
	const GLchar* vertexShaderSourceCodePyramid =
		"#version 450 core \n" \
		"\n" \
		"in vec4 vPosition; \n" \
		"in vec2 vTexcoord; \n" \
		"uniform mat4 u_mvpMatrix; \n" \
		"out vec2 out_texcoord; \n" \
		"void main(void) \n" \
		"{ \n" \
		"	gl_Position = u_mvpMatrix * vPosition; \n" \
		"	out_texcoord = vTexcoord; \n" \
		"} \n";

	glShaderSource(vertexShaderObjectPyramid, 1, (const GLchar**)&vertexShaderSourceCodePyramid, NULL);

	//compile shader
	glCompileShader(vertexShaderObjectPyramid);

	//vertex shader compilation error checking
	GLint infoLogLength = 0;
	GLint shaderCompiledStatus = 0;
	char* szBuffer = NULL;

	glGetShaderiv(vertexShaderObjectPyramid, GL_COMPILE_STATUS, &shaderCompiledStatus);

	if (shaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObjectPyramid, GL_INFO_LOG_LENGTH, &infoLogLength);

		if (infoLogLength > 0)
		{
			szBuffer = (char*)malloc(infoLogLength);
			if (szBuffer != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(vertexShaderObjectPyramid, infoLogLength, &written, szBuffer);
				fprintf(gpFile, "\nVertex Shader Compilation Log : %s\n", szBuffer);
				free(szBuffer);
				Uninitialize();
			}

		}
	}


	/*****FRAGMENT SHADER PYRAMID*****/

	//create shader
	fragmentShaderObjectPyramid = glCreateShader(GL_FRAGMENT_SHADER);

	//provide source code to fragment shader
	const GLchar* fragmentShaderSourceCodePyramid =
		"#version 450 core \n" \
		"\n" \
		"in vec2 out_texcoord; \n" \
		"uniform sampler2D u_texture_sampler; \n" \
		"out vec4 FragColor; \n" \
		"void main(void) \n" \
		"{ \n" \
		"	FragColor = texture(u_texture_sampler, out_texcoord); \n" \
		"} \n";

	glShaderSource(fragmentShaderObjectPyramid, 1, (const GLchar**)&fragmentShaderSourceCodePyramid, NULL);

	//compile shader
	glCompileShader(fragmentShaderObjectPyramid);

	//fragment shader compilation error checking
	infoLogLength = 0;
	shaderCompiledStatus = 0;
	szBuffer = NULL;

	glGetShaderiv(fragmentShaderObjectPyramid, GL_COMPILE_STATUS, &shaderCompiledStatus);

	if (shaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(fragmentShaderObjectPyramid, GL_INFO_LOG_LENGTH, &infoLogLength);

		if (infoLogLength > 0)
		{
			szBuffer = (char*)malloc(infoLogLength);
			if (szBuffer != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(fragmentShaderObjectPyramid, infoLogLength, &written, szBuffer);
				fprintf(gpFile, "\nFragment Shader Compilation Log : %s\n", szBuffer);
				free(szBuffer);
				Uninitialize();

			}
		}
	}


	/*****SHADER PROGRAM PYRAMID*****/

	//create
	shaderProgramObjectPyramid = glCreateProgram();

	//attach vertex shader to shader program
	glAttachShader(shaderProgramObjectPyramid, vertexShaderObjectPyramid);

	//attach fragment shader to shader program
	glAttachShader(shaderProgramObjectPyramid, fragmentShaderObjectPyramid);

	//pre-linking binding
	glBindAttribLocation(shaderProgramObjectPyramid, SPK_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(shaderProgramObjectPyramid, SPK_ATTRIBUTE_TEXCOORD, "vTexcoord");

	//link shader
	glLinkProgram(shaderProgramObjectPyramid);

	//shader linking error checking
	infoLogLength = 0;
	GLint shaderProgramLinkStatus;
	szBuffer = NULL;

	glGetProgramiv(shaderProgramObjectPyramid, GL_LINK_STATUS, &shaderProgramLinkStatus);

	if (shaderProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObjectPyramid, GL_INFO_LOG_LENGTH, &infoLogLength);

		if (infoLogLength > 0)
		{
			szBuffer = (char*)malloc(infoLogLength);

			if (szBuffer != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(shaderProgramObjectPyramid, infoLogLength, &written, szBuffer);
				fprintf(gpFile, "\nShader Program Link Log : %s\n", szBuffer);
				free(szBuffer);
				Uninitialize();
			}
		}
	}

	//get MVP uniform location
	mvpMatrixUniformPyramid = glGetUniformLocation(shaderProgramObjectPyramid, "u_mvpMatrix");
	textureSamplerUniformPyramid = glGetUniformLocation(shaderProgramObjectPyramid, "u_texture_sampler");




	/*****VERTEX SHADER CUBE*****/

	//create shader
	vertexShaderObjectCube = glCreateShader(GL_VERTEX_SHADER);

	//provide source code to vertex shader
	const GLchar* vertexShaderSourceCodeCube =
		"#version 450 core \n" \
		"\n" \
		"in vec4 vPosition; \n" \
		"in vec2 vTexcoord; \n" \
		"uniform mat4 u_mvpMatrix; \n" \
		"out vec2 out_texcoord; \n" \
		"void main(void) \n" \
		"{ \n" \
		"	gl_Position = u_mvpMatrix * vPosition; \n" \
		"	out_texcoord = vTexcoord; \n" \
		"} \n";

	glShaderSource(vertexShaderObjectCube, 1, (const GLchar**)&vertexShaderSourceCodeCube, NULL);

	//compile shader
	glCompileShader(vertexShaderObjectCube);

	//vertex shader compilation error checking
	infoLogLength = 0;
	shaderCompiledStatus = 0;
	szBuffer = NULL;

	glGetShaderiv(vertexShaderObjectCube, GL_COMPILE_STATUS, &shaderCompiledStatus);

	if (shaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObjectCube, GL_INFO_LOG_LENGTH, &infoLogLength);

		if (infoLogLength > 0)
		{
			szBuffer = (char*)malloc(infoLogLength);
			if (szBuffer != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(vertexShaderObjectCube, infoLogLength, &written, szBuffer);
				fprintf(gpFile, "\nVertex Shader Compilation Log : %s\n", szBuffer);
				free(szBuffer);
				Uninitialize();
			}

		}
	}


	/*****FRAGMENT SHADER CUBE*****/

	//create shader
	fragmentShaderObjectCube = glCreateShader(GL_FRAGMENT_SHADER);

	//provide source code to fragment shader
	const GLchar* fragmentShaderSourceCodeCube =
		"#version 450 core \n" \
		"\n" \
		"in vec2 out_texcoord; \n" \
		"uniform sampler2D u_texture_sampler; \n" \
		"out vec4 FragColor; \n" \
		"void main(void) \n" \
		"{ \n" \
		"	vec3 col = texture(u_texture_sampler, out_texcoord).rgb; \n" \
		"	FragColor = vec4(col, 1.0); \n" \
		"} \n";

	glShaderSource(fragmentShaderObjectCube, 1, (const GLchar**)&fragmentShaderSourceCodeCube, NULL);

	//compile shader
	glCompileShader(fragmentShaderObjectCube);

	//fragment shader compilation error checking
	infoLogLength = 0;
	shaderCompiledStatus = 0;
	szBuffer = NULL;

	glGetShaderiv(fragmentShaderObjectCube, GL_COMPILE_STATUS, &shaderCompiledStatus);

	if (shaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(fragmentShaderObjectCube, GL_INFO_LOG_LENGTH, &infoLogLength);

		if (infoLogLength > 0)
		{
			szBuffer = (char*)malloc(infoLogLength);
			if (szBuffer != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(fragmentShaderObjectCube, infoLogLength, &written, szBuffer);
				fprintf(gpFile, "\nFragment Shader Compilation Log : %s\n", szBuffer);
				free(szBuffer);
				Uninitialize();

			}
		}
	}


	/*****SHADER PROGRAM CUBE*****/

	//create
	shaderProgramObjectCube = glCreateProgram();

	//attach vertex shader to shader program
	glAttachShader(shaderProgramObjectCube, vertexShaderObjectPyramid);

	//attach fragment shader to shader program
	glAttachShader(shaderProgramObjectCube, fragmentShaderObjectPyramid);

	//pre-linking binding
	glBindAttribLocation(shaderProgramObjectCube, SPK_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(shaderProgramObjectCube, SPK_ATTRIBUTE_TEXCOORD, "vTexcoord");

	//link shader
	glLinkProgram(shaderProgramObjectCube);

	//shader linking error checking
	infoLogLength = 0;
	shaderProgramLinkStatus;
	szBuffer = NULL;

	glGetProgramiv(shaderProgramObjectCube, GL_LINK_STATUS, &shaderProgramLinkStatus);

	if (shaderProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObjectCube, GL_INFO_LOG_LENGTH, &infoLogLength);

		if (infoLogLength > 0)
		{
			szBuffer = (char*)malloc(infoLogLength);

			if (szBuffer != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(shaderProgramObjectCube, infoLogLength, &written, szBuffer);
				fprintf(gpFile, "\nShader Program Link Log : %s\n", szBuffer);
				free(szBuffer);
				Uninitialize();
			}
		}
	}

	//get MVP uniform location
	mvpMatrixUniformCube = glGetUniformLocation(shaderProgramObjectCube, "u_mvpMatrix");
	textureSamplerUniformCube = glGetUniformLocation(shaderProgramObjectCube, "u_texture_sampler");


	//vertices array declaration

	//PYRAMID

	//vertices
	const GLfloat pyramidVertices[] = { 0.0f, 0.5f, 0.0f,     //front
									   -0.5f, -0.5f, 0.5f,
										0.5f, -0.5f, 0.5f,
										0.0f, 0.5f, 0.0f,     //right
										0.5f, -0.5f, 0.5f,
										0.5f, -0.5f, -0.5f,
										0.0f, 0.5f, 0.0f,     //back
										0.5f, -0.5f, -0.5f,
									   -0.5f, -0.5f, -0.5f,
										0.0f, 0.5f, 0.0f,     //left
									   -0.5f, -0.5f, -0.5f,
									   -0.5f, -0.5f, 0.5f };

	//color
	const GLfloat pyramidTexture[] = { 0.5f, 1.0f,      //front
									   0.0f, 0.0f,
									   1.0f, 0.0f,
								       0.5f, 1.0f,      //right
									   1.0f, 0.0f, 
									   0.0f, 0.0f,
									   0.5f, 1.0f,      //back
									   1.0f, 0.0f,
									   0.0f, 0.0f,
									   0.5f, 1.0f,      //left
									   0.0f, 0.0f,
									   1.0f, 0.0f, };


	//CUBE

	//vertices
	const GLfloat cubeVertices[] = { 1.0f, 1.0f, 1.0f,        //front
									-1.0f, 1.0f, 1.0f,
									-1.0f, -1.0f, 1.0f,
									 1.0f, -1.0f, 1.0f,
									 1.0f, 1.0f, -1.0f,       //right
									 1.0f, 1.0f, 1.0f,
									 1.0f, -1.0f, 1.0f,
									 1.0f, -1.0f, -1.0f,
									-1.0f, 1.0f, -1.0f,       //back
									 1.0f, 1.0f, -1.0f,
									 1.0f, -1.0f, -1.0f,
									-1.0f, -1.0f, -1.0f,
									-1.0f, 1.0f, 1.0f,        //left
									-1.0f, 1.0f, -1.0f,
									-1.0f, -1.0f, -1.0f,
									-1.0f, -1.0f, 1.0f,
									 1.0f, 1.0f, -1.0f,       //top
									-1.0f, 1.0f, -1.0f,
									-1.0f, 1.0f, 1.0f,
									 1.0f, 1.0f, 1.0f,
									 1.0f, -1.0f, -1.0f,      //bottom
									-1.0f, -1.0f, -1.0f,
									-1.0f, -1.0f, 1.0f,
									 1.0f, -1.0f, 1.0f };

	//color
	const GLfloat cubeTexture[] = { 1.0f, 1.0f,        //front
								    0.0f, 1.0f,
								    0.0f, 0.0f,
								    1.0f, 0.0f,
								    0.0f, 1.0f,        //right
								    1.0f, 1.0f,
								    1.0f, 0.0f,
								    0.0f, 0.0f,
								    0.0f, 0.0f,        //back
						  		    1.0f, 0.0f,
								    1.0f, 1.0f,
								    0.0f, 1.0f,
								    0.0f, 1.0f,        //left
								    1.0f, 1.0f,
								    1.0f, 0.0f,
								    0.0f, 0.0f,
								    1.0f, 1.0f,        //top
								    0.0f, 1.0f,
								    0.0f, 0.0f,
								    1.0f, 0.0f,
								    1.0f, 1.0f,        //bottom
								    0.0f, 1.0f,
								    0.0f, 0.0f,
								    1.0f, 0.0f, };




	/*****PYRAMID VAO*****/

	glGenVertexArrays(1, &vao_pyramid);
	glBindVertexArray(vao_pyramid);

	glGenBuffers(1, &vbo_position_pyramid);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_pyramid);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(SPK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SPK_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vbo_texture_pyramid);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_texture_pyramid);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidTexture), pyramidTexture, GL_STATIC_DRAW);
	glVertexAttribPointer(SPK_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SPK_ATTRIBUTE_TEXCOORD);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	/*****CUBE VAO*****/

	glGenVertexArrays(1, &vao_cube);
	glBindVertexArray(vao_cube);

	glGenBuffers(1, &vbo_position_cube);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_cube);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(SPK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SPK_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vbo_texture_cube);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_texture_cube);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeTexture), cubeTexture, GL_STATIC_DRAW);
	glVertexAttribPointer(SPK_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SPK_ATTRIBUTE_TEXCOORD);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	/******FBO******/

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glGenTextures(1, &textureRenderBuffer);
	glBindTexture(GL_TEXTURE_2D, textureRenderBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, giWindowWidth, giWindowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureRenderBuffer, 0);

	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, giWindowWidth, giWindowHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);


    //depth
    
    glShadeModel(GL_SMOOTH);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);    
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    
    //load textures
    glEnable(GL_TEXTURE_2D);
    stone_texture = LoadBitmapAsTexture("Stone.bmp");
    
    //SetClearColor
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //black
    
    //set perspective matrix to identity matrix
	perspectiveProjectionMatrix = mat4::identity();
    
    Resize(giWindowWidth, giWindowHeight);
    
    
}


GLuint LoadBitmapAsTexture(const char * path)
{
    //variable declarations    
    int width, height;
    unsigned char * imageData = NULL;
    
    
    //code
    imageData = SOIL_load_image(path, &width, &height, NULL, SOIL_LOAD_RGB);
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    
    glGenTextures(1, &textureID);
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
    glGenerateMipmap(GL_TEXTURE_2D);
    
   
    
    SOIL_free_image_data(imageData);
    
    return(textureID);
    
}


void Resize(int width, int height)
{
    //code
	if (height == 0)
		height = 1;
    
    fbWidth = width;
	fbHeight = height;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}


void Draw(void)
{
   //code
	//bind fbo
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, (GLsizei)giWindowWidth, (GLsizei)giWindowHeight);

	//start using OpenGL program object
	glUseProgram(shaderProgramObjectPyramid);

	//OpenGL Drawing
	mat4 modelViewMatrix;
	mat4 modelViewProjectionMatrix;
	mat4 translateMatrix;
	mat4 rotateXMatrix;
	mat4 rotateYMatrix;
	mat4 rotateZMatrix;
	mat4 scaleMatrix;

	//PYRAMID TRANSFORMATION
	translateMatrix = mat4::identity();
	rotateYMatrix = mat4::identity();
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();

	translateMatrix = translate(0.0f, 0.0f, -3.0f);
	rotateYMatrix = rotate(pyramidAngle, 0.0f, 1.0f, 0.0f);

	modelViewMatrix = translateMatrix * rotateYMatrix;

	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;  //pre-multiplication of matrices

	glUniformMatrix4fv(mvpMatrixUniformPyramid, 1, GL_FALSE, modelViewProjectionMatrix);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, stone_texture);
	glUniform1i(textureSamplerUniformPyramid, 0);

	//bind vao_pyramid
	glBindVertexArray(vao_pyramid);

	glDrawArrays(GL_TRIANGLES, 0, 12);

	//unbind vao_triangle
	glBindVertexArray(0);

	//stop using OpenGL program object
	glUseProgram(0);

	//reset color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	//unbind fbo
	glBindFramebuffer(GL_FRAMEBUFFER, 0);



	//CUBE TRANSFORMATION
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, (GLsizei)fbWidth, (GLsizei)fbHeight);

	//start using OpenGL program object
	glUseProgram(shaderProgramObjectCube);


	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	rotateXMatrix = mat4::identity();
	rotateYMatrix = mat4::identity();
	rotateZMatrix = mat4::identity();
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();

	translateMatrix = translate(0.0f, 0.0f, -3.0f);
	scaleMatrix = scale(0.45f, 0.45f, 0.45f);
	rotateXMatrix = rotate(cubeAngle, 1.0f, 0.0f, 0.0f);
	rotateYMatrix = rotate(cubeAngle, 0.0f, 1.0f, 0.0f);
	rotateZMatrix = rotate(cubeAngle, 0.0f, 0.0f, 1.0f);


	modelViewMatrix = translateMatrix * scaleMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix;

	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	glUniformMatrix4fv(mvpMatrixUniformCube, 1, GL_FALSE, modelViewProjectionMatrix);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureRenderBuffer);
	glUniform1i(textureSamplerUniformCube, 0);

	//bind vao_rectangle
	glBindVertexArray(vao_cube);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);

	//unbind vao_rectangle
	glBindVertexArray(0);


	//stop using OpenGL program object
	glUseProgram(0);

    glXSwapBuffers(gpDisplay, gWindow);
}


void Update(void)
{
	//code
	pyramidAngle += 0.5f;
	if (pyramidAngle >= 360.0f)
		pyramidAngle = 0.0f;

	cubeAngle -= 0.5f;
	if (cubeAngle <= -360.0f)
		cubeAngle = 0.0f;

}


void Uninitialize(void)
{
    //variable declarations
    GLXContext currentGLXContext;
    
    
  //pyramid

	if (vao_pyramid)
	{
		glDeleteVertexArrays(1, &vao_pyramid);
		vao_pyramid = 0;
	}

	if (vbo_position_pyramid)
	{
		glDeleteBuffers(1, &vbo_position_pyramid);
		vbo_position_pyramid = 0;
	}

	if (vbo_texture_pyramid)
	{
		glDeleteBuffers(1, &vbo_texture_pyramid);
		vbo_texture_pyramid = 0;
	}

	//cube

	if (vao_cube)
	{
		glDeleteVertexArrays(1, &vao_cube);
		vao_cube = 0;
	}

	if (vbo_position_cube)
	{
		glDeleteBuffers(1, &vbo_position_cube);
		vbo_position_cube = 0;
	}

	if (vbo_texture_cube)
	{
		glDeleteBuffers(1, &vbo_texture_cube);
		vbo_texture_cube = 0;
	}



	/*****SAFE SHADER CLEAN-UP*****/

	if (shaderProgramObjectPyramid)
	{
		glUseProgram(shaderProgramObjectPyramid);
		GLsizei shaderCount;
		glGetProgramiv(shaderProgramObjectPyramid, GL_ATTACHED_SHADERS, &shaderCount);

		GLuint* pShaders = NULL;
		pShaders = (GLuint*)malloc(sizeof(GLuint) * shaderCount);

		glGetAttachedShaders(shaderProgramObjectPyramid, shaderCount, &shaderCount, pShaders);

		for (GLsizei i = 0; i < shaderCount; i++)
		{
			glDetachShader(shaderProgramObjectPyramid, pShaders[i]);
			glDeleteShader(pShaders[i]);
			pShaders[i] = 0;
		}
		free(pShaders);

		glDeleteProgram(shaderProgramObjectPyramid);
		shaderProgramObjectPyramid = 0;
		glUseProgram(0);

	}

	if (shaderProgramObjectCube)
	{
		glUseProgram(shaderProgramObjectCube);
		GLsizei shaderCount;
		glGetProgramiv(shaderProgramObjectCube, GL_ATTACHED_SHADERS, &shaderCount);

		GLuint* pShaders = NULL;
		pShaders = (GLuint*)malloc(sizeof(GLuint) * shaderCount);

		glGetAttachedShaders(shaderProgramObjectCube, shaderCount, &shaderCount, pShaders);

		for (GLsizei i = 0; i < shaderCount; i++)
		{
			glDetachShader(shaderProgramObjectCube, pShaders[i]);
			glDeleteShader(pShaders[i]);
			pShaders[i] = 0;
		}
		free(pShaders);

		glDeleteProgram(shaderProgramObjectCube);
		shaderProgramObjectCube = 0;
		glUseProgram(0);

	}

	
	//delete textures
    glDeleteTextures(1, &stone_texture);
    
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
    
    if (gpFile)
	{
		fprintf(gpFile, ("\nLog File Closed Successfully, Program Completed Successfully.\n"));
		fclose(gpFile);
		gpFile = NULL;
    }
    
}

