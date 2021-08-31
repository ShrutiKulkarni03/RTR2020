#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<memory.h>
#include<GL/glew.h>
#include<GL/gl.h>
#include<GL/glx.h>   //bridging API

#include<CL/opencl.h>

#include"vmath.h"

#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include<X11/XKBlib.h>
#include<X11/keysym.h>

#define MY_ARRAY_SIZE meshWidth * meshHeight * 4

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

void Uninitialize(void);
void launchCPUKernel(unsigned int, unsigned int, float);

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

GLuint shaderProgramObject;

GLuint vao;                          //vertex array object
GLuint vbo_cpu;                          //vertex buffer object
GLuint vbo_gpu;                    //vertex buffer object
GLuint mvpMatrixUniform;

mat4 perspectiveProjectionMatrix;   //4x4 matrix

//interop
cl_int clSuccess;
cl_mem cl_graphics_resource;
cl_device_id clDeviceID;
cl_context clContext;
cl_command_queue clCommandQueue;
cl_program clProgram;
cl_kernel clKernel;

char* clSourceCode = NULL;
size_t sizeKernelCodeLength;

bool bOnGPU = false;

//for sine wave
const unsigned int meshWidth = 1024;
const unsigned int meshHeight = 1024;
float pos[meshWidth][meshHeight][4];

float animationTime = 0.0f;

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
    //void Update(void);
    
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
                            
                        case XK_C:
                        case XK_c:
                            bOnGPU = false;
                            break;
                            
                        case XK_G:
                        case XK_g:
                            bOnGPU = true;
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
        //Update();
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
    char* loadOclKernelSource(const char*, const char*, size_t*);
    
    GLuint vertexShaderObject;
	GLuint fragmentShaderObject;
    
    
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
	
	
	/*INTEROP*/
	cl_platform_id clPlatformID;
	cl_device_id* clDeviceIDs;
	cl_uint devCount;

	clSuccess = clGetPlatformIDs(1, &clPlatformID, NULL);
	if (clSuccess != CL_SUCCESS)
	{
		fprintf(gpFile, "clGetPlatformIDs() unsuccessfull\n");
		fflush(gpFile);
		exit(EXIT_FAILURE);
	}

	clSuccess = clGetDeviceIDs(clPlatformID, CL_DEVICE_TYPE_GPU, 0, NULL, &devCount);
	if (clSuccess != CL_SUCCESS)
	{
		fprintf(gpFile, "clGetDeviceIDs() unsuccessfull\n");
		fflush(gpFile);
		exit(EXIT_FAILURE);
	}
	else if (devCount == 0)
	{
		fprintf(gpFile, "devCount = 0\n");
		fflush(gpFile);
		exit(EXIT_FAILURE);
	}
	else
	{
		clDeviceIDs = (cl_device_id*)malloc(sizeof(cl_device_id) * devCount);
		clSuccess = clGetDeviceIDs(clPlatformID, CL_DEVICE_TYPE_GPU, devCount, clDeviceIDs, NULL);
		if (clSuccess != CL_SUCCESS)
		{
			fprintf(gpFile, "clGetDeviceIDs() unsuccessfull\n");
			fflush(gpFile);
			exit(EXIT_FAILURE);
		}

		clDeviceID = clDeviceIDs[0];

		free(clDeviceIDs);
	}

	cl_context_properties context_properties[] = { CL_GL_CONTEXT_KHR, (cl_context_properties)glXGetCurrentContext(),
												   CL_GLX_DISPLAY_KHR, (cl_context_properties)glXGetCurrentDisplay(),
												   CL_CONTEXT_PLATFORM, (cl_context_properties)clPlatformID, 0 };

	clContext = clCreateContext(context_properties, 1, &clDeviceID, NULL, NULL, &clSuccess);
	if (clSuccess != CL_SUCCESS)
	{
		fprintf(gpFile, "clCreateContext() unsuccessfull\n");
		fflush(gpFile);
		exit(EXIT_FAILURE);
	}

	clCommandQueue = clCreateCommandQueueWithProperties(clContext, clDeviceID, 0, &clSuccess);
	if (clSuccess != CL_SUCCESS)
	{
		fprintf(gpFile, "clCreateCommandQueueWithProperties() unsuccessfull\n");
		fflush(gpFile);
		exit(EXIT_FAILURE);
	}

	clSourceCode = loadOclKernelSource("SineWave.cl", "", &sizeKernelCodeLength);
	clProgram = clCreateProgramWithSource(clContext, 1, (const char**)&clSourceCode, &sizeKernelCodeLength, &clSuccess);
	if (clSuccess != CL_SUCCESS)
	{
		fprintf(gpFile, "clCreateProgramWithSource() unsuccessfull\n");
		fflush(gpFile);
		exit(EXIT_FAILURE);
	}

	clSuccess = clBuildProgram(clProgram, 0, NULL, "-cl-fast-relaxed-math", NULL, NULL);
	if (clSuccess != CL_SUCCESS)
	{
		fprintf(gpFile, "clBuildProgram() unsuccessfull\n");
		fflush(gpFile);
		exit(EXIT_FAILURE);
	}

	clKernel = clCreateKernel(clProgram, "SineWave_kernel", &clSuccess);
	if (clSuccess != CL_SUCCESS)
	{
		fprintf(gpFile, "clCreateKernel() unsuccessfull\n");
		fflush(gpFile);
		exit(EXIT_FAILURE);
	}
	
	
	/********SHADERS********/

	
	/*****VERTEX SHADER*****/

	//create shader
	vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	//provide source code to vertex shader
	const GLchar *vertexShaderSourceCode =
		"#version 450 core \n" \
		"\n" \
		"in vec4 vPosition; \n" \
		"uniform mat4 u_mvpMatrix; \n" \
		"void main(void) \n" \
		"{ \n" \
		" gl_Position = u_mvpMatrix * vPosition; \n" \
		"} \n";
        
        
	glShaderSource(vertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);

	//compile shader
	glCompileShader(vertexShaderObject);

	//vertex shader compilation error checking
	GLint infoLogLength = 0;
	GLint shaderCompiledStatus = 0;
	char* szBuffer = NULL;

	glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &shaderCompiledStatus);

	if (shaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);

		if (infoLogLength > 0)
		{
			szBuffer = (char*)malloc(infoLogLength);
			if (szBuffer != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(vertexShaderObject, infoLogLength, &written, szBuffer);
				fprintf(gpFile, "\nVertex Shader Compilation Log : %s\n", szBuffer);
				free(szBuffer);
				Uninitialize();
			}

		}
	}


	/*****FRAGMENT SHADER*****/

	//create shader
	fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	//provide source code to fragment shader
	const GLchar* fragmentShaderSourceCode =
		"#version 450 core \n" \
		"\n" \
		"out vec4 FragColor; \n" \
		"void main(void) \n" \
		"{ \n" \
		" FragColor = vec4(1.0f, 0.5f, 0.0f, 1.0f); \n" \
		"} \n";

	glShaderSource(fragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);

	//compile shader
	glCompileShader(fragmentShaderObject);

	//fragment shader compilation error checking
	infoLogLength = 0;
	shaderCompiledStatus = 0;
	szBuffer = NULL;

	glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &shaderCompiledStatus);

	if (shaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);

		if (infoLogLength > 0)
		{
			szBuffer = (char*)malloc(infoLogLength);
			if (szBuffer != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(fragmentShaderObject, infoLogLength, &written, szBuffer);
				fprintf(gpFile, "\nFragment Shader Compilation Log : %s\n", szBuffer);
				free(szBuffer);
				Uninitialize();

			}
		}
	}


	/*****SHADER PROGRAM*****/

	//create
	shaderProgramObject = glCreateProgram();

	//attach vertex shader to shader program
	glAttachShader(shaderProgramObject, vertexShaderObject);

	//attach fragment shader to shader program
	glAttachShader(shaderProgramObject, fragmentShaderObject);

	//pre-linking binding
	glBindAttribLocation(shaderProgramObject, SPK_ATTRIBUTE_POSITION, "vPosition");

	//link shader
	glLinkProgram(shaderProgramObject);

	//shader linking error checking
	infoLogLength = 0;
	GLint shaderProgramLinkStatus;
	szBuffer = NULL;

	glGetProgramiv(shaderProgramObject, GL_LINK_STATUS, &shaderProgramLinkStatus);

	if (shaderProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObject, GL_INFO_LOG_LENGTH, &infoLogLength);

		if (infoLogLength > 0)
		{
			szBuffer = (char*)malloc(infoLogLength);

			if (szBuffer != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(shaderProgramObject, infoLogLength, &written, szBuffer);
				fprintf(gpFile, "\nShader Program Link Log : %s\n", szBuffer);
				free(szBuffer);
				Uninitialize();
			}
		}
	}

	//get MVP uniform location
	mvpMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_mvpMatrix");


    //vertices array declaration
    
    for (int i = 0; i < meshWidth; i++)
    {
        for (int j = 0; j < meshHeight; j++)
        {
            for (int k = 0; k < 4; k++)
            {
                pos[i][j][k] = 0.0f;
            }
        }
    }
    
                                    
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo_cpu);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_cpu);
	glBufferData(GL_ARRAY_BUFFER, MY_ARRAY_SIZE * sizeof(float), NULL, GL_DYNAMIC_DRAW);	
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vbo_gpu);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_gpu);
	glBufferData(GL_ARRAY_BUFFER, MY_ARRAY_SIZE * sizeof(float), NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

    cl_graphics_resource = clCreateFromGLBuffer(clContext, CL_MEM_WRITE_ONLY, vbo_gpu, &clSuccess);
    if (clSuccess != CL_SUCCESS)
    {
        fprintf(gpFile, "clCreateFromGLBuffer() unsuccessfull\n");
        fflush(gpFile);
        exit(EXIT_FAILURE);
    }

	glBindVertexArray(0);
    
    glShadeModel(GL_SMOOTH);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);    
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    
    //SetClearColor
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //black
    
    //set perspective matrix to identity matrix
	perspectiveProjectionMatrix = mat4::identity();
    
    Resize(giWindowWidth, giWindowHeight);
    
    
}


void Resize(int width, int height)
{
    //code
	if (height == 0)
		height = 1;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}



void launchCPUKernel(unsigned int meshWidth, unsigned int meshHeight, float animationTime)
{
	for (int i = 0; i < meshWidth; i++)
	{
		for (int j = 0; j < meshHeight; j++)
		{
			for (int k = 0; k < 4; k++)
			{
				float u = i / (float)meshWidth;
				float v = j / (float)meshHeight;

				u = u * 2.0f - 1.0f;
				v = v * 2.0f - 1.0f;

				float frequency = 4.0f;

				float w = sinf(u * frequency + animationTime) * cosf(v * frequency + animationTime) * 0.5f;

				if (k == 0)
					pos[i][j][k] = u;

				if (k == 1)
					pos[i][j][k] = w;

				if (k == 2)
					pos[i][j][k] = v;

				if (k == 3)
					pos[i][j][k] = 1.0;
				
			}
		}
	}
}


void Draw(void)
{
    size_t globalWorkSize[2];
    
    //code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//start using OpenGL program object
	glUseProgram(shaderProgramObject);

	//OpenGL Drawing

	mat4 modelViewMatrix;
	mat4 modelViewProjectionMatrix;
	
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();

	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;  //pre-multiplication of matrices

	glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	//bind vao
	glBindVertexArray(vao);

	if (bOnGPU == true)
	{
		clSuccess = clSetKernelArg(clKernel, 0, sizeof(cl_mem), (void*)&cl_graphics_resource);
		if (clSuccess != CL_SUCCESS)
		{
			fprintf(gpFile, "clSetKernelArg() for cl_graphics_resource is unsuccessfull\n");
			fflush(gpFile);
			exit(EXIT_FAILURE);
		}
	
		clSuccess = clSetKernelArg(clKernel, 1, sizeof(unsigned int), (void*)&meshWidth);
		if (clSuccess != CL_SUCCESS)
		{
			fprintf(gpFile, "clSetKernelArg() for meshWidth is unsuccessfull\n");
			fflush(gpFile);
			exit(EXIT_FAILURE);
		}

		clSuccess = clSetKernelArg(clKernel, 2, sizeof(unsigned int), (void*)&meshHeight);
		if (clSuccess != CL_SUCCESS)
		{
			fprintf(gpFile, "clSetKernelArg() for meshHeight is unsuccessfull\n");
			fflush(gpFile);
			exit(EXIT_FAILURE);
		}

		clSuccess = clSetKernelArg(clKernel, 3, sizeof(float), (void*)&animationTime);
		if (clSuccess != CL_SUCCESS)
		{
			fprintf(gpFile, "clSetKernelArg() for animationTime is unsuccessfull\n");
			fflush(gpFile);
			exit(EXIT_FAILURE);
		}

		clSuccess = clEnqueueAcquireGLObjects(clCommandQueue, 1, &cl_graphics_resource, 0, NULL, NULL);
		if (clSuccess != CL_SUCCESS)
		{
			fprintf(gpFile, "clEnqueueAcquireGLObjects() unsuccessfull\n");
			fflush(gpFile);
			exit(EXIT_FAILURE);
		}

		globalWorkSize[0] = meshWidth;
		globalWorkSize[1] = meshHeight;

		clSuccess = clEnqueueNDRangeKernel(clCommandQueue, clKernel, 2, NULL, globalWorkSize, 0, 0, NULL, NULL);
		if (clSuccess != CL_SUCCESS)
		{
			fprintf(gpFile, "clEnqueueNDRangeKernel() unsuccessfull\n");
			fflush(gpFile);
			exit(EXIT_FAILURE);
		}

		clSuccess = clEnqueueReleaseGLObjects(clCommandQueue, 1, &cl_graphics_resource, 0, NULL, NULL);
		if (clSuccess != CL_SUCCESS)
		{
			fprintf(gpFile, "clEnqueueReleaseGLObjects() unsuccessfull\n");
			fflush(gpFile);
			exit(EXIT_FAILURE);
		}

		clFinish(clCommandQueue);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_gpu);
	
	}
	else
	{
		launchCPUKernel(meshWidth, meshHeight, animationTime);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_cpu);
		glBufferData(GL_ARRAY_BUFFER, MY_ARRAY_SIZE * sizeof(float), pos, GL_DYNAMIC_DRAW);
	}
		
	glVertexAttribPointer(SPK_ATTRIBUTE_POSITION, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SPK_ATTRIBUTE_POSITION);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//glPointSize(2.0f);
	glDrawArrays(GL_POINTS, 0, meshWidth * meshHeight);

	animationTime += 0.01f;	

	//unbind vao
	glBindVertexArray(0);


	//stop using OpenGL program object
	glUseProgram(0);
    
    glXSwapBuffers(gpDisplay, gWindow);
}


char* loadOclKernelSource(const char* filename, const char* preamble, size_t* sizeFinalLength)
{
	//locals
	FILE* pFile = NULL;
	size_t sizeSourceLength;

	pFile = fopen(filename, "rb");
	if (pFile == NULL)
		return(NULL);

	size_t sizePreambleLength = (size_t)strlen(preamble);

	//get the length of the source code
	fseek(pFile, 0, SEEK_END);
	sizeSourceLength = ftell(pFile);
	fseek(pFile, 0, SEEK_SET); //reset to beginning

	//allocate a buffer for the source code string and read it in
	char* sourceString = (char*)malloc(sizeSourceLength + sizePreambleLength + 1);
	memcpy(sourceString, preamble, sizePreambleLength);

	if (fread((sourceString)+sizePreambleLength, sizeSourceLength, 1, pFile) != 1)
	{
		fclose(pFile);
		free(sourceString);
		return(0);
	}

	//close the file and return the total length of the combined (preamble + source) string

	fclose(pFile);
	if (sizeFinalLength != 0)
	{
		*sizeFinalLength = sizeSourceLength + sizePreambleLength;
	}

	sourceString[sizeSourceLength + sizePreambleLength] = '\0';

	return(sourceString);


}



void Uninitialize(void)
{
    //variable declarations
    GLXContext currentGLXContext;
    
  if (clSourceCode)
	{
		free((void*)clSourceCode);
		clSourceCode = NULL;
	}

	if (cl_graphics_resource)
	{
		clReleaseMemObject(cl_graphics_resource);
		cl_graphics_resource = NULL;
	}

	if (clKernel)
	{
		clReleaseKernel(clKernel);
		clKernel = NULL;
	}

	if (clProgram)
	{
		clReleaseProgram(clProgram);
		clProgram = NULL;
	}

	if (clCommandQueue)
	{
		clReleaseCommandQueue(clCommandQueue);
		clCommandQueue = NULL;
	}

	if (clContext)
	{
		clReleaseContext(clContext);
		clContext = NULL;
	}

	if (vao)
	{
		glDeleteVertexArrays(1, &vao);
		vao = 0;
	}

	if (vbo_cpu)
	{
		glDeleteBuffers(1, &vbo_cpu);
		vbo_cpu = 0;
	}

	if (vbo_gpu)
	{
		glDeleteBuffers(1, &vbo_gpu);
		vbo_gpu = 0;
	}

	/*****SAFE SHADER CLEAN-UP*****/

	if (shaderProgramObject)
	{
		glUseProgram(shaderProgramObject);
		GLsizei shaderCount;
		glGetProgramiv(shaderProgramObject, GL_ATTACHED_SHADERS, &shaderCount);

		GLuint* pShaders = NULL;
		pShaders = (GLuint*)malloc(sizeof(GLuint) * shaderCount);

		glGetAttachedShaders(shaderProgramObject, shaderCount, &shaderCount, pShaders);

		for (GLsizei i = 0; i < shaderCount; i++)
		{
			glDetachShader(shaderProgramObject, pShaders[i]);
			glDeleteShader(pShaders[i]);
			pShaders[i] = 0;
		}
		free(pShaders);

		glDeleteProgram(shaderProgramObject);
		shaderProgramObject = 0;
		glUseProgram(0);

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
    
    if (gpFile)
	{
		fprintf(gpFile, ("\nLog File Closed Successfully, Program Completed Successfully.\n"));
		fclose(gpFile);
		gpFile = NULL;
    }
    
}

