#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h> //analogous to windows.h
#import <quartzCore/CVDisplayLink.h>
#import <OpenGL/GL3.h>  //gl.h for FFP

#include "vmath.h"
#include "SphereMesh.h"
using namespace vmath;

CVReturn myDisplayLinkCallback(CVDisplayLinkRef, const CVTimeStamp*, const CVTimeStamp*, CVOptionFlags, CVOptionFlags*, void*);

FILE *gpFile = NULL;

@interface AppDelegate : NSObject <NSApplicationDelegate, NSWindowDelegate>  //next step - NS
@end

int main(int argc, char* argv[])
{
    //code
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc]init];
    NSApp = [NSApplication sharedApplication];
    [NSApp setDelegate : [[AppDelegate alloc]init]];
    
    [NSApp run];       //this is called as run loop / message loop / event loop
    
    [pool release];
    
    return(0);
}

@interface MyOpenGLView : NSOpenGLView
@end

@implementation AppDelegate
{
    @private
    NSWindow *window;
    MyOpenGLView *myOpenGLView;
}

-(void)applicationDidFinishLaunching : (NSNotification *)aNotification
{
    //code
    NSBundle *appBundle = [NSBundle mainBundle];
    NSString *appDirPath = [appBundle bundlePath];
    NSString *parentDirPath = [appDirPath stringByDeletingLastPathComponent];
    NSString *logFileNameWithPath = [NSString stringWithFormat : @"%@/Log.txt", parentDirPath];
    const char *pszLogFileNameWithPath = [logFileNameWithPath cStringUsingEncoding : NSASCIIStringEncoding];
    
    gpFile = fopen(pszLogFileNameWithPath, "w");
    if(gpFile == NULL)
    {
        [self release];
        [NSApp terminate : self];
    }
    else
    {
        fprintf(gpFile, "Program Started Successfully. \n");
    }
    
    
    NSRect win_rect = NSMakeRect(0.0, 0.0, 800.0, 600.0);
    
    window = [[NSWindow alloc]initWithContentRect : win_rect
                                        styleMask : NSWindowStyleMaskTitled |
                                                    NSWindowStyleMaskClosable |
                                                    NSWindowStyleMaskMiniaturizable |
                                                    NSWindowStyleMaskResizable
                                          backing : NSBackingStoreBuffered
                                            defer : NO];
    
    [window setTitle : @"SPK : MacOS Window"];
    [window center];
    
    myOpenGLView = [[MyOpenGLView alloc]initWithFrame : win_rect];
    
    [window setContentView : myOpenGLView];
    [window setDelegate : self];
    [window makeKeyAndOrderFront : self];
    
}

-(void)applicationWillTerminate : (NSNotification *)aNotification
{
    //code
    if(gpFile)
    {
        fprintf(gpFile, "Program Terminated Successfully. \n");
        fclose(gpFile);
        gpFile = NULL;
    }
}

-(void)windowWillClose : (NSNotification *)aNotification
{
    //code
    [NSApp terminate : self];
}

-(void)dealloc
{
    //code
    [myOpenGLView release];
    [window release];
    [super dealloc];
    
}

@end

@implementation MyOpenGLView
{
    @private
    CVDisplayLinkRef displayLink;
    
    //enum
    
    
    //shader objects
    GLuint vertexShaderObject;
    GLuint fragmentShaderObject;
    GLuint shaderProgramObject;
    
    //UNIFORMS

    GLuint modelMatrixUniform;
    GLuint viewMatrixUniform;
    GLuint projectionMatrixUniform;

    GLuint LKeyPressedUniform;
    GLuint XKeyPressedUniform;
    GLuint YKeyPressedUniform;
    GLuint ZKeyPressedUniform;

    GLuint lAUniform;
    GLuint lDUniform;
    GLuint lSUniform;
    GLuint lightPositionUniform;

    GLuint kAUniform;
    GLuint kDUniform;
    GLuint kSUniform;
    GLfloat kShininessUniform;

    GLint KeyPressed;
    GLfloat AngleForXRotation;
    GLfloat AngleForYRotation;
    GLfloat AngleForZRotation;
    
    int vWidth;
    int vHeight;

    bool bLight;
    vec4 lightPosition;

    vmath::mat4 perspectiveProjectionMatrix;   //4x4 matrix
}

-(id)initWithFrame : (NSRect)frame
{
    //code
    self = [super initWithFrame : frame];
    
    if(self)
    {
        NSOpenGLPixelFormatAttribute attributes[] =
        {
            NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion4_1Core,      //NSOpenGLProfileVersionLegacy for FFP
            NSOpenGLPFAScreenMask, CGDisplayIDToOpenGLDisplayMask(kCGDirectMainDisplay),
            NSOpenGLPFANoRecovery,
            NSOpenGLPFAAccelerated,
            NSOpenGLPFAColorSize, 24,
            NSOpenGLPFADepthSize, 24,
            NSOpenGLPFAAlphaSize, 8,
            NSOpenGLPFADoubleBuffer,
            0
        };
        
        NSOpenGLPixelFormat* pixelFormat = [[[NSOpenGLPixelFormat alloc]initWithAttributes : attributes]autorelease];
        
        if(pixelFormat == nil)
        {
            fprintf(gpFile, "Cannot Obtain Pixel Format. Exiting. \n");
            [self release];
            [NSApp terminate : self];
            
        }
        
        NSOpenGLContext* glContext = [[[NSOpenGLContext alloc]initWithFormat : pixelFormat shareContext : nil]autorelease];
        
        [self setPixelFormat : pixelFormat];
        [self setOpenGLContext : glContext];
    }
    
    return(self);
}

-(CVReturn)getFrameForTime : (const CVTimeStamp*)outputTime
{
    //code
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc]init];
    [self drawView];
    [pool release];
    
    return(kCVReturnSuccess);
}

-(void)prepareOpenGL
{
    //code
    [super prepareOpenGL];
    
    [[self openGLContext]makeCurrentContext];
    
    //swap interval
    GLint swapInt = 1;
    [[self openGLContext]setValues : &swapInt forParameter : NSOpenGLCPSwapInterval];
    
    //opengl code
    
    /*****VERTEX SHADER*****/

    //create shader
    vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

    //provide source code to vertex shader
    const GLchar *vertexShaderSourceCode =
        "#version 410 core" \
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
        "    if(u_lKeyPressed==1) \n" \
        "    { \n" \
        "        vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition; \n" \
        "        transformed_normal = mat3(u_view_matrix * u_model_matrix) * vNormal; \n" \
        "        light_direction = vec3(u_light_position - eye_coordinates); \n" \
        "        view_vector = -eye_coordinates.xyz; \n" \
        "    } \n" \
        "\n" \
        "    gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition; \n" \
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
                [self release];
                [NSApp terminate : self];
            }

        }
    }


    /*****FRAGMENT SHADER*****/

    //create shader
    fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

    //provide source code to fragment shader
    const GLchar* fragmentShaderSourceCode =
        "#version 410 core" \
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
        "   if(u_lKeyPressed==1) \n" \
        "   { \n" \
        "       vec3 normalized_transformed_normals = normalize(transformed_normal); \n" \
        "       vec3 normalized_light_direction = normalize(light_direction); \n" \
        "       vec3 normalized_view_vector = normalize(view_vector); \n" \
        "       vec3 reflection_vector = reflect(-normalized_light_direction, normalized_transformed_normals); \n" \
        "       vec3 ambient = u_lA * u_kA; \n" \
        "       vec3 diffuse = u_lD * u_kD * max(dot(normalized_light_direction, normalized_transformed_normals), 0.0); \n" \
        "       vec3 specular = u_lS * u_kS * pow(max(dot(reflection_vector, normalized_view_vector), 0.0), u_materialShininess); \n" \
        "       phong_ads_light = ambient + diffuse + specular; \n" \
        "   } \n" \
        "   else \n" \
        "   { \n" \
        "       phong_ads_light = vec3(1.0, 1.0, 1.0); \n" \
        "   } \n" \
        "   FragColor = vec4(phong_ads_light, 1.0f); \n" \
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
                [self release];
                [NSApp terminate : self];

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
                [self release];
                [NSApp terminate : self];
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


    //vertices array declaration
    makeSphere(1.0, 50, 50);
    
    //depth
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    //glEnable(GL_CULL_FACE);
    
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    
    //set perspective matrix to identity matrix
    perspectiveProjectionMatrix = vmath::mat4::identity();
    
    //variables initialization
    KeyPressed = 0;
    AngleForXRotation = 0.0f;
    AngleForYRotation = 0.0f;
    AngleForZRotation = 0.0f;
    
//    vWidth = 800;
//    vHeight = 600;
    
    lightPosition = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    
    //CV and CGL related code
    CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
    CVDisplayLinkSetOutputCallback(displayLink, &myDisplayLinkCallback, self);
    CGLContextObj cglContext = (CGLContextObj)[[self openGLContext]CGLContextObj];
    CGLPixelFormatObj cglPixelFormat = (CGLPixelFormatObj)[[self pixelFormat]CGLPixelFormatObj];
    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink, cglContext, cglPixelFormat);
    CVDisplayLinkStart(displayLink);
}

-(void)reshape
{
    //code
    [super reshape];
    CGLLockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
    
//    NSRect rect = [self bounds];
    NSRect rect = [self convertRectToBacking:[self bounds]];
    
    if(rect.size.height<0)
        rect.size.height = 1;
    
    vWidth= rect.size.width;
    vHeight= rect.size.height;
    
    glViewport(0, 0, (GLsizei)rect.size.width, (GLsizei)rect.size.height);
    
    perspectiveProjectionMatrix = vmath::perspective(45.0f, ((GLfloat)rect.size.width / 4) / ((GLfloat)rect.size.height / 6), 0.1f, 100.0f);
//    perspectiveProjectionMatrix = vmath::perspective(45.0f, ((GLfloat)rect.size.width) / ((GLfloat)rect.size.height), 0.1f, 100.0f);
    
    fprintf(gpFile, "rectWidth = %f rectHeight = %f\n", rect.size.width, rect.size.height);
    
    CGLUnlockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
    
}

-(void)drawRect : (NSRect)dirtyRect
{
    //code
    [self drawView];
}

-(void)drawView
{
    GLfloat r = 5.0f;
    
    //code
    [[self openGLContext]makeCurrentContext];
    CGLLockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
    
    //opengl code
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //start using OpenGL program object
    glUseProgram(shaderProgramObject);

    if (bLight == true)
    {

        glUniform1i(LKeyPressedUniform, 1);

        /****GOURAUD****/


        glUniform3f(lAUniform, 0.0f, 0.0f, 0.0f);
        glUniform3f(lDUniform, 1.0f, 1.0f, 1.0f);
        glUniform3f(lSUniform, 1.0f, 1.0f, 1.0f);
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
    
    
    //drawSphere();

    
    [self draw24Spheres];

    //stop using OpenGL program object
    glUseProgram(0);

    //update func()
    [self myUpdate];
    
    CGLFlushDrawable((CGLContextObj)[[self openGLContext]CGLContextObj]);
    CGLUnlockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
}

-(void)draw24Spheres
{
    //code
    mat4 modelMatrix;
    mat4 viewMatrix;
    mat4 projectionMatrix;
    mat4 translateMatrix;
    mat4 scaleMatrix;

    GLfloat sphereScale = 1.0f;


    // ***** 1st sphere on 1st row, emerald *****


    glUniform3f(kAUniform, 0.0215f, 0.1745f, 0.0215f);
    glUniform3f(kDUniform, 0.07568f, 0.61424f, 0.07568f);
    glUniform3f(kSUniform, 0.633f, 0.727811f, 0.633f);
    glUniform1f(kShininessUniform, 0.6 * 128);

    perspectiveProjectionMatrix = vmath::perspective(45.0f, ((GLfloat)vWidth / 4) / ((GLfloat)vHeight / 6), 0.1f, 100.0f);
    

    glViewport(0, 5 * vHeight / 6, (GLsizei)vWidth / 4, (GLsizei)vHeight / 6);
    
    fprintf(gpFile, "X = 0, Y = %d\n", 5 * vHeight / 6);
    
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

    drawSphere();




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
    //translateMatrix = translate(-1.08f, 1.0f, -3.0f);
    translateMatrix = translate(0.0f, 0.0f, -3.0f);
    scaleMatrix = scale(sphereScale, sphereScale, sphereScale);

    modelMatrix = translateMatrix * scaleMatrix;

    projectionMatrix = perspectiveProjectionMatrix;

    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

    drawSphere();





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

    drawSphere();




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

    drawSphere();



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

    drawSphere();




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

    drawSphere();








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

    drawSphere();


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

    drawSphere();




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

    drawSphere();




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

    drawSphere();



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

    drawSphere();




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

    drawSphere();


    

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

    drawSphere();




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


    drawSphere();





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

    drawSphere();




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


    drawSphere();



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

    drawSphere();





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

    drawSphere();




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

    drawSphere();




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

    drawSphere();




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

    drawSphere();




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

    drawSphere();


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


    drawSphere();



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


    drawSphere();

}

-(void)myUpdate
{
    //code
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

-(void)uninitialize
{
    //code
    deallocate();

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
}

-(BOOL)acceptsFirstResponder
{
    //code
    [[self window]makeFirstResponder : self];
    
    return(YES);
}

-(void)keyDown : (NSEvent *)theEvent
{
    //code
    int key = [[theEvent characters]characterAtIndex : 0];
    
    switch(key)
    {
        case 27:
            [self release];
            [NSApp terminate : self];
            break;
            
        case 'F':
        case 'f':
            [[self window]toggleFullScreen : self];
            break;
            
        case 'L':
        case 'l':
            if (bLight == true)
                bLight = false;
            else
                bLight = true;
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

    }
}

-(void)mouseDown : (NSEvent *)theEvent
{
    //code
    
}

-(void)rightMouseDown : (NSEvent *)theEvent
{
    //code
   
}

-(void)otherMouseDown : (NSEvent *)theEvent
{
    //code
    
}

-(void)dealloc
{
    //code
    
    CVDisplayLinkStop(displayLink);
    CVDisplayLinkRelease(displayLink);
    
    //opengl uninit()
    [self uninitialize];
    
    [super dealloc];
}

@end

CVReturn myDisplayLinkCallback(CVDisplayLinkRef displayLink, const CVTimeStamp* now, const CVTimeStamp* outputTime, CVOptionFlags flagsIn, CVOptionFlags *flagsOut, void* displayLinkContext)
{
    //code
    CVReturn result = [(MyOpenGLView *)displayLinkContext getFrameForTime : outputTime];
    
    return(result);
}




