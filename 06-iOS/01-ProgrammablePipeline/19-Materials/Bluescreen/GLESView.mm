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
    
    //shader objects
    GLuint vertexShaderObject;
    GLuint fragmentShaderObject;
    GLuint shaderProgramObject;
    
    //UNIFORMS

    GLuint modelMatrixUniform;
    GLuint viewMatrixUniform;
    GLuint projectionMatrixUniform;

    GLuint doubleTapUniform;

    GLuint lAUniform;
    GLuint lDUniform;
    GLuint lSUniform;
    GLuint lightPositionUniform;

    GLuint kAUniform;
    GLuint kDUniform;
    GLuint kSUniform;
    GLfloat kShininessUniform;

    GLint singleTap;
    GLfloat AngleForXRotation;
    GLfloat AngleForYRotation;
    GLfloat AngleForZRotation;
    
    int vWidth;
    int vHeight;

    bool bLight;
    vec4 lightPosition;

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
            "precision lowp int; \n" \
            "in vec4 vPosition; \n" \
            "in vec3 vNormal; \n" \
            "uniform int u_doubleTap; \n" \
            "uniform vec4 u_light_position; \n" \
            "uniform mat4 u_projection_matrix; \n" \
            "uniform mat4 u_model_matrix; \n" \
            "uniform mat4 u_view_matrix; \n" \
            "out vec3 transformed_normal; \n" \
            "out vec3 light_direction; \n" \
            "out vec3 view_vector; \n" \
            "\n" \
            "void main(void) \n" \
            "{ \n" \
            "    if(u_doubleTap==1) \n" \
            "    { \n" \
            "        vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition; \n" \
            "        transformed_normal = mat3(u_view_matrix * u_model_matrix) * vNormal; \n" \
            "        light_direction = vec3(u_light_position - eye_coordinates); \n" \
            "        view_vector = -eye_coordinates.xyz; \n" \
            "    } \n" \
            "\n" \
            "    gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition; \n" \
            "\n" \
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
            "precision lowp int; \n" \
            "precision highp float; \n" \
            "in vec3 transformed_normal; \n" \
            "in vec3 light_direction; \n" \
            "in vec3 view_vector; \n" \
            "uniform int u_doubleTap; \n" \
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
            "   if(u_doubleTap==1) \n" \
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
                    printf("\nShader Program Link Log : %s\n", szBuffer);
                    free(szBuffer);
                    [self release];
                    [self uninitialize];
                }
            }
        }

        //get MVP uniform location
        modelMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_model_matrix");
        viewMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_view_matrix");
        projectionMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_projection_matrix");

        doubleTapUniform = glGetUniformLocation(shaderProgramObject, "u_doubleTap");

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
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        //glEnable(GL_CULL_FACE);
        
        //variables initialization
        singleTap = 0;
        AngleForXRotation = 0.0f;
        AngleForYRotation = 0.0f;
        AngleForZRotation = 0.0f;
        
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        
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
    
    vWidth= width;
    vHeight= height;
    
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    
    perspectiveProjectionMatrix = vmath::perspective(45.0f, ((GLfloat)width / 4) / ((GLfloat)height / 6), 0.1f, 100.0f);
    
    
    [self drawView:nil];
}

-(void)drawView:(id)sender
{
    //code
    [EAGLContext setCurrentContext:eaglContext];
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
    
    GLfloat r = 5.0f;
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //start using OpenGL program object
    glUseProgram(shaderProgramObject);

    if (bLight == true)
    {

        glUniform1i(doubleTapUniform, 1);

        /****GOURAUD****/


        glUniform3f(lAUniform, 0.0f, 0.0f, 0.0f);
        glUniform3f(lDUniform, 1.0f, 1.0f, 1.0f);
        glUniform3f(lSUniform, 1.0f, 1.0f, 1.0f);
        glUniform4fv(lightPositionUniform, 1, lightPosition);

    }
    else
    {
        glUniform1i(doubleTapUniform, 0);
    }

    if (singleTap == 1)
    {
        lightPosition[0] = 0.0f;
        lightPosition[1] = r * sin(AngleForXRotation);
        lightPosition[2] = r * cos(AngleForXRotation);
        lightPosition[3] = 1.0f;

    }
    else if (singleTap == 2)
    {
        lightPosition[0] = r * sin(AngleForYRotation);
        lightPosition[1] = 0.0f;
        lightPosition[2] = r * cos(AngleForYRotation);
        lightPosition[3] = 1.0f;
    }
    else if (singleTap == 3)
    {
        lightPosition[0] = r * sin(AngleForZRotation);
        lightPosition[1] = r * cos(AngleForZRotation);
        lightPosition[2] = 0.0f;
        lightPosition[3] = 1.0f;

    }


    glUniform3f(kAUniform, 0.0f, 0.0f, 0.0f);
    glUniform3f(kDUniform, 1.0f, 1.0f, 1.0f);
    glUniform3f(kSUniform, 1.0f, 1.0f, 1.0f);
    glUniform1f(kShininessUniform, 50.0);
    
    
    //drawSphere();

    
    [self draw24Spheres];

    //stop using OpenGL program object
    glUseProgram(0);

    //update func()
    [self myUpdate];
    
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
    [eaglContext presentRenderbuffer:GL_RENDERBUFFER]; //swapbuffers
}

-(void)draw24Spheres
{
    //code
    mat4 modelMatrix;
    mat4 viewMatrix;
    mat4 projectionMatrix;
    mat4 translateMatrix;
    mat4 scaleMatrix;

    GLfloat sphereScale = 1.0f;


    // ***** 1st sphere on 1st row, emerald *****


    glUniform3f(kAUniform, 0.0215f, 0.1745f, 0.0215f);
    glUniform3f(kDUniform, 0.07568f, 0.61424f, 0.07568f);
    glUniform3f(kSUniform, 0.633f, 0.727811f, 0.633f);
    glUniform1f(kShininessUniform, 0.6 * 128);

    perspectiveProjectionMatrix = vmath::perspective(45.0f, ((GLfloat)vWidth / 4) / ((GLfloat)vHeight / 6), 0.1f, 100.0f);
    

    glViewport(0, 5 * vHeight / 6, (GLsizei)vWidth / 4, (GLsizei)vHeight / 6);
    
    modelMatrix = mat4::identity();
    viewMatrix = mat4::identity();
    projectionMatrix = mat4::identity();
    translateMatrix = mat4::identity();
    scaleMatrix = mat4::identity();
    
    //translateMatrix = translate(-1.8f, 1.0f, -3.0f);
    translateMatrix = translate(0.0f, 0.0f, -3.0f);
    scaleMatrix = scale(sphereScale, sphereScale, sphereScale);

    modelMatrix = translateMatrix * scaleMatrix;

    projectionMatrix = perspectiveProjectionMatrix;

    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

    drawSphere();




    // ***** 2nd sphere on 1st row, jade *****


    glUniform3f(kAUniform, 0.135f, 0.2225f, 0.1575f);
    glUniform3f(kDUniform, 0.54f, 0.89f, 0.63f);
    glUniform3f(kSUniform, 0.316228f, 0.316228f, 0.316228f);
    glUniform1f(kShininessUniform, 0.1 * 128);

    glViewport(vWidth / 4, 5 * vHeight / 6, (GLsizei)vWidth / 4, (GLsizei)vHeight / 6); //////////////////////////////////////////////////////////////

    modelMatrix = mat4::identity();
    viewMatrix = mat4::identity();
    projectionMatrix = mat4::identity();
    translateMatrix = mat4::identity();
    scaleMatrix = mat4::identity();
    //translateMatrix = translate(-1.08f, 1.0f, -3.0f);
    translateMatrix = translate(0.0f, 0.0f, -3.0f);
    scaleMatrix = scale(sphereScale, sphereScale, sphereScale);

    modelMatrix = translateMatrix * scaleMatrix;

    projectionMatrix = perspectiveProjectionMatrix;

    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

    drawSphere();





    // ***** 3rd sphere on 1st row, obsidian *****


    glUniform3f(kAUniform, 0.05375f, 0.05f, 0.06625f);
    glUniform3f(kDUniform, 0.18275f, 0.17f, 0.22525f);
    glUniform3f(kSUniform, 0.332741f, 0.328634f, 0.346435f);
    glUniform1f(kShininessUniform, 0.3 * 128);


    glViewport(vWidth / 2, 5 * vHeight / 6, (GLsizei)vWidth / 4, (GLsizei)vHeight / 6);


    modelMatrix = mat4::identity();
    viewMatrix = mat4::identity();
    projectionMatrix = mat4::identity();
    translateMatrix = mat4::identity();
    scaleMatrix = mat4::identity();
    //translateMatrix = translate(-0.36f, 1.0f, -3.0f);
    translateMatrix = translate(0.0f, 0.0f, -3.0f);
    scaleMatrix = scale(sphereScale, sphereScale, sphereScale);

    modelMatrix = translateMatrix * scaleMatrix;

    projectionMatrix = perspectiveProjectionMatrix;

    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

    drawSphere();




    // ***** 4th sphere on 1st row, pearl *****


    glUniform3f(kAUniform, 0.25f, 0.20725f, 0.20725f);
    glUniform3f(kDUniform, 1.0f, 0.829f, 0.829f);
    glUniform3f(kSUniform, 0.296648f, 0.296648f, 0.296648f);
    glUniform1f(kShininessUniform, 0.088 * 128);

    glViewport(3 * vWidth / 4, 5 * vHeight / 6, (GLsizei)vWidth / 4, (GLsizei)vHeight / 6);


    modelMatrix = mat4::identity();
    viewMatrix = mat4::identity();
    projectionMatrix = mat4::identity();
    translateMatrix = mat4::identity();
    scaleMatrix = mat4::identity();
    translateMatrix = translate(0.0f, 0.0f, -3.0f);
    //translateMatrix = translate(0.36f, 1.0f, -3.0f);
    scaleMatrix = scale(sphereScale, sphereScale, sphereScale);

    modelMatrix = translateMatrix * scaleMatrix;

    projectionMatrix = perspectiveProjectionMatrix;

    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

    drawSphere();



    // ***** 1st sphere on 2nd row, ruby *****


    glUniform3f(kAUniform, 0.1745f, 0.01175f, 0.01175f);
    glUniform3f(kDUniform, 0.61424f, 0.04136f, 0.04136f);
    glUniform3f(kSUniform, 0.727811f, 0.626959f, 0.626959f);
    glUniform1f(kShininessUniform, 0.6 * 128);

    glViewport(0, 2 * vHeight / 3, (GLsizei)vWidth / 4, (GLsizei)vHeight / 6);


    modelMatrix = mat4::identity();
    viewMatrix = mat4::identity();
    projectionMatrix = mat4::identity();
    translateMatrix = mat4::identity();
    scaleMatrix = mat4::identity();
    translateMatrix = translate(0.0f, 0.0f, -3.0f);
    //translateMatrix = translate(1.08f, 1.0f, -3.0f);
    scaleMatrix = scale(sphereScale, sphereScale, sphereScale);

    modelMatrix = translateMatrix * scaleMatrix;

    projectionMatrix = perspectiveProjectionMatrix;

    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

    drawSphere();




    // ***** 2nd sphere on 2nd row, turquoise *****



    glUniform3f(kAUniform, 0.1f, 0.18725f, 0.1745f);
    glUniform3f(kDUniform, 0.396f, 0.74151f, 0.69102f);
    glUniform3f(kSUniform, 0.297254f, 0.30829f, 0.306678f);
    glUniform1f(kShininessUniform, 0.1 * 128);

    glViewport(vWidth / 4, 2 * vHeight / 3, (GLsizei)vWidth / 4, (GLsizei)vHeight / 6);


    modelMatrix = mat4::identity();
    viewMatrix = mat4::identity();
    projectionMatrix = mat4::identity();
    translateMatrix = mat4::identity();
    scaleMatrix = mat4::identity();
    translateMatrix = translate(0.0f, 0.0f, -3.0f);
    //translateMatrix = translate(1.8f, 1.0f, -3.0f);
    scaleMatrix = scale(sphereScale, sphereScale, sphereScale);

    modelMatrix = translateMatrix * scaleMatrix;

    projectionMatrix = perspectiveProjectionMatrix;

    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

    drawSphere();








    // ***** 3rd sphere on 2nd row, brass *****


    glUniform3f(kAUniform, 0.329412f, 0.223529f, 0.027451f);
    glUniform3f(kDUniform, 0.780392f, 0.568627f, 0.113725f);
    glUniform3f(kSUniform, 0.992157f, 0.941176f, 0.807843f);
    glUniform1f(kShininessUniform, 0.21794872 * 128);

    glViewport(vWidth / 2, 2 * vHeight / 3, (GLsizei)vWidth / 4, (GLsizei)vHeight / 6);


    modelMatrix = mat4::identity();
    viewMatrix = mat4::identity();
    projectionMatrix = mat4::identity();
    translateMatrix = mat4::identity();
    scaleMatrix = mat4::identity();
    translateMatrix = translate(0.0f, 0.0f, -3.0f);
    //translateMatrix = translate(-1.8f, 0.34f, -3.0f);
    scaleMatrix = scale(sphereScale, sphereScale, sphereScale);

    modelMatrix = translateMatrix * scaleMatrix;

    projectionMatrix = perspectiveProjectionMatrix;

    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

    drawSphere();


    // ***** 4th sphere on 2nd row, bronze *****


    glUniform3f(kAUniform, 0.0215f, 0.1275f, 0.054f);
    glUniform3f(kDUniform, 0.714f, 0.4284f, 0.18144f);
    glUniform3f(kSUniform, 0.393548f, 0.271906f, 0.166721f);
    glUniform1f(kShininessUniform, 0.2 * 128);

    glViewport(3 * vWidth / 4, 2 * vHeight / 3, (GLsizei)vWidth / 4, (GLsizei)vHeight / 6);


    modelMatrix = mat4::identity();
    viewMatrix = mat4::identity();
    projectionMatrix = mat4::identity();
    translateMatrix = mat4::identity();
    scaleMatrix = mat4::identity();
    translateMatrix = translate(0.0f, 0.0f, -3.0f);
    //translateMatrix = translate(-1.08f, 0.34f, -3.0f);
    scaleMatrix = scale(sphereScale, sphereScale, sphereScale);

    modelMatrix = translateMatrix * scaleMatrix;

    projectionMatrix = perspectiveProjectionMatrix;

    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

    drawSphere();




    // ***** 1st sphere on 3rd row, chrome *****



    glUniform3f(kAUniform, 0.25f, 0.25f, 0.25f);
    glUniform3f(kDUniform, 0.4f, 0.4f, 0.4f);
    glUniform3f(kSUniform, 0.774597f, 0.774597f, 0.774597f);
    glUniform1f(kShininessUniform, 0.6 * 128);

    glViewport(0, vHeight / 2, (GLsizei)vWidth / 4, (GLsizei)vHeight / 6);


    modelMatrix = mat4::identity();
    viewMatrix = mat4::identity();
    projectionMatrix = mat4::identity();
    translateMatrix = mat4::identity();
    scaleMatrix = mat4::identity();
    translateMatrix = translate(0.0f, 0.0f, -3.0f);
    //translateMatrix = translate(-0.36f, 0.34f, -3.0f);
    scaleMatrix = scale(sphereScale, sphereScale, sphereScale);

    modelMatrix = translateMatrix * scaleMatrix;

    projectionMatrix = perspectiveProjectionMatrix;

    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

    drawSphere();




    // ***** 2nd sphere on 3rd row, copper *****


    glUniform3f(kAUniform, 0.19125f, 0.0735f, 0.0225f);
    glUniform3f(kDUniform, 0.7038f, 0.27048f, 0.0828f);
    glUniform3f(kSUniform, 0.256777f, 0.137622f, 0.086014f);
    glUniform1f(kShininessUniform, 0.1 * 128);

    glViewport(vWidth / 4, vHeight / 2, (GLsizei)vWidth / 4, (GLsizei)vHeight / 6);


    modelMatrix = mat4::identity();
    viewMatrix = mat4::identity();
    projectionMatrix = mat4::identity();
    translateMatrix = mat4::identity();
    scaleMatrix = mat4::identity();
    translateMatrix = translate(0.0f, 0.0f, -3.0f);
    //translateMatrix = translate(0.36f, 0.34f, -3.0f);
    scaleMatrix = scale(sphereScale, sphereScale, sphereScale);

    modelMatrix = translateMatrix * scaleMatrix;

    projectionMatrix = perspectiveProjectionMatrix;

    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

    drawSphere();



    // ***** 3rd sphere on 3rd row, gold *****


    glUniform3f(kAUniform, 0.24725f, 0.1995f, 0.0745f);
    glUniform3f(kDUniform, 0.75164f, 0.60648f, 0.22648f);
    glUniform3f(kSUniform, 0.628281f, 0.555802f, 0.366065f);
    glUniform1f(kShininessUniform, 0.4 * 128);

    glViewport(vWidth / 2, vHeight / 2, (GLsizei)vWidth / 4, (GLsizei)vHeight / 6);


    modelMatrix = mat4::identity();
    viewMatrix = mat4::identity();
    projectionMatrix = mat4::identity();
    translateMatrix = mat4::identity();
    scaleMatrix = mat4::identity();
    translateMatrix = translate(0.0f, 0.0f, -3.0f);
    //translateMatrix = translate(1.08f, 0.34f, -3.0f);
    scaleMatrix = scale(sphereScale, sphereScale, sphereScale);

    modelMatrix = translateMatrix * scaleMatrix;

    projectionMatrix = perspectiveProjectionMatrix;

    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

    drawSphere();




    // ***** 4th sphere on 3rd row, silver *****


    glUniform3f(kAUniform, 0.19225f, 0.19225f, 0.19225f);
    glUniform3f(kDUniform, 0.50754f, 0.50754f, 0.50754f);
    glUniform3f(kSUniform, 0.508273f, 0.508273f, 0.508273f);
    glUniform1f(kShininessUniform, 0.4 * 128);

    glViewport(3 * vWidth / 4, vHeight / 2, (GLsizei)vWidth / 4, (GLsizei)vHeight / 6);

    modelMatrix = mat4::identity();
    viewMatrix = mat4::identity();
    projectionMatrix = mat4::identity();
    translateMatrix = mat4::identity();
    scaleMatrix = mat4::identity();
    translateMatrix = translate(0.0f, 0.0f, -3.0f);
    //translateMatrix = translate(1.8f, 0.34f, -3.0f);
    scaleMatrix = scale(sphereScale, sphereScale, sphereScale);

    modelMatrix = translateMatrix * scaleMatrix;

    projectionMatrix = perspectiveProjectionMatrix;

    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

    drawSphere();


    

    // ***** 1st sphere on 4th row, black *****

    glUniform3f(kAUniform, 0.0f, 0.0f, 0.0f);
    glUniform3f(kDUniform, 0.01f, 0.01f, 0.01f);
    glUniform3f(kSUniform, 0.5f, 0.5f, 0.5f);
    glUniform1f(kShininessUniform, 0.25 * 128);

    glViewport(0, vHeight / 3, (GLsizei)vWidth / 4, (GLsizei)vHeight / 6);


    modelMatrix = mat4::identity();
    viewMatrix = mat4::identity();
    projectionMatrix = mat4::identity();
    translateMatrix = mat4::identity();
    scaleMatrix = mat4::identity();
    translateMatrix = translate(0.0f, 0.0f, -3.0f);
    //translateMatrix = translate(-1.8f, -0.34f, -3.0f);
    scaleMatrix = scale(sphereScale, sphereScale, sphereScale);

    modelMatrix = translateMatrix * scaleMatrix;

    projectionMatrix = perspectiveProjectionMatrix;

    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

    drawSphere();




    // ***** 2nd sphere on 4th row, cyan *****


    glUniform3f(kAUniform, 0.0f, 0.1f, 0.06f);
    glUniform3f(kDUniform, 0.0f, 0.50980392f, 0.50980392f);
    glUniform3f(kSUniform, 0.50196078f, 0.50196078f, 0.50196078f);
    glUniform1f(kShininessUniform, 0.25 * 128);

    glViewport(vWidth / 4, vHeight / 3, (GLsizei)vWidth / 4, (GLsizei)vHeight / 6);


    modelMatrix = mat4::identity();
    viewMatrix = mat4::identity();
    projectionMatrix = mat4::identity();
    translateMatrix = mat4::identity();
    scaleMatrix = mat4::identity();
    translateMatrix = translate(0.0f, 0.0f, -3.0f);
    //translateMatrix = translate(-1.08f, -0.34f, -3.0f);
    scaleMatrix = scale(sphereScale, sphereScale, sphereScale);

    modelMatrix = translateMatrix * scaleMatrix;

    projectionMatrix = perspectiveProjectionMatrix;

    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);


    drawSphere();





    // ***** 3rd sphere on 4th row, green *****


    glUniform3f(kAUniform, 0.0f, 0.0f, 0.0f);
    glUniform3f(kDUniform, 0.1f, 0.35f, 0.1f);
    glUniform3f(kSUniform, 0.45f, 0.55f, 0.45f);
    glUniform1f(kShininessUniform, 0.25 * 128);

    glViewport(vWidth / 2, vHeight / 3, (GLsizei)vWidth / 4, (GLsizei)vHeight / 6);

    modelMatrix = mat4::identity();
    viewMatrix = mat4::identity();
    projectionMatrix = mat4::identity();
    translateMatrix = mat4::identity();
    scaleMatrix = mat4::identity();
    translateMatrix = translate(0.0f, 0.0f, -3.0f);
    //translateMatrix = translate(-0.36f, -0.34f, -3.0f);
    scaleMatrix = scale(sphereScale, sphereScale, sphereScale);

    modelMatrix = translateMatrix * scaleMatrix;

    projectionMatrix = perspectiveProjectionMatrix;

    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

    drawSphere();




    // ***** 4th sphere on 4th row, red *****


    glUniform3f(kAUniform, 0.0f, 0.0f, 0.0f);
    glUniform3f(kDUniform, 0.5f, 0.0f, 0.0f);
    glUniform3f(kSUniform, 0.7f, 0.6f, 0.6f);
    glUniform1f(kShininessUniform, 0.25 * 128);

    glViewport(3 * vWidth / 4, vHeight / 3, (GLsizei)vWidth / 4, (GLsizei)vHeight / 6);

    modelMatrix = mat4::identity();
    viewMatrix = mat4::identity();
    projectionMatrix = mat4::identity();
    translateMatrix = mat4::identity();
    scaleMatrix = mat4::identity();
    translateMatrix = translate(0.0f, 0.0f, -3.0f);
    //translateMatrix = translate(0.36f, -0.34f, -3.0f);
    scaleMatrix = scale(sphereScale, sphereScale, sphereScale);

    modelMatrix = translateMatrix * scaleMatrix;

    projectionMatrix = perspectiveProjectionMatrix;

    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);


    drawSphere();



    // ***** 1st sphere on 5th row, white *****


    glUniform3f(kAUniform, 0.0f, 0.0f, 0.0f);
    glUniform3f(kDUniform, 0.55f, 0.55f, 0.55f);
    glUniform3f(kSUniform, 0.7f, 0.7f, 0.7f);
    glUniform1f(kShininessUniform, 0.25 * 128);

    glViewport(0, vHeight / 6, (GLsizei)vWidth / 4, (GLsizei)vHeight / 6);

    modelMatrix = mat4::identity();
    viewMatrix = mat4::identity();
    projectionMatrix = mat4::identity();
    translateMatrix = mat4::identity();
    scaleMatrix = mat4::identity();
    translateMatrix = translate(0.0f, 0.0f, -3.0f);
    //translateMatrix = translate(1.08f, -0.34f, -3.0f);
    scaleMatrix = scale(sphereScale, sphereScale, sphereScale);

    modelMatrix = translateMatrix * scaleMatrix;

    projectionMatrix = perspectiveProjectionMatrix;

    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

    drawSphere();





    // ***** 2nd sphere on 5th row, yellow plastic *****


    glUniform3f(kAUniform, 0.0f, 0.0f, 0.0f);
    glUniform3f(kDUniform, 0.5f, 0.5f, 0.0f);
    glUniform3f(kSUniform, 0.6f, 0.6f, 0.5f);
    glUniform1f(kShininessUniform, 0.25 * 128);

    glViewport(vWidth / 4, vHeight / 6, (GLsizei)vWidth / 4, (GLsizei)vHeight / 6);

    modelMatrix = mat4::identity();
    viewMatrix = mat4::identity();
    projectionMatrix = mat4::identity();
    translateMatrix = mat4::identity();
    scaleMatrix = mat4::identity();
    translateMatrix = translate(0.0f, 0.0f, -3.0f);
    //translateMatrix = translate(1.8f, -0.34f, -3.0f);
    scaleMatrix = scale(sphereScale, sphereScale, sphereScale);

    modelMatrix = translateMatrix * scaleMatrix;

    projectionMatrix = perspectiveProjectionMatrix;

    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

    drawSphere();




    // ***** 3rd sphere on 5th row, black *****


    glUniform3f(kAUniform, 0.02f, 0.02f, 0.02f);
    glUniform3f(kDUniform, 0.01f, 0.01f, 0.01f);
    glUniform3f(kSUniform, 0.4f, 0.4f, 0.4f);
    glUniform1f(kShininessUniform, 0.078125 * 128);


    glViewport(vWidth / 2, vHeight / 6, (GLsizei)vWidth / 4, (GLsizei)vHeight / 6);


    modelMatrix = mat4::identity();
    viewMatrix = mat4::identity();
    projectionMatrix = mat4::identity();
    translateMatrix = mat4::identity();
    scaleMatrix = mat4::identity();
    translateMatrix = translate(0.0f, 0.0f, -3.0f);
    //translateMatrix = translate(-1.8f, -1.0f, -3.0f);
    scaleMatrix = scale(sphereScale, sphereScale, sphereScale);

    modelMatrix = translateMatrix * scaleMatrix;

    projectionMatrix = perspectiveProjectionMatrix;

    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

    drawSphere();




    // ***** 4th sphere on 5th row, cyan *****


    glUniform3f(kAUniform, 0.0f, 0.05f, 0.05f);
    glUniform3f(kDUniform, 0.4f, 0.5f, 0.5f);
    glUniform3f(kSUniform, 0.04f, 0.7f, 0.7f);
    glUniform1f(kShininessUniform, 0.078125 * 128);

    glViewport(3 * vWidth / 4, vHeight / 6, (GLsizei)vWidth / 4, (GLsizei)vHeight / 6);


    modelMatrix = mat4::identity();
    viewMatrix = mat4::identity();
    projectionMatrix = mat4::identity();
    translateMatrix = mat4::identity();
    scaleMatrix = mat4::identity();
    translateMatrix = translate(0.0f, 0.0f, -3.0f);
    //translateMatrix = translate(-1.08f, -1.0f, -3.0f);
    scaleMatrix = scale(sphereScale, sphereScale, sphereScale);

    modelMatrix = translateMatrix * scaleMatrix;

    projectionMatrix = perspectiveProjectionMatrix;

    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

    drawSphere();




    // ***** 1st sphere on 6th row, green *****


    glUniform3f(kAUniform, 0.0f, 0.05f, 0.0f);
    glUniform3f(kDUniform, 0.4f, 0.5f, 0.4f);
    glUniform3f(kSUniform, 0.04f, 0.7f, 0.04f);
    glUniform1f(kShininessUniform, 0.078125 * 128);

    glViewport(0, 0, (GLsizei)vWidth / 4, (GLsizei)vHeight / 6);

    modelMatrix = mat4::identity();
    viewMatrix = mat4::identity();
    projectionMatrix = mat4::identity();
    translateMatrix = mat4::identity();
    scaleMatrix = mat4::identity();
    translateMatrix = translate(0.0f, 0.0f, -3.0f);
    //translateMatrix = translate(-0.36f, -1.0f, -3.0f);
    scaleMatrix = scale(sphereScale, sphereScale, sphereScale);

    modelMatrix = translateMatrix * scaleMatrix;

    projectionMatrix = perspectiveProjectionMatrix;

    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

    drawSphere();




    // ***** 2nd sphere on 6th row, red *****


    glUniform3f(kAUniform, 0.05f, 0.0f, 0.0f);
    glUniform3f(kDUniform, 0.5f, 0.4f, 0.4f);
    glUniform3f(kSUniform, 0.7f, 0.04f, 0.04f);
    glUniform1f(kShininessUniform, 0.078125 * 128);

    glViewport(vWidth / 4, 0, (GLsizei)vWidth / 4, (GLsizei)vHeight / 6);

    modelMatrix = mat4::identity();
    viewMatrix = mat4::identity();
    projectionMatrix = mat4::identity();
    translateMatrix = mat4::identity();
    scaleMatrix = mat4::identity();
    translateMatrix = translate(0.0f, 0.0f, -3.0f);
    //translateMatrix = translate(0.36f, -1.0f, -3.0f);
    scaleMatrix = scale(sphereScale, sphereScale, sphereScale);

    modelMatrix = translateMatrix * scaleMatrix;

    projectionMatrix = perspectiveProjectionMatrix;

    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);

    drawSphere();


    // ***** 3rd sphere on 6th row, white *****


    glUniform3f(kAUniform, 0.05f, 0.05f, 0.05f);
    glUniform3f(kDUniform, 0.5f, 0.5f, 0.5f);
    glUniform3f(kSUniform, 0.7f, 0.7f, 0.7f);
    glUniform1f(kShininessUniform, 0.078125 * 128);

    glViewport(vWidth / 2, 0, (GLsizei)vWidth / 4, (GLsizei)vHeight / 6);

    modelMatrix = mat4::identity();
    viewMatrix = mat4::identity();
    projectionMatrix = mat4::identity();
    translateMatrix = mat4::identity();
    scaleMatrix = mat4::identity();
    translateMatrix = translate(0.0f, 0.0f, -3.0f);
    //translateMatrix = translate(1.08f, -1.0f, -3.0f);
    scaleMatrix = scale(sphereScale, sphereScale, sphereScale);

    modelMatrix = translateMatrix * scaleMatrix;

    projectionMatrix = perspectiveProjectionMatrix;

    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);


    drawSphere();



    // ***** 4th sphere on 6th row, yellow rubber *****


    glUniform3f(kAUniform, 0.05f, 0.05f, 0.0f);
    glUniform3f(kDUniform, 0.5f, 0.5f, 0.0f);
    glUniform3f(kSUniform, 0.7f, 0.7f, 0.04f);
    glUniform1f(kShininessUniform, 0.078125 * 128);

    glViewport(3 * vWidth / 4, 0, (GLsizei)vWidth / 4, (GLsizei)vHeight / 6);

    modelMatrix = mat4::identity();
    viewMatrix = mat4::identity();
    projectionMatrix = mat4::identity();
    translateMatrix = mat4::identity();
    scaleMatrix = mat4::identity();
    translateMatrix = translate(0.0f, 0.0f, -3.0f);
    //translateMatrix = translate(1.8f, -1.0f, -3.0f);
    scaleMatrix = scale(sphereScale, sphereScale, sphereScale);

    modelMatrix = translateMatrix * scaleMatrix;

    projectionMatrix = perspectiveProjectionMatrix;

    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);


    drawSphere();

}


-(void)myUpdate
{
    //code
    if (singleTap == 1)
    {
        AngleForXRotation += 0.05f;
    }
    else if (singleTap == 2)
    {
        AngleForYRotation += 0.05f;
    }
    else if (singleTap == 3)
    {
        AngleForZRotation += 0.05f;
    }
    
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
    singleTap++;
    if(singleTap>3)
    {
        singleTap = 0;
        AngleForXRotation = 0.0f;
        AngleForYRotation = 0.0f;
        AngleForZRotation = 0.0f;
    }
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
