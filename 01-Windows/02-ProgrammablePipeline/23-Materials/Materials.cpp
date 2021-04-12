//header files
#include<windows.h>
#include<stdio.h>

#include<gl\glew.h>

#include<GL/GL.h>
#include "Resources.h"
#include "Sphere.h"
#include "vmath.h"


//macro functions
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "Sphere.lib")


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
vec4 lightAmbient;
vec4 lightDiffuse;
vec4 lightSpecular;
vec4 lightPosition;

GLuint shaderProgramObject;

GLuint vao;                          //vertex array object
GLuint vbo_sphere_position;          //vertex buffer object
GLuint vbo_sphere_normal;
GLuint vbo_sphere_elements;

//UNIFORMS

GLuint modelMatrixUniform;
GLuint viewMatrixUniform;
GLuint projectionMatrixUniform;

GLuint LKeyPressedUniform;
GLuint XKeyPressedUniform;
GLuint YKeyPressedUniform;
GLuint ZKeyPressedUniform;

GLuint lAUniform;
GLuint lDUniform;
GLuint lSUniform;
GLuint lightPositionUniform;

GLuint kAUniform;
GLuint kDUniform;
GLuint kSUniform;
GLfloat kShininessUniform;

GLint KeyPressed = 0;
GLfloat AngleForXRotation = 0.0f;
GLfloat AngleForYRotation = 0.0f;
GLfloat AngleForZRotation = 0.0f;


int vWidth;
int vHeight;

mat4 perspectiveProjectionMatrix;   //4x4 matrix


//SPHERES

GLfloat sphere_vertices[1146];
GLfloat sphere_normals[1146];
GLfloat sphere_textures[764];
unsigned short sphere_elements[2280];
GLuint numVertices;
GLuint numElements;



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
		100,
		100,
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

		case 'X':
		case 'x':
			KeyPressed = 1;
			AngleForXRotation = 0.0f;
			break;


		case 'Y':
		case 'y':
			KeyPressed = 2;
			AngleForYRotation = 0.0f;
			break;


		case 'Z':
		case 'z':
			KeyPressed = 3;
			AngleForZRotation = 0.0f;
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
		/*"uniform int u_xKeyPressed; \n" \
		"uniform int u_yKeyPressed; \n" \
		"uniform int u_zKeyPressed; \n" \*/
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
		"	if(u_lKeyPressed==1) \n" \
		"	{ \n" \
		"		vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition; \n" \
		"		transformed_normal = mat3(u_view_matrix * u_model_matrix) * vNormal; \n" \
		"		light_direction = vec3(u_light_position - eye_coordinates); \n" \
		"		view_vector = -eye_coordinates.xyz; \n" \
		"	} \n" \
		"\n" \
		"	gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition; \n" \
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
		"in vec3 transformed_normal; \n" \
		"in vec3 light_direction; \n" \
		"in vec3 view_vector; \n" \
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
		"if(u_lKeyPressed==1) \n" \
		"{ \n" \
		"vec3 normalized_transformed_normals = normalize(transformed_normal); \n" \
		"vec3 normalized_light_direction = normalize(light_direction); \n" \
		"vec3 normalized_view_vector = normalize(view_vector); \n" \
		"vec3 reflection_vector = reflect(-normalized_light_direction, normalized_transformed_normals); \n" \
		"vec3 ambient = u_lA * u_kA; \n" \
		"vec3 diffuse = u_lD * u_kD * max(dot(normalized_light_direction, normalized_transformed_normals), 0.0); \n" \
		"vec3 specular = u_lS * u_kS * pow(max(dot(reflection_vector, normalized_view_vector), 0.0), u_materialShininess); \n" \
		"phong_ads_light = ambient + diffuse + specular; \n" \
		"} \n" \
		"else \n" \
		"{ \n" \
		"phong_ads_light = vec3(1.0, 1.0, 1.0); \n" \
		"} \n" \
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

	lAUniform = glGetUniformLocation(shaderProgramObject, "u_lA");
	lDUniform = glGetUniformLocation(shaderProgramObject, "u_lD");
	lSUniform = glGetUniformLocation(shaderProgramObject, "u_lS");
	lightPositionUniform = glGetUniformLocation(shaderProgramObject, "u_light_position");

	kAUniform = glGetUniformLocation(shaderProgramObject, "u_kA");
	kDUniform = glGetUniformLocation(shaderProgramObject, "u_kD");
	kSUniform = glGetUniformLocation(shaderProgramObject, "u_kS");
	kShininessUniform = glGetUniformLocation(shaderProgramObject, "u_materialShininess");


	getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
	numVertices = getNumberOfSphereVertices();
	numElements = getNumberOfSphereElements();

	//VAO

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo_sphere_position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_sphere_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(SPK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SPK_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glGenBuffers(1, &vbo_sphere_normal);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_sphere_normal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);
	glVertexAttribPointer(SPK_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SPK_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glGenBuffers(1, &vbo_sphere_elements);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_elements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	lightAmbient = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	lightDiffuse = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	lightSpecular = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	lightPosition = vec4(0.0f, 0.0f, 0.0f, 1.0f);

	//depth
	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_CULL_FACE);

	//SetClearColor
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	//set perspective matrix to identity matrix
	perspectiveProjectionMatrix = mat4::identity();

	Resize(WIN_WIDTH, WIN_HEIGHT);
}

void Resize(int width, int height)
{
	vWidth = width;
	vHeight = height;

	//code
	if (height == 0)
		height = 1;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void Display(void)
{

	mat4 modelMatrix;
	mat4 viewMatrix;
	mat4 projectionMatrix;
	mat4 translateMatrix;
	mat4 scaleMatrix;

	GLfloat sphereScale = 1.0f;


	GLfloat r = 5.0f;

	void drawSpheres(void);

	
	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//start using OpenGL program object
	glUseProgram(shaderProgramObject);


	if (bLight == true)
	{

		glUniform1i(LKeyPressedUniform, 1);

		/****GOURAUD****/


		glUniform3fv(lAUniform, 1, lightAmbient);
		glUniform3fv(lDUniform, 1, lightDiffuse);
		glUniform3fv(lSUniform, 1, lightSpecular);
		glUniform4fv(lightPositionUniform, 1, lightPosition);

	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}

	if (KeyPressed == 1)
	{
		lightPosition[0] = 0.0f;
		lightPosition[1] = r * sin(AngleForXRotation);
		lightPosition[2] = r * cos(AngleForXRotation);
		lightPosition[3] = 1.0f;

	}
	else if (KeyPressed == 2)
	{
		lightPosition[0] = r * sin(AngleForYRotation);
		lightPosition[1] = 0.0f;
		lightPosition[2] = r * cos(AngleForYRotation);
		lightPosition[3] = 1.0f;
	}
	else if (KeyPressed == 3)
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

	//OpenGL Drawing

	drawSpheres();

	
	//stop using OpenGL program object
	glUseProgram(0);


	SwapBuffers(ghdc);
}

void drawSpheres(void)
{
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


	glViewport(0, 5 * vHeight / 6, (GLsizei)vWidth / 4, (GLsizei)vHeight / 6); 
	//perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)vWidth / (GLfloat)vHeight, 0.1f, 100.0f);
	
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



	//bind vao
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_elements);
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);

	//unbind vao
	glBindVertexArray(0);




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
	translateMatrix = translate(-1.08f, 1.0f, -3.0f);
	translateMatrix = translate(0.0f, 0.0f, -3.0f);
	scaleMatrix = scale(sphereScale, sphereScale, sphereScale);

	modelMatrix = translateMatrix * scaleMatrix;

	projectionMatrix = perspectiveProjectionMatrix;

	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);


	//bind vao
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_elements);
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);

	//unbind vao
	glBindVertexArray(0);





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


	//bind vao
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_elements);
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);

	//unbind vao
	glBindVertexArray(0);




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


	//bind vao
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_elements);
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);

	//unbind vao
	glBindVertexArray(0);



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


	//bind vao
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_elements);
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);

	//unbind vao
	glBindVertexArray(0);




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


	//bind vao
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_elements);
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);

	//unbind vao
	glBindVertexArray(0);








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


	//bind vao
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_elements);
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);

	//unbind vao
	glBindVertexArray(0);


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


	//bind vao
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_elements);
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);

	//unbind vao
	glBindVertexArray(0);




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


	//bind vao
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_elements);
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);

	//unbind vao
	glBindVertexArray(0);




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


	//bind vao
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_elements);
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);

	//unbind vao
	glBindVertexArray(0);



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


	//bind vao
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_elements);
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);

	//unbind vao
	glBindVertexArray(0);




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


	//bind vao
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_elements);
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);

	//unbind vao
	glBindVertexArray(0);








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


	//bind vao
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_elements);
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);

	//unbind vao
	glBindVertexArray(0);




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


	//bind vao
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_elements);
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);

	//unbind vao
	glBindVertexArray(0);





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


	//bind vao
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_elements);
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);

	//unbind vao
	glBindVertexArray(0);




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


	//bind vao
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_elements);
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);

	//unbind vao
	glBindVertexArray(0);



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


	//bind vao
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_elements);
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);

	//unbind vao
	glBindVertexArray(0);





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


	//bind vao
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_elements);
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);

	//unbind vao
	glBindVertexArray(0);




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


	//bind vao
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_elements);
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);

	//unbind vao
	glBindVertexArray(0);




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


	//bind vao
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_elements);
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);

	//unbind vao
	glBindVertexArray(0);




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


	//bind vao
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_elements);
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);

	//unbind vao
	glBindVertexArray(0);




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


	//bind vao
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_elements);
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);

	//unbind vao
	glBindVertexArray(0);



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


	//bind vao
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_elements);
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);

	//unbind vao
	glBindVertexArray(0);




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


	//bind vao
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_elements);
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);

	//unbind vao
	glBindVertexArray(0);


}

void Update(void)
{
	if (KeyPressed == 1)
	{
		AngleForXRotation += 0.01f;
	}
	else if (KeyPressed == 2)
	{
		AngleForYRotation += 0.01f;
	}
	else if (KeyPressed == 3)
	{
		AngleForZRotation += 0.01f;
	}
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

	if (vao)
	{
		glDeleteVertexArrays(1, &vao);
		vao = 0;
	}

	if (vbo_sphere_position)
	{
		glDeleteBuffers(1, &vbo_sphere_position);
		vbo_sphere_position = 0;
	}

	if (vbo_sphere_normal)
	{
		glDeleteBuffers(1, &vbo_sphere_normal);
		vbo_sphere_normal = 0;
	}

	if (vbo_sphere_elements)
	{
		glDeleteBuffers(1, &vbo_sphere_elements);
		vbo_sphere_elements = 0;
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

