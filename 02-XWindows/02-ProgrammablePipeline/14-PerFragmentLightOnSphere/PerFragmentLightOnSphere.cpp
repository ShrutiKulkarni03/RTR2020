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


bool bLight;

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
		"void main(void) \n" \
		"{ \n" \
		"if(u_lKeyPressed==1) \n" \
		"{ \n" \
		"vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition; \n" \
		"transformed_normal = mat3(u_view_matrix * u_model_matrix) * vNormal; \n" \
		"light_direction = vec3(u_light_position - eye_coordinates); \n" \
		"view_vector = -eye_coordinates.xyz; \n" \
		"} \n" \
		"gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition; \n" \
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


void Draw(void)
{
   //code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//start using OpenGL program object
	glUseProgram(shaderProgramObject);


	if (bLight == true)
	{
		GLfloat lightPosition[] = { 100.0f, 100.0f, 100.0f, 1.0f };

		glUniform1i(LKeyPressedUniform, 1);

		///****GOURAUD****/

		//glUniform3f(lAUniform, 0.0f, 0.0f, 0.0f);
		//glUniform3f(lDUniform, 1.0f, 1.0f, 1.0f);
		//glUniform3f(lSUniform, 1.0f, 1.0f, 1.0f);
		//glUniform4fv(lightPositionUniform, 1, (GLfloat*)lightPosition);

		//glUniform3f(kAUniform, 0.0f, 0.0f, 0.0f);
		//glUniform3f(kDUniform, 1.0f, 1.0f, 1.0f);
		//glUniform3f(kSUniform, 1.0f, 1.0f, 1.0f);
		//glUniform1f(kShininessUniform, 50.0);

		/****ALBEDO****/
		
		glUniform3f(lAUniform, 0.1f, 0.1f, 0.1f);
		glUniform3f(lDUniform, 1.0f, 1.0f, 1.0f);
		glUniform3f(lSUniform, 1.0f, 1.0f, 1.0f);
		glUniform4fv(lightPositionUniform, 1, (GLfloat*)lightPosition);

		glUniform3f(kAUniform, 0.0f, 0.0f, 0.0f);
		glUniform3f(kDUniform, 0.5f, 0.2f, 0.7f);
		glUniform3f(kSUniform, 0.7f, 0.7f, 0.7f);
		glUniform1f(kShininessUniform, 128.0);
		
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}


	//OpenGL Drawing

	
	mat4 modelMatrix;
	mat4 viewMatrix;
	mat4 projectionMatrix;
	mat4 translateMatrix;
	mat4 scaleMatrix;

	
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	projectionMatrix = mat4::identity();
	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	translateMatrix = translate(0.0f, 0.0f, -3.0f);
	scaleMatrix = scale(1.5f, 1.5f, 1.5f);

	modelMatrix = translateMatrix * scaleMatrix;

	projectionMatrix = perspectiveProjectionMatrix * viewMatrix * modelMatrix;  //pre-multiplication of matrices
	//projectionMatrix = perspectiveProjectionMatrix;

	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);


	//bind vao
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_elements);
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);

	//unbind vao
	glBindVertexArray(0);

	//stop using OpenGL program object
	glUseProgram(0);
    
    glXSwapBuffers(gpDisplay, gWindow);
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

