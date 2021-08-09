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
    GLuint vbo_texture_pyramid;

    //vao for cube
    GLuint vao_cube;
    GLuint vbo_position_cube;
    GLuint vbo_texture_cube;
    
    GLuint mvpMatrixUniform;
    GLuint textureSamplerUniform;

    vmath::mat4 perspectiveProjectionMatrix;   //4x4 matrix

    GLfloat pyramidAngle;
    GLfloat cubeAngle;
    
    GLuint pyramid_texture;
    GLuint cube_texture;
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
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        //glEnable(GL_CULL_FACE);
        
        //variables initialization
        pyramidAngle = 0.0f;
        cubeAngle = 0.0f;
        
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        
        //texture
        pyramid_texture = [self loadTextureFromBMP:@"Stone" :@"bmp"];
        cube_texture = [self loadTextureFromBMP:@"Vijay_Kundali" :@"bmp"];
        
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

-(GLuint)loadTextureFromBMP:(NSString *)imageFileName :(NSString *)extension
{
    //code
    NSString *imageFileNameWithPath = [[NSBundle mainBundle]pathForResource:imageFileName ofType:extension];
    
    //get NSImage representaion of image file
    UIImage *bmpImage = [[UIImage alloc]initWithContentsOfFile : imageFileNameWithPath];
    
    //error checking
    if(!bmpImage)
    {
        printf("\nNSImage conversion of image file failed\n");
        return(0);
    }
    
    //get CGImage representaion of NSImage
    CGImageRef cgImage = [bmpImage CGImage];
    
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
