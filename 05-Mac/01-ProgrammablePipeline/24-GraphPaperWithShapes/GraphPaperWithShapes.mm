#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h> //analogous to windows.h
#import <quartzCore/CVDisplayLink.h>
#import <OpenGL/GL3.h>  //gl.h for FFP

#include "vmath.h"
#include "CircleVector.h"

#define PI 3.1415

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

    //vao for line
    GLuint vao_line;
    GLuint vbo_position_line;

    //vao for triangle
    GLuint vao_triangle;                          //vertex array object
    GLuint vbo_position_triangle;                          //vertex buffer object

    //vao for rectangle
    GLuint vao_rectangle;
    GLuint vbo_position_rectangle;

    //vao incircle
    GLuint vao_incircle;
    GLuint vbo_position_incircle;

    //vao excircle
    GLuint vao_excircle;
    GLuint vbo_position_excircle;

    struct vector* incircle;
    struct vector* excircle;

    GLuint in_num_vertices;
    GLuint ex_num_vertices;

    GLuint mvpMatrixUniform;
    mat4 perspectiveProjectionMatrix;   //4x4 matrix

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
        "in vec4 vColor; \n" \
        "uniform mat4 u_mvpMatrix; \n" \
        "out vec4 out_color; \n" \
        "void main(void) \n" \
        "{ \n" \
        "    gl_Position = u_mvpMatrix * vPosition; \n" \
        "    out_color = vColor; \n" \
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
        "in vec4 out_color; \n" \
        "out vec4 FragColor; \n" \
        "void main(void) \n" \
        "{ \n" \
        "    FragColor = out_color; \n" \
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
    glBindAttribLocation(shaderProgramObject, SPK_ATTRIBUTE_COLOR, "vColor");
    
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

    //LINE
    const GLfloat lineVertices[] = { 0.0f, 1.0f, 0.0f,
                                     0.0f, -1.0f, 0.0f };

    //TRIANGLE

    //vertices
    const GLfloat triangleVertices[] = { 0.0f, 0.5f, 0.0f,
                                        -0.5f, -0.5f, 0.0f,
                                         0.5f, -0.5f, 0.0f };

    //RECTANGLE

    //vertices
    const GLfloat rectangleVertices[] = { 0.5f, 0.5f, 0.0f,
                                         -0.5f, 0.5f, 0.0f,
                                         -0.5f, -0.5f, 0.0f,
                                          0.5f, -0.5f, 0.0f };

    //IN-CIRCLE

    GLfloat in_x_coord;
    GLfloat in_y_coord;
    GLfloat in_z_coord;

    GLfloat x1 = 0.0f;
    GLfloat x2 = -0.5f;
    GLfloat x3 = 0.5f;

    GLfloat y1 = 0.5f;
    GLfloat y2 = -0.5f;
    GLfloat y3 = -0.5f;

    GLfloat in_x, in_y;

    GLfloat da, db, dc, a, b, c, in_r, value;
    GLfloat s;

    //calculations for in-circle
    da = ((x2 - x3) * (x2 - x3)) + ((y2 - y3) * (y2 - y3));
    a = sqrt(da);

    db = ((x3 - x1) * (x3 - x1)) + ((y3 - y1) * (y3 - y1));
    b = sqrt(db);

    dc = ((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2));
    c = sqrt(dc);

    in_x = ((a * x1) + (b * x2) + (c * x3)) / (a + b + c);
    in_y = ((a * y1) + (b * y2) + (c * y3)) / (a + b + c);

    s = (a + b + c) / 2;

    value = s * (s - a) * (s - b) * (s - c);

    in_r = (sqrt(value)) / s;

    incircle = createVector();

    for (GLfloat angle = 0.0f; angle <= (2 * PI); angle += 0.1f)
    {
        in_x_coord = in_r * cosf(angle) + in_x;
        in_y_coord = in_r * sinf(angle) + in_y;
        in_z_coord = 0.0f;

        pushBack(incircle, in_x_coord);
        pushBack(incircle, in_y_coord);
        pushBack(incircle, in_z_coord);

        in_num_vertices++;
    }

    //EX-CIRCLE

    GLfloat ex_x_coord;
    GLfloat ex_y_coord;
    GLfloat ex_z_coord;

    GLfloat rect_x1 = -0.5f;
    GLfloat rect_x2 = 0.5f;
    GLfloat rect_y1 = 0.5f;
    GLfloat rect_y2 = -0.5f;

    GLfloat ex_r, diagonal;

    //calculations for excircle
    diagonal = sqrt(((rect_x2 - rect_x1) * (rect_x2 - rect_x1)) + ((rect_y2 - rect_y1) * (rect_y2 - rect_y1)));

    ex_r = diagonal / 2;

    excircle = createVector();
    
    for (GLfloat angle = 0.0f; angle <= (2 * PI); angle += 0.1f)
    {
        ex_x_coord = ex_r * cos(angle);
        ex_y_coord = ex_r * sin(angle);
        ex_z_coord = 0.0f;

        pushBack(excircle, ex_x_coord);
        pushBack(excircle, ex_y_coord);
        pushBack(excircle, ex_z_coord);

        ex_num_vertices++;
    }
    fprintf(gpFile, "\nsize = %d\ninNumVertices = %d\nexNumVertices = %d\n", excircle->size, in_num_vertices, ex_num_vertices);


    /*****LINE VAO*****/

    glGenVertexArrays(1, &vao_line);
    glBindVertexArray(vao_line);

    glGenBuffers(1, &vbo_position_line);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_position_line);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(SPK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(SPK_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glVertexAttrib3f(SPK_ATTRIBUTE_COLOR, 0.0f, 0.0f, 1.0f);

    glBindVertexArray(0);


    /*****TRIANGLE VAO*****/

    glGenVertexArrays(1, &vao_triangle);
    glBindVertexArray(vao_triangle);

    glGenBuffers(1, &vbo_position_triangle);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_position_triangle);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(SPK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(SPK_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glVertexAttrib3f(SPK_ATTRIBUTE_COLOR, 1.0f, 1.0f, 1.0f);

    glBindVertexArray(0);

    /*****RECTANGLE VAO*****/

    glGenVertexArrays(1, &vao_rectangle);
    glBindVertexArray(vao_rectangle);

    glGenBuffers(1, &vbo_position_rectangle);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_position_rectangle);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(SPK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(SPK_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glVertexAttrib3f(SPK_ATTRIBUTE_COLOR, 1.0f, 1.0f, 1.0f);

    glBindVertexArray(0);

    /*****INCIRCLE VAO*****/

    glGenVertexArrays(1, &vao_incircle);
    glBindVertexArray(vao_incircle);

    glGenBuffers(1, &vbo_position_incircle);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_position_incircle);
    glBufferData(GL_ARRAY_BUFFER, in_num_vertices * sizeof(float) * 3, incircle->vertArray, GL_STATIC_DRAW);
    glVertexAttribPointer(SPK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(SPK_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glVertexAttrib3f(SPK_ATTRIBUTE_COLOR, 1.0f, 1.0f, 1.0f);

    glBindVertexArray(0);

    /*****EXCIRCLE VAO*****/

    glGenVertexArrays(1, &vao_excircle);
    glBindVertexArray(vao_excircle);

    glGenBuffers(1, &vbo_position_excircle);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_position_excircle);
    glBufferData(GL_ARRAY_BUFFER, ex_num_vertices * sizeof(float) * 3, excircle->vertArray, GL_STATIC_DRAW);
    glVertexAttribPointer(SPK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(SPK_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glVertexAttrib3f(SPK_ATTRIBUTE_COLOR, 1.0f, 1.0f, 1.0f);

    glBindVertexArray(0);


    //depth
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    //variable declarations
    
    GLuint in_num_vertices = 0;
    GLuint ex_num_vertices = 0;
    
    //set perspective matrix to identity matrix
    perspectiveProjectionMatrix = vmath::mat4::identity();
    
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

    //OpenGL Drawing
    //OpenGL Drawing

    mat4 translateMatrix;
    mat4 rotateZMatrix;
    mat4 modelViewMatrix;
    mat4 modelViewProjectionMatrix;

    //VERTICAL LINES TRANSFORMATION

    translateMatrix = mat4::identity();
    modelViewMatrix = mat4::identity();
    modelViewProjectionMatrix = mat4::identity();

    for (GLfloat x = -1.0f; x <= 1.05f; x += 0.05f)
    {
        translateMatrix = translate(x, 0.0f, -3.0f);

        modelViewMatrix = translateMatrix;

        modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

        glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

        //bind vao_rectangle
        glBindVertexArray(vao_line);

        glVertexAttrib3f(SPK_ATTRIBUTE_COLOR, 0.0f, 0.0f, 1.0f);
        glDrawArrays(GL_LINES, 0, 2);
        glVertexAttrib3f(SPK_ATTRIBUTE_COLOR, 1.0f, 1.0f, 1.0f);

        //unbind vao_rectangle
        glBindVertexArray(0);
    }

    // HORIZONTAL LINES TRANSFORMATION

    translateMatrix = mat4::identity();
    rotateZMatrix = mat4::identity();
    modelViewMatrix = mat4::identity();
    modelViewProjectionMatrix = mat4::identity();

    for (GLfloat y = -1.0f; y <= 1.05f; y += 0.05f)
    {
        translateMatrix = translate(0.0f, y, -3.0f);
        rotateZMatrix = rotate(90.0f, 0.0f, 0.0f, 1.0f);

        modelViewMatrix = translateMatrix * rotateZMatrix;

        modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

        glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

        //bind vao_rectangle
        glBindVertexArray(vao_line);

        glVertexAttrib3f(SPK_ATTRIBUTE_COLOR, 0.0f, 0.0f, 1.0f);
        glDrawArrays(GL_LINES, 0, 2);
        glVertexAttrib3f(SPK_ATTRIBUTE_COLOR, 1.0f, 1.0f, 1.0f);

        //unbind vao_rectangle
        glBindVertexArray(0);
    }

    // X AXIS TRANSFORMATION

    translateMatrix = mat4::identity();
    rotateZMatrix = mat4::identity();
    modelViewMatrix = mat4::identity();
    modelViewProjectionMatrix = mat4::identity();

    translateMatrix = translate(0.0f, 0.0f, -3.0f);
    rotateZMatrix = rotate(90.0f, 0.0f, 0.0f, 1.0f);

    modelViewMatrix = translateMatrix * rotateZMatrix;

    modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

    //bind vao_rectangle
    glBindVertexArray(vao_line);

    glVertexAttrib3f(SPK_ATTRIBUTE_COLOR, 1.0f, 0.0f, 0.0f);
    glDrawArrays(GL_LINES, 0, 2);
    glVertexAttrib3f(SPK_ATTRIBUTE_COLOR, 1.0f, 1.0f, 1.0f);

    //unbind vao_rectangle
    glBindVertexArray(0);




    // Y AXIS TRANSFORMATION

    translateMatrix = mat4::identity();
    modelViewMatrix = mat4::identity();
    modelViewProjectionMatrix = mat4::identity();

    translateMatrix = translate(0.0f, 0.0f, -3.0f);

    modelViewMatrix = translateMatrix;

    modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

    //bind vao_rectangle
    glBindVertexArray(vao_line);

    glVertexAttrib3f(SPK_ATTRIBUTE_COLOR, 0.0f, 1.0f, 0.0f);
    glDrawArrays(GL_LINES, 0, 2);
    glVertexAttrib3f(SPK_ATTRIBUTE_COLOR, 1.0f, 1.0f, 1.0f);

    //unbind vao_rectangle
    glBindVertexArray(0);


    

    //TRIANGLE TRANSFORMATION

    translateMatrix = mat4::identity();
    modelViewMatrix = mat4::identity();
    modelViewProjectionMatrix = mat4::identity();

    translateMatrix = translate(0.0f, 0.0f, -3.0f);
    
    modelViewMatrix = translateMatrix;

    modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;  //pre-multiplication of matrices

    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

    //bind vao_triangle
    glBindVertexArray(vao_triangle);

    glDrawArrays(GL_LINE_LOOP, 0, 3);

    //unbind vao_triangle
    glBindVertexArray(0);



    //RECTANGLE TRANSFORMATION

    translateMatrix = mat4::identity();
    modelViewMatrix = mat4::identity();
    modelViewProjectionMatrix = mat4::identity();

    translateMatrix = translate(0.0f, 0.0f, -3.0f);

    modelViewMatrix = translateMatrix;

    modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

    //bind vao_rectangle
    glBindVertexArray(vao_rectangle);

    glDrawArrays(GL_LINE_LOOP, 0, 4);

    //unbind vao_rectangle
    glBindVertexArray(0);


    //INCIRCLE TRANSFORMATION

    translateMatrix = mat4::identity();
    modelViewMatrix = mat4::identity();
    modelViewProjectionMatrix = mat4::identity();

    translateMatrix = translate(0.0f, 0.0f, -3.0f);

    modelViewMatrix = translateMatrix;

    modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

    //bind vao_rectangle
    glBindVertexArray(vao_incircle);

    glDrawArrays(GL_LINE_LOOP, 0, in_num_vertices);

    //unbind vao_rectangle
    glBindVertexArray(0);


    //EXCIRCLE TRANSFORMATION

    translateMatrix = mat4::identity();
    modelViewMatrix = mat4::identity();
    modelViewProjectionMatrix = mat4::identity();

    translateMatrix = translate(0.0f, 0.0f, -3.0f);

    modelViewMatrix = translateMatrix;

    modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

    //bind vao_rectangle
    glBindVertexArray(vao_excircle);

    glDrawArrays(GL_LINE_LOOP, 0, ex_num_vertices);

    //unbind vao_rectangle
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
    //line

    if (vao_line)
    {
        glDeleteVertexArrays(1, &vao_line);
        vao_line = 0;
    }

    if (vbo_position_line)
    {
        glDeleteBuffers(1, &vbo_position_line);
        vbo_position_line = 0;
    }

    //triangle

    if (vao_triangle)
    {
        glDeleteVertexArrays(1, &vao_triangle);
        vao_triangle = 0;
    }

    if (vbo_position_triangle)
    {
        glDeleteBuffers(1, &vbo_position_triangle);
        vbo_position_triangle = 0;
    }

    //rectangle

    if (vao_rectangle)
    {
        glDeleteVertexArrays(1, &vao_rectangle);
        vao_rectangle = 0;
    }

    if (vbo_position_rectangle)
    {
        glDeleteBuffers(1, &vbo_position_rectangle);
        vbo_position_rectangle = 0;
    }

    //circle
    if (vao_incircle)
    {
        glDeleteVertexArrays(1, &vao_incircle);
        vao_incircle = 0;
    }

    if (vbo_position_incircle)
    {
        glDeleteBuffers(1, &vbo_position_incircle);
        vbo_position_incircle = 0;
    }

    if (vao_excircle)
    {
        glDeleteVertexArrays(1, &vao_excircle);
        vao_excircle = 0;
    }

    if (vbo_position_excircle)
    {
        glDeleteBuffers(1, &vbo_position_excircle);
        vbo_position_excircle = 0;
    }

    destroyVector(incircle);
    destroyVector(excircle);

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




