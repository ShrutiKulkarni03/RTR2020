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
bool pvLighting = true;
struct Light light[3];

GLuint shaderProgramObjectPV;
GLuint shaderProgramObjectPF;

GLuint vao;                          //vertex array object
GLuint vbo_sphere_position;                          //vertex buffer object
GLuint vbo_sphere_normal;
GLuint vbo_sphere_elements;

GLfloat sphere_vertices[1146];
GLfloat sphere_normals[1146];
GLfloat sphere_textures[764];
unsigned short sphere_elements[2280];
GLuint numVertices;
GLuint numElements;


//PV

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


//PF

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
GLfloat kShininessUniformPF;;

GLuint LKeyPressedUniform;



mat4 perspectiveProjectionMatrix;   //4x4 matrix

GLfloat lightAngle0 = 0.0f;
GLfloat lightAngle1 = 0.0f;
GLfloat lightAngle2 = 0.0f;


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
			ToggleFullscreen();
			break;
		}
		break;

	case WM_CHAR:
		switch (wParam)
		{
		case 'Q':
		case 'q':
			DestroyWindow(hwnd);
			break;

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

		case 'V':
		case 'v':
			pvLighting = true;
			break;


		case 'F':
		case 'f':
			pvLighting = false;
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
	GLuint vertexShaderObjectPV;
	GLuint fragmentShaderObjectPV;
	GLuint vertexShaderObjectPF;
	GLuint fragmentShaderObjectPF;


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


	//PER VERTEX LIGHTING


	/*****VERTEX SHADER*****/

	//create shader
	vertexShaderObjectPV = glCreateShader(GL_VERTEX_SHADER);

	//provide source code to vertex shader
	const GLchar* vertexShaderSourceCodePV =
		"#version 450 core \n" \
		"\n" \
		"in vec4 vPosition; \n" \
		"in vec3 vNormal; \n" \
		"uniform int u_lKeyPressed; \n" \
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
		"\t if(u_lKeyPressed==1) \n" \
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
				fprintf(gpFile, "\nVertex Shader Compilation Log of PV Lighting: %s\n", szBuffer);
				free(szBuffer);
				DestroyWindow(ghwnd);
			}

		}
	}


	/*****FRAGMENT SHADER*****/

	//create shader
	fragmentShaderObjectPV = glCreateShader(GL_FRAGMENT_SHADER);

	//provide source code to fragment shader
	const GLchar* fragmentShaderSourceCodePV =
		"#version 450 core \n" \
		"\n" \
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
				fprintf(gpFile, "\nFragment Shader Compilation Log of PV Lighting : %s\n", szBuffer);
				free(szBuffer);
				DestroyWindow(ghwnd);

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
	GLint shaderProgramLinkStatus = 0;
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
				fprintf(gpFile, "\nShader Program Link Log of PV Lighting : %s\n", szBuffer);
				free(szBuffer);
				DestroyWindow(ghwnd);
			}
		}
	}

	//get uniform location
	modelMatrixUniformPV = glGetUniformLocation(shaderProgramObjectPV, "u_model_matrix");
	viewMatrixUniformPV = glGetUniformLocation(shaderProgramObjectPV, "u_view_matrix");
	projectionMatrixUniformPV = glGetUniformLocation(shaderProgramObjectPV, "u_projection_matrix");

	LKeyPressedUniform = glGetUniformLocation(shaderProgramObjectPV, "u_lKeyPressed");

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



	/*******************************************************************************************************/


	//PER FRAGMENT LIGHTING


	/*****VERTEX SHADER*****/

	//create shader
	vertexShaderObjectPF = glCreateShader(GL_VERTEX_SHADER);

	//provide source code to vertex shader
	const GLchar* vertexShaderSourceCodePF =
		"#version 450 core \n" \
		"\n" \
		"in vec4 vPosition; \n" \
		"in vec3 vNormal; \n" \
		"uniform int u_lKeyPressed; \n" \
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
		"\t if(u_lKeyPressed==1) \n" \
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
	infoLogLength = 0;
	shaderCompiledStatus = 0;
	szBuffer = NULL;

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
				fprintf(gpFile, "\nVertex Shader Compilation Log of PF Lighting : %s\n", szBuffer);
				free(szBuffer);
				DestroyWindow(ghwnd);
			}

		}
	}


	/*****FRAGMENT SHADER*****/

	//create shader
	fragmentShaderObjectPF = glCreateShader(GL_FRAGMENT_SHADER);

	//provide source code to fragment shader
	const GLchar* fragmentShaderSourceCodePF =
		"#version 450 core \n" \
		"\n" \
		"in vec3 transformed_normal; \n" \
		"in vec3 light_direction[3]; \n" \
		"in vec3 view_vector; \n" \
		"uniform int u_lKeyPressed; \n" \
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
		"\t vec3 normalized_transformed_normals; \n" \
		"\t vec3 normalized_view_vector; \n" \
		"\t vec3 normalized_light_direction[3]; \n" \
		"\t vec3 reflection_vector[3]; \n" \
		"\t vec3 ambient[3]; \n" \
		"\t vec3 diffuse[3]; \n" \
		"\t vec3 specular[3]; \n" \
		"\n" \
		"\t if(u_lKeyPressed==1) \n" \
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
				fprintf(gpFile, "\nFragment Shader Compilation Log of PF Lighting : %s\n", szBuffer);
				free(szBuffer);
				DestroyWindow(ghwnd);

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
				fprintf(gpFile, "\nShader Program Link Log of PF Lighting: %s\n", szBuffer);
				free(szBuffer);
				DestroyWindow(ghwnd);
			}
		}
	}

	//get uniform location
	modelMatrixUniformPF = glGetUniformLocation(shaderProgramObjectPF, "u_model_matrix");
	viewMatrixUniformPF = glGetUniformLocation(shaderProgramObjectPF, "u_view_matrix");
	projectionMatrixUniformPF = glGetUniformLocation(shaderProgramObjectPF, "u_projection_matrix");

	LKeyPressedUniform = glGetUniformLocation(shaderProgramObjectPF, "u_lKeyPressed");

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



	/*******************************************************************************************************/





	//vertices array declaration

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



	//LIGHT INITIALIZATION


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
	GLfloat r = 5.0f;

	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
			glUniform1i(LKeyPressedUniform, 1);

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
			glUniform1i(LKeyPressedUniform, 1);

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
		glUniform1i(LKeyPressedUniform, 0);
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


	//bind vao
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_elements);
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);

	//unbind vao
	glBindVertexArray(0);


	//stop using OpenGL program object
	glUseProgram(0);


	SwapBuffers(ghdc);
}

void Update(void)
{
	lightAngle0 += 0.01f;
	if (lightAngle0 >= 360.0f)
	{
		lightAngle0 = 0.0f;
	}

	lightAngle1 += 0.01f;
	if (lightAngle1 >= 360.0f)
	{
		lightAngle1 = 0.0f;
	}

	lightAngle2 += 0.01f;
	if (lightAngle2 >= 360.0f)
	{
		lightAngle2 = 0.0f;
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

