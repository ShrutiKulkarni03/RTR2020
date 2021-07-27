#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h> //analogous to windows.h
#import <quartzCore/CVDisplayLink.h>
#import <OpenGL/GL3.h>  //gl.h for FFP

#include "vmath.h"

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
    //PROPERTIES OF VERTEX:
    enum
    {
        SPK_ATTRIBUTE_POSITION = 0,
        SPK_ATTRIBUTE_COLOR,
        SPK_ATTRIBUTE_NORMAL,
        SPK_ATTRIBUTE_TEXCOORD,
    };
    
    //global variables for opengl
    GLuint shaderProgramObject;
    GLuint vertexShaderObject;
    GLuint fragmentShaderObject;

    GLuint vao;                          //vertex array object
    GLuint vbo_position;                 //vertex buffer object
    GLuint mvpMatrixUniform;

    vmath::mat4 orthographicProjectionMatrix;   //4x4 matrix

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
        "in vec4 vPosition;" \
        "uniform mat4 u_mvpMatrix;" \
        "void main(void)" \
        "{" \
        "gl_Position = u_mvpMatrix * vPosition;" \
        "}";

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
        "out vec4 FragColor;" \
        "void main(void)" \
        "{" \
        "FragColor = vec4(1.0, 1.0, 1.0, 1.0);" \
        "}";

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

    //get MVP uniform location
    mvpMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_mvpMatrix");


    //vertices array declaration
    const GLfloat triangleVertices[] = { 0.0f, 50.0f, 0.0f,
                                        -50.0f, -50.0f, 0.0f,
                                        50.0f, -50.0f, 0.0f };


    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo_position);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(SPK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(SPK_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glBindVertexArray(0);

    //depth
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    
    //set orthographic matrix to identity matrix
    orthographicProjectionMatrix = vmath::mat4::identity();
    
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
    
    if (rect.size.width <= rect.size.height)
    {
        orthographicProjectionMatrix = vmath::ortho(-100.0f,    //field of view 'y' FOVY
                                            100.0f,        //field of view 'x' FOVX
                                            -100.0f * ((GLfloat)rect.size.height / (GLfloat)rect.size.width),
                                            100.0f * ((GLfloat)rect.size.height / (GLfloat)rect.size.width),
                                            -100.0f,    //near
                                            100.0f);
    }
    else
    {
        orthographicProjectionMatrix = vmath::ortho(-100.0f * ((GLfloat)rect.size.width / (GLfloat)rect.size.height),
                                            100.0f * ((GLfloat)rect.size.width / (GLfloat)rect.size.height),
                                            -100.0f,
                                            100.0f,
                                            -100.0f,
                                            100.0f);
    }
    
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

    //OpenGL Drawing

    vmath::mat4 modelViewMatrix;
    vmath::mat4 modelViewProjectionMatrix;

    modelViewMatrix = vmath::mat4::identity();
    modelViewProjectionMatrix = vmath::mat4::identity();

    modelViewProjectionMatrix = orthographicProjectionMatrix * modelViewMatrix;  //pre-multiplication of matrices

    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

    //bind vao
    glBindVertexArray(vao);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    //unbind vao
    glBindVertexArray(0);

    //stop using OpenGL program object
    glUseProgram(0);

    //update func()
    
    
    CGLFlushDrawable((CGLContextObj)[[self openGLContext]CGLContextObj]);
    CGLUnlockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
}

-(void)uninitialize
{
    //code
    if (vao)
    {
        glDeleteVertexArrays(1, &vao);
        vao = 0;
    }

    if (vbo_position)
    {
        glDeleteBuffers(1, &vbo_position);
        vbo_position = 0;
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




