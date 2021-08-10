//
//  MyView.m
//  Window
//
//  Created by Shruti Kulkarni on 03/07/21.
//

#import <OpenGLES/ES3/gl.h>
#import <OpenGLES/ES3/glext.h>

#import "GLESView.h"

#import "vmath.h"
using namespace vmath;

#define CHECK_IMAGE_WIDTH 64
#define CHECK_IMAGE_HEIGHT 64

enum
{
    SPK_ATTRIBUTE_POSITION = 0,
    SPK_ATTRIBUTE_COLOR,
    SPK_ATTRIBUTE_NORMAL,
    SPK_ATTRIBUTE_TEXCOORD,
};

@implementation GLESView
{
    @private
    EAGLContext *eaglContext;
    
    GLuint defaultFramebuffer;
    GLuint colorRenderbuffer;
    GLuint depthRenderbuffer;
    
    id displayLink;
    NSInteger animationFrameInterval;
    bool isAnimating;
    
    //global variables for opengl (field variables)
    GLuint shaderProgramObject;
    GLuint vertexShaderObject;
    GLuint fragmentShaderObject;

    //vao for square
    GLuint vao_square;
    GLuint vbo_position_square;
    GLuint vbo_texture_square;

    GLuint mvpMatrixUniform;
    GLuint textureSamplerUniform;

    //texture variables

    GLfloat square_vertices[12];

    GLubyte CheckImage[CHECK_IMAGE_HEIGHT][CHECK_IMAGE_WIDTH][4];
    GLuint TexImage;

    vmath::mat4 perspectiveProjectionMatrix;   //4x4 matrix

}

-(id)initWithFrame:(CGRect)frame
{
    //code
    self = [super initWithFrame:frame];
    
    if(self)
    {
        CAEAGLLayer *eaglLayer = (CAEAGLLayer *)[super layer];
        [eaglLayer setOpaque:YES];
        [eaglLayer setDrawableProperties:[NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking,
                                           kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil]];
        
        eaglContext = [[EAGLContext alloc]initWithAPI:kEAGLRenderingAPIOpenGLES3];
        if(eaglContext == nil)
        {
            printf("OpenGL-ES context creation failed.\n");
            return(nil);
        }
        
        [EAGLContext setCurrentContext:eaglContext];
        
        //render to texture:
        glGenFramebuffers(1, &defaultFramebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
        
        glGenRenderbuffers(1, &colorRenderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
        [eaglContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:eaglLayer];
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderbuffer);
        
        GLint backingWidth;
        GLint backingHeight;
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &backingWidth);
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &backingHeight);
        
        //for depth buffer
        glGenRenderbuffers(1, &depthRenderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, backingWidth, backingHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);
        
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            printf("Framebuffer is not complete.\n");
            [self uninitialize];
            return(nil);
        }
        
        printf("%s\n", glGetString(GL_RENDERER));
        printf("%s\n", glGetString(GL_VERSION));
        printf("%s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
        
        animationFrameInterval = 60; //it's default from iOS 8.2
        isAnimating = NO;
        
        //opengl code
        
        /*****VERTEX SHADER*****/

        //create shader
        vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

        //provide source code to vertex shader
        const GLchar *vertexShaderSourceCode =
            "#version 300 es \n" \
            "\n" \
            "in vec4 vPosition; \n" \
            "in vec2 vTexcoord; \n" \
            "uniform mat4 u_mvpMatrix; \n" \
            "out vec2 out_texcoord; \n" \
            "void main(void) \n" \
            "{ \n" \
            "   gl_Position = u_mvpMatrix * vPosition; \n" \
            "   out_texcoord = vTexcoord; \n" \
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
                    printf("\nVertex Shader Compilation Log : %s\n", szBuffer);
                    free(szBuffer);
                    [self release];
                    [self uninitialize];
                }

            }
        }


        /*****FRAGMENT SHADER*****/

        //create shader
        fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

        //provide source code to fragment shader
        const GLchar* fragmentShaderSourceCode =
            "#version 300 es \n" \
            "\n" \
            "precision highp float; \n" \
            "in vec2 out_texcoord; \n" \
            "uniform sampler2D u_texture_sampler; \n" \
            "out vec4 FragColor; \n" \
            "void main(void) \n" \
            "{ \n" \
            "   FragColor = texture(u_texture_sampler, out_texcoord); \n" \
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
                    printf("\nFragment Shader Compilation Log : %s\n", szBuffer);
                    free(szBuffer);
                    [self release];
                    [self uninitialize];
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
                    printf("\nShader Program Link Log : %s\n", szBuffer);
                    free(szBuffer);
                    [self release];
                    [self uninitialize];
                }
            }
        }

        //get MVP uniform location
        mvpMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_mvpMatrix");
        textureSamplerUniform = glGetUniformLocation(shaderProgramObject, "u_texture_sampler");

        //vertices array declaration


        //SQUARE
        //texcoords
        const GLfloat squareTexcoords[] = { 0.0f, 0.0f,        //front
                                            0.0f, 1.0f,
                                            1.0f, 1.0f,
                                            1.0f, 0.0f };
        
        /*****SQUARE VAO*****/

        glGenVertexArrays(1, &vao_square);
        glBindVertexArray(vao_square);

        //position vbo

        glGenBuffers(1, &vbo_position_square);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_position_square);
        glBufferData(GL_ARRAY_BUFFER, 4*3*sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(SPK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(SPK_ATTRIBUTE_POSITION);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        //texcoords vbo

        glGenBuffers(1, &vbo_texture_square);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_texture_square);
        glBufferData(GL_ARRAY_BUFFER, sizeof(squareTexcoords), squareTexcoords, GL_STATIC_DRAW);
        glVertexAttribPointer(SPK_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(SPK_ATTRIBUTE_TEXCOORD);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);
        
        //depth
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        //glEnable(GL_CULL_FACE);
        
        //variable declarations
        TexImage = 0;
        glEnable(GL_TEXTURE_2D);
        [self loadGLTexture];
        
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        
        //set perspective matrix to identity matrix
        perspectiveProjectionMatrix = vmath::mat4::identity();
        
        //gestures
        UITapGestureRecognizer *singleTapGestureRecognizer = [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(onSingleTap:)];
        [singleTapGestureRecognizer setNumberOfTapsRequired:1];
        [singleTapGestureRecognizer setNumberOfTouchesRequired:1];
        [singleTapGestureRecognizer setDelegate:self];
        [self addGestureRecognizer:singleTapGestureRecognizer];
        
        UITapGestureRecognizer *doubleTapGestureRecognizer = [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(onDoubleTap:)];
        [doubleTapGestureRecognizer setNumberOfTapsRequired:2];
        [doubleTapGestureRecognizer setNumberOfTouchesRequired:1];
        [doubleTapGestureRecognizer setDelegate:self];
        [self addGestureRecognizer:doubleTapGestureRecognizer];
        
        [singleTapGestureRecognizer requireGestureRecognizerToFail:doubleTapGestureRecognizer];
        
        UISwipeGestureRecognizer *swipeGestureRecognizer = [[UISwipeGestureRecognizer alloc]initWithTarget:self action:@selector(onSwipe:)];
        [swipeGestureRecognizer setDelegate:self];
        [self addGestureRecognizer:swipeGestureRecognizer];
        
        UILongPressGestureRecognizer *longPressGestureRecognizer = [[UILongPressGestureRecognizer alloc]initWithTarget:self action:@selector(onLongPress:)];
        [longPressGestureRecognizer setDelegate:self];
        [self addGestureRecognizer:longPressGestureRecognizer];
        
    }
    
    return(self);
}

+(Class)layerClass
{
    //code
    return([CAEAGLLayer class]);
}

-(void)loadGLTexture
{
    [self makeCheckImage];
    
    glGenTextures(1, &TexImage);

    glBindTexture(GL_TEXTURE_2D, TexImage);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECK_IMAGE_WIDTH, CHECK_IMAGE_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, CheckImage);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
}

-(void)makeCheckImage
{
    int i, j, c;

    for (i = 0; i < CHECK_IMAGE_HEIGHT; i++)
    {
        for (j = 0; j < CHECK_IMAGE_WIDTH; j++)
        {
            c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0)) * 255;

            CheckImage[i][j][0] = (GLubyte)c;
            CheckImage[i][j][1] = (GLubyte)c;
            CheckImage[i][j][2] = (GLubyte)c;
            CheckImage[i][j][3] = (GLubyte)255;

        }
    }
}

/*-(void)drawRect:(CGRect)rect
{
    // Drawing code
    
}*/

-(void)layoutSubviews
{
    //code
    
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
    [eaglContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer *)[self layer]];
   
    GLint width;
    GLint height;
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &width);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &height);
    
    //for depth buffer
    glGenRenderbuffers(1, &depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);
    
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Resize-Framebuffer is not complete.\n");
    }
    
    if(height < 0)
        height = 1;
    
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    
    perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
    
    
    [self drawView:nil];
}

-(void)drawView:(id)sender
{
    //code
    [EAGLContext setCurrentContext:eaglContext];
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //start using OpenGL program object
    glUseProgram(shaderProgramObject);

    //OpenGL Drawing

    mat4 modelViewMatrix;
    mat4 modelViewProjectionMatrix;
    mat4 translateMatrix;

    
    //SQUARE TRANSFORMATION

    translateMatrix = mat4::identity();
    modelViewMatrix = mat4::identity();
    modelViewProjectionMatrix = mat4::identity();

    translateMatrix = translate(0.0f, 0.0f, -4.0f);

    modelViewMatrix = translateMatrix;

    modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, TexImage);
    glUniform1i(textureSamplerUniform, 0);
    

    //bind vao_square
    glBindVertexArray(vao_square);
    
    //front

    square_vertices[0] = -2.0f;
    square_vertices[1] = -1.0f;
    square_vertices[2] = 0.0f;
    square_vertices[3] = -2.0f;
    square_vertices[4] = 1.0f;
    square_vertices[5] = 0.0f;
    square_vertices[6] = 0.0f;
    square_vertices[7] = 1.0f;
    square_vertices[8] = 0.0f;
    square_vertices[9] = 0.0f;
    square_vertices[10] = -1.0f;
    square_vertices[11] = 0.0f;

    glBindBuffer(GL_ARRAY_BUFFER, vbo_position_square);
    glBufferData(GL_ARRAY_BUFFER, sizeof(square_vertices), square_vertices, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(SPK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(SPK_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    
    //angular
    
    square_vertices[0] = 1.0f;
    square_vertices[1] = -1.0f;
    square_vertices[2] = 0.0f;
    square_vertices[3] = 1.0f;
    square_vertices[4] = 1.0f;
    square_vertices[5] = 0.0f;
    square_vertices[6] = 2.41421f;
    square_vertices[7] = 1.0f;
    square_vertices[8] = -1.41421f;
    square_vertices[9] = 2.41421f;
    square_vertices[10] = -1.0f;
    square_vertices[11] = -1.41421f;

    glBindBuffer(GL_ARRAY_BUFFER, vbo_position_square);
    glBufferData(GL_ARRAY_BUFFER, sizeof(square_vertices), square_vertices, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(SPK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(SPK_ATTRIBUTE_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    //unbind vao_rectangle
    glBindVertexArray(0);

    //stop using OpenGL program object
    glUseProgram(0);

    //update func()
    [self myUpdate];
    
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
    [eaglContext presentRenderbuffer:GL_RENDERBUFFER]; //swapbuffers
}

-(void)myUpdate
{
    //code
}

-(void)startAnimation
{
    //code
    if(isAnimating == NO)
    {
        displayLink = [NSClassFromString(@"CADisplayLink") displayLinkWithTarget:self selector:@selector(drawView:)];
        [displayLink setPreferredFramesPerSecond:animationFrameInterval];
        [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        
        isAnimating = YES;
    }
}

-(void)stopAnimation
{
    //code
    if(isAnimating == YES)
    {
        [displayLink invalidate];
        displayLink = nil;
        
        isAnimating = NO;
    }
}

-(void)onSingleTap:(UITapGestureRecognizer *)gr
{
    //code
}

-(void)onDoubleTap:(UITapGestureRecognizer *)gr
{
    //code
}

-(void)onSwipe:(UISwipeGestureRecognizer *)gr
{
    //code
    [self uninitialize];
    [self release];
    exit(0);
}

-(void)onLongPress:(UILongPressGestureRecognizer *)gr
{
    //code
}

-(void)uninitialize
{
    //code
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
    
    if(depthRenderbuffer)
    {
        glDeleteRenderbuffers(1, &depthRenderbuffer);
        depthRenderbuffer = 0;
    }
    
    if(colorRenderbuffer)
    {
        glDeleteRenderbuffers(1, &colorRenderbuffer);
        colorRenderbuffer = 0;
    }
    
    if(defaultFramebuffer)
    {
        glDeleteFramebuffers(1, &defaultFramebuffer);
    }
    
    if(eaglContext)
    {
        if([EAGLContext currentContext] == eaglContext)
        {
            [EAGLContext setCurrentContext:nil];
            [eaglContext release];
            eaglContext = nil;
        }
    }
}

-(void)dealloc
{
    //code
    [self uninitialize];
    [super dealloc];
}

@end
