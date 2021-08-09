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

    //vao for pyramid
    GLuint vao_pyramid;                          //vertex array object
    GLuint vbo_position_pyramid;                 //vertex buffer object
    GLuint vbo_color_pyramid;

    //vao for cube
    GLuint vao_cube;
    GLuint vbo_position_cube;
    GLuint vbo_color_cube;
    
    GLuint mvpMatrixUniform;

    vmath::mat4 perspectiveProjectionMatrix;   //4x4 matrix

    GLfloat pyramidAngle;
    GLfloat cubeAngle;
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
            "in vec4 vColor; \n" \
            "uniform mat4 u_mvpMatrix; \n" \
            "out vec4 out_color; \n" \
            "void main(void) \n" \
            "{ \n" \
            "   gl_Position = u_mvpMatrix * vPosition; \n" \
            "   out_color = vColor; \n" \
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
            "in vec4 out_color; \n" \
            "out vec4 FragColor; \n" \
            "void main(void) \n" \
            "{ \n" \
            "   FragColor = out_color; \n" \
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
                    printf("\nShader Program Link Log : %s\n", szBuffer);
                    free(szBuffer);
                    [self release];
                    [self uninitialize];
                }
            }
        }

        //get MVP uniform location
        mvpMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_mvpMatrix");


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
                                           -0.5f, -0.5f, 0.5f};

        //color
        const GLfloat pyramidColor[] = { 1.0f, 0.0f, 0.0f,      //front
                                         0.0f, 1.0f, 0.0f,
                                         0.0f, 0.0f, 1.0f,
                                         1.0f, 0.0f, 0.0f,      //right
                                         0.0f, 0.0f, 1.0f,
                                         0.0f, 1.0f, 0.0f,
                                         1.0f, 0.0f, 0.0f,      //back
                                         0.0f, 1.0f, 0.0f,
                                         0.0f, 0.0f, 1.0f,
                                         1.0f, 0.0f, 0.0f,      //left
                                         0.0f, 0.0f, 1.0f,
                                         0.0f, 1.0f, 0.0f };


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
        const GLfloat cubeColor[] = { 1.0f, 0.0f, 0.0f,
                                      1.0f, 0.0f, 0.0f,
                                      1.0f, 0.0f, 0.0f,
                                      1.0f, 0.0f, 0.0f,
                                      0.0f, 1.0f, 0.0f,
                                      0.0f, 1.0f, 0.0f,
                                      0.0f, 1.0f, 0.0f,
                                      0.0f, 1.0f, 0.0f,
                                      0.0f, 0.0f, 1.0f,
                                      0.0f, 0.0f, 1.0f,
                                      0.0f, 0.0f, 1.0f,
                                      0.0f, 0.0f, 1.0f,
                                      0.0f, 1.0f, 1.0f,
                                      0.0f, 1.0f, 1.0f,
                                      0.0f, 1.0f, 1.0f,
                                      0.0f, 1.0f, 1.0f,
                                      1.0f, 0.0f, 1.0f,
                                      1.0f, 0.0f, 1.0f,
                                      1.0f, 0.0f, 1.0f,
                                      1.0f, 0.0f, 1.0f,
                                      1.0f, 1.0f, 0.0f,
                                      1.0f, 1.0f, 0.0f,
                                      1.0f, 1.0f, 0.0f,
                                      1.0f, 1.0f, 0.0f };


        /*****PYRAMID VAO*****/

        glGenVertexArrays(1, &vao_pyramid);
        glBindVertexArray(vao_pyramid);

        glGenBuffers(1, &vbo_position_pyramid);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_position_pyramid);
        glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(SPK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(SPK_ATTRIBUTE_POSITION);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glGenBuffers(1, &vbo_color_pyramid);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_color_pyramid);
        glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidColor), pyramidColor, GL_STATIC_DRAW);
        glVertexAttribPointer(SPK_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(SPK_ATTRIBUTE_COLOR);
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

        glGenBuffers(1, &vbo_color_cube);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_color_cube);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeColor), cubeColor, GL_STATIC_DRAW);
        glVertexAttribPointer(SPK_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(SPK_ATTRIBUTE_COLOR);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);

        //depth
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        //glEnable(GL_CULL_FACE);
        
        //variables initialization
        pyramidAngle = 0.0f;
        cubeAngle = 0.0f;
        
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
    vmath::mat4 modelViewMatrix;
    vmath::mat4 modelViewProjectionMatrix;
    vmath::mat4 translateMatrix;
    vmath::mat4 rotateXMatrix;
    vmath::mat4 rotateYMatrix;
    vmath::mat4 rotateZMatrix;
    vmath::mat4 scaleMatrix;
    
    
    //PYRAMID TRANSFORMATION
    translateMatrix = vmath::mat4::identity();
    rotateYMatrix = vmath::mat4::identity();
    modelViewMatrix = vmath::mat4::identity();
    modelViewProjectionMatrix = vmath::mat4::identity();

    translateMatrix = vmath::translate(-1.0f, 0.0f, -3.0f);
    rotateYMatrix = vmath::rotate(pyramidAngle, 0.0f, 1.0f, 0.0f);

    modelViewMatrix = translateMatrix * rotateYMatrix;

    modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;  //pre-multiplication of matrices

    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

    //bind vao_pyramid
    glBindVertexArray(vao_pyramid);

    glDrawArrays(GL_TRIANGLES, 0, 12);

    //unbind vao_triangle
    glBindVertexArray(0);

    
    //CUBE TRANSFORMATION
    translateMatrix = vmath::mat4::identity();
    scaleMatrix = vmath::mat4::identity();
    rotateXMatrix = vmath::mat4::identity();
    rotateYMatrix = vmath::mat4::identity();
    rotateZMatrix = vmath::mat4::identity();
    modelViewMatrix = vmath::mat4::identity();
    modelViewProjectionMatrix = vmath::mat4::identity();

    translateMatrix = vmath::translate(1.0f, 0.0f, -3.0f);
    scaleMatrix = vmath::scale(0.4f, 0.4f, 0.4f);
    rotateXMatrix = vmath::rotate(cubeAngle, 1.0f, 0.0f, 0.0f);
    rotateYMatrix = vmath::rotate(cubeAngle, 0.0f, 1.0f, 0.0f);
    rotateZMatrix = vmath::rotate(cubeAngle, 0.0f, 0.0f, 1.0f);


    modelViewMatrix = translateMatrix * scaleMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix;

    modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

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
    
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
    [eaglContext presentRenderbuffer:GL_RENDERBUFFER]; //swapbuffers
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

    if (vbo_color_pyramid)
    {
        glDeleteBuffers(1, &vbo_color_pyramid);
        vbo_color_pyramid = 0;
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

    if (vbo_color_cube)
    {
        glDeleteBuffers(1, &vbo_color_cube);
        vbo_color_cube = 0;
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
