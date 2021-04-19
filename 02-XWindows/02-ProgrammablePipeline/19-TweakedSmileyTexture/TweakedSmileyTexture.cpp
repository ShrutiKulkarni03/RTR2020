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
GLuint textureID;
char keys[26];


typedef GLXContext(* glXCreateContextAttribsARBProc)(Display *, GLXFBConfig, GLXContext, Bool, const int *);
glXCreateContextAttribsARBProc glXCreateContextAttribsARB = NULL;
GLXFBConfig gGLXFBConfig;

int giWindowWidth=800;
int giWindowHeight=600;


//shader variables

GLuint shaderProgramObject;

//vao for square
GLuint vao_square;
GLuint vbo_position_square;
GLuint vbo_texture_square;

GLuint mvpMatrixUniform;
GLuint textureSamplerUniform;
mat4 perspectiveProjectionMatrix;   //4x4 matrix

//update variables


//texture variables

GLfloat texcoord[8];
GLuint smiley_texture;
GLuint white_texture;
GLuint PressedDigit = 0;

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
                            //bDone=true;
                            Uninitialize();
                            exit(0);
                            break;
                            
                        
                    }
                                
                    
                    //XLookupString(gpDisplay, keys, sizeof(keys), NULL, NULL);
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
                            
                        case '1':
                            PressedDigit = 1;
                            break;
                            
                        case '2':
                            PressedDigit = 2;
                            break;
                            
                        case '3':
                            PressedDigit = 3;
                            break;
                            
                        case '4':
                            PressedDigit = 4;
                            break;
                            
                        default:
                            PressedDigit = 0;
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
	
	
	
	/********SHADERS********/

	
	/*****VERTEX SHADER*****/

	//create shader
	vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	//provide source code to vertex shader
	const GLchar *vertexShaderSourceCode =
		"#version 450 core \n" \
		"\n" \
		"in vec4 vPosition; \n" \
		"in vec2 vTexcoord; \n" \
		"uniform mat4 u_mvpMatrix; \n" \
		"out vec2 out_texcoord; \n" \
		"void main(void) \n" \
		"{ \n" \
		"gl_Position = u_mvpMatrix * vPosition; \n" \
		"out_texcoord = vTexcoord; \n" \
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
		"in vec2 out_texcoord; \n" \
		"uniform sampler2D u_texture_sampler; \n" \
		"out vec4 FragColor; \n" \
		"void main(void) \n" \
		"{ \n" \
		"FragColor = texture(u_texture_sampler, out_texcoord); \n" \
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
	glBindAttribLocation(shaderProgramObject, SPK_ATTRIBUTE_TEXCOORD, "vTexcoord");

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
	textureSamplerUniform = glGetUniformLocation(shaderProgramObject, "u_texture_sampler");

	//vertices array declaration


	//SQUARE

	//vertices
	const GLfloat squareVertices[] = { 1.0f, 1.0f, 0.0f,        //front
									-1.0f, 1.0f, 0.0f,
									-1.0f, -1.0f, 0.0f,
									 1.0f, -1.0f, 0.0f };

	

	/*****SQUARE VAO*****/

	glGenVertexArrays(1, &vao_square);
	glBindVertexArray(vao_square);

	//position vbo

	glGenBuffers(1, &vbo_position_square);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_square);
	glBufferData(GL_ARRAY_BUFFER, sizeof(squareVertices), squareVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(SPK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SPK_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//texcoords vbo

	glGenBuffers(1, &vbo_texture_square);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_texture_square);
	glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(SPK_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SPK_ATTRIBUTE_TEXCOORD);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

    //depth
    
    glShadeModel(GL_SMOOTH);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);    
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    
    //load textures
    glEnable(GL_TEXTURE_2D);
    smiley_texture = LoadBitmapAsTexture("Smiley.bmp");
    white_texture = LoadBitmapAsTexture("White.bmp");    
    
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

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}


void Draw(void)
{
   //code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	

	//start using OpenGL program object
	glUseProgram(shaderProgramObject);

	//OpenGL Drawing

	mat4 modelViewMatrix;
	mat4 modelViewProjectionMatrix;
	mat4 translateMatrix;
	mat4 scaleMatrix;


	//SQUARE TRANSFORMATION	

	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();

	translateMatrix = translate(0.0f, 0.0f, -3.0f);
	scaleMatrix = scale(0.5f, 0.5f, 0.0f);

	modelViewMatrix = translateMatrix * scaleMatrix;

	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, smiley_texture);
	glUniform1i(textureSamplerUniform, 0);

	//bind vao_rectangle
	glBindVertexArray(vao_square);

	if (PressedDigit == 1)
	{
		texcoord[0] = 1.0f;     //s
		texcoord[1] = 1.0f;     //t
		texcoord[2] = 0.0f;     //s
		texcoord[3] = 1.0f;     //t
		texcoord[4] = 0.0f;     //s
		texcoord[5] = 0.0f;     //t
		texcoord[6] = 1.0f;     //s
		texcoord[7] = 0.0f;     //t
	}
	else if (PressedDigit == 2)
	{
		texcoord[0] = 0.5f;     //s
		texcoord[1] = 0.5f;     //t
		texcoord[2] = 0.0f;     //s
		texcoord[3] = 0.5f;     //t
		texcoord[4] = 0.0f;     //s
		texcoord[5] = 0.0f;     //t
		texcoord[6] = 0.5f;     //s
		texcoord[7] = 0.0f;     //t
	}
	else if (PressedDigit == 3)
	{
		texcoord[0] = 2.0f;     //s
		texcoord[1] = 2.0f;     //t
		texcoord[2] = 0.0f;     //s
		texcoord[3] = 2.0f;     //t
		texcoord[4] = 0.0f;     //s
		texcoord[5] = 0.0f;     //t
		texcoord[6] = 2.0f;     //s
		texcoord[7] = 0.0f;     //t
	}
	else if (PressedDigit == 4)
	{
		texcoord[0] = 0.5f;     //s
		texcoord[1] = 0.5f;     //t
		texcoord[2] = 0.5f;     //s
		texcoord[3] = 0.5f;     //t
		texcoord[4] = 0.5f;     //s
		texcoord[5] = 0.5f;     //t
		texcoord[6] = 0.5f;     //s
		texcoord[7] = 0.5f;     //t
	}
	else if (PressedDigit == 0)
	{
		glBindTexture(GL_TEXTURE_2D, white_texture);

		texcoord[0] = 0.5f;     //s
		texcoord[1] = 0.5f;     //t
		texcoord[2] = 0.5f;     //s
		texcoord[3] = 0.5f;     //t
		texcoord[4] = 0.5f;     //s
		texcoord[5] = 0.5f;     //t
		texcoord[6] = 0.5f;     //s
		texcoord[7] = 0.5f;     //t
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbo_texture_square);
	glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(texcoord), texcoord, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(SPK_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SPK_ATTRIBUTE_TEXCOORD);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	
	//unbind vao_rectangle
	glBindVertexArray(0);


	//stop using OpenGL program object
	glUseProgram(0);
    
    glXSwapBuffers(gpDisplay, gWindow);
}


void Update(void)
{
	//code
}


void Uninitialize(void)
{
    //variable declarations
    GLXContext currentGLXContext;
    
  //square

	if (vao_square)
	{
		glDeleteVertexArrays(1, &vao_square);
		vao_square = 0;
	}

	if (vbo_position_square)
	{
		glDeleteBuffers(1, &vbo_position_square);
		vbo_position_square = 0;
	}

	if (vbo_texture_square)
	{
		glDeleteBuffers(1, &vbo_texture_square);
		vbo_texture_square = 0;
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
	
	//delete textures
    glDeleteTextures(1, &smiley_texture);

    
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

