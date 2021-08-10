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

    //vao for cube
    GLuint vao;
    GLuint vbo;
    
    GLuint modelMatrixUniform;
    GLuint viewMatrixUniform;
    GLuint projectionMatrixUniform;
    
    GLuint singleTapUniform;

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
            "in vec4 vColor; \n" \
            "in vec2 vTexcoord; \n" \
            "in vec3 vNormal; \n" \
            "out vec4 out_color; \n" \
            "uniform int u_singleTap; \n" \
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
            "    if(u_singleTap==1) \n" \
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
            "precision lowp int; \n" \
            "in vec4 out_color; \n" \
            "in vec3 transformed_normal; \n" \
            "in vec3 light_direction; \n" \
            "in vec3 view_vector; \n" \
            "in vec2 out_texcoord; \n" \
            "uniform sampler2D u_texture_sampler; \n" \
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
            "    if(u_singleTap==1) \n" \
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
                    printf("\nShader Program Link Log : %s\n", szBuffer);
                    free(szBuffer);
                    [self release];
                    [self uninitialize];
                }
            }
        }

        //get uniform location
        modelMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_model_matrix");
        viewMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_view_matrix");
        projectionMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_projection_matrix");

        singleTapUniform = glGetUniformLocation(shaderProgramObject, "u_singleTap");

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
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        //glEnable(GL_CULL_FACE);
        
        //variables initialization
        cubeAngle = 0.0f;
        bLight = false;
        
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        
        //texture
        cube_texture = [self loadTextureFromBMP:@"marble" :@"bmp"];
        
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

    if (bLight == true)
    {
        GLfloat lightPosition[] = { 100.0f, 100.0f, 100.0f, 1.0f };

        glUniform1i(singleTapUniform, 1);


        /****ALBEDO****/
        
        glUniform3f(lAUniform, 0.2f, 0.2f, 0.2f);
        glUniform3f(lDUniform, 1.0f, 1.0f, 1.0f);
        glUniform3f(lSUniform, 1.0f, 1.0f, 1.0f);
        glUniform4fv(lightPositionUniform, 1, (GLfloat*)lightPosition);

        glUniform1f(kShininessUniform, 128.0);
        
    }
    else
    {
        glUniform1i(singleTapUniform, 0);
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
    
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
    [eaglContext presentRenderbuffer:GL_RENDERBUFFER]; //swapbuffers
}

-(void)myUpdate
{
    //code
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
    if (bLight == true)
        bLight = false;
    else
        bLight = true;
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
