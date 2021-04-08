//header files
#include<windows.h>
#include<stdio.h>

#include<gl\glew.h>

#include<GL/GL.h>
#include "Resources.h"
#include "vmath.h"


//macro functions
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "OpenGL32.lib")


#define WIN_WIDTH 800
#define WIN_HEIGHT 600

using namespace vmath;

//PROPERTIES OF VERTEX:
enum
{
	SPK_ATTRIBUTE_POSITION = 0,
	SPK_ATTRIBUTE_COLOR,
	SPK_ATTRIBUTE_NORMAL,
	SPK_ATTRIBUTE_TEXCOORD,
};


struct Light
{
	vec4 lightAmbient;
	vec4 lightDiffuse;
	vec4 lightSpecular;
	vec4 lightPosition;
};


//global function declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void Initialize(void);
void Display(void);
void Uninitialize(void);
void Update(void);

//global variable declaration
DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
bool gbFullscreen = false;
HWND ghwnd = NULL;
bool gbActiveWindow = false;
FILE* gpFile = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;

bool bLight;
struct Light light[2];

GLuint shaderProgramObject;

//vao for pyramid
GLuint vao_pyramid;                          //vertex array object
GLuint vbo_position_pyramid;                 //vertex buffer object
GLuint vbo_normals_pyramid;


//UNIFORMS

GLuint modelMatrixUniform;
GLuint viewMatrixUniform;
GLuint projectionMatrixUniform;

GLuint LKeyPressedUniform;

GLuint lAUniform[2];
GLuint lDUniform[2];
GLuint lSUniform[2];
GLuint lightPositionUniform[2];

GLuint kAUniform;
GLuint kDUniform;
GLuint kSUniform;

GLfloat kShininessUniform;


mat4 perspectiveProjectionMatrix;   //4x4 matrix

GLfloat pAngle = 0.0f;


//WinMain
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//local variable declaration
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("MyApp");
	bool bDone = false;
	int x, y, width, height;

	if (fopen_s(&gpFile, "RenderLog.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("Cannot Open The Desired File\n"), TEXT("Error"), MB_OK);
		exit(0);
	}
	else
	{
		fprintf(gpFile, ("Log File Created Successfully, Program Started Successfully.\n\n"));
	}

	//code
	width = GetSystemMetrics(SM_CXSCREEN);
	height = GetSystemMetrics(SM_CYSCREEN);

	x = (width / 2) - 400;
	y = (height / 2) - 300;

	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));

	RegisterClassEx(&wndclass);

	//CreateWindow
	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szAppName,
		TEXT("My Application - SHRUTI KULKARNI"),
		(WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE),
		x,
		y,
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ghwnd = hwnd;

	//funtion call
	Initialize();
	ShowWindow(hwnd, iCmdShow);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	//game loop
	while (bDone == false)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				bDone = true;
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if (gbActiveWindow == true)
			{
				//update function for OpenGL rendering

				//display function for OpenGL rendering
				Display();
				Update();
			}
		}
	}
	return(msg.wParam);
}

//WndProc
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//function declaration
	void ToggleFullscreen(void);
	void Resize(int, int);

	switch (iMsg)
	{
	case WM_SETFOCUS:
		gbActiveWindow = true;
		break;

	case WM_KILLFOCUS:
		gbActiveWindow = false;
		break;

	case WM_ERASEBKGND:
		return(0);

	case WM_SIZE:
		Resize(LOWORD(lParam), HIWORD(lParam));
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hwnd);
			break;

		case 0x46:
		case 0x66:
			ToggleFullscreen();
			break;
		}
		break;

	case WM_CHAR:
		switch (wParam)
		{
		case 'L':
		case 'l':
			if (bLight == true)
			{
				glDisable(GL_LIGHTING);
				bLight = false;

			}
			else
			{
				glEnable(GL_LIGHTING);
				bLight = true;
			}
			break;
		}
		break;

	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;

	case WM_DESTROY:
		Uninitialize();
		PostQuitMessage(0);
		break;
	}

	return(DefWindowProc(hwnd, iMsg, wParam, lParam));

}

void ToggleFullscreen(void)
{
	//local variable declaration
	MONITORINFO mi = { sizeof(MONITORINFO) };

	//code
	if (gbFullscreen == false)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);

		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, (dwStyle & ~WS_OVERLAPPEDWINDOW));
				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
			}

		}
		ShowCursor(false);
		gbFullscreen = true;
	}
	else
	{
		SetWindowLong(ghwnd, GWL_STYLE, (dwStyle | WS_OVERLAPPEDWINDOW));
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);

		ShowCursor(true);
		gbFullscreen = false;
	}
}

void Initialize(void)
{
	//function prototype
	void Resize(int, int);

	//variable declaration
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;
	GLuint vertexShaderObject;
	GLuint fragmentShaderObject;


	//code
	ghdc = GetDC(ghwnd);

	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	//struct pfd
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 32;

	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);

	if (iPixelFormatIndex == 0)
	{
		fprintf(gpFile, ("ChoosePixelFormat() Failed.\n"));
		DestroyWindow(ghwnd);
	}

	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
	{
		fprintf(gpFile, ("SetPixelFormat() Failed.\n"));
		DestroyWindow(ghwnd);
	}

	ghrc = wglCreateContext(ghdc);

	if (ghrc == NULL)
	{
		fprintf(gpFile, ("wglCreateContext() Failed.\n"));
		DestroyWindow(ghwnd);
	}

	if (wglMakeCurrent(ghdc, ghrc) == FALSE)
	{
		fprintf(gpFile, ("wglMakeCurrent() Failed\n"));
		DestroyWindow(ghwnd);
	}

	GLenum glew_error = glewInit();
	if (glew_error != GLEW_OK)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	//OpenGL related LOG

	fprintf(gpFile, "OpenGL VENDOR : %s\n", glGetString(GL_VENDOR));
	fprintf(gpFile, "OpenGL RENDERER : %s\n", glGetString(GL_RENDERER));
	fprintf(gpFile, "OpenGL VERSION : %s\n", glGetString(GL_VERSION));
	fprintf(gpFile, "GLSL[Graphics Library Shading Language] VERSION : %s\n\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	fprintf(gpFile, "EXTENTIONS : \n");

	//OpenGL Enabled Extensions

	GLint numExt;

	glGetIntegerv(GL_NUM_EXTENSIONS, &numExt);

	for (int i = 0; i < numExt; i++)
	{
		fprintf(gpFile, "%s\n", glGetStringi(GL_EXTENSIONS, i));
	}

	/********SHADERS********/


	/*****VERTEX SHADER*****/

	//create shader
	vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	//provide source code to vertex shader
	const GLchar* vertexShaderSourceCode =
		"#version 450 core \n" \
		"\n" \
		"in vec4 vPosition; \n" \
		"in vec3 vNormal; \n" \
		"uniform int u_lKeyPressed; \n" \
		"uniform vec3 u_lA[2]; \n" \
		"uniform vec3 u_lD[2]; \n" \
		"uniform vec3 u_lS[2]; \n" \
		"uniform vec4 u_light_position[2]; \n" \
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
		"\t if(u_lKeyPressed==1) \n" \
		"\t { \n" \
		"\t \t vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition; \n" \
		"\t \t vec3 transformed_normal = normalize(mat3(u_view_matrix * u_model_matrix) * vNormal); \n" \
		"\t \t vec3 view_vector = normalize(-eye_coordinates.xyz); \n" \
		"\t \t vec3 ambient[2]; \n" \
		"\t \t vec3 diffuse[2]; \n" \
		"\t \t vec3 specular[2]; \n" \
		"\t \t vec3 light_direction[2]; \n" \
		"\t \t vec3 reflection_vector[2]; \n" \
		"\n" \
		"\t \t for(int i=0; i<2; i++) \n" \
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
				DestroyWindow(ghwnd);
			}

		}
	}


	/*****FRAGMENT SHADER*****/

	//create shader
	fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	//provide source code to fragment shader
	const GLchar* fragmentShaderSourceCode =
		"#version 450 core \n" \
		"\n" \
		"in vec3 phong_ads_light; \n" \
		"out vec4 FragColor; \n" \
		"void main(void) \n" \
		"{ \n" \
		"FragColor = vec4(phong_ads_light, 1.0f); \n" \
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
				DestroyWindow(ghwnd);

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
				fprintf(gpFile, "\nShader Program Link Log : %s\n", szBuffer);
				free(szBuffer);
				DestroyWindow(ghwnd);
			}
		}
	}

	//get uniform location
	modelMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_model_matrix");
	viewMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_view_matrix");
	projectionMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_projection_matrix");

	LKeyPressedUniform = glGetUniformLocation(shaderProgramObject, "u_lKeyPressed");

	lAUniform[0] = glGetUniformLocation(shaderProgramObject, "u_lA[0]");
	lDUniform[0] = glGetUniformLocation(shaderProgramObject, "u_lD[0]");
	lSUniform[0] = glGetUniformLocation(shaderProgramObject, "u_lS[0]");
	lightPositionUniform[0] = glGetUniformLocation(shaderProgramObject, "u_light_position[0]");

	lAUniform[1] = glGetUniformLocation(shaderProgramObject, "u_lA[1]");
	lDUniform[1] = glGetUniformLocation(shaderProgramObject, "u_lD[1]");
	lSUniform[1] = glGetUniformLocation(shaderProgramObject, "u_lS[1]");
	lightPositionUniform[1] = glGetUniformLocation(shaderProgramObject, "u_light_position[1]");

	kAUniform = glGetUniformLocation(shaderProgramObject, "u_kA");
	kDUniform = glGetUniformLocation(shaderProgramObject, "u_kD");
	kSUniform = glGetUniformLocation(shaderProgramObject, "u_kS");
	kShininessUniform = glGetUniformLocation(shaderProgramObject, "u_materialShininess");



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


	//normals
	const GLfloat pyramidNormals[] = { 0.0f, 0.447214f, 0.894427f,
									   0.0f, 0.447214f, 0.894427f,
									   0.0f, 0.447214f, 0.894427f,
									   0.894427f, 0.447214f, 0.0f,
									   0.894427f, 0.447214f, 0.0f,
									   0.894427f, 0.447214f, 0.0f,
									   0.0f, 0.447214f, -0.894427f,
									   0.0f, 0.447214f, -0.894427f,
									   0.0f, 0.447214f, -0.894427f,
									  -0.894427f, 0.447214f, 0.0f,
									  -0.894427f, 0.447214f, 0.0f,
									  -0.894427f, 0.447214f, 0.0f };





	/*****PYRAMID VAO*****/

	glGenVertexArrays(1, &vao_pyramid);
	glBindVertexArray(vao_pyramid);

	glGenBuffers(1, &vbo_position_pyramid);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_pyramid);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(SPK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SPK_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vbo_normals_pyramid);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_normals_pyramid);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidNormals), pyramidNormals, GL_STATIC_DRAW);
	glVertexAttribPointer(SPK_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SPK_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);



	glBindVertexArray(0);



	//LIGHT INITIALIZATION


	light[0].lightAmbient = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	light[0].lightDiffuse = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	light[0].lightSpecular = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	light[0].lightPosition = vec4(2.0f, 0.0f, 0.0f, 1.0f);

	light[1].lightAmbient = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	light[1].lightDiffuse = vec4(0.0f, 0.0f, 1.0f, 1.0f);
	light[1].lightSpecular = vec4(0.0f, 0.0f, 1.0f, 1.0f);
	light[1].lightPosition = vec4(-2.0f, 0.0f, 0.0f, 1.0f);



	//depth
	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	//glEnable(GL_CULL_FACE);

	//SetClearColor
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	//set perspective matrix to identity matrix
	perspectiveProjectionMatrix = mat4::identity();

	Resize(WIN_WIDTH, WIN_HEIGHT);
}

void Resize(int width, int height)
{
	//code
	if (height == 0)
		height = 1;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	perspectiveProjectionMatrix = perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void Display(void)
{
	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//start using OpenGL program object
	glUseProgram(shaderProgramObject);


	if (bLight == true)
	{

		glUniform1i(LKeyPressedUniform, 1);

		/****GOURAUD****/

		glUniform3fv(lAUniform[0], 1, light[0].lightAmbient);
		glUniform3fv(lDUniform[0], 1, light[0].lightDiffuse);
		glUniform3fv(lSUniform[0], 1, light[0].lightSpecular);
		glUniform4fv(lightPositionUniform[0], 1, light[0].lightPosition);

		glUniform3fv(lAUniform[1], 1, light[1].lightAmbient);
		glUniform3fv(lDUniform[1], 1, light[1].lightDiffuse);
		glUniform3fv(lSUniform[1], 1, light[1].lightSpecular);
		glUniform4fv(lightPositionUniform[1], 1, light[1].lightPosition);

		glUniform3f(kAUniform, 0.0f, 0.0f, 0.0f);
		glUniform3f(kDUniform, 1.0f, 1.0f, 1.0f);
		glUniform3f(kSUniform, 1.0f, 1.0f, 1.0f);
		glUniform1f(kShininessUniform, 50.0);

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
		glUniform1i(LKeyPressedUniform, 0);
	}



	//OpenGL Drawing

	mat4 modelMatrix;
	mat4 viewMatrix;
	mat4 projectionMatrix;
	mat4 translateMatrix;
	mat4 rotateYMatrix;


	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	projectionMatrix = mat4::identity();
	translateMatrix = mat4::identity();
	rotateYMatrix = mat4::identity();
	translateMatrix = translate(0.0f, 0.0f, -3.0f);
	rotateYMatrix = rotate(pAngle, 0.0f, 1.0f, 0.0f);

	modelMatrix = translateMatrix * rotateYMatrix;

	//projectionMatrix = perspectiveProjectionMatrix * viewMatrix * modelMatrix;  //pre-multiplication of matrices
	projectionMatrix = perspectiveProjectionMatrix;

	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);


	//bind vao_pyramid
	glBindVertexArray(vao_pyramid);

	glDrawArrays(GL_TRIANGLES, 0, 12);

	//unbind vao_triangle
	glBindVertexArray(0);


	//stop using OpenGL program object
	glUseProgram(0);


	SwapBuffers(ghdc);
}

void Update(void)
{
	//code
	pAngle += 1.0f;
	if (pAngle >= 360.0f)
		pAngle = 0.0f;


}

void Uninitialize(void)
{
	//code
	if (gbFullscreen == true)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);

		SetWindowLong(ghwnd, GWL_STYLE, (dwStyle | WS_OVERLAPPEDWINDOW));
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);

		ShowCursor(true);
	}

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

	if (vbo_normals_pyramid)
	{
		glDeleteBuffers(1, &vbo_normals_pyramid);
		vbo_normals_pyramid = 0;
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



	//
	if (wglGetCurrentContext() == ghrc)
	{
		wglMakeCurrent(NULL, NULL);
	}

	//
	if (ghrc)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
	}

	//
	if (ghdc)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	//
	if (gpFile)
	{
		fprintf(gpFile, ("\nLog File Closed Successfully, Program Completed Successfully.\n"));
		fclose(gpFile);
		gpFile = NULL;
	}
}

