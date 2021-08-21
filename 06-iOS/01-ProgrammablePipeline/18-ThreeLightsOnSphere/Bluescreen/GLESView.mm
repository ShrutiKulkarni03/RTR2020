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
    struct Light
    {
        vec4 lightAmbient;
        vec4 lightDiffuse;
        vec4 lightSpecular;
        vec4 lightPosition;
    };

    //PV
    GLuint shaderProgramObjectPV;
    GLuint vertexShaderObjectPV;
    GLuint fragmentShaderObjectPV;
    
    GLuint modelMatrixUniformPV;
    GLuint viewMatrixUniformPV;
    GLuint projectionMatrixUniformPV;

    GLuint lAUniformPV[3];
    GLuint lDUniformPV[3];
    GLuint lSUniformPV[3];
    GLuint lightPositionUniformPV[3];

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

    GLuint lAUniformPF[3];
    GLuint lDUniformPF[3];
    GLuint lSUniformPF[3];
    GLuint lightPositionUniformPF[3];

    GLuint kAUniformPF;
    GLuint kDUniformPF;
    GLuint kSUniformPF;
    GLfloat kShininessUniformPF;
    
    GLuint singleTapUniformPF;

    vmath::mat4 perspectiveProjectionMatrix;   //4x4 matrix
    
    bool bLight;
    bool pvLighting;
    struct Light light[3];
    
    GLfloat lightAngle0;
    GLfloat lightAngle1;
    GLfloat lightAngle2;
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
            "uniform vec3 u_lA[3]; \n" \
            "uniform vec3 u_lD[3]; \n" \
            "uniform vec3 u_lS[3]; \n" \
            "uniform vec4 u_light_position[3]; \n" \
            "uniform vec3 u_kA; \n" \
            "uniform vec3 u_kD; \n" \
            "uniform vec3 u_kS; \n" \
            "uniform float u_materialShininess; \n" \
            "uniform mat4 u_projection_matrix; \n" \
            "uniform mat4 u_view_matrix; \n" \
            "uniform mat4 u_model_matrix; \n" \
            "out vec3 phong_ads_light; \n" \
            "void main(void) \n" \
            "{ \n" \
            "   phong_ads_light = vec3(0.0, 0.0, 0.0); \n" \
            "\t if(u_singleTap==1) \n" \
            "\t { \n" \
            "\t \t vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition; \n" \
            "\t \t vec3 transformed_normal = normalize(mat3(u_view_matrix * u_model_matrix) * vNormal); \n" \
            "\t \t vec3 view_vector = normalize(-eye_coordinates.xyz); \n" \
            "\t \t vec3 ambient[3]; \n" \
            "\t \t vec3 diffuse[3]; \n" \
            "\t \t vec3 specular[3]; \n" \
            "\t \t vec3 light_direction[3]; \n" \
            "\t \t vec3 reflection_vector[3]; \n" \
            "\n" \
            "\t \t for(int i=0; i<3; i++) \n" \
            "\t \t { \n" \
            "\t \t \t light_direction[i] = normalize(vec3(u_light_position[i] -eye_coordinates)); \n" \
            "\t \t \t reflection_vector[i] = reflect(-light_direction[i], transformed_normal); \n" \
            "\t \t \t ambient[i] = u_lA[i] * u_kA; \n" \
            "\t \t \t diffuse[i] = u_lD[i] * u_kD * max(dot(light_direction[i], transformed_normal), 0.0); \n" \
            "\t \t \t specular[i] = u_lS[i] * u_kS * pow(max(dot(reflection_vector[i], view_vector), 0.0), u_materialShininess); \n" \
            "\t \t \t phong_ads_light = phong_ads_light + ambient[i] + diffuse[i] + specular[i]; \n" \
            "\t \t } \n" \
            "\t } \n" \
            "\t else \n" \
            "\t { \n" \
            "\t \t phong_ads_light = vec3(1.0, 1.0, 1.0); \n" \
            "\t } \n" \
            "\n" \
            "\t gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition; \n" \
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
            "\t FragColor = vec4(phong_ads_light, 1.0f); \n" \
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

        lAUniformPV[0] = glGetUniformLocation(shaderProgramObjectPV, "u_lA[0]");
        lDUniformPV[0] = glGetUniformLocation(shaderProgramObjectPV, "u_lD[0]");
        lSUniformPV[0] = glGetUniformLocation(shaderProgramObjectPV, "u_lS[0]");
        lightPositionUniformPV[0] = glGetUniformLocation(shaderProgramObjectPV, "u_light_position[0]");

        lAUniformPV[1] = glGetUniformLocation(shaderProgramObjectPV, "u_lA[1]");
        lDUniformPV[1] = glGetUniformLocation(shaderProgramObjectPV, "u_lD[1]");
        lSUniformPV[1] = glGetUniformLocation(shaderProgramObjectPV, "u_lS[1]");
        lightPositionUniformPV[1] = glGetUniformLocation(shaderProgramObjectPV, "u_light_position[1]");

        lAUniformPV[2] = glGetUniformLocation(shaderProgramObjectPV, "u_lA[2]");
        lDUniformPV[2] = glGetUniformLocation(shaderProgramObjectPV, "u_lD[2]");
        lSUniformPV[2] = glGetUniformLocation(shaderProgramObjectPV, "u_lS[2]");
        lightPositionUniformPV[2] = glGetUniformLocation(shaderProgramObjectPV, "u_light_position[2]");

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
            "uniform vec4 u_light_position[3]; \n" \
            "uniform mat4 u_projection_matrix; \n" \
            "uniform mat4 u_model_matrix; \n" \
            "uniform mat4 u_view_matrix; \n" \
            "out vec3 transformed_normal; \n" \
            "out vec3 light_direction[3]; \n" \
            "out vec3 view_vector; \n" \
            "\n" \
            "void main(void) \n" \
            "{ \n" \
            "\t if(u_singleTap==1) \n" \
            "\t { \n" \
            "\t \t vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition; \n" \
            "\t \t transformed_normal = mat3(u_view_matrix * u_model_matrix) * vNormal; \n" \
            "\t \t view_vector = -eye_coordinates.xyz; \n" \
            "\n" \
            "\t \t for(int i = 0; i < 3; i++) \n" \
            "\t \t { \n" \
            "\t \t \t light_direction[i] = vec3(u_light_position[i] - eye_coordinates); \n" \
            "\t \t } \n" \
            "\t } \n" \
            "\t gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition; \n" \
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
            "in vec3 light_direction[3]; \n" \
            "in vec3 view_vector; \n" \
            "uniform int u_singleTap; \n" \
            "uniform vec3 u_lA[3]; \n" \
            "uniform vec3 u_lD[3]; \n" \
            "uniform vec3 u_lS[3]; \n" \
            "uniform vec3 u_kA; \n" \
            "uniform vec3 u_kD; \n" \
            "uniform vec3 u_kS; \n" \
            "uniform float u_materialShininess; \n" \
            "out vec4 FragColor; \n" \
            "vec3 phong_ads_light; \n" \
            "void main(void) \n" \
            "{ \n" \
            "   phong_ads_light = vec3(0.0, 0.0, 0.0); \n" \
            "\t vec3 normalized_transformed_normals; \n" \
            "\t vec3 normalized_view_vector; \n" \
            "\t vec3 normalized_light_direction[3]; \n" \
            "\t vec3 reflection_vector[3]; \n" \
            "\t vec3 ambient[3]; \n" \
            "\t vec3 diffuse[3]; \n" \
            "\t vec3 specular[3]; \n" \
            "\n" \
            "\t if(u_singleTap==1) \n" \
            "\t { \n" \
            "\t \t normalized_transformed_normals = normalize(transformed_normal); \n" \
            "\t \t normalized_view_vector = normalize(view_vector); \n" \
            "\n" \
            "\t \t for(int i = 0; i < 3; i++) \n" \
            "\t \t { \n" \
            "\t \t \t normalized_light_direction[i] = normalize(light_direction[i]); \n" \
            "\t \t \t reflection_vector[i] = reflect(-normalized_light_direction[i], normalized_transformed_normals); \n" \
            "\t \t \t ambient[i] = u_lA[i] * u_kA; \n" \
            "\t \t \t diffuse[i] = u_lD[i] * u_kD * max(dot(normalized_light_direction[i], normalized_transformed_normals), 0.0); \n" \
            "\t \t \t specular[i] = u_lS[i] * u_kS * pow(max(dot(reflection_vector[i], normalized_view_vector), 0.0), u_materialShininess); \n" \
            "\t \t \t phong_ads_light += ambient[i] + diffuse[i] + specular[i]; \n" \
            "\t \t } \n" \
            "\t } \n" \
            "\t else \n" \
            "\t { \n" \
            "\t \t phong_ads_light = vec3(1.0, 1.0, 1.0); \n" \
            "\t } \n" \
            "\n" \
            "\t FragColor = vec4(phong_ads_light, 1.0f); \n" \
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

        lAUniformPF[0] = glGetUniformLocation(shaderProgramObjectPF, "u_lA[0]");
        lDUniformPF[0] = glGetUniformLocation(shaderProgramObjectPF, "u_lD[0]");
        lSUniformPF[0] = glGetUniformLocation(shaderProgramObjectPF, "u_lS[0]");
        lightPositionUniformPF[0] = glGetUniformLocation(shaderProgramObjectPF, "u_light_position[0]");

        lAUniformPF[1] = glGetUniformLocation(shaderProgramObjectPF, "u_lA[1]");
        lDUniformPF[1] = glGetUniformLocation(shaderProgramObjectPF, "u_lD[1]");
        lSUniformPF[1] = glGetUniformLocation(shaderProgramObjectPF, "u_lS[1]");
        lightPositionUniformPF[1] = glGetUniformLocation(shaderProgramObjectPF, "u_light_position[1]");

        lAUniformPF[2] = glGetUniformLocation(shaderProgramObjectPF, "u_lA[2]");
        lDUniformPF[2] = glGetUniformLocation(shaderProgramObjectPF, "u_lD[2]");
        lSUniformPF[2] = glGetUniformLocation(shaderProgramObjectPF, "u_lS[2]");
        lightPositionUniformPF[2] = glGetUniformLocation(shaderProgramObjectPF, "u_light_position[2]");

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
        
        light[0].lightAmbient = vec4(0.0f, 0.0f, 0.0f, 1.0f);
        light[0].lightDiffuse = vec4(1.0f, 0.0f, 0.0f, 1.0f);
        light[0].lightSpecular = vec4(1.0f, 0.0f, 0.0f, 1.0f);
        light[0].lightPosition = vec4(2.0f, 0.0f, 0.0f, 1.0f);

        light[1].lightAmbient = vec4(0.0f, 0.0f, 0.0f, 1.0f);
        light[1].lightDiffuse = vec4(0.0f, 1.0f, 0.0f, 1.0f);
        light[1].lightSpecular = vec4(0.0f, 1.0f, 0.0f, 1.0f);
        light[1].lightPosition = vec4(0.0f, 0.0f, 0.0f, 1.0f);

        light[2].lightAmbient = vec4(0.0f, 0.0f, 0.0f, 1.0f);
        light[2].lightDiffuse = vec4(0.0f, 0.0f, 1.0f, 1.0f);
        light[2].lightSpecular = vec4(0.0f, 0.0f, 1.0f, 1.0f);
        light[2].lightPosition = vec4(-2.0f, 0.0f, 0.0f, 1.0f);

        
        lightAngle0 = 0.0f;
        lightAngle1 = 0.0f;
        lightAngle2 = 0.0f;
        
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
    
    GLfloat r = 5.0f;
    
    //start using OpenGL program object
    if (pvLighting == true)
    {
        glUseProgram(shaderProgramObjectPV);
    }

    else
    {
        glUseProgram(shaderProgramObjectPF);
    }


    if (bLight == true)
    {
        if (pvLighting == true)
        {
            glUniform1i(singleTapUniformPV, 1);

            /****GOURAUD****/

            glUniform3fv(lAUniformPV[0], 1, light[0].lightAmbient);
            glUniform3fv(lDUniformPV[0], 1, light[0].lightDiffuse);
            glUniform3fv(lSUniformPV[0], 1, light[0].lightSpecular);
            glUniform4fv(lightPositionUniformPV[0], 1, light[0].lightPosition);

            glUniform3fv(lAUniformPV[1], 1, light[1].lightAmbient);
            glUniform3fv(lDUniformPV[1], 1, light[1].lightDiffuse);
            glUniform3fv(lSUniformPV[1], 1, light[1].lightSpecular);
            glUniform4fv(lightPositionUniformPV[1], 1, light[1].lightPosition);

            glUniform3fv(lAUniformPV[2], 1, light[2].lightAmbient);
            glUniform3fv(lDUniformPV[2], 1, light[2].lightDiffuse);
            glUniform3fv(lSUniformPV[2], 1, light[2].lightSpecular);
            glUniform4fv(lightPositionUniformPV[2], 1, light[2].lightPosition);

            glUniform3f(kAUniformPV, 0.0f, 0.0f, 0.0f);
            glUniform3f(kDUniformPV, 1.0f, 1.0f, 1.0f);
            glUniform3f(kSUniformPV, 1.0f, 1.0f, 1.0f);
            glUniform1f(kShininessUniformPV, 50.0);

            ///****ALBEDO****/
            //
            //glUniform3f(lAUniform, 0.1f, 0.1f, 0.1f);
            //glUniform3f(lDUniform, 1.0f, 1.0f, 1.0f);
            //glUniform3f(lSUniform, 1.0f, 1.0f, 1.0f);
            //glUniform4fv(lightPositionUniform, 1, (GLfloat*)lightPosition);

            //glUniform3f(kAUniform, 0.0f, 0.0f, 0.0f);
            //glUniform3f(kDUniform, 0.5f, 0.2f, 0.7f);
            //glUniform3f(kSUniform, 0.7f, 0.7f, 0.7f);
            //glUniform1f(kShininessUniform, 128.0);

        }

        else
        {
            glUniform1i(singleTapUniformPF, 1);

            /****GOURAUD****/

            glUniform3fv(lAUniformPF[0], 1, light[0].lightAmbient);
            glUniform3fv(lDUniformPF[0], 1, light[0].lightDiffuse);
            glUniform3fv(lSUniformPF[0], 1, light[0].lightSpecular);
            glUniform4fv(lightPositionUniformPF[0], 1, light[0].lightPosition);

            glUniform3fv(lAUniformPF[1], 1, light[1].lightAmbient);
            glUniform3fv(lDUniformPF[1], 1, light[1].lightDiffuse);
            glUniform3fv(lSUniformPF[1], 1, light[1].lightSpecular);
            glUniform4fv(lightPositionUniformPF[1], 1, light[1].lightPosition);

            glUniform3fv(lAUniformPF[2], 1, light[2].lightAmbient);
            glUniform3fv(lDUniformPF[2], 1, light[2].lightDiffuse);
            glUniform3fv(lSUniformPF[2], 1, light[2].lightSpecular);
            glUniform4fv(lightPositionUniformPF[2], 1, light[2].lightPosition);

            glUniform3f(kAUniformPF, 0.0f, 0.0f, 0.0f);
            glUniform3f(kDUniformPF, 1.0f, 1.0f, 1.0f);
            glUniform3f(kSUniformPF, 1.0f, 1.0f, 1.0f);
            glUniform1f(kShininessUniformPF, 128.0);

            ///****ALBEDO****/
            //
            //glUniform3f(lAUniform, 0.1f, 0.1f, 0.1f);
            //glUniform3f(lDUniform, 1.0f, 1.0f, 1.0f);
            //glUniform3f(lSUniform, 1.0f, 1.0f, 1.0f);
            //glUniform4fv(lightPositionUniform, 1, (GLfloat*)lightPosition);

            //glUniform3f(kAUniform, 0.0f, 0.0f, 0.0f);
            //glUniform3f(kDUniform, 0.5f, 0.2f, 0.7f);
            //glUniform3f(kSUniform, 0.7f, 0.7f, 0.7f);
            //glUniform1f(kShininessUniform, 128.0);

        }

        


        light[0].lightPosition[0] = 0.0f;
        light[0].lightPosition[1] = r * sin(lightAngle0);
        light[0].lightPosition[2] = r * cos(lightAngle0);
        light[0].lightPosition[3] = 1.0f;

        light[1].lightPosition[0] = r * sin(lightAngle1);
        light[1].lightPosition[1] = 0.0f;
        light[1].lightPosition[2] = r * cos(lightAngle1);
        light[1].lightPosition[3] = 1.0f;

        light[2].lightPosition[0] = r * sin(lightAngle2);
        light[2].lightPosition[1] = r * cos(lightAngle2);
        light[2].lightPosition[2] = 0.0f;
        light[2].lightPosition[3] = 1.0f;



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


    modelMatrix = mat4::identity();
    viewMatrix = mat4::identity();
    projectionMatrix = mat4::identity();
    translateMatrix = mat4::identity();
    translateMatrix = translate(0.0f, 0.0f, -3.0f);

    modelMatrix = translateMatrix;

    //projectionMatrix = perspectiveProjectionMatrix * viewMatrix * modelMatrix;  //pre-multiplication of matrices
    projectionMatrix = perspectiveProjectionMatrix;

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
    lightAngle0 += 0.05f;
    if (lightAngle0 >= 360.0f)
    {
        lightAngle0 = 0.0f;
    }

    lightAngle1 += 0.05f;
    if (lightAngle1 >= 360.0f)
    {
        lightAngle1 = 0.0f;
    }

    lightAngle2 += 0.05f;
    if (lightAngle2 >= 360.0f)
    {
        lightAngle2 = 0.0f;
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
