//
//  MyView.m
//  Window
//
//  Created by Shruti Kulkarni on 03/07/21.
//

#import <OpenGLES/ES3/gl.h>
#import <OpenGLES/ES3/glext.h>

#import "GLESView.h"

#import "SphereMesh.h"
#import "vmath.h"
using namespace vmath;

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
    
    //global variables for opengl
    
    //PV
    GLuint shaderProgramObjectPV;
    GLuint vertexShaderObjectPV;
    GLuint fragmentShaderObjectPV;
    
    GLuint modelMatrixUniformPV;
    GLuint viewMatrixUniformPV;
    GLuint projectionMatrixUniformPV;

    GLuint lAUniformPV;
    GLuint lDUniformPV;
    GLuint lSUniformPV;
    GLuint lightPositionUniformPV;

    GLuint kAUniformPV;
    GLuint kDUniformPV;
    GLuint kSUniformPV;
    GLfloat kShininessUniformPV;
    
    GLuint singleTapUniformPV;
    
    
    //PF
    GLuint shaderProgramObjectPF;
    GLuint vertexShaderObjectPF;
    GLuint fragmentShaderObjectPF;
    
    GLuint modelMatrixUniformPF;
    GLuint viewMatrixUniformPF;
    GLuint projectionMatrixUniformPF;

    GLuint lAUniformPF;
    GLuint lDUniformPF;
    GLuint lSUniformPF;
    GLuint lightPositionUniformPF;

    GLuint kAUniformPF;
    GLuint kDUniformPF;
    GLuint kSUniformPF;
    GLfloat kShininessUniformPF;
    
    GLuint singleTapUniformPF;

    vmath::mat4 perspectiveProjectionMatrix;   //4x4 matrix
    
    bool bLight;
    bool pvLighting;
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
        
        /****PV LIGHTING****/


        /*****VERTEX SHADER*****/

            //create shader
        vertexShaderObjectPV = glCreateShader(GL_VERTEX_SHADER);

        //provide source code to vertex shader
        const GLchar* vertexShaderSourceCodePV =
            "#version 300 es \n" \
            "\n" \
            "precision lowp int; \n" \
            "precision highp float; \n" \
            "in vec4 vPosition; \n" \
            "in vec3 vNormal; \n" \
            "uniform int u_singleTap; \n" \
            "uniform vec3 u_lA; \n" \
            "uniform vec3 u_lD; \n" \
            "uniform vec3 u_lS; \n" \
            "uniform vec4 u_light_position; \n" \
            "uniform vec3 u_kA; \n" \
            "uniform vec3 u_kD; \n" \
            "uniform vec3 u_kS; \n" \
            "uniform float u_materialShininess; \n" \
            "uniform mat4 u_projection_matrix; \n" \
            "uniform mat4 u_model_matrix; \n" \
            "uniform mat4 u_view_matrix; \n" \
            "out vec3 phong_ads_light; \n" \
            "void main(void) \n" \
            "{ \n" \
            "   if(u_singleTap==1) \n" \
            "   { \n" \
            "       vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition; \n" \
            "       vec3 transformed_normal = normalize(mat3(u_view_matrix * u_model_matrix) * vNormal); \n" \
            "       vec3 light_direction = normalize(vec3(u_light_position - eye_coordinates)); \n" \
            "       vec3 reflection_vector = reflect(-light_direction, transformed_normal); \n" \
            "       vec3 view_vector = normalize(-eye_coordinates.xyz); \n" \
            "       vec3 ambient = u_lA * u_kA; \n" \
            "       vec3 diffuse = u_lD * u_kD * max(dot(light_direction, transformed_normal), 0.0); \n" \
            "       vec3 specular = u_lS * u_kS * pow(max(dot(reflection_vector, view_vector), 0.0), u_materialShininess); \n" \
            "       phong_ads_light = ambient + diffuse + specular; \n" \
            "   } \n" \
            "   else \n" \
            "   { \n" \
            "       phong_ads_light = vec3(1.0, 1.0, 1.0); \n" \
            "   } \n" \
            "   gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition; \n" \
            "} \n";

        glShaderSource(vertexShaderObjectPV, 1, (const GLchar**)&vertexShaderSourceCodePV, NULL);

        //compile shader
        glCompileShader(vertexShaderObjectPV);

        //vertex shader compilation error checking
        GLint infoLogLength = 0;
        GLint shaderCompiledStatus = 0;
        char* szBuffer = NULL;

        glGetShaderiv(vertexShaderObjectPV, GL_COMPILE_STATUS, &shaderCompiledStatus);

        if (shaderCompiledStatus == GL_FALSE)
        {
            glGetShaderiv(vertexShaderObjectPV, GL_INFO_LOG_LENGTH, &infoLogLength);

            if (infoLogLength > 0)
            {
                szBuffer = (char*)malloc(infoLogLength);
                if (szBuffer != NULL)
                {
                    GLsizei written;
                    glGetShaderInfoLog(vertexShaderObjectPV, infoLogLength, &written, szBuffer);
                    printf("\nVertex Shader Compilation Log of Per Vertex Lighting : %s\n", szBuffer);
                    free(szBuffer);
                    [self release];
                    [self uninitialize];
                }

            }
        }


        /*****FRAGMENT SHADER*****/

        //create shader
        fragmentShaderObjectPV = glCreateShader(GL_FRAGMENT_SHADER);

        //provide source code to fragment shader
        const GLchar* fragmentShaderSourceCodePV =
            "#version 300 es \n" \
            "\n" \
            "precision highp float; \n" \
            "in vec3 phong_ads_light; \n" \
            "out vec4 FragColor; \n" \
            "void main(void) \n" \
            "{ \n" \
            "   FragColor = vec4(phong_ads_light, 1.0f); \n" \
            "} \n";

        glShaderSource(fragmentShaderObjectPV, 1, (const GLchar**)&fragmentShaderSourceCodePV, NULL);

        //compile shader
        glCompileShader(fragmentShaderObjectPV);

        //fragment shader compilation error checking
        infoLogLength = 0;
        shaderCompiledStatus = 0;
        szBuffer = NULL;

        glGetShaderiv(fragmentShaderObjectPV, GL_COMPILE_STATUS, &shaderCompiledStatus);

        if (shaderCompiledStatus == GL_FALSE)
        {
            glGetShaderiv(fragmentShaderObjectPV, GL_INFO_LOG_LENGTH, &infoLogLength);

            if (infoLogLength > 0)
            {
                szBuffer = (char*)malloc(infoLogLength);
                if (szBuffer != NULL)
                {
                    GLsizei written;
                    glGetShaderInfoLog(fragmentShaderObjectPV, infoLogLength, &written, szBuffer);
                    printf("\nFragment Shader Compilation Log of Per Vertex Lighting : %s\n", szBuffer);
                    free(szBuffer);
                    [self release];
                    [self uninitialize];
                }
            }
        }


        /*****SHADER PROGRAM*****/

        //create
        shaderProgramObjectPV = glCreateProgram();

        //attach vertex shader to shader program
        glAttachShader(shaderProgramObjectPV, vertexShaderObjectPV);

        //attach fragment shader to shader program
        glAttachShader(shaderProgramObjectPV, fragmentShaderObjectPV);

        //pre-linking binding
        glBindAttribLocation(shaderProgramObjectPV, SPK_ATTRIBUTE_POSITION, "vPosition");
        glBindAttribLocation(shaderProgramObjectPV, SPK_ATTRIBUTE_NORMAL, "vNormal");

        //link shader
        glLinkProgram(shaderProgramObjectPV);

        //shader linking error checking
        infoLogLength = 0;
        GLint shaderProgramLinkStatus;
        szBuffer = NULL;

        glGetProgramiv(shaderProgramObjectPV, GL_LINK_STATUS, &shaderProgramLinkStatus);

        if (shaderProgramLinkStatus == GL_FALSE)
        {
            glGetProgramiv(shaderProgramObjectPV, GL_INFO_LOG_LENGTH, &infoLogLength);

            if (infoLogLength > 0)
            {
                szBuffer = (char*)malloc(infoLogLength);

                if (szBuffer != NULL)
                {
                    GLsizei written;
                    glGetProgramInfoLog(shaderProgramObjectPV, infoLogLength, &written, szBuffer);
                    printf("\nShader Program Link Log of Per Vertex Lighting : %s\n", szBuffer);
                    free(szBuffer);
                    [self release];
                    [self uninitialize];
                }
            }
        }

        //get uniform location
        modelMatrixUniformPV = glGetUniformLocation(shaderProgramObjectPV, "u_model_matrix");
        viewMatrixUniformPV = glGetUniformLocation(shaderProgramObjectPV, "u_view_matrix");
        projectionMatrixUniformPV = glGetUniformLocation(shaderProgramObjectPV, "u_projection_matrix");

        singleTapUniformPV = glGetUniformLocation(shaderProgramObjectPV, "u_singleTap");

        lAUniformPV = glGetUniformLocation(shaderProgramObjectPV, "u_lA");
        lDUniformPV = glGetUniformLocation(shaderProgramObjectPV, "u_lD");
        lSUniformPV = glGetUniformLocation(shaderProgramObjectPV, "u_lS");
        lightPositionUniformPV = glGetUniformLocation(shaderProgramObjectPV, "u_light_position");

        kAUniformPV = glGetUniformLocation(shaderProgramObjectPV, "u_kA");
        kDUniformPV = glGetUniformLocation(shaderProgramObjectPV, "u_kD");
        kSUniformPV = glGetUniformLocation(shaderProgramObjectPV, "u_kS");
        kShininessUniformPV = glGetUniformLocation(shaderProgramObjectPV, "u_materialShininess");

        

        /****PF LIGHTING****/


        /*****VERTEX SHADER*****/

        //create shader
        vertexShaderObjectPF = glCreateShader(GL_VERTEX_SHADER);

        //provide source code to vertex shader
        const GLchar* vertexShaderSourceCodePF =
            "#version 300 es \n" \
            "\n" \
            "precision lowp int; \n" \
            "precision highp float; \n" \
            "in vec4 vPosition; \n" \
            "in vec3 vNormal; \n" \
            "uniform int u_singleTap; \n" \
            "uniform vec4 u_light_position; \n" \
            "uniform mat4 u_projection_matrix; \n" \
            "uniform mat4 u_model_matrix; \n" \
            "uniform mat4 u_view_matrix; \n" \
            "out vec3 transformed_normal; \n" \
            "out vec3 light_direction; \n" \
            "out vec3 view_vector; \n" \
            "void main(void) \n" \
            "{ \n" \
            "   if(u_singleTap==1) \n" \
            "   { \n" \
            "       vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition; \n" \
            "       transformed_normal = mat3(u_view_matrix * u_model_matrix) * vNormal; \n" \
            "       light_direction = vec3(u_light_position - eye_coordinates); \n" \
            "       view_vector = -eye_coordinates.xyz; \n" \
            "   } \n" \
            "   gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition; \n" \
            "} \n";


        glShaderSource(vertexShaderObjectPF, 1, (const GLchar**)&vertexShaderSourceCodePF, NULL);

        //compile shader
        glCompileShader(vertexShaderObjectPF);

        //vertex shader compilation error checking
        /*GLint infoLogLengthPF = 0;
        GLint shaderCompiledStatusPF = 0;
        char* szBufferPF = NULL;*/

        glGetShaderiv(vertexShaderObjectPF, GL_COMPILE_STATUS, &shaderCompiledStatus);

        if (shaderCompiledStatus == GL_FALSE)
        {
            glGetShaderiv(vertexShaderObjectPF, GL_INFO_LOG_LENGTH, &infoLogLength);

            if (infoLogLength > 0)
            {
                szBuffer = (char*)malloc(infoLogLength);
                if (szBuffer != NULL)
                {
                    GLsizei written;
                    glGetShaderInfoLog(vertexShaderObjectPF, infoLogLength, &written, szBuffer);
                    printf("\nVertex Shader Compilation Log of Per Fragment Lighting : %s\n", szBuffer);
                    free(szBuffer);
                    [self release];
                    [self uninitialize];
                }

            }
        }


        /*****FRAGMENT SHADER*****/

        //create shader
        fragmentShaderObjectPF = glCreateShader(GL_FRAGMENT_SHADER);

        //provide source code to fragment shader
        const GLchar* fragmentShaderSourceCodePF =
            "#version 300 es \n" \
            "\n" \
            "precision lowp int; \n" \
            "precision highp float; \n" \
            "in vec3 transformed_normal; \n" \
            "in vec3 light_direction; \n" \
            "in vec3 view_vector; \n" \
            "uniform int u_singleTap; \n" \
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
            "   if(u_singleTap==1) \n" \
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

        glShaderSource(fragmentShaderObjectPF, 1, (const GLchar**)&fragmentShaderSourceCodePF, NULL);

        //compile shader
        glCompileShader(fragmentShaderObjectPF);

        //fragment shader compilation error checking
        infoLogLength = 0;
        shaderCompiledStatus = 0;
        szBuffer = NULL;

        glGetShaderiv(fragmentShaderObjectPF, GL_COMPILE_STATUS, &shaderCompiledStatus);

        if (shaderCompiledStatus == GL_FALSE)
        {
            glGetShaderiv(fragmentShaderObjectPF, GL_INFO_LOG_LENGTH, &infoLogLength);

            if (infoLogLength > 0)
            {
                szBuffer = (char*)malloc(infoLogLength);
                if (szBuffer != NULL)
                {
                    GLsizei written;
                    glGetShaderInfoLog(fragmentShaderObjectPF, infoLogLength, &written, szBuffer);
                    printf("\nFragment Shader Compilation Log of Per Fragment Lighting : %s\n", szBuffer);
                    free(szBuffer);
                    [self release];
                    [self uninitialize];

                }
            }
        }


        /*****SHADER PROGRAM*****/

        //create
        shaderProgramObjectPF = glCreateProgram();

        //attach vertex shader to shader program
        glAttachShader(shaderProgramObjectPF, vertexShaderObjectPF);

        //attach fragment shader to shader program
        glAttachShader(shaderProgramObjectPF, fragmentShaderObjectPF);

        //pre-linking binding
        glBindAttribLocation(shaderProgramObjectPF, SPK_ATTRIBUTE_POSITION, "vPosition");
        glBindAttribLocation(shaderProgramObjectPF, SPK_ATTRIBUTE_NORMAL, "vNormal");

        //link shader
        glLinkProgram(shaderProgramObjectPF);

        //shader linking error checking
        infoLogLength = 0;
        shaderProgramLinkStatus = 0;
        szBuffer = NULL;

        glGetProgramiv(shaderProgramObjectPF, GL_LINK_STATUS, &shaderProgramLinkStatus);

        if (shaderProgramLinkStatus == GL_FALSE)
        {
            glGetProgramiv(shaderProgramObjectPF, GL_INFO_LOG_LENGTH, &infoLogLength);

            if (infoLogLength > 0)
            {
                szBuffer = (char*)malloc(infoLogLength);

                if (szBuffer != NULL)
                {
                    GLsizei written;
                    glGetProgramInfoLog(shaderProgramObjectPF, infoLogLength, &written, szBuffer);
                    printf("\nShader Program Link Log of Per Fragment Lighting : %s\n", szBuffer);
                    free(szBuffer);
                    [self release];
                    [self uninitialize];
                }
            }
        }

        //get uniform location
        modelMatrixUniformPF = glGetUniformLocation(shaderProgramObjectPF, "u_model_matrix");
        viewMatrixUniformPF = glGetUniformLocation(shaderProgramObjectPF, "u_view_matrix");
        projectionMatrixUniformPF = glGetUniformLocation(shaderProgramObjectPF, "u_projection_matrix");

        singleTapUniformPF = glGetUniformLocation(shaderProgramObjectPF, "u_singleTap");

        lAUniformPF = glGetUniformLocation(shaderProgramObjectPF, "u_lA");
        lDUniformPF = glGetUniformLocation(shaderProgramObjectPF, "u_lD");
        lSUniformPF = glGetUniformLocation(shaderProgramObjectPF, "u_lS");
        lightPositionUniformPF = glGetUniformLocation(shaderProgramObjectPF, "u_light_position");

        kAUniformPF = glGetUniformLocation(shaderProgramObjectPF, "u_kA");
        kDUniformPF = glGetUniformLocation(shaderProgramObjectPF, "u_kD");
        kSUniformPF = glGetUniformLocation(shaderProgramObjectPF, "u_kS");
        kShininessUniformPF = glGetUniformLocation(shaderProgramObjectPF, "u_materialShininess");
        

        //vertices array declaration

        makeSphere(1.0, 50, 50);
        
        //depth
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        //glEnable(GL_CULL_FACE);
        
        //variables initialization
        pvLighting = true;
        bLight = false;
        
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
    if (pvLighting == true)
        glUseProgram(shaderProgramObjectPV);

    else
        glUseProgram(shaderProgramObjectPF);


    if (bLight == true)
    {
        if (pvLighting == true)
        {
            GLfloat lightPosition[] = { 100.0f, 100.0f, 100.0f, 1.0f };

            glUniform1i(singleTapUniformPV, 1);

            /****ALBEDO****/

            glUniform3f(lAUniformPV, 0.1f, 0.1f, 0.1f);
            glUniform3f(lDUniformPV, 1.0f, 1.0f, 1.0f);
            glUniform3f(lSUniformPV, 1.0f, 1.0f, 1.0f);
            glUniform4fv(lightPositionUniformPV, 1, (GLfloat*)lightPosition);

            glUniform3f(kAUniformPV, 0.0f, 0.0f, 0.0f);
            glUniform3f(kDUniformPV, 0.5f, 0.2f, 0.7f);
            glUniform3f(kSUniformPV, 0.7f, 0.7f, 0.7f);
            glUniform1f(kShininessUniformPV, 128.0);
        }
        else
        {
            GLfloat lightPosition[] = { 100.0f, 100.0f, 100.0f, 1.0f };

            glUniform1i(singleTapUniformPF, 1);

            /****ALBEDO****/

            glUniform3f(lAUniformPF, 0.1f, 0.1f, 0.1f);
            glUniform3f(lDUniformPF, 1.0f, 1.0f, 1.0f);
            glUniform3f(lSUniformPF, 1.0f, 1.0f, 1.0f);
            glUniform4fv(lightPositionUniformPF, 1, (GLfloat*)lightPosition);

            glUniform3f(kAUniformPF, 0.0f, 0.0f, 0.0f);
            glUniform3f(kDUniformPF, 0.5f, 0.2f, 0.7f);
            glUniform3f(kSUniformPF, 0.7f, 0.7f, 0.7f);
            glUniform1f(kShininessUniformPF, 128.0);
        }
    }
    else
    {
        if (pvLighting == true)
        {
            glUniform1i(singleTapUniformPV, 0);
        }
        else
        {
            glUniform1i(singleTapUniformPF, 0);
        }
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

    if (pvLighting == true)
    {
        glUniformMatrix4fv(modelMatrixUniformPV, 1, GL_FALSE, modelMatrix);
        glUniformMatrix4fv(viewMatrixUniformPV, 1, GL_FALSE, viewMatrix);
        glUniformMatrix4fv(projectionMatrixUniformPV, 1, GL_FALSE, projectionMatrix);
    }
    else
    {
        glUniformMatrix4fv(modelMatrixUniformPF, 1, GL_FALSE, modelMatrix);
        glUniformMatrix4fv(viewMatrixUniformPF, 1, GL_FALSE, viewMatrix);
        glUniformMatrix4fv(projectionMatrixUniformPF, 1, GL_FALSE, projectionMatrix);
    }

    drawSphere();

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
    if (pvLighting == true)
        pvLighting = false;
    else
        pvLighting = true;
    
}

-(void)onDoubleTap:(UITapGestureRecognizer *)gr
{
    //code
    if (bLight == true)
        bLight = false;
    else
        bLight = true;
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
    deallocate();

    /*****SAFE SHADER CLEAN-UP*****/

    if (shaderProgramObjectPV)
    {
        glUseProgram(shaderProgramObjectPV);
        GLsizei shaderCount;
        glGetProgramiv(shaderProgramObjectPV, GL_ATTACHED_SHADERS, &shaderCount);

        GLuint* pShaders = NULL;
        pShaders = (GLuint*)malloc(sizeof(GLuint) * shaderCount);

        glGetAttachedShaders(shaderProgramObjectPV, shaderCount, &shaderCount, pShaders);

        for (GLsizei i = 0; i < shaderCount; i++)
        {
            glDetachShader(shaderProgramObjectPV, pShaders[i]);
            glDeleteShader(pShaders[i]);
            pShaders[i] = 0;
        }
        free(pShaders);

        glDeleteProgram(shaderProgramObjectPV);
        shaderProgramObjectPV = 0;
        glUseProgram(0);

    }

    if (shaderProgramObjectPF)
    {
        glUseProgram(shaderProgramObjectPF);
        GLsizei shaderCount;
        glGetProgramiv(shaderProgramObjectPF, GL_ATTACHED_SHADERS, &shaderCount);

        GLuint* pShaders = NULL;
        pShaders = (GLuint*)malloc(sizeof(GLuint) * shaderCount);

        glGetAttachedShaders(shaderProgramObjectPF, shaderCount, &shaderCount, pShaders);

        for (GLsizei i = 0; i < shaderCount; i++)
        {
            glDetachShader(shaderProgramObjectPF, pShaders[i]);
            glDeleteShader(pShaders[i]);
            pShaders[i] = 0;
        }
        free(pShaders);

        glDeleteProgram(shaderProgramObjectPF);
        shaderProgramObjectPF = 0;
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
