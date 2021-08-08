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

    GLuint lAUniform;
    GLuint lDUniform;
    GLuint lSUniform;
    GLuint lightPositionUniform;

    GLuint kAUniform;
    GLuint kDUniform;
    GLuint kSUniform;
    GLfloat kShininessUniform;
    
    bool bLight;

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
        "void main(void) \n" \
        "{ \n" \
        "   if(u_lKeyPressed==1) \n" \
        "   { \n" \
        "       vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition; \n" \
        "       transformed_normal = mat3(u_view_matrix * u_model_matrix) * vNormal; \n" \
        "       light_direction = vec3(u_light_position - eye_coordinates); \n" \
        "       view_vector = -eye_coordinates.xyz; \n" \
        "   } \n" \
        "   gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition; \n" \
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
        "       FragColor = vec4(phong_ads_light, 1.0f); \n" \
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
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    //set perspective matrix to identity matrix
    perspectiveProjectionMatrix = vmath::mat4::identity();
    
    //variables initialization
    
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
    NSRect rect = [self bounds];
    
    if(rect.size.height<0)
        rect.size.height = 1;
    
    glViewport(0, 0, (GLsizei)rect.size.width, (GLsizei)rect.size.height);
    
    perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)rect.size.width / (GLfloat)rect.size.height, 0.1f, 100.0f);
    
    CGLUnlockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
    
}

-(void)drawRect : (NSRect)dirtyRect
{
    //code
    [self drawView];
}

-(void)drawView
{
    //code
    [[self openGLContext]makeCurrentContext];
    CGLLockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
    
    //opengl code
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //start using OpenGL program object
    glUseProgram(shaderProgramObject);

    if (bLight == true)
    {
        GLfloat lightPosition[] = { 100.0f, 100.0f, 100.0f, 1.0f };

        glUniform1i(LKeyPressedUniform, 1);

        /****GOURAUD****/

        glUniform3f(lAUniform, 0.0f, 0.0f, 0.0f);
        glUniform3f(lDUniform, 1.0f, 1.0f, 1.0f);
        glUniform3f(lSUniform, 1.0f, 1.0f, 1.0f);
        glUniform4fv(lightPositionUniform, 1, (GLfloat*)lightPosition);

        glUniform3f(kAUniform, 0.0f, 0.0f, 0.0f);
        glUniform3f(kDUniform, 1.0f, 1.0f, 1.0f);
        glUniform3f(kSUniform, 1.0f, 1.0f, 1.0f);
        glUniform1f(kShininessUniform, 50.0);
        
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
    translateMatrix = translate(0.0f, 0.0f, -5.0f);
    scaleMatrix = scale(1.5f, 1.5f, 1.5f);

    modelMatrix = translateMatrix * scaleMatrix;

    projectionMatrix = perspectiveProjectionMatrix * viewMatrix * modelMatrix;  //pre-multiplication of matrices
    //projectionMatrix = perspectiveProjectionMatrix;

    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

    drawSphere();

    //stop using OpenGL program object
    glUseProgram(0);

    //update func()
    [self myUpdate];
    
    CGLFlushDrawable((CGLContextObj)[[self openGLContext]CGLContextObj]);
    CGLUnlockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
}

-(void)myUpdate
{
    //code
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




