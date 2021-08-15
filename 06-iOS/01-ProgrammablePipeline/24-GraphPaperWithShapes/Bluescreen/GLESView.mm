//
//  MyView.m
//  Window
//
//  Created by Shruti Kulkarni on 03/07/21.
//

#import <OpenGLES/ES3/gl.h>
#import <OpenGLES/ES3/glext.h>

#import "GLESView.h"

#include "CircleVector.h"

#import "vmath.h"
using namespace vmath;

#define PI 3.1415

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
        printf("\nsize = %d\ninNumVertices = %d\nexNumVertices = %d\n", excircle->size, in_num_vertices, ex_num_vertices);


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
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_CULL_FACE);
        
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        
        //variable declarations
        
        
        
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
