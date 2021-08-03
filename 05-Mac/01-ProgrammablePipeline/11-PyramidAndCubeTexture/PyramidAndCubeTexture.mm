#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h> //analogous to windows.h
#import <quartzCore/CVDisplayLink.h>
#import <OpenGL/GL3.h>  //gl.h for FFP

#include "vmath.h"
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

    //vao for pyramid
    GLuint vao_pyramid;                          //vertex array object
    GLuint vbo_position_pyramid;                 //vertex buffer object
    GLuint vbo_texture_pyramid;

    //vao for cube
    GLuint vao_cube;
    GLuint vbo_position_cube;
    GLuint vbo_texture_cube;
    
    GLuint mvpMatrixUniform;
    GLuint textureSamplerUniform;

    vmath::mat4 perspectiveProjectionMatrix;   //4x4 matrix
    
    GLuint pyramid_texture;
    GLuint cube_texture;

    GLfloat pyramidAngle;
    GLfloat cubeAngle;

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
                [self release];
                [NSApp terminate : self];
            }
        }
    }

    //get MVP uniform location
    mvpMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_mvpMatrix");
    textureSamplerUniform = glGetUniformLocation(shaderProgramObject, "u_texture_sampler");


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

    //depth
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    //glEnable(GL_CULL_FACE);
    
    //load texture
    pyramid_texture = [self loadGLTextureFromBMP : "Stone.bmp"];
    if(pyramid_texture == 0)
    {
        fprintf(gpFile, "\nPyramid texture cannot be obtained\n");
        [self release];
        [NSApp terminate : self];
        
    }
    
    cube_texture = [self loadGLTextureFromBMP : "Vijay_Kundali.bmp"];
    if(cube_texture == 0)
    {
        fprintf(gpFile, "\nCube texture cannot be obtained\n");
        [self release];
        [NSApp terminate : self];
        
    }
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    //set perspective matrix to identity matrix
    perspectiveProjectionMatrix = vmath::mat4::identity();
    
    //variables initialization
    pyramidAngle = 0.0f;
    cubeAngle = 0.0f;
    
    //CV and CGL related code
    CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
    CVDisplayLinkSetOutputCallback(displayLink, &myDisplayLinkCallback, self);
    CGLContextObj cglContext = (CGLContextObj)[[self openGLContext]CGLContextObj];
    CGLPixelFormatObj cglPixelFormat = (CGLPixelFormatObj)[[self pixelFormat]CGLPixelFormatObj];
    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink, cglContext, cglPixelFormat);
    CVDisplayLinkStart(displayLink);
}

-(GLuint)loadGLTextureFromBMP : (const char *)imageFileName
{
    //code
    
    //get NSImage representaion of image file
    NSBundle *appBundle = [NSBundle mainBundle];
    NSString *appDirPath = [appBundle bundlePath];
    NSString *parentDirPath = [appDirPath stringByDeletingLastPathComponent];
    NSString *imageFileNameWithPath = [NSString stringWithFormat : @"%@/%s", parentDirPath, imageFileName];
    NSImage *bmpImage = [[NSImage alloc]initWithContentsOfFile : imageFileNameWithPath];
    
    //error checking
    if(!bmpImage)
    {
        fprintf(gpFile, "\nNSImage conversion of image file failed\n");
        return(0);
    }
    
    //get CGImage representaion of NSImage
    CGImageRef cgImage = [bmpImage CGImageForProposedRect : nil context : nil hints : nil];
    
    //get width and height of CGImage representation on image
    int imageWidth = (int)CGImageGetWidth(cgImage);
    int imageHeight = (int)CGImageGetHeight(cgImage);
    
    //get CFData represention
    CFDataRef imageData = CGDataProviderCopyData(CGImageGetDataProvider(cgImage));
    
    //convert CFDataRef format data inti void * generic format
    const UInt8* pixels = CFDataGetBytePtr(imageData);  // there is pointer in pixels and not the data
    
    GLuint bitmapTexture;
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &bitmapTexture);
    glBindTexture(GL_TEXTURE_2D, bitmapTexture);

    //setting texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    CFRelease(imageData);
    
    return(bitmapTexture);
    
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

    translateMatrix = translate(-1.0f, 0.0f, -3.0f);
    rotateYMatrix = rotate(pyramidAngle, 0.0f, 1.0f, 0.0f);

    modelViewMatrix = translateMatrix * rotateYMatrix;

    modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;  //pre-multiplication of matrices

    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, pyramid_texture);
    glUniform1i(textureSamplerUniform, 0);

    //bind vao_pyramid
    glBindVertexArray(vao_pyramid);

    glDrawArrays(GL_TRIANGLES, 0, 12);

    //unbind vao_triangle
    glBindVertexArray(0);

    
    //CUBE TRANSFORMATION

    translateMatrix = mat4::identity();
    scaleMatrix = mat4::identity();
    rotateXMatrix = mat4::identity();
    rotateYMatrix = mat4::identity();
    rotateZMatrix = mat4::identity();
    modelViewMatrix = mat4::identity();
    modelViewProjectionMatrix = mat4::identity();

    translateMatrix = translate(1.0f, 0.0f, -3.0f);
    scaleMatrix = scale(0.45f, 0.45f, 0.45f);
    rotateXMatrix = rotate(cubeAngle, 1.0f, 0.0f, 0.0f);
    rotateYMatrix = rotate(cubeAngle, 0.0f, 1.0f, 0.0f);
    rotateZMatrix = rotate(cubeAngle, 0.0f, 0.0f, 1.0f);


    modelViewMatrix = translateMatrix * scaleMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix;

    modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, cube_texture);
    glUniform1i(textureSamplerUniform, 0);

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

    //update func()
    [self myUpdate];
    
    CGLFlushDrawable((CGLContextObj)[[self openGLContext]CGLContextObj]);
    CGLUnlockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
}

-(void)myUpdate
{
    //code
    pyramidAngle += 1.0f;
    if (pyramidAngle >= 360.0f)
        pyramidAngle = 0.0f;

    cubeAngle -= 1.0f;
    if (cubeAngle <= -360.0f)
        cubeAngle = 0.0f;
    
}

-(void)uninitialize
{
    //code
    
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

    //texture
    glDeleteTextures(1, &pyramid_texture);
    glDeleteTextures(1, &cube_texture);

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




