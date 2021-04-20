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

struct Light
{
	vec4 lightAmbient;
	vec4 lightDiffuse;
	vec4 lightSpecular;
	vec4 lightPosition;
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
bool pvLighting = true;
struct Light light[3];

GLuint shaderProgramObjectPV;
GLuint shaderProgramObjectPF;

GLuint vao;                          //vertex array object
GLuint vbo_sphere_position;                          //vertex buffer object
GLuint vbo_sphere_normal;
GLuint vbo_sphere_elements;

GLfloat sphere_vertices[1146];
GLfloat sphere_normals[1146];
GLfloat sphere_textures[764];
unsigned short sphere_elements[2280];
GLuint numVertices;
GLuint numElements;


//PV

GLuint modelMatrixUniformPV;
GLuint viewMatrixUniformPV;
GLuint projectionMatrixUniformPV;

GLuint lAUniformPV[3];
GLuint lDUniformPV[3];
GLuint lSUniformPV[3];
GLuint lightPositionUniformPV[3];

GLuint kAUniformPV;
GLuint kDUniformPV;
GLuint kSUniformPV;
GLfloat kShininessUniformPV;


//PF

GLuint modelMatrixUniformPF;
GLuint viewMatrixUniformPF;
GLuint projectionMatrixUniformPF;

GLuint lAUniformPF[3];
GLuint lDUniformPF[3];
GLuint lSUniformPF[3];
GLuint lightPositionUniformPF[3];

GLuint kAUniformPF;
GLuint kDUniformPF;
GLuint kSUniformPF;
GLfloat kShininessUniformPF;;

GLuint LKeyPressedUniform;



mat4 perspectiveProjectionMatrix;   //4x4 matrix

GLfloat lightAngle0 = 0.0f;
GLfloat lightAngle1 = 0.0f;
GLfloat lightAngle2 = 0.0f;


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
                            
                    }
                    
                    XLookupString((XKeyEvent*)&event, keys, sizeof(keys), NULL, NULL);
                    
                    switch(keys[0])
                    {
                            
                        case 'Q':
                        case 'q':
                            bDone=true;
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

                        case 'V':
                        case 'v':
                            pvLighting = true;
                            break;


                        case 'F':
                        case 'f':
                            pvLighting = false;
                            //pfLighting = true;
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
    
    GLuint vertexShaderObjectPV;
	GLuint fragmentShaderObjectPV;
	GLuint vertexShaderObjectPF;
	GLuint fragmentShaderObjectPF;
    
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


	//PER VERTEX LIGHTING


	/*****VERTEX SHADER*****/

	//create shader
	vertexShaderObjectPV = glCreateShader(GL_VERTEX_SHADER);

	//provide source code to vertex shader
	const GLchar* vertexShaderSourceCodePV =
		"#version 450 core \n" \
		"\n" \
		"in vec4 vPosition; \n" \
		"in vec3 vNormal; \n" \
		"uniform int u_lKeyPressed; \n" \
		"uniform vec3 u_lA[3]; \n" \
		"uniform vec3 u_lD[3]; \n" \
		"uniform vec3 u_lS[3]; \n" \
		"uniform vec4 u_light_position[3]; \n" \
		"uniform vec3 u_kA; \n" \
		"uniform vec3 u_kD; \n" \
		"uniform vec3 u_kS; \n" \
		"uniform float u_materialShininess; \n" \
		"uniform mat4 u_projection_matrix; \n" \
		"uniform mat4 u_view_matrix; \n" \
		"uniform mat4 u_model_matrix; \n" \
		"out vec3 phong_ads_light; \n" \
		"void main(void) \n" \
		"{ \n" \
		"\t if(u_lKeyPressed==1) \n" \
		"\t { \n" \
		"\t \t vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition; \n" \
		"\t \t vec3 transformed_normal = normalize(mat3(u_view_matrix * u_model_matrix) * vNormal); \n" \
		"\t \t vec3 view_vector = normalize(-eye_coordinates.xyz); \n" \
		"\t \t vec3 ambient[3]; \n" \
		"\t \t vec3 diffuse[3]; \n" \
		"\t \t vec3 specular[3]; \n" \
		"\t \t vec3 light_direction[3]; \n" \
		"\t \t vec3 reflection_vector[3]; \n" \
		"\n" \
		"\t \t for(int i=0; i<3; i++) \n" \
		"\t \t { \n" \
		"\t \t \t light_direction[i] = normalize(vec3(u_light_position[i] -eye_coordinates)); \n" \
		"\t \t \t reflection_vector[i] = reflect(-light_direction[i], transformed_normal); \n" \
		"\t \t \t ambient[i] = u_lA[i] * u_kA; \n" \
		"\t \t \t diffuse[i] = u_lD[i] * u_kD * max(dot(light_direction[i], transformed_normal), 0.0); \n" \
		"\t \t \t specular[i] = u_lS[i] * u_kS * pow(max(dot(reflection_vector[i], view_vector), 0.0), u_materialShininess); \n" \
		"\t \t \t phong_ads_light = phong_ads_light + ambient[i] + diffuse[i] + specular[i]; \n" \
		"\t \t } \n" \
		"\t } \n" \
		"\t else \n" \
		"\t { \n" \
		"\t \t phong_ads_light = vec3(1.0, 1.0, 1.0); \n" \
		"\t } \n" \
		"\n" \
		"\t gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition; \n" \
		"} \n";

	glShaderSource(vertexShaderObjectPV, 1, (const GLchar**)&vertexShaderSourceCodePV, NULL);

	//compile shader
	glCompileShader(vertexShaderObjectPV);

	//vertex shader compilation error checking
	GLint infoLogLength = 0;
	GLint shaderCompiledStatus = 0;
	char* szBuffer = NULL;

	glGetShaderiv(vertexShaderObjectPV, GL_COMPILE_STATUS, &shaderCompiledStatus);

	if (shaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObjectPV, GL_INFO_LOG_LENGTH, &infoLogLength);

		if (infoLogLength > 0)
		{
			szBuffer = (char*)malloc(infoLogLength);
			if (szBuffer != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(vertexShaderObjectPV, infoLogLength, &written, szBuffer);
				fprintf(gpFile, "\nVertex Shader Compilation Log of PV Lighting: %s\n", szBuffer);
				free(szBuffer);
				Uninitialize();
			}

		}
	}


	/*****FRAGMENT SHADER*****/

	//create shader
	fragmentShaderObjectPV = glCreateShader(GL_FRAGMENT_SHADER);

	//provide source code to fragment shader
	const GLchar* fragmentShaderSourceCodePV =
		"#version 450 core \n" \
		"\n" \
		"in vec3 phong_ads_light; \n" \
		"out vec4 FragColor; \n" \
		"void main(void) \n" \
		"{ \n" \
		"\t FragColor = vec4(phong_ads_light, 1.0f); \n" \
		"} \n";

	glShaderSource(fragmentShaderObjectPV, 1, (const GLchar**)&fragmentShaderSourceCodePV, NULL);

	//compile shader
	glCompileShader(fragmentShaderObjectPV);

	//fragment shader compilation error checking
	infoLogLength = 0;
	shaderCompiledStatus = 0;
	szBuffer = NULL;

	glGetShaderiv(fragmentShaderObjectPV, GL_COMPILE_STATUS, &shaderCompiledStatus);

	if (shaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(fragmentShaderObjectPV, GL_INFO_LOG_LENGTH, &infoLogLength);

		if (infoLogLength > 0)
		{
			szBuffer = (char*)malloc(infoLogLength);
			if (szBuffer != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(fragmentShaderObjectPV, infoLogLength, &written, szBuffer);
				fprintf(gpFile, "\nFragment Shader Compilation Log of PV Lighting : %s\n", szBuffer);
				free(szBuffer);
				Uninitialize();

			}
		}
	}


	/*****SHADER PROGRAM*****/

	//create
	shaderProgramObjectPV = glCreateProgram();

	//attach vertex shader to shader program
	glAttachShader(shaderProgramObjectPV, vertexShaderObjectPV);

	//attach fragment shader to shader program
	glAttachShader(shaderProgramObjectPV, fragmentShaderObjectPV);

	//pre-linking binding
	glBindAttribLocation(shaderProgramObjectPV, SPK_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(shaderProgramObjectPV, SPK_ATTRIBUTE_NORMAL, "vNormal");

	//link shader
	glLinkProgram(shaderProgramObjectPV);

	//shader linking error checking
	infoLogLength = 0;
	GLint shaderProgramLinkStatus = 0;
	szBuffer = NULL;

	glGetProgramiv(shaderProgramObjectPV, GL_LINK_STATUS, &shaderProgramLinkStatus);

	if (shaderProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObjectPV, GL_INFO_LOG_LENGTH, &infoLogLength);

		if (infoLogLength > 0)
		{
			szBuffer = (char*)malloc(infoLogLength);

			if (szBuffer != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(shaderProgramObjectPV, infoLogLength, &written, szBuffer);
				fprintf(gpFile, "\nShader Program Link Log of PV Lighting : %s\n", szBuffer);
				free(szBuffer);
				Uninitialize();
			}
		}
	}

	//get uniform location
	modelMatrixUniformPV = glGetUniformLocation(shaderProgramObjectPV, "u_model_matrix");
	viewMatrixUniformPV = glGetUniformLocation(shaderProgramObjectPV, "u_view_matrix");
	projectionMatrixUniformPV = glGetUniformLocation(shaderProgramObjectPV, "u_projection_matrix");

	LKeyPressedUniform = glGetUniformLocation(shaderProgramObjectPV, "u_lKeyPressed");

	lAUniformPV[0] = glGetUniformLocation(shaderProgramObjectPV, "u_lA[0]");
	lDUniformPV[0] = glGetUniformLocation(shaderProgramObjectPV, "u_lD[0]");
	lSUniformPV[0] = glGetUniformLocation(shaderProgramObjectPV, "u_lS[0]");
	lightPositionUniformPV[0] = glGetUniformLocation(shaderProgramObjectPV, "u_light_position[0]");

	lAUniformPV[1] = glGetUniformLocation(shaderProgramObjectPV, "u_lA[1]");
	lDUniformPV[1] = glGetUniformLocation(shaderProgramObjectPV, "u_lD[1]");
	lSUniformPV[1] = glGetUniformLocation(shaderProgramObjectPV, "u_lS[1]");
	lightPositionUniformPV[1] = glGetUniformLocation(shaderProgramObjectPV, "u_light_position[1]");

	lAUniformPV[2] = glGetUniformLocation(shaderProgramObjectPV, "u_lA[2]");
	lDUniformPV[2] = glGetUniformLocation(shaderProgramObjectPV, "u_lD[2]");
	lSUniformPV[2] = glGetUniformLocation(shaderProgramObjectPV, "u_lS[2]");
	lightPositionUniformPV[2] = glGetUniformLocation(shaderProgramObjectPV, "u_light_position[2]");

	kAUniformPV = glGetUniformLocation(shaderProgramObjectPV, "u_kA");
	kDUniformPV = glGetUniformLocation(shaderProgramObjectPV, "u_kD");
	kSUniformPV = glGetUniformLocation(shaderProgramObjectPV, "u_kS");
	kShininessUniformPV = glGetUniformLocation(shaderProgramObjectPV, "u_materialShininess");



	/*******************************************************************************************************/


	//PER FRAGMENT LIGHTING


	/*****VERTEX SHADER*****/

	//create shader
	vertexShaderObjectPF = glCreateShader(GL_VERTEX_SHADER);

	//provide source code to vertex shader
	const GLchar* vertexShaderSourceCodePF =
		"#version 450 core \n" \
		"\n" \
		"in vec4 vPosition; \n" \
		"in vec3 vNormal; \n" \
		"uniform int u_lKeyPressed; \n" \
		"uniform vec4 u_light_position[3]; \n" \
		"uniform mat4 u_projection_matrix; \n" \
		"uniform mat4 u_model_matrix; \n" \
		"uniform mat4 u_view_matrix; \n" \
		"out vec3 transformed_normal; \n" \
		"out vec3 light_direction[3]; \n" \
		"out vec3 view_vector; \n" \
		"\n" \
		"void main(void) \n" \
		"{ \n" \
		"\t if(u_lKeyPressed==1) \n" \
		"\t { \n" \
		"\t \t vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition; \n" \
		"\t \t transformed_normal = mat3(u_view_matrix * u_model_matrix) * vNormal; \n" \
		"\t \t view_vector = -eye_coordinates.xyz; \n" \
		"\n" \
		"\t \t for(int i = 0; i < 3; i++) \n" \
		"\t \t { \n" \
		"\t \t \t light_direction[i] = vec3(u_light_position[i] - eye_coordinates); \n" \
		"\t \t } \n" \
		"\t } \n" \
		"\t gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition; \n" \
		"} \n";

	glShaderSource(vertexShaderObjectPF, 1, (const GLchar**)&vertexShaderSourceCodePF, NULL);

	//compile shader
	glCompileShader(vertexShaderObjectPF);

	//vertex shader compilation error checking
	infoLogLength = 0;
	shaderCompiledStatus = 0;
	szBuffer = NULL;

	glGetShaderiv(vertexShaderObjectPF, GL_COMPILE_STATUS, &shaderCompiledStatus);

	if (shaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObjectPF, GL_INFO_LOG_LENGTH, &infoLogLength);

		if (infoLogLength > 0)
		{
			szBuffer = (char*)malloc(infoLogLength);
			if (szBuffer != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(vertexShaderObjectPF, infoLogLength, &written, szBuffer);
				fprintf(gpFile, "\nVertex Shader Compilation Log of PF Lighting : %s\n", szBuffer);
				free(szBuffer);
				Uninitialize();
			}

		}
	}


	/*****FRAGMENT SHADER*****/

	//create shader
	fragmentShaderObjectPF = glCreateShader(GL_FRAGMENT_SHADER);

	//provide source code to fragment shader
	const GLchar* fragmentShaderSourceCodePF =
		"#version 450 core \n" \
		"\n" \
		"in vec3 transformed_normal; \n" \
		"in vec3 light_direction[3]; \n" \
		"in vec3 view_vector; \n" \
		"uniform int u_lKeyPressed; \n" \
		"uniform vec3 u_lA[3]; \n" \
		"uniform vec3 u_lD[3]; \n" \
		"uniform vec3 u_lS[3]; \n" \
		"uniform vec3 u_kA; \n" \
		"uniform vec3 u_kD; \n" \
		"uniform vec3 u_kS; \n" \
		"uniform float u_materialShininess; \n" \
		"out vec4 FragColor; \n" \
		"vec3 phong_ads_light; \n" \
		"void main(void) \n" \
		"{ \n" \
		"\t vec3 normalized_transformed_normals; \n" \
		"\t vec3 normalized_view_vector; \n" \
		"\t vec3 normalized_light_direction[3]; \n" \
		"\t vec3 reflection_vector[3]; \n" \
		"\t vec3 ambient[3]; \n" \
		"\t vec3 diffuse[3]; \n" \
		"\t vec3 specular[3]; \n" \
		"\n" \
		"\t if(u_lKeyPressed==1) \n" \
		"\t { \n" \
		"\t \t normalized_transformed_normals = normalize(transformed_normal); \n" \
		"\t \t normalized_view_vector = normalize(view_vector); \n" \
		"\n" \
		"\t \t for(int i = 0; i < 3; i++) \n" \
		"\t \t { \n" \
		"\t \t \t normalized_light_direction[i] = normalize(light_direction[i]); \n" \
		"\t \t \t reflection_vector[i] = reflect(-normalized_light_direction[i], normalized_transformed_normals); \n" \
		"\t \t \t ambient[i] = u_lA[i] * u_kA; \n" \
		"\t \t \t diffuse[i] = u_lD[i] * u_kD * max(dot(normalized_light_direction[i], normalized_transformed_normals), 0.0); \n" \
		"\t \t \t specular[i] = u_lS[i] * u_kS * pow(max(dot(reflection_vector[i], normalized_view_vector), 0.0), u_materialShininess); \n" \
		"\t \t \t phong_ads_light += ambient[i] + diffuse[i] + specular[i]; \n" \
		"\t \t } \n" \
		"\t } \n" \
		"\t else \n" \
		"\t { \n" \
		"\t \t phong_ads_light = vec3(1.0, 1.0, 1.0); \n" \
		"\t } \n" \
		"\n" \
		"\t FragColor = vec4(phong_ads_light, 1.0f); \n" \
		"} \n";

	glShaderSource(fragmentShaderObjectPF, 1, (const GLchar**)&fragmentShaderSourceCodePF, NULL);

	//compile shader
	glCompileShader(fragmentShaderObjectPF);

	//fragment shader compilation error checking
	infoLogLength = 0;
	shaderCompiledStatus = 0;
	szBuffer = NULL;

	glGetShaderiv(fragmentShaderObjectPF, GL_COMPILE_STATUS, &shaderCompiledStatus);

	if (shaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(fragmentShaderObjectPF, GL_INFO_LOG_LENGTH, &infoLogLength);

		if (infoLogLength > 0)
		{
			szBuffer = (char*)malloc(infoLogLength);
			if (szBuffer != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(fragmentShaderObjectPF, infoLogLength, &written, szBuffer);
				fprintf(gpFile, "\nFragment Shader Compilation Log of PF Lighting : %s\n", szBuffer);
				free(szBuffer);
				Uninitialize();

			}
		}
	}


	/*****SHADER PROGRAM*****/

	//create
	shaderProgramObjectPF = glCreateProgram();

	//attach vertex shader to shader program
	glAttachShader(shaderProgramObjectPF, vertexShaderObjectPF);

	//attach fragment shader to shader program
	glAttachShader(shaderProgramObjectPF, fragmentShaderObjectPF);

	//pre-linking binding
	glBindAttribLocation(shaderProgramObjectPF, SPK_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(shaderProgramObjectPF, SPK_ATTRIBUTE_NORMAL, "vNormal");

	//link shader
	glLinkProgram(shaderProgramObjectPF);

	//shader linking error checking
	infoLogLength = 0;
	shaderProgramLinkStatus = 0;
	szBuffer = NULL;

	glGetProgramiv(shaderProgramObjectPF, GL_LINK_STATUS, &shaderProgramLinkStatus);

	if (shaderProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObjectPF, GL_INFO_LOG_LENGTH, &infoLogLength);

		if (infoLogLength > 0)
		{
			szBuffer = (char*)malloc(infoLogLength);

			if (szBuffer != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(shaderProgramObjectPF, infoLogLength, &written, szBuffer);
				fprintf(gpFile, "\nShader Program Link Log of PF Lighting: %s\n", szBuffer);
				free(szBuffer);
				Uninitialize();
			}
		}
	}

	//get uniform location
	modelMatrixUniformPF = glGetUniformLocation(shaderProgramObjectPF, "u_model_matrix");
	viewMatrixUniformPF = glGetUniformLocation(shaderProgramObjectPF, "u_view_matrix");
	projectionMatrixUniformPF = glGetUniformLocation(shaderProgramObjectPF, "u_projection_matrix");

	LKeyPressedUniform = glGetUniformLocation(shaderProgramObjectPF, "u_lKeyPressed");

	lAUniformPF[0] = glGetUniformLocation(shaderProgramObjectPF, "u_lA[0]");
	lDUniformPF[0] = glGetUniformLocation(shaderProgramObjectPF, "u_lD[0]");
	lSUniformPF[0] = glGetUniformLocation(shaderProgramObjectPF, "u_lS[0]");
	lightPositionUniformPF[0] = glGetUniformLocation(shaderProgramObjectPF, "u_light_position[0]");

	lAUniformPF[1] = glGetUniformLocation(shaderProgramObjectPF, "u_lA[1]");
	lDUniformPF[1] = glGetUniformLocation(shaderProgramObjectPF, "u_lD[1]");
	lSUniformPF[1] = glGetUniformLocation(shaderProgramObjectPF, "u_lS[1]");
	lightPositionUniformPF[1] = glGetUniformLocation(shaderProgramObjectPF, "u_light_position[1]");

	lAUniformPF[2] = glGetUniformLocation(shaderProgramObjectPF, "u_lA[2]");
	lDUniformPF[2] = glGetUniformLocation(shaderProgramObjectPF, "u_lD[2]");
	lSUniformPF[2] = glGetUniformLocation(shaderProgramObjectPF, "u_lS[2]");
	lightPositionUniformPF[2] = glGetUniformLocation(shaderProgramObjectPF, "u_light_position[2]");

	kAUniformPF = glGetUniformLocation(shaderProgramObjectPF, "u_kA");
	kDUniformPF = glGetUniformLocation(shaderProgramObjectPF, "u_kD");
	kSUniformPF = glGetUniformLocation(shaderProgramObjectPF, "u_kS");
	kShininessUniformPF = glGetUniformLocation(shaderProgramObjectPF, "u_materialShininess");



	/*******************************************************************************************************/





	//vertices array declaration

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



	//LIGHT INITIALIZATION


	light[0].lightAmbient = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	light[0].lightDiffuse = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	light[0].lightSpecular = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	light[0].lightPosition = vec4(2.0f, 0.0f, 0.0f, 1.0f);

	light[1].lightAmbient = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	light[1].lightDiffuse = vec4(0.0f, 1.0f, 0.0f, 1.0f);
	light[1].lightSpecular = vec4(0.0f, 1.0f, 0.0f, 1.0f);
	light[1].lightPosition = vec4(0.0f, 0.0f, 0.0f, 1.0f);

	light[2].lightAmbient = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	light[2].lightDiffuse = vec4(0.0f, 0.0f, 1.0f, 1.0f);
	light[2].lightSpecular = vec4(0.0f, 0.0f, 1.0f, 1.0f);
	light[2].lightPosition = vec4(-2.0f, 0.0f, 0.0f, 1.0f);


    
    
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
   GLfloat r = 5.0f;

	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//start using OpenGL program object
	if (pvLighting == true)
	{
		glUseProgram(shaderProgramObjectPV);
	}

	else
	{
		glUseProgram(shaderProgramObjectPF);
	}


	if (bLight == true)
	{
		if (pvLighting == true)
		{
			glUniform1i(LKeyPressedUniform, 1);

			/****GOURAUD****/

			glUniform3fv(lAUniformPV[0], 1, light[0].lightAmbient);
			glUniform3fv(lDUniformPV[0], 1, light[0].lightDiffuse);
			glUniform3fv(lSUniformPV[0], 1, light[0].lightSpecular);
			glUniform4fv(lightPositionUniformPV[0], 1, light[0].lightPosition);

			glUniform3fv(lAUniformPV[1], 1, light[1].lightAmbient);
			glUniform3fv(lDUniformPV[1], 1, light[1].lightDiffuse);
			glUniform3fv(lSUniformPV[1], 1, light[1].lightSpecular);
			glUniform4fv(lightPositionUniformPV[1], 1, light[1].lightPosition);

			glUniform3fv(lAUniformPV[2], 1, light[2].lightAmbient);
			glUniform3fv(lDUniformPV[2], 1, light[2].lightDiffuse);
			glUniform3fv(lSUniformPV[2], 1, light[2].lightSpecular);
			glUniform4fv(lightPositionUniformPV[2], 1, light[2].lightPosition);

			glUniform3f(kAUniformPV, 0.0f, 0.0f, 0.0f);
			glUniform3f(kDUniformPV, 1.0f, 1.0f, 1.0f);
			glUniform3f(kSUniformPV, 1.0f, 1.0f, 1.0f);
			glUniform1f(kShininessUniformPV, 50.0);

			///****ALBEDO****/
			//
			//glUniform3f(lAUniform, 0.1f, 0.1f, 0.1f);
			//glUniform3f(lDUniform, 1.0f, 1.0f, 1.0f);
			//glUniform3f(lSUniform, 1.0f, 1.0f, 1.0f);
			//glUniform4fv(lightPositionUniform, 1, (GLfloat*)lightPosition);

			//glUniform3f(kAUniform, 0.0f, 0.0f, 0.0f);
			//glUniform3f(kDUniform, 0.5f, 0.2f, 0.7f);
			//glUniform3f(kSUniform, 0.7f, 0.7f, 0.7f);
			//glUniform1f(kShininessUniform, 128.0);

		}

		else
		{
			glUniform1i(LKeyPressedUniform, 1);

			/****GOURAUD****/

			glUniform3fv(lAUniformPF[0], 1, light[0].lightAmbient);
			glUniform3fv(lDUniformPF[0], 1, light[0].lightDiffuse);
			glUniform3fv(lSUniformPF[0], 1, light[0].lightSpecular);
			glUniform4fv(lightPositionUniformPF[0], 1, light[0].lightPosition);

			glUniform3fv(lAUniformPF[1], 1, light[1].lightAmbient);
			glUniform3fv(lDUniformPF[1], 1, light[1].lightDiffuse);
			glUniform3fv(lSUniformPF[1], 1, light[1].lightSpecular);
			glUniform4fv(lightPositionUniformPF[1], 1, light[1].lightPosition);

			glUniform3fv(lAUniformPF[2], 1, light[2].lightAmbient);
			glUniform3fv(lDUniformPF[2], 1, light[2].lightDiffuse);
			glUniform3fv(lSUniformPF[2], 1, light[2].lightSpecular);
			glUniform4fv(lightPositionUniformPF[2], 1, light[2].lightPosition);

			glUniform3f(kAUniformPF, 0.0f, 0.0f, 0.0f);
			glUniform3f(kDUniformPF, 1.0f, 1.0f, 1.0f);
			glUniform3f(kSUniformPF, 1.0f, 1.0f, 1.0f);
			glUniform1f(kShininessUniformPF, 128.0);

			///****ALBEDO****/
			//
			//glUniform3f(lAUniform, 0.1f, 0.1f, 0.1f);
			//glUniform3f(lDUniform, 1.0f, 1.0f, 1.0f);
			//glUniform3f(lSUniform, 1.0f, 1.0f, 1.0f);
			//glUniform4fv(lightPositionUniform, 1, (GLfloat*)lightPosition);

			//glUniform3f(kAUniform, 0.0f, 0.0f, 0.0f);
			//glUniform3f(kDUniform, 0.5f, 0.2f, 0.7f);
			//glUniform3f(kSUniform, 0.7f, 0.7f, 0.7f);
			//glUniform1f(kShininessUniform, 128.0);

		}

		


		light[0].lightPosition[0] = 0.0f;
		light[0].lightPosition[1] = r * sin(lightAngle0);
		light[0].lightPosition[2] = r * cos(lightAngle0);
		light[0].lightPosition[3] = 1.0f;

		light[1].lightPosition[0] = r * sin(lightAngle1);
		light[1].lightPosition[1] = 0.0f;
		light[1].lightPosition[2] = r * cos(lightAngle1);
		light[1].lightPosition[3] = 1.0f;

		light[2].lightPosition[0] = r * sin(lightAngle2);
		light[2].lightPosition[1] = r * cos(lightAngle2);
		light[2].lightPosition[2] = 0.0f;
		light[2].lightPosition[3] = 1.0f;



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


	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	projectionMatrix = mat4::identity();
	translateMatrix = mat4::identity();
	translateMatrix = translate(0.0f, 0.0f, -3.0f);

	modelMatrix = translateMatrix;

	//projectionMatrix = perspectiveProjectionMatrix * viewMatrix * modelMatrix;  //pre-multiplication of matrices
	projectionMatrix = perspectiveProjectionMatrix;

	if (pvLighting == true)
	{
		glUniformMatrix4fv(modelMatrixUniformPV, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(viewMatrixUniformPV, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionMatrixUniformPV, 1, GL_FALSE, projectionMatrix);
	}
	else
	{
		glUniformMatrix4fv(modelMatrixUniformPF, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(viewMatrixUniformPF, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionMatrixUniformPF, 1, GL_FALSE, projectionMatrix);
	}


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


void Update(void)
{
	lightAngle0 += 0.01f;
	if (lightAngle0 >= 360.0f)
	{
		lightAngle0 = 0.0f;
	}

	lightAngle1 += 0.01f;
	if (lightAngle1 >= 360.0f)
	{
		lightAngle1 = 0.0f;
	}

	lightAngle2 += 0.01f;
	if (lightAngle2 >= 360.0f)
	{
		lightAngle2 = 0.0f;
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

	if (shaderProgramObjectPV)
	{
		glUseProgram(shaderProgramObjectPV);
		GLsizei shaderCount;
		glGetProgramiv(shaderProgramObjectPV, GL_ATTACHED_SHADERS, &shaderCount);

		GLuint* pShaders = NULL;
		pShaders = (GLuint*)malloc(sizeof(GLuint) * shaderCount);

		glGetAttachedShaders(shaderProgramObjectPV, shaderCount, &shaderCount, pShaders);

		for (GLsizei i = 0; i < shaderCount; i++)
		{
			glDetachShader(shaderProgramObjectPV, pShaders[i]);
			glDeleteShader(pShaders[i]);
			pShaders[i] = 0;
		}
		free(pShaders);

		glDeleteProgram(shaderProgramObjectPV);
		shaderProgramObjectPV = 0;
		glUseProgram(0);

	}


	if (shaderProgramObjectPF)
	{
		glUseProgram(shaderProgramObjectPF);
		GLsizei shaderCount;
		glGetProgramiv(shaderProgramObjectPF, GL_ATTACHED_SHADERS, &shaderCount);

		GLuint* pShaders = NULL;
		pShaders = (GLuint*)malloc(sizeof(GLuint) * shaderCount);

		glGetAttachedShaders(shaderProgramObjectPF, shaderCount, &shaderCount, pShaders);

		for (GLsizei i = 0; i < shaderCount; i++)
		{
			glDetachShader(shaderProgramObjectPF, pShaders[i]);
			glDeleteShader(pShaders[i]);
			pShaders[i] = 0;
		}
		free(pShaders);

		glDeleteProgram(shaderProgramObjectPF);
		shaderProgramObjectPF = 0;
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

