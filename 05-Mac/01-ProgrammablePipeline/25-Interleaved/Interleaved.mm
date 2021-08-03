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

    //vao for cube
    GLuint vao;
    GLuint vbo;
    
    GLuint modelMatrixUniform;
    GLuint viewMatrixUniform;
    GLuint projectionMatrixUniform;
    
    GLuint LKeyPressedUniform;

    GLuint lAUniform;
    GLuint lDUniform;
    GLuint lSUniform;
    GLuint lightPositionUniform;
    
    GLfloat kShininessUniform;
    GLuint textureSamplerUniform;

    vmath::mat4 perspectiveProjectionMatrix;   //4x4 matrix
    
    GLuint cube_texture;

    GLfloat cubeAngle;
    
    bool bLight;

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
    const GLchar* vertexShaderSourceCode =
        "#version 410 core \n" \
        "\n" \
        "in vec4 vPosition; \n" \
        "in vec4 vColor; \n" \
        "in vec2 vTexcoord; \n" \
        "in vec3 vNormal; \n" \
        "out vec4 out_color; \n" \
        "uniform int u_lKeyPressed; \n" \
        "uniform vec4 u_light_position; \n" \
        "uniform mat4 u_projection_matrix; \n" \
        "uniform mat4 u_model_matrix; \n" \
        "uniform mat4 u_view_matrix; \n" \
        "out vec2 out_texcoord; \n" \
        "out vec3 transformed_normal; \n" \
        "out vec3 light_direction; \n" \
        "out vec3 view_vector; \n" \
        "void main(void) \n" \
        "{ \n" \
        "    if(u_lKeyPressed==1) \n" \
        "    { \n" \
        "        vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition; \n" \
        "        transformed_normal = mat3(u_view_matrix * u_model_matrix) * vNormal; \n" \
        "        light_direction = vec3(u_light_position - eye_coordinates); \n" \
        "        view_vector = -eye_coordinates.xyz; \n" \
        "    } \n" \
        "    gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition; \n" \
        "    out_color = vColor; \n" \
        "    out_texcoord = vTexcoord; \n" \
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
        "#version 410 core \n" \
        "\n" \
        "in vec4 out_color; \n" \
        "in vec3 transformed_normal; \n" \
        "in vec3 light_direction; \n" \
        "in vec3 view_vector; \n" \
        "in vec2 out_texcoord; \n" \
        "uniform sampler2D u_texture_sampler; \n" \
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
        "    if(u_lKeyPressed==1) \n" \
        "    { \n" \
        "        vec3 normalized_transformed_normals = normalize(transformed_normal); \n" \
        "        vec3 normalized_light_direction = normalize(light_direction); \n" \
        "        vec3 normalized_view_vector = normalize(view_vector); \n" \
        "        vec3 reflection_vector = reflect(-normalized_light_direction, normalized_transformed_normals); \n" \
        "        vec3 ambient = u_lA * texture(u_texture_sampler, out_texcoord).rgb; \n" \
        "        vec3 diffuse = u_lD * texture(u_texture_sampler, out_texcoord).rgb * max(dot(normalized_light_direction, normalized_transformed_normals), 0.0); \n" \
        "        vec3 specular = u_lS * pow(max(dot(reflection_vector, normalized_view_vector), 0.0), u_materialShininess); \n" \
        "        phong_ads_light = ambient + diffuse + specular; \n" \
        "    } \n" \
        "    else \n" \
        "    { \n" \
        "        phong_ads_light = vec3(1.0, 1.0, 1.0); \n" \
        "    } \n" \
        "    FragColor = vec4(phong_ads_light, 1.0f) * out_color; \n" \
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
    glBindAttribLocation(shaderProgramObject, SPK_ATTRIBUTE_TEXCOORD, "vTexcoord");
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

    kShininessUniform = glGetUniformLocation(shaderProgramObject, "u_materialShininess");

    textureSamplerUniform = glGetUniformLocation(shaderProgramObject, "u_texture_sampler");


    //vertices array declaration

    //CUBE
                                 //verices              //colors              //normals             //texcoords
    const GLfloat cubeVCNT[] = {  1.0f, 1.0f, 1.0f,     1.0f, 0.0f, 0.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
                                -1.0f, 1.0f, 1.0f,      1.0f, 0.0f, 0.0f,     0.0f, 0.0f, 1.0f,     0.0f, 1.0f,
                                -1.0f, -1.0f, 1.0f,     1.0f, 0.0f, 0.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
                                 1.0f, -1.0f, 1.0f,     1.0f, 0.0f, 0.0f,     0.0f, 0.0f, 1.0f,     1.0f, 0.0f,
                                 1.0f, 1.0f, -1.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f, 0.0f,     0.0f, 1.0f,
                                 1.0f, 1.0f, 1.0f,      0.0f, 1.0f, 0.0f,     1.0f, 0.0f, 0.0f,     1.0f, 1.0f,
                                 1.0f, -1.0f, 1.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f, 0.0f,     1.0f, 0.0f,
                                 1.0f, -1.0f, -1.0f,    0.0f, 1.0f, 0.0f,     1.0f, 0.0f, 0.0f,     0.0f, 0.0f,
                                -1.0f, 1.0f, -1.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f, -1.0f,    0.0f, 0.0f,
                                 1.0f, 1.0f, -1.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f, -1.0f,    1.0f, 0.0f,
                                 1.0f, -1.0f, -1.0f,    0.0f, 0.0f, 1.0f,     0.0f, 0.0f, -1.0f,    1.0f, 1.0f,
                                -1.0f, -1.0f, -1.0f,    0.0f, 0.0f, 1.0f,     0.0f, 0.0f, -1.0f,    0.0f, 1.0f,
                                -1.0f, 1.0f, 1.0f,      0.0f, 1.0f, 1.0f,    -1.0f, 0.0f, 0.0f,     0.0f, 1.0f,
                                -1.0f, 1.0f, -1.0f,     0.0f, 1.0f, 1.0f,    -1.0f, 0.0f, 0.0f,     1.0f, 1.0f,
                                -1.0f, -1.0f, -1.0f,    0.0f, 1.0f, 1.0f,    -1.0f, 0.0f, 0.0f,     1.0f, 0.0f,
                                -1.0f, -1.0f, 1.0f,     0.0f, 1.0f, 1.0f,    -1.0f, 0.0f, 0.0f,     0.0f, 0.0f,
                                 1.0f, 1.0f, -1.0f,     1.0f, 0.0f, 1.0f,     0.0f, 1.0f, 0.0f,     1.0f, 1.0f,
                                -1.0f, 1.0f, -1.0f,     1.0f, 0.0f, 1.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
                                -1.0f, 1.0f, 1.0f,      1.0f, 0.0f, 1.0f,     0.0f, 1.0f, 0.0f,     0.0f, 0.0f,
                                 1.0f, 1.0f, 1.0f,      1.0f, 0.0f, 1.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
                                 1.0f, -1.0f, -1.0f,    1.0f, 1.0f, 0.0f,     0.0f, -1.0f, 0.0f,    1.0f, 1.0f,
                                -1.0f, -1.0f, -1.0f,    1.0f, 1.0f, 0.0f,     0.0f, -1.0f, 0.0f,    0.0f, 1.0f,
                                -1.0f, -1.0f, 1.0f,     1.0f, 1.0f, 0.0f,     0.0f, -1.0f, 0.0f,    0.0f, 0.0f,
                                 1.0f, -1.0f, 1.0f,     1.0f, 1.0f, 0.0f,     0.0f, -1.0f, 0.0f,    1.0f, 0.0f };

    
    //VAO

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 24 * 11 * sizeof(float), cubeVCNT, GL_STATIC_DRAW);
    glVertexAttribPointer(SPK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), 0);
    glEnableVertexAttribArray(SPK_ATTRIBUTE_POSITION);
    glVertexAttribPointer(SPK_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(SPK_ATTRIBUTE_COLOR);
    glVertexAttribPointer(SPK_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(SPK_ATTRIBUTE_NORMAL);
    glVertexAttribPointer(SPK_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(9 * sizeof(float)));
    glEnableVertexAttribArray(SPK_ATTRIBUTE_TEXCOORD);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);


    //depth
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    //glEnable(GL_CULL_FACE);
    
    //load texture
    cube_texture = [self loadGLTextureFromBMP : "marble.bmp"];
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
    cubeAngle = 0.0f;
    bLight = false;
    
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

    if (bLight == true)
    {
        GLfloat lightPosition[] = { 100.0f, 100.0f, 100.0f, 1.0f };

        glUniform1i(LKeyPressedUniform, 1);


        /****ALBEDO****/
        
        glUniform3f(lAUniform, 0.2f, 0.2f, 0.2f);
        glUniform3f(lDUniform, 1.0f, 1.0f, 1.0f);
        glUniform3f(lSUniform, 1.0f, 1.0f, 1.0f);
        glUniform4fv(lightPositionUniform, 1, (GLfloat*)lightPosition);

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
    mat4 rotateXMatrix;
    mat4 rotateYMatrix;
    mat4 rotateZMatrix;
    mat4 scaleMatrix;

    //CUBE TRANSFORMATION

    translateMatrix = mat4::identity();
    scaleMatrix = mat4::identity();
    rotateXMatrix = mat4::identity();
    rotateYMatrix = mat4::identity();
    rotateZMatrix = mat4::identity();
    modelMatrix = mat4::identity();
    viewMatrix = mat4::identity();
    projectionMatrix = mat4::identity();

    translateMatrix = translate(0.0f, 0.0f, -3.0f);
    scaleMatrix = scale(0.5f, 0.5f, 0.5f);
    rotateXMatrix = rotate(cubeAngle, 1.0f, 0.0f, 0.0f);
    rotateYMatrix = rotate(cubeAngle, 0.0f, 1.0f, 0.0f);
    rotateZMatrix = rotate(cubeAngle, 0.0f, 0.0f, 1.0f);

    modelMatrix = translateMatrix * scaleMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix;

    //projectionMatrix = perspectiveProjectionMatrix * viewMatrix * modelMatrix;
    projectionMatrix = perspectiveProjectionMatrix;

    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, cube_texture);
    glUniform1i(textureSamplerUniform, 0);

    //bind vao_rectangle
    glBindVertexArray(vao);

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
    cubeAngle -= 1.0f;
    if (cubeAngle <= -360.0f)
        cubeAngle = 0.0f;
    
}

-(void)uninitialize
{
    //code

    //cube

    if (vao)
    {
        glDeleteVertexArrays(1, &vao);
        vao = 0;
    }

    if (vbo)
    {
        glDeleteBuffers(1, &vbo);
        vbo = 0;
    }


    //texture
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
            
        case 'L':
        case 'l':
            if (bLight == true)
            {
                bLight = false;
            }
            else
            {
                bLight = true;
            }
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




