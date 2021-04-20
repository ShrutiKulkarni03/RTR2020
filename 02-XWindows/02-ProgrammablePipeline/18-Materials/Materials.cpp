#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<memory.h>
#include<GL/glew.h>
#include<GL/gl.h>
#include<GL/glx.h>   //bridging API
#include"vmath.h"

#include"Sphere.h"

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

char keys[26];
bool bLight;
vec4 lightAmbient;
vec4 lightDiffuse;
vec4 lightSpecular;
vec4 lightPosition;

GLuint shaderProgramObject;

GLuint vao;                          //vertex array object
GLuint vbo_sphere_position;          //vertex buffer object
GLuint vbo_sphere_normal;
GLuint vbo_sphere_elements;

//UNIFORMS

GLuint modelMatrixUniform;
GLuint viewMatrixUniform;
GLuint projectionMatrixUniform;

GLuint LKeyPressedUniform;

GLuint lAUniform;
GLuint lDUniform;
GLuint lSUniform;
GLuint lightPositionUniform;

GLuint kAUniform;
GLuint kDUniform;
GLuint kSUniform;
GLfloat kShininessUniform;

GLint KeyPressed = 0;
GLfloat AngleForXRotation = 0.0f;
GLfloat AngleForYRotation = 0.0f;
GLfloat AngleForZRotation = 0.0f;


int vWidth;
int vHeight;

mat4 perspectiveProjectionMatrix;   //4x4 matrix

//SPHERES

GLfloat sphere_vertices[1146];
GLfloat sphere_normals[1146];
GLfloat sphere_textures[764];
unsigned short sphere_elements[2280];
GLuint numVertices;
GLuint numElements;


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
		"in vec3 vNormal; \n" \
		"uniform int u_lKeyPressed; \n" \
		"uniform vec4 u_light_position; \n" \
		"uniform mat4 u_projection_matrix; \n" \
		"uniform mat4 u_model_matrix; \n" \
		"uniform mat4 u_view_matrix; \n" \
		"out vec3 transformed_normal; \n" \
		"out vec3 light_direction; \n" \
		"out vec3 view_vector; \n" \
		"\n" \
		"void main(void) \n" \
		"{ \n" \
		"	if(u_lKeyPressed==1) \n" \
		"	{ \n" \
		"		vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition; \n" \
		"		transformed_normal = mat3(u_view_matrix * u_model_matrix) * vNormal; \n" \
		"		light_direction = vec3(u_light_position - eye_coordinates); \n" \
		"		view_vector = -eye_coordinates.xyz; \n" \
		"	} \n" \
		"\n" \
		"	gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition; \n" \
		"\n" \
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
		"in vec3 transformed_normal; \n" \
		"in vec3 light_direction; \n" \
		"in vec3 view_vector; \n" \
		"uniform int u_lKeyPressed; \n" \
		"uniform vec3 u_lA; \n" \
		"uniform vec3 u_lD; \n" \
		"uniform vec3 u_lS; \n" \
		"uniform vec3 u_kA; \n" \
		"uniform vec3 u_kD; \n" \
		"uniform vec3 u_kS; \n" \
		"uniform float u_materialShininess; \n" \
		"out vec4 FragColor; \n" \
		"vec3 phong_ads_light; \n" \
		"void main(void) \n" \
		"{ \n" \
		"if(u_lKeyPressed==1) \n" \
		"{ \n" \
		"vec3 normalized_transformed_normals = normalize(transformed_normal); \n" \
		"vec3 normalized_light_direction = normalize(light_direction); \n" \
		"vec3 normalized_view_vector = normalize(view_vector); \n" \
		"vec3 reflection_vector = reflect(-normalized_light_direction, normalized_transformed_normals); \n" \
		"vec3 ambient = u_lA * u_kA; \n" \
		"vec3 diffuse = u_lD * u_kD * max(dot(normalized_light_direction, normalized_transformed_normals), 0.0); \n" \
		"vec3 specular = u_lS * u_kS * pow(max(dot(reflection_vector, normalized_view_vector), 0.0), u_materialShininess); \n" \
		"phong_ads_light = ambient + diffuse + specular; \n" \
		"} \n" \
		"else \n" \
		"{ \n" \
		"phong_ads_light = vec3(1.0, 1.0, 1.0); \n" \
		"} \n" \
		"FragColor = vec4(phong_ads_light, 1.0f); \n" \
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
	glBindAttribLocation(shaderProgramObject, SPK_ATTRIBUTE_NORMAL, "vNormal");

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

	//get uniform location
	modelMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_model_matrix");
	viewMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_view_matrix");
	projectionMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_projection_matrix");

	LKeyPressedUniform = glGetUniformLocation(shaderProgramObject, "u_lKeyPressed");

	lAUniform = glGetUniformLocation(shaderProgramObject, "u_lA");
	lDUniform = glGetUniformLocation(shaderProgramObject, "u_lD");
	lSUniform = glGetUniformLocation(shaderProgramObject, "u_lS");
	lightPositionUniform = glGetUniformLocation(shaderProgramObject, "u_light_position");

	kAUniform = glGetUniformLocation(shaderProgramObject, "u_kA");
	kDUniform = glGetUniformLocation(shaderProgramObject, "u_kD");
	kSUniform = glGetUniformLocation(shaderProgramObject, "u_kS");
	kShininessUniform = glGetUniformLocation(shaderProgramObject, "u_materialShininess");


	getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
	numVertices = getNumberOfSphereVertices();
	numElements = getNumberOfSphereElements();

	//VAO

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo_sphere_position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_sphere_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(SPK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SPK_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glGenBuffers(1, &vbo_sphere_normal);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_sphere_normal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);
	glVertexAttribPointer(SPK_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SPK_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glGenBuffers(1, &vbo_sphere_elements);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_elements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	lightAmbient = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	lightDiffuse = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	lightSpecular = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	lightPosition = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    
    
    glShadeModel(GL_SMOOTH);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);    
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    
    //SetClearColor
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f); //black
    
    //set perspective matrix to identity matrix
	perspectiveProjectionMatrix = mat4::identity();
    
    Resize(giWindowWidth, giWindowHeight);
    
    
}


void Resize(int width, int height)
{
    vWidth= width;
	vHeight= height;


	//code
	if (height == 0)
		height = 1;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	perspectiveProjectionMatrix = vmath::perspective(45.0f, ((GLfloat)width / 4) / ((GLfloat)height / 6), 0.1f, 100.0f);
}


void Draw(void)
{
   GLfloat r = 5.0f;

	void drawSpheres(void);

	
	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//start using OpenGL program object
	glUseProgram(shaderProgramObject);


	if (bLight == true)
	{

		glUniform1i(LKeyPressedUniform, 1);

		/****GOURAUD****/


		glUniform3fv(lAUniform, 1, lightAmbient);
		glUniform3fv(lDUniform, 1, lightDiffuse);
		glUniform3fv(lSUniform, 1, lightSpecular);
		glUniform4fv(lightPositionUniform, 1, lightPosition);

	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}

	if (KeyPressed == 1)
	{
		lightPosition[0] = 0.0f;
		lightPosition[1] = r * sin(AngleForXRotation);
		lightPosition[2] = r * cos(AngleForXRotation);
		lightPosition[3] = 1.0f;

	}
	else if (KeyPressed == 2)
	{
		lightPosition[0] = r * sin(AngleForYRotation);
		lightPosition[1] = 0.0f;
		lightPosition[2] = r * cos(AngleForYRotation);
		lightPosition[3] = 1.0f;
	}
	else if (KeyPressed == 3)
	{
		lightPosition[0] = r * sin(AngleForZRotation);
		lightPosition[1] = r * cos(AngleForZRotation);
		lightPosition[2] = 0.0f;
		lightPosition[3] = 1.0f;

	}


	glUniform3f(kAUniform, 0.0f, 0.0f, 0.0f);
	glUniform3f(kDUniform, 1.0f, 1.0f, 1.0f);
	glUniform3f(kSUniform, 1.0f, 1.0f, 1.0f);
	glUniform1f(kShininessUniform, 50.0);

	//OpenGL Drawing

	drawSpheres();

	
	//stop using OpenGL program object
	glUseProgram(0);
    
    glXSwapBuffers(gpDisplay, gWindow);
}


void drawSpheres(void)
{
	mat4 modelMatrix;
	mat4 viewMatrix;
	mat4 projectionMatrix;
	mat4 translateMatrix;
	mat4 scaleMatrix;

	GLfloat sphereScale = 1.5f;


	

	// ***** 1st sphere on 1st row, emerald *****


	glUniform3f(kAUniform, 0.0215f, 0.1745f, 0.0215f);
	glUniform3f(kDUniform, 0.07568f, 0.61424f, 0.07568f);
	glUniform3f(kSUniform, 0.633f, 0.727811f, 0.633f);
	glUniform1f(kShininessUniform, 0.6 * 128);


	glViewport(0, 5 * vHeight / 6, (GLsizei)vWidth / 4, (GLsizei)vHeight / 6); 
	//perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)vWidth / (GLfloat)vHeight, 0.1f, 100.0f);
	
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	projectionMatrix = mat4::identity();
	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	//translateMatrix = translate(-1.8f, 1.0f, -3.0f);
	translateMatrix = translate(0.0f, 0.0f, -3.0f);
	scaleMatrix = scale(sphereScale, sphereScale, sphereScale);

	modelMatrix = translateMatrix * scaleMatrix;

	projectionMatrix = perspectiveProjectionMatrix;

	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);



	//bind vao
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_elements);
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);

	//unbind vao
	glBindVertexArray(0);




	// ***** 2nd sphere on 1st row, jade *****


	glUniform3f(kAUniform, 0.135f, 0.2225f, 0.1575f);
	glUniform3f(kDUniform, 0.54f, 0.89f, 0.63f);
	glUniform3f(kSUniform, 0.316228f, 0.316228f, 0.316228f);
	glUniform1f(kShininessUniform, 0.1 * 128);

	glViewport(vWidth / 4, 5 * vHeight / 6, (GLsizei)vWidth / 4, (GLsizei)vHeight / 6); //////////////////////////////////////////////////////////////


	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	projectionMatrix = mat4::identity();
	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	translateMatrix = translate(-1.08f, 1.0f, -3.0f);
	translateMatrix = translate(0.0f, 0.0f, -3.0f);
	scaleMatrix = scale(sphereScale, sphereScale, sphereScale);

	modelMatrix = translateMatrix * scaleMatrix;

	projectionMatrix = perspectiveProjectionMatrix;

	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);


	//bind vao
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_elements);
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);

	//unbind vao
	glBindVertexArray(0);





	// ***** 3rd sphere on 1st row, obsidian *****


	glUniform3f(kAUniform, 0.05375f, 0.05f, 0.06625f);
	glUniform3f(kDUniform, 0.18275f, 0.17f, 0.22525f);
	glUniform3f(kSUniform, 0.332741f, 0.328634f, 0.346435f);
	glUniform1f(kShininessUniform, 0.3 * 128);


	glViewport(vWidth / 2, 5 * vHeight / 6, (GLsizei)vWidth / 4, (GLsizei)vHeight / 6);


	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	projectionMatrix = mat4::identity();
	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	//translateMatrix = translate(-0.36f, 1.0f, -3.0f);
	translateMatrix = translate(0.0f, 0.0f, -3.0f);
	scaleMatrix = scale(sphereScale, sphereScale, sphereScale);

	modelMatrix = translateMatrix * scaleMatrix;

	projectionMatrix = perspectiveProjectionMatrix;

	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);


	//bind vao
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_elements);
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);

	//unbind vao
	glBindVertexArray(0);




	// ***** 4th sphere on 1st row, pearl *****


	glUniform3f(kAUniform, 0.25f, 0.20725f, 0.20725f);
	glUniform3f(kDUniform, 1.0f, 0.829f, 0.829f);
	glUniform3f(kSUniform, 0.296648f, 0.296648f, 0.296648f);
	glUniform1f(kShininessUniform, 0.088 * 128);

	glViewport(3 * vWidth / 4, 5 * vHeight / 6, (GLsizei)vWidth / 4, (GLsizei)vHeight / 6);


	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	projectionMatrix = mat4::identity();
	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	translateMatrix = translate(0.0f, 0.0f, -3.0f);
	//translateMatrix = translate(0.36f, 1.0f, -3.0f);
	scaleMatrix = scale(sphereScale, sphereScale, sphereScale);

	modelMatrix = translateMatrix * scaleMatrix;

	projectionMatrix = perspectiveProjectionMatrix;

	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);


	//bind vao
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_elements);
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);

	//unbind vao
	glBindVertexArray(0);



	// ***** 1st sphere on 2nd row, ruby *****


	glUniform3f(kAUniform, 0.1745f, 0.01175f, 0.01175f);
	glUniform3f(kDUniform, 0.61424f, 0.04136f, 0.04136f);
	glUniform3f(kSUniform, 0.727811f, 0.626959f, 0.626959f);
	glUniform1f(kShininessUniform, 0.6 * 128);

	glViewport(0, 2 * vHeight / 3, (GLsizei)vWidth / 4, (GLsizei)vHeight / 6);


	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	projectionMatrix = mat4::identity();
	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	translateMatrix = translate(0.0f, 0.0f, -3.0f);
	//translateMatrix = translate(1.08f, 1.0f, -3.0f);
	scaleMatrix = scale(sphereScale, sphereScale, sphereScale);

	modelMatrix = translateMatrix * scaleMatrix;

	projectionMatrix = perspectiveProjectionMatrix;

	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);


	//bind vao
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_elements);
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);

	//unbind vao
	glBindVertexArray(0);




	// ***** 2nd sphere on 2nd row, turquoise *****



	glUniform3f(kAUniform, 0.1f, 0.18725f, 0.1745f);
	glUniform3f(kDUniform, 0.396f, 0.74151f, 0.69102f);
	glUniform3f(kSUniform, 0.297254f, 0.30829f, 0.306678f);
	glUniform1f(kShininessUniform, 0.1 * 128);

	glViewport(vWidth / 4, 2 * vHeight / 3, (GLsizei)vWidth / 4, (GLsizei)vHeight / 6);


	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	projectionMatrix = mat4::identity();
	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	translateMatrix = translate(0.0f, 0.0f, -3.0f);
	//translateMatrix = translate(1.8f, 1.0f, -3.0f);
	scaleMatrix = scale(sphereScale, sphereScale, sphereScale);

	modelMatrix = translateMatrix * scaleMatrix;

	projectionMatrix = perspectiveProjectionMatrix;

	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);


	//bind vao
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_elements);
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);

	//unbind vao
	glBindVertexArray(0);








	// ***** 3rd sphere on 2nd row, brass *****


	glUniform3f(kAUniform, 0.329412f, 0.223529f, 0.027451f);
	glUniform3f(kDUniform, 0.780392f, 0.568627f, 0.113725f);
	glUniform3f(kSUniform, 0.992157f, 0.941176f, 0.807843f);
	glUniform1f(kShininessUniform, 0.21794872 * 128);

	glViewport(vWidth / 2, 2 * vHeight / 3, (GLsizei)vWidth / 4, (GLsizei)vHeight / 6);


	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	projectionMatrix = mat4::identity();
	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	translateMatrix = translate(0.0f, 0.0f, -3.0f);
	//translateMatrix = translate(-1.8f, 0.34f, -3.0f);
	scaleMatrix = scale(sphereScale, sphereScale, sphereScale);

	modelMatrix = translateMatrix * scaleMatrix;

	projectionMatrix = perspectiveProjectionMatrix;

	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);


	//bind vao
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_elements);
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);

	//unbind vao
	glBindVertexArray(0);


	// ***** 4th sphere on 2nd row, bronze *****


	glUniform3f(kAUniform, 0.0215f, 0.1275f, 0.054f);
	glUniform3f(kDUniform, 0.714f, 0.4284f, 0.18144f);
	glUniform3f(kSUniform, 0.393548f, 0.271906f, 0.166721f);
	glUniform1f(kShininessUniform, 0.2 * 128);

	glViewport(3 * vWidth / 4, 2 * vHeight / 3, (GLsizei)vWidth / 4, (GLsizei)vHeight / 6);


	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	projectionMatrix = mat4::identity();
	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	translateMatrix = translate(0.0f, 0.0f, -3.0f);
	//translateMatrix = translate(-1.08f, 0.34f, -3.0f);
	scaleMatrix = scale(sphereScale, sphereScale, sphereScale);

	modelMatrix = translateMatrix * scaleMatrix;

	projectionMatrix = perspectiveProjectionMatrix;

	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);


	//bind vao
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_elements);
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);

	//unbind vao
	glBindVertexArray(0);




	// ***** 1st sphere on 3rd row, chrome *****



	glUniform3f(kAUniform, 0.25f, 0.25f, 0.25f);
	glUniform3f(kDUniform, 0.4f, 0.4f, 0.4f);
	glUniform3f(kSUniform, 0.774597f, 0.774597f, 0.774597f);
	glUniform1f(kShininessUniform, 0.6 * 128);

	glViewport(0, vHeight / 2, (GLsizei)vWidth / 4, (GLsizei)vHeight / 6);


	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	projectionMatrix = mat4::identity();
	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	translateMatrix = translate(0.0f, 0.0f, -3.0f);
	//translateMatrix = translate(-0.36f, 0.34f, -3.0f);
	scaleMatrix = scale(sphereScale, sphereScale, sphereScale);

	modelMatrix = translateMatrix * scaleMatrix;

	projectionMatrix = perspectiveProjectionMatrix;

	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);


	//bind vao
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_elements);
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);

	//unbind vao
	glBindVertexArray(0);




	// ***** 2nd sphere on 3rd row, copper *****


	glUniform3f(kAUniform, 0.19125f, 0.0735f, 0.0225f);
	glUniform3f(kDUniform, 0.7038f, 0.27048f, 0.0828f);
	glUniform3f(kSUniform, 0.256777f, 0.137622f, 0.086014f);
	glUniform1f(kShininessUniform, 0.1 * 128);

	glViewport(vWidth / 4, vHeight / 2, (GLsizei)vWidth / 4, (GLsizei)vHeight / 6);


	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	projectionMatrix = mat4::identity();
	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	translateMatrix = translate(0.0f, 0.0f, -3.0f);
	//translateMatrix = translate(0.36f, 0.34f, -3.0f);
	scaleMatrix = scale(sphereScale, sphereScale, sphereScale);

	modelMatrix = translateMatrix * scaleMatrix;

	projectionMatrix = perspectiveProjectionMatrix;

	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);


	//bind vao
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_elements);
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);

	//unbind vao
	glBindVertexArray(0);



	// ***** 3rd sphere on 3rd row, gold *****


	glUniform3f(kAUniform, 0.24725f, 0.1995f, 0.0745f);
	glUniform3f(kDUniform, 0.75164f, 0.60648f, 0.22648f);
	glUniform3f(kSUniform, 0.628281f, 0.555802f, 0.366065f);
	glUniform1f(kShininessUniform, 0.4 * 128);

	glViewport(vWidth / 2, vHeight / 2, (GLsizei)vWidth / 4, (GLsizei)vHeight / 6);


	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	projectionMatrix = mat4::identity();
	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	translateMatrix = translate(0.0f, 0.0f, -3.0f);
	//translateMatrix = translate(1.08f, 0.34f, -3.0f);
	scaleMatrix = scale(sphereScale, sphereScale, sphereScale);

	modelMatrix = translateMatrix * scaleMatrix;

	projectionMatrix = perspectiveProjectionMatrix;

	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);


	//bind vao
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_elements);
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);

	//unbind vao
	glBindVertexArray(0);




	// ***** 4th sphere on 3rd row, silver *****


	glUniform3f(kAUniform, 0.19225f, 0.19225f, 0.19225f);
	glUniform3f(kDUniform, 0.50754f, 0.50754f, 0.50754f);
	glUniform3f(kSUniform, 0.508273f, 0.508273f, 0.508273f);
	glUniform1f(kShininessUniform, 0.4 * 128);

	glViewport(3 * vWidth / 4, vHeight / 2, (GLsizei)vWidth / 4, (GLsizei)vHeight / 6);

	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	projectionMatrix = mat4::identity();
	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	translateMatrix = translate(0.0f, 0.0f, -3.0f);
	//translateMatrix = translate(1.8f, 0.34f, -3.0f);
	scaleMatrix = scale(sphereScale, sphereScale, sphereScale);

	modelMatrix = translateMatrix * scaleMatrix;

	projectionMatrix = perspectiveProjectionMatrix;

	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);


	//bind vao
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_elements);
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);

	//unbind vao
	glBindVertexArray(0);








	// ***** 1st sphere on 4th row, black *****

	glUniform3f(kAUniform, 0.0f, 0.0f, 0.0f);
	glUniform3f(kDUniform, 0.01f, 0.01f, 0.01f);
	glUniform3f(kSUniform, 0.5f, 0.5f, 0.5f);
	glUniform1f(kShininessUniform, 0.25 * 128);

	glViewport(0, vHeight / 3, (GLsizei)vWidth / 4, (GLsizei)vHeight / 6);


	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	projectionMatrix = mat4::identity();
	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	translateMatrix = translate(0.0f, 0.0f, -3.0f);
	//translateMatrix = translate(-1.8f, -0.34f, -3.0f);
	scaleMatrix = scale(sphereScale, sphereScale, sphereScale);

	modelMatrix = translateMatrix * scaleMatrix;

	projectionMatrix = perspectiveProjectionMatrix;

	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);


	//bind vao
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_elements);
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);

	//unbind vao
	glBindVertexArray(0);




	// ***** 2nd sphere on 4th row, cyan *****


	glUniform3f(kAUniform, 0.0f, 0.1f, 0.06f);
	glUniform3f(kDUniform, 0.0f, 0.50980392f, 0.50980392f);
	glUniform3f(kSUniform, 0.50196078f, 0.50196078f, 0.50196078f);
	glUniform1f(kShininessUniform, 0.25 * 128);

	glViewport(vWidth / 4, vHeight / 3, (GLsizei)vWidth / 4, (GLsizei)vHeight / 6);


	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	projectionMatrix = mat4::identity();
	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	translateMatrix = translate(0.0f, 0.0f, -3.0f);
	//translateMatrix = translate(-1.08f, -0.34f, -3.0f);
	scaleMatrix = scale(sphereScale, sphereScale, sphereScale);

	modelMatrix = translateMatrix * scaleMatrix;

	projectionMatrix = perspectiveProjectionMatrix;

	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);


	//bind vao
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_elements);
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);

	//unbind vao
	glBindVertexArray(0);





	// ***** 3rd sphere on 4th row, green *****


	glUniform3f(kAUniform, 0.0f, 0.0f, 0.0f);
	glUniform3f(kDUniform, 0.1f, 0.35f, 0.1f);
	glUniform3f(kSUniform, 0.45f, 0.55f, 0.45f);
	glUniform1f(kShininessUniform, 0.25 * 128);

	glViewport(vWidth / 2, vHeight / 3, (GLsizei)vWidth / 4, (GLsizei)vHeight / 6);

	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	projectionMatrix = mat4::identity();
	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	translateMatrix = translate(0.0f, 0.0f, -3.0f);
	//translateMatrix = translate(-0.36f, -0.34f, -3.0f);
	scaleMatrix = scale(sphereScale, sphereScale, sphereScale);

	modelMatrix = translateMatrix * scaleMatrix;

	projectionMatrix = perspectiveProjectionMatrix;

	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);


	//bind vao
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_elements);
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);

	//unbind vao
	glBindVertexArray(0);




	// ***** 4th sphere on 4th row, red *****


	glUniform3f(kAUniform, 0.0f, 0.0f, 0.0f);
	glUniform3f(kDUniform, 0.5f, 0.0f, 0.0f);
	glUniform3f(kSUniform, 0.7f, 0.6f, 0.6f);
	glUniform1f(kShininessUniform, 0.25 * 128);

	glViewport(3 * vWidth / 4, vHeight / 3, (GLsizei)vWidth / 4, (GLsizei)vHeight / 6);

	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	projectionMatrix = mat4::identity();
	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	translateMatrix = translate(0.0f, 0.0f, -3.0f);
	//translateMatrix = translate(0.36f, -0.34f, -3.0f);
	scaleMatrix = scale(sphereScale, sphereScale, sphereScale);

	modelMatrix = translateMatrix * scaleMatrix;

	projectionMatrix = perspectiveProjectionMatrix;

	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);


	//bind vao
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_elements);
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);

	//unbind vao
	glBindVertexArray(0);



	// ***** 1st sphere on 5th row, white *****


	glUniform3f(kAUniform, 0.0f, 0.0f, 0.0f);
	glUniform3f(kDUniform, 0.55f, 0.55f, 0.55f);
	glUniform3f(kSUniform, 0.7f, 0.7f, 0.7f);
	glUniform1f(kShininessUniform, 0.25 * 128);

	glViewport(0, vHeight / 6, (GLsizei)vWidth / 4, (GLsizei)vHeight / 6);

	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	projectionMatrix = mat4::identity();
	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	translateMatrix = translate(0.0f, 0.0f, -3.0f);
	//translateMatrix = translate(1.08f, -0.34f, -3.0f);
	scaleMatrix = scale(sphereScale, sphereScale, sphereScale);

	modelMatrix = translateMatrix * scaleMatrix;

	projectionMatrix = perspectiveProjectionMatrix;

	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);


	//bind vao
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_elements);
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);

	//unbind vao
	glBindVertexArray(0);





	// ***** 2nd sphere on 5th row, yellow plastic *****


	glUniform3f(kAUniform, 0.0f, 0.0f, 0.0f);
	glUniform3f(kDUniform, 0.5f, 0.5f, 0.0f);
	glUniform3f(kSUniform, 0.6f, 0.6f, 0.5f);
	glUniform1f(kShininessUniform, 0.25 * 128);

	glViewport(vWidth / 4, vHeight / 6, (GLsizei)vWidth / 4, (GLsizei)vHeight / 6);

	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	projectionMatrix = mat4::identity();
	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	translateMatrix = translate(0.0f, 0.0f, -3.0f);
	//translateMatrix = translate(1.8f, -0.34f, -3.0f);
	scaleMatrix = scale(sphereScale, sphereScale, sphereScale);

	modelMatrix = translateMatrix * scaleMatrix;

	projectionMatrix = perspectiveProjectionMatrix;

	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);


	//bind vao
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_elements);
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);

	//unbind vao
	glBindVertexArray(0);




	// ***** 3rd sphere on 5th row, black *****


	glUniform3f(kAUniform, 0.02f, 0.02f, 0.02f);
	glUniform3f(kDUniform, 0.01f, 0.01f, 0.01f);
	glUniform3f(kSUniform, 0.4f, 0.4f, 0.4f);
	glUniform1f(kShininessUniform, 0.078125 * 128);


	glViewport(vWidth / 2, vHeight / 6, (GLsizei)vWidth / 4, (GLsizei)vHeight / 6);


	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	projectionMatrix = mat4::identity();
	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	translateMatrix = translate(0.0f, 0.0f, -3.0f);
	//translateMatrix = translate(-1.8f, -1.0f, -3.0f);
	scaleMatrix = scale(sphereScale, sphereScale, sphereScale);

	modelMatrix = translateMatrix * scaleMatrix;

	projectionMatrix = perspectiveProjectionMatrix;

	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);


	//bind vao
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_elements);
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);

	//unbind vao
	glBindVertexArray(0);




	// ***** 4th sphere on 5th row, cyan *****


	glUniform3f(kAUniform, 0.0f, 0.05f, 0.05f);
	glUniform3f(kDUniform, 0.4f, 0.5f, 0.5f);
	glUniform3f(kSUniform, 0.04f, 0.7f, 0.7f);
	glUniform1f(kShininessUniform, 0.078125 * 128);

	glViewport(3 * vWidth / 4, vHeight / 6, (GLsizei)vWidth / 4, (GLsizei)vHeight / 6);


	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	projectionMatrix = mat4::identity();
	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	translateMatrix = translate(0.0f, 0.0f, -3.0f);
	//translateMatrix = translate(-1.08f, -1.0f, -3.0f);
	scaleMatrix = scale(sphereScale, sphereScale, sphereScale);

	modelMatrix = translateMatrix * scaleMatrix;

	projectionMatrix = perspectiveProjectionMatrix;

	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);


	//bind vao
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_elements);
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);

	//unbind vao
	glBindVertexArray(0);




	// ***** 1st sphere on 6th row, green *****


	glUniform3f(kAUniform, 0.0f, 0.05f, 0.0f);
	glUniform3f(kDUniform, 0.4f, 0.5f, 0.4f);
	glUniform3f(kSUniform, 0.04f, 0.7f, 0.04f);
	glUniform1f(kShininessUniform, 0.078125 * 128);

	glViewport(0, 0, (GLsizei)vWidth / 4, (GLsizei)vHeight / 6);

	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	projectionMatrix = mat4::identity();
	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	translateMatrix = translate(0.0f, 0.0f, -3.0f);
	//translateMatrix = translate(-0.36f, -1.0f, -3.0f);
	scaleMatrix = scale(sphereScale, sphereScale, sphereScale);

	modelMatrix = translateMatrix * scaleMatrix;

	projectionMatrix = perspectiveProjectionMatrix;

	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);


	//bind vao
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_elements);
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);

	//unbind vao
	glBindVertexArray(0);




	// ***** 2nd sphere on 6th row, red *****


	glUniform3f(kAUniform, 0.05f, 0.0f, 0.0f);
	glUniform3f(kDUniform, 0.5f, 0.4f, 0.4f);
	glUniform3f(kSUniform, 0.7f, 0.04f, 0.04f);
	glUniform1f(kShininessUniform, 0.078125 * 128);

	glViewport(vWidth / 4, 0, (GLsizei)vWidth / 4, (GLsizei)vHeight / 6);

	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	projectionMatrix = mat4::identity();
	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	translateMatrix = translate(0.0f, 0.0f, -3.0f);
	//translateMatrix = translate(0.36f, -1.0f, -3.0f);
	scaleMatrix = scale(sphereScale, sphereScale, sphereScale);

	modelMatrix = translateMatrix * scaleMatrix;

	projectionMatrix = perspectiveProjectionMatrix;

	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);


	//bind vao
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_elements);
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);

	//unbind vao
	glBindVertexArray(0);



	// ***** 3rd sphere on 6th row, white *****


	glUniform3f(kAUniform, 0.05f, 0.05f, 0.05f);
	glUniform3f(kDUniform, 0.5f, 0.5f, 0.5f);
	glUniform3f(kSUniform, 0.7f, 0.7f, 0.7f);
	glUniform1f(kShininessUniform, 0.078125 * 128);

	glViewport(vWidth / 2, 0, (GLsizei)vWidth / 4, (GLsizei)vHeight / 6);

	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	projectionMatrix = mat4::identity();
	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	translateMatrix = translate(0.0f, 0.0f, -3.0f);
	//translateMatrix = translate(1.08f, -1.0f, -3.0f);
	scaleMatrix = scale(sphereScale, sphereScale, sphereScale);

	modelMatrix = translateMatrix * scaleMatrix;

	projectionMatrix = perspectiveProjectionMatrix;

	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);


	//bind vao
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_elements);
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);

	//unbind vao
	glBindVertexArray(0);




	// ***** 4th sphere on 6th row, yellow rubber *****


	glUniform3f(kAUniform, 0.05f, 0.05f, 0.0f);
	glUniform3f(kDUniform, 0.5f, 0.5f, 0.0f);
	glUniform3f(kSUniform, 0.7f, 0.7f, 0.04f);
	glUniform1f(kShininessUniform, 0.078125 * 128);

	glViewport(3 * vWidth / 4, 0, (GLsizei)vWidth / 4, (GLsizei)vHeight / 6);

	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	projectionMatrix = mat4::identity();
	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	translateMatrix = translate(0.0f, 0.0f, -3.0f);
	//translateMatrix = translate(1.8f, -1.0f, -3.0f);
	scaleMatrix = scale(sphereScale, sphereScale, sphereScale);

	modelMatrix = translateMatrix * scaleMatrix;

	projectionMatrix = perspectiveProjectionMatrix;

	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);


	//bind vao
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_elements);
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);

	//unbind vao
	glBindVertexArray(0);


}


void Update(void)
{
	if (KeyPressed == 1)
	{
		AngleForXRotation += 0.01f;
	}
	else if (KeyPressed == 2)
	{
		AngleForYRotation += 0.01f;
	}
	else if (KeyPressed == 3)
	{
		AngleForZRotation += 0.01f;
	}
}


void Uninitialize(void)
{
    //variable declarations
    GLXContext currentGLXContext;
    
   
	if (vao)
	{
		glDeleteVertexArrays(1, &vao);
		vao = 0;
	}

	if (vbo_sphere_position)
	{
		glDeleteBuffers(1, &vbo_sphere_position);
		vbo_sphere_position = 0;
	}

	if (vbo_sphere_normal)
	{
		glDeleteBuffers(1, &vbo_sphere_normal);
		vbo_sphere_normal = 0;
	}

	if (vbo_sphere_elements)
	{
		glDeleteBuffers(1, &vbo_sphere_elements);
		vbo_sphere_elements = 0;
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

