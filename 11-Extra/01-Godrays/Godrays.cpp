//header files
#include<windows.h>
#include<stdio.h>

#include<gl\glew.h>
#include <SOIL.h>
#include<GL/GL.h>

#include "Resources.h"
#include "vmath.h"
#include "SphereMesh.h"

//macro functions
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "soil.lib")

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

using namespace vmath;

//PROPERTIES OF VERTEX:
//enum
//{
//	SPK_ATTRIBUTE_POSITION = 0,
//	SPK_ATTRIBUTE_COLOR,
//	SPK_ATTRIBUTE_NORMAL,
//	SPK_ATTRIBUTE_TEXCOORD,
//};

//global function declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void Initialize(void);
void Display(void);
void Uninitialize(void);
GLuint LoadImageAsTexture(const char*);
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

int gWidth, gHeight;

//shader variables

//scene light shader
GLuint sceneShaderProgramObject;

GLuint modelMatrixUniformS;
GLuint viewMatrixUniformS;
GLuint projectionMatrixUniformS;

GLuint passUniformS;
GLuint textureSamplerUniformS;
GLuint lAUniformS;
GLuint lDUniformS;
GLuint lSUniformS;
GLuint lightPositionUniformS;
GLfloat kShininessUniformS;

//godrays shader
GLuint godraysShaderProgramObject;

GLuint modelMatrixUniformG;
GLuint viewMatrixUniformG;
GLuint projectionMatrixUniformG;

GLuint occlusionTextureSamplerUniformG;
GLuint densityUniformG;
GLuint weightUniformG;
GLuint decayUniformG;
GLuint exposureUniformG;
GLuint lightPositionUniformG;
GLuint numSamplesUniformG;

//final pass shader
GLuint finalPassShaderProgramObject;

GLuint pass1TextureSamplerUniformF;
GLuint pass2TextureSamplerUniformF;

mat4 perspectiveProjectionMatrix;   //4x4 matrix

//scene variables
GLuint fbo_pass1;
GLuint silhouetteTexture_pass1;
GLuint rbo_pass1;

GLuint fbo_pass2;
GLuint colorTexture_pass2;
GLuint rbo_pass2;

GLuint fbo_godrays;
GLuint colorBuffer;

//vao for square
GLuint vao_square;
GLuint vbo_position_square;
GLuint vbo_texture_square;
GLuint vbo_normal_square;

//update variables
GLfloat sunPosX = 0.0f;
GLfloat sunPosY = 0.0f;

//texture variables
GLuint textureID;
GLuint pngTexture;


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
	bool loadGLTexture(GLuint*, TCHAR[]);

	//variable declaration
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;

	GLuint sceneVertexShaderObject;
	GLuint sceneFragmentShaderObject;

	GLuint godraysVertexShaderObject;
	GLuint godraysFragmentShaderObject;

	GLuint finalPassVertexShaderObject;
	GLuint finalPassFragmentShaderObject;

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
	sceneVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	//provide source code to vertex shader
	const GLchar* sceneVertexShaderSourceCode =
		"#version 450 core \n" \
		"\n" \
		"in vec4 vPosition; \n" \
		"in vec4 vColor; \n" \
		"in vec2 vTexcoord; \n" \
		"in vec3 vNormal; \n" \
		"uniform vec4 u_light_position; \n" \
		"uniform mat4 u_projection_matrix; \n" \
		"uniform mat4 u_model_matrix; \n" \
		"uniform mat4 u_view_matrix; \n" \
		"out vec4 out_color; \n" \
		"out vec2 out_texcoord; \n" \
		"out vec3 transformed_normal; \n" \
		"out vec3 light_direction; \n" \
		"out vec3 view_vector; \n" \
		"void main(void) \n" \
		"{ \n" \
		"	vec4 eye_coordinates = u_view_matrix * u_model_matrix * vPosition; \n" \
		"	transformed_normal = mat3(u_view_matrix * u_model_matrix) * vNormal; \n" \
		"	light_direction = vec3(u_light_position - eye_coordinates); \n" \
		"	view_vector = -eye_coordinates.xyz; \n" \
		"	gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition; \n" \
		"	out_color = vColor; \n" \
		"	out_texcoord = vTexcoord; \n" \
		"} \n";

	glShaderSource(sceneVertexShaderObject, 1, (const GLchar**)&sceneVertexShaderSourceCode, NULL);

	//compile shader
	glCompileShader(sceneVertexShaderObject);

	//vertex shader compilation error checking
	GLint infoLogLength = 0;
	GLint shaderCompiledStatus = 0;
	char* szBuffer = NULL;

	glGetShaderiv(sceneVertexShaderObject, GL_COMPILE_STATUS, &shaderCompiledStatus);

	if (shaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(sceneVertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);

		if (infoLogLength > 0)
		{
			szBuffer = (char*)malloc(infoLogLength);
			if (szBuffer != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(sceneVertexShaderObject, infoLogLength, &written, szBuffer);
				fprintf(gpFile, "\nS: Vertex Shader Compilation Log : %s\n", szBuffer);
				free(szBuffer);
				DestroyWindow(ghwnd);
			}

		}
	}


	/*****FRAGMENT SHADER*****/

	//create shader
	sceneFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	//provide source code to fragment shader
	const GLchar* sceneFragmentShaderSourceCode =
		"#version 450 core \n" \
		"\n" \
		"in vec4 out_color; \n" \
		"in vec3 transformed_normal; \n" \
		"in vec3 light_direction; \n" \
		"in vec3 view_vector; \n" \
		"in vec2 out_texcoord; \n" \
		"uniform int u_pass; \n" \
		"uniform sampler2D u_texture_sampler; \n" \
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
		"	if(u_pass == 1) \n" \
		"	{ \n" \
		"		vec4 texColor = texture(u_texture_sampler, out_texcoord); \n" \
		"		if(texColor.a < 0.1) \n" \
		"			discard; \n" \
		"		FragColor = out_color; \n" \
		"	} \n" \

		"	else \n" \
		"	{ \n" \
		"		vec4 texColor = texture(u_texture_sampler, out_texcoord); \n" \
		"		if(texColor.a < 0.1) \n" \
		"			discard; \n" \

		"		vec3 normalized_transformed_normals = normalize(transformed_normal); \n" \
		"		vec3 normalized_light_direction = normalize(light_direction); \n" \
		"		vec3 normalized_view_vector = normalize(view_vector); \n" \
		"		vec3 reflection_vector = reflect(-normalized_light_direction, normalized_transformed_normals); \n" \
		"		vec3 ambient = u_lA * texColor.rgb; \n" \
		"		vec3 diffuse = u_lD * texColor.rgb * max(dot(normalized_light_direction, normalized_transformed_normals), 0.0); \n" \
		"		vec3 specular = u_lS * pow(max(dot(reflection_vector, normalized_view_vector), 0.0), u_materialShininess); \n" \
		"		phong_ads_light = ambient + diffuse + specular; \n" \
		"		FragColor = vec4(phong_ads_light, 1.0f); \n" \
		"	} \n" \

		"} \n";

	glShaderSource(sceneFragmentShaderObject, 1, (const GLchar**)&sceneFragmentShaderSourceCode, NULL);

	//compile shader
	glCompileShader(sceneFragmentShaderObject);

	//fragment shader compilation error checking
	infoLogLength = 0;
	shaderCompiledStatus = 0;
	szBuffer = NULL;

	glGetShaderiv(sceneFragmentShaderObject, GL_COMPILE_STATUS, &shaderCompiledStatus);

	if (shaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(sceneFragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);

		if (infoLogLength > 0)
		{
			szBuffer = (char*)malloc(infoLogLength);
			if (szBuffer != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(sceneFragmentShaderObject, infoLogLength, &written, szBuffer);
				fprintf(gpFile, "\nS: Fragment Shader Compilation Log : %s\n", szBuffer);
				free(szBuffer);
				DestroyWindow(ghwnd);

			}
		}
	}


	/*****SHADER PROGRAM*****/

	//create
	sceneShaderProgramObject = glCreateProgram();

	//attach vertex shader to shader program
	glAttachShader(sceneShaderProgramObject, sceneVertexShaderObject);

	//attach fragment shader to shader program
	glAttachShader(sceneShaderProgramObject, sceneFragmentShaderObject);

	//pre-linking binding
	glBindAttribLocation(sceneShaderProgramObject, SPK_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(sceneShaderProgramObject, SPK_ATTRIBUTE_COLOR, "vColor");
	glBindAttribLocation(sceneShaderProgramObject, SPK_ATTRIBUTE_TEXCOORD, "vTexcoord");
	glBindAttribLocation(sceneShaderProgramObject, SPK_ATTRIBUTE_NORMAL, "vNormal");

	//link shader
	glLinkProgram(sceneShaderProgramObject);

	//shader linking error checking
	infoLogLength = 0;
	GLint shaderProgramLinkStatus;
	szBuffer = NULL;

	glGetProgramiv(sceneShaderProgramObject, GL_LINK_STATUS, &shaderProgramLinkStatus);

	if (shaderProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(sceneShaderProgramObject, GL_INFO_LOG_LENGTH, &infoLogLength);

		if (infoLogLength > 0)
		{
			szBuffer = (char*)malloc(infoLogLength);

			if (szBuffer != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(sceneShaderProgramObject, infoLogLength, &written, szBuffer);
				fprintf(gpFile, "\nS: Shader Program Link Log : %s\n", szBuffer);
				free(szBuffer);
				DestroyWindow(ghwnd);
			}
		}
	}

	//get MVP uniform location
	modelMatrixUniformS = glGetUniformLocation(sceneShaderProgramObject, "u_model_matrix");
	viewMatrixUniformS = glGetUniformLocation(sceneShaderProgramObject, "u_view_matrix");
	projectionMatrixUniformS = glGetUniformLocation(sceneShaderProgramObject, "u_projection_matrix");

	passUniformS = glGetUniformLocation(sceneShaderProgramObject, "u_pass");

	lAUniformS = glGetUniformLocation(sceneShaderProgramObject, "u_lA");
	lDUniformS = glGetUniformLocation(sceneShaderProgramObject, "u_lD");
	lSUniformS = glGetUniformLocation(sceneShaderProgramObject, "u_lS");
	lightPositionUniformS = glGetUniformLocation(sceneShaderProgramObject, "u_light_position");

	kShininessUniformS = glGetUniformLocation(sceneShaderProgramObject, "u_materialShininess");

	textureSamplerUniformS = glGetUniformLocation(sceneShaderProgramObject, "u_texture_sampler");


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



	/*****VERTEX SHADER*****/

	//create shader
	godraysVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	//provide source code to vertex shader
	const GLchar* godraysVertexShaderSourceCode =
		"#version 450 core \n" \
		"\n" \
		"in vec4 vPosition; \n" \
		"in vec2 vTexcoord; \n" \
		"in vec3 vNormal; \n" \
		"out vec3 out_normal; \n" \
		"out vec2 out_texcoord; \n" \
		"void main(void) \n" \
		"{ \n" \
		"	out_normal = vNormal; \n" \
		"	out_texcoord = vTexcoord; \n" \
		"	gl_Position = vPosition; \n" \
		"} \n";

	glShaderSource(godraysVertexShaderObject, 1, (const GLchar**)&godraysVertexShaderSourceCode, NULL);

	//compile shader
	glCompileShader(godraysVertexShaderObject);

	//vertex shader compilation error checking
	infoLogLength = 0;
	shaderCompiledStatus = 0;
	szBuffer = NULL;

	glGetShaderiv(godraysVertexShaderObject, GL_COMPILE_STATUS, &shaderCompiledStatus);

	if (shaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(godraysVertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);

		if (infoLogLength > 0)
		{
			szBuffer = (char*)malloc(infoLogLength);
			if (szBuffer != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(godraysVertexShaderObject, infoLogLength, &written, szBuffer);
				fprintf(gpFile, "\nG: Vertex Shader Compilation Log : %s\n", szBuffer);
				free(szBuffer);
				DestroyWindow(ghwnd);
			}

		}
	}


	/*****FRAGMENT SHADER*****/

	//create shader
	godraysFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	//provide source code to fragment shader
	const GLchar* godraysFragmentShaderSourceCode =
		"#version 450 core \n" \
		"\n" \
		"in vec2 out_texcoord; \n" \
		"uniform mat4 u_projection_matrix; \n" \
		"uniform mat4 u_model_matrix; \n" \
		"uniform mat4 u_view_matrix; \n" \
		"uniform float u_exposure; \n" \
		"uniform float u_decay; \n" \
		"uniform float u_density; \n" \
		"uniform float u_weight; \n" \
		"uniform vec4 u_lightPosition; \n" \
		"uniform sampler2D u_occlusionTexture; \n" \
		"const int NUM_SAMPLES = 100; \n" \
		"out vec4 FragColor; \n" \
		"void main(void) \n" \
		"{ \n" \

		"	vec4 lightPos3D = mat4(u_projection_matrix * u_view_matrix * u_model_matrix) * u_lightPosition; \n" \
		"	lightPos3D /= lightPos3D.w; \n" \
		"	lightPos3D += vec4(1.0); \n" \
		"	lightPos3D *= 0.5; \n" \
		"	vec2 sunPosXY = lightPos3D.xy; \n" \

		"	vec2 deltaTexcoord = vec2(out_texcoord.st - sunPosXY); \n" \
		"	vec2 tex = out_texcoord.st; \n" \
		"	deltaTexcoord *= 1.0 / float(NUM_SAMPLES) * u_density; \n" \
		"	float illuminationDecay = 1.0; \n" \

		"	for(int i = 0; i < NUM_SAMPLES; i++) \n" \
		"	{ \n" \
		"		tex -= deltaTexcoord; \n" \
		"		vec4 sampled = texture(u_occlusionTexture, tex); \n" \
		"		sampled *= (illuminationDecay * u_weight); \n" \
		"		FragColor += sampled; \n" \
		"		illuminationDecay *= u_decay; \n" \
		"	} \n" \

		"	FragColor *= u_exposure; \n" \
		"	FragColor = vec4(FragColor.rgb, 1.0); \n" \
		"} \n";

	glShaderSource(godraysFragmentShaderObject, 1, (const GLchar**)&godraysFragmentShaderSourceCode, NULL);

	//compile shader
	glCompileShader(godraysFragmentShaderObject);

	//fragment shader compilation error checking
	infoLogLength = 0;
	shaderCompiledStatus = 0;
	szBuffer = NULL;

	glGetShaderiv(godraysFragmentShaderObject, GL_COMPILE_STATUS, &shaderCompiledStatus);

	if (shaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(godraysFragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);

		if (infoLogLength > 0)
		{
			szBuffer = (char*)malloc(infoLogLength);
			if (szBuffer != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(godraysFragmentShaderObject, infoLogLength, &written, szBuffer);
				fprintf(gpFile, "\nG: Fragment Shader Compilation Log : %s\n", szBuffer);
				free(szBuffer);
				DestroyWindow(ghwnd);

			}
		}
	}


	/*****SHADER PROGRAM*****/

	//create
	godraysShaderProgramObject = glCreateProgram();

	//attach vertex shader to shader program
	glAttachShader(godraysShaderProgramObject, godraysVertexShaderObject);

	//attach fragment shader to shader program
	glAttachShader(godraysShaderProgramObject, godraysFragmentShaderObject);

	//pre-linking binding
	glBindAttribLocation(godraysShaderProgramObject, SPK_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(godraysShaderProgramObject, SPK_ATTRIBUTE_COLOR, "vColor");
	glBindAttribLocation(godraysShaderProgramObject, SPK_ATTRIBUTE_NORMAL, "vNormal");
	glBindAttribLocation(godraysShaderProgramObject, SPK_ATTRIBUTE_TEXCOORD, "vTexcoord");

	//link shader
	glLinkProgram(godraysShaderProgramObject);

	//shader linking error checking
	infoLogLength = 0;
	shaderProgramLinkStatus;
	szBuffer = NULL;

	glGetProgramiv(godraysShaderProgramObject, GL_LINK_STATUS, &shaderProgramLinkStatus);

	if (shaderProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(godraysShaderProgramObject, GL_INFO_LOG_LENGTH, &infoLogLength);

		if (infoLogLength > 0)
		{
			szBuffer = (char*)malloc(infoLogLength);

			if (szBuffer != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(godraysShaderProgramObject, infoLogLength, &written, szBuffer);
				fprintf(gpFile, "\nG: Shader Program Link Log : %s\n", szBuffer);
				free(szBuffer);
				DestroyWindow(ghwnd);
			}
		}
	}

	//get MVP uniform location
	modelMatrixUniformG = glGetUniformLocation(godraysShaderProgramObject, "u_model_matrix");
	viewMatrixUniformG = glGetUniformLocation(godraysShaderProgramObject, "u_view_matrix");
	projectionMatrixUniformG = glGetUniformLocation(godraysShaderProgramObject, "u_projection_matrix");

	exposureUniformG = glGetUniformLocation(godraysShaderProgramObject, "u_exposure");
	decayUniformG = glGetUniformLocation(godraysShaderProgramObject, "u_decay");
	densityUniformG = glGetUniformLocation(godraysShaderProgramObject, "u_density");
	weightUniformG = glGetUniformLocation(godraysShaderProgramObject, "u_weight");
	lightPositionUniformG = glGetUniformLocation(godraysShaderProgramObject, "u_lightPosition");
	occlusionTextureSamplerUniformG = glGetUniformLocation(godraysShaderProgramObject, "u_occlusionTexture");


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



	/*****VERTEX SHADER*****/

	//create shader
	finalPassVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	//provide source code to vertex shader
	const GLchar* finalPassVertexShaderSourceCode =
		"#version 450 core \n" \
		"\n" \
		"in vec4 vPosition; \n" \
		"in vec2 vTexcoord; \n" \
		"out vec2 out_texcoord; \n" \
		"void main(void) \n" \
		"{ \n" \
		"	out_texcoord = vTexcoord; \n" \
		"	gl_Position = vPosition; \n" \
		"} \n";

	glShaderSource(finalPassVertexShaderObject, 1, (const GLchar**)&finalPassVertexShaderSourceCode, NULL);

	//compile shader
	glCompileShader(finalPassVertexShaderObject);

	//vertex shader compilation error checking
	infoLogLength = 0;
	shaderCompiledStatus = 0;
	szBuffer = NULL;

	glGetShaderiv(finalPassVertexShaderObject, GL_COMPILE_STATUS, &shaderCompiledStatus);

	if (shaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(finalPassVertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);

		if (infoLogLength > 0)
		{
			szBuffer = (char*)malloc(infoLogLength);
			if (szBuffer != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(finalPassVertexShaderObject, infoLogLength, &written, szBuffer);
				fprintf(gpFile, "\nG: Vertex Shader Compilation Log : %s\n", szBuffer);
				free(szBuffer);
				DestroyWindow(ghwnd);
			}

		}
	}


	/*****FRAGMENT SHADER*****/

	//create shader
	finalPassFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	//provide source code to fragment shader
	const GLchar* finalPassFragmentShaderSourceCode =
		"#version 450 core \n" \
		"\n" \
		"in vec2 out_texcoord; \n" \
		"uniform sampler2D u_pass1Tex; \n" \
		"uniform sampler2D u_pass2Tex; \n" \
		"out vec4 FragColor; \n" \
		"void main(void) \n" \
		"{ \n" \
		"	vec4 result = texture(u_pass1Tex, out_texcoord) + texture(u_pass2Tex, out_texcoord);"
		"	FragColor = vec4(result.rgb, 1.0); \n" \
		"} \n";

	glShaderSource(finalPassFragmentShaderObject, 1, (const GLchar**)&finalPassFragmentShaderSourceCode, NULL);

	//compile shader
	glCompileShader(finalPassFragmentShaderObject);

	//fragment shader compilation error checking
	infoLogLength = 0;
	shaderCompiledStatus = 0;
	szBuffer = NULL;

	glGetShaderiv(finalPassFragmentShaderObject, GL_COMPILE_STATUS, &shaderCompiledStatus);

	if (shaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(finalPassFragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);

		if (infoLogLength > 0)
		{
			szBuffer = (char*)malloc(infoLogLength);
			if (szBuffer != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(finalPassFragmentShaderObject, infoLogLength, &written, szBuffer);
				fprintf(gpFile, "\nG: Fragment Shader Compilation Log : %s\n", szBuffer);
				free(szBuffer);
				DestroyWindow(ghwnd);

			}
		}
	}


	/*****SHADER PROGRAM*****/

	//create
	finalPassShaderProgramObject = glCreateProgram();

	//attach vertex shader to shader program
	glAttachShader(finalPassShaderProgramObject, finalPassVertexShaderObject);

	//attach fragment shader to shader program
	glAttachShader(finalPassShaderProgramObject, finalPassFragmentShaderObject);

	//pre-linking binding
	glBindAttribLocation(finalPassShaderProgramObject, SPK_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(finalPassShaderProgramObject, SPK_ATTRIBUTE_COLOR, "vColor");
	glBindAttribLocation(finalPassShaderProgramObject, SPK_ATTRIBUTE_NORMAL, "vNormal");
	glBindAttribLocation(finalPassShaderProgramObject, SPK_ATTRIBUTE_TEXCOORD, "vTexcoord");

	//link shader
	glLinkProgram(finalPassShaderProgramObject);

	//shader linking error checking
	infoLogLength = 0;
	shaderProgramLinkStatus;
	szBuffer = NULL;

	glGetProgramiv(finalPassShaderProgramObject, GL_LINK_STATUS, &shaderProgramLinkStatus);

	if (shaderProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(finalPassShaderProgramObject, GL_INFO_LOG_LENGTH, &infoLogLength);

		if (infoLogLength > 0)
		{
			szBuffer = (char*)malloc(infoLogLength);

			if (szBuffer != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(finalPassShaderProgramObject, infoLogLength, &written, szBuffer);
				fprintf(gpFile, "\nG: Shader Program Link Log : %s\n", szBuffer);
				free(szBuffer);
				DestroyWindow(ghwnd);
			}
		}
	}

	//get MVP uniform location
	pass1TextureSamplerUniformF = glGetUniformLocation(finalPassShaderProgramObject, "u_pass1Tex");
	pass2TextureSamplerUniformF = glGetUniformLocation(finalPassShaderProgramObject, "u_pass2Tex");


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	//vertices array declaration

	//SPHERE_SUN
	makeSphere(0.08, 40, 40);
	fprintf(gpFile, "\nnumVertices = %d\n", numVertices);
	fprintf(gpFile, "numElements = %d\n", numElements);

	//SQUARE
	//vertices
	const GLfloat squareVertices[] = { 1.0f, 1.0f, 1.0f,        //front
									  -1.0f, 1.0f, 1.0f,
									  -1.0f, -1.0f, 1.0f,
									   1.0f, -1.0f, 1.0f };

	//color
	const GLfloat squareTexture[] = { 1.0f, 1.0f,        //front
									0.0f, 1.0f,
									0.0f, 0.0f,
									1.0f, 0.0f };

	//normals
	const GLfloat squareNormals[] = { 0.0f, 0.0f, 1.0f,
									  0.0f, 0.0f, 1.0f,
									  0.0f, 0.0f, 1.0f,
									  0.0f, 0.0f, 1.0f };



	/*****SQUARE VAO*****/

	glGenVertexArrays(1, &vao_square);
	glBindVertexArray(vao_square);

	glGenBuffers(1, &vbo_position_square);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_square);
	glBufferData(GL_ARRAY_BUFFER, sizeof(squareVertices), squareVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(SPK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SPK_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vbo_texture_square);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_texture_square);
	glBufferData(GL_ARRAY_BUFFER, sizeof(squareTexture), squareTexture, GL_STATIC_DRAW);
	glVertexAttribPointer(SPK_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SPK_ATTRIBUTE_TEXCOORD);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vbo_normal_square);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_normal_square);
	glBufferData(GL_ARRAY_BUFFER, sizeof(squareNormals), squareNormals, GL_STATIC_DRAW);
	glVertexAttribPointer(SPK_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SPK_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	/*FBO*/

	//FBO_pass1
	glGenFramebuffers(1, &fbo_pass1);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_pass1);

	glGenTextures(1, &silhouetteTexture_pass1);
	glBindTexture(GL_TEXTURE_2D, silhouetteTexture_pass1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIN_WIDTH, WIN_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, silhouetteTexture_pass1, 0);

	glGenRenderbuffers(1, &rbo_pass1);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo_pass1);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WIN_WIDTH, WIN_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo_pass1);

	GLenum buffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		fprintf(gpFile, "FBO_pass1 failed\n");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//FBO_pass2
	glGenFramebuffers(1, &fbo_pass2);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_pass2);

	glGenTextures(1, &colorTexture_pass2);
	glBindTexture(GL_TEXTURE_2D, colorTexture_pass2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIN_WIDTH, WIN_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture_pass2, 0);

	glGenRenderbuffers(1, &rbo_pass2);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo_pass2);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WIN_WIDTH, WIN_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo_pass1);

	glDrawBuffers(1, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		fprintf(gpFile, "FBO_pass2 failed\n");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//FBO_pass3 - godrays
	glGenFramebuffers(1, &fbo_godrays);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_godrays);

	glGenTextures(1, &colorBuffer);
	glBindTexture(GL_TEXTURE_2D, colorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIN_WIDTH, WIN_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);

	glDrawBuffers(1, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		fprintf(gpFile, "FBO_pass3 failed\n");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);



	//depth
	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	//glEnable(GL_CULL_FACE);

	//Loading Textures
	//loadGLTexture(&smiley_texture, MAKEINTRESOURCE(SMILEY_BITMAP));
	pngTexture = LoadImageAsTexture("grass.png");

	glEnable(GL_TEXTURE_2D);

	//SetClearColor
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	//set perspective matrix to identity matrix
	perspectiveProjectionMatrix = mat4::identity();

	Resize(WIN_WIDTH, WIN_HEIGHT);
}

GLuint LoadImageAsTexture(const char* path)
{
	//variable declarations    
	int width, height;
	unsigned char* imageData = NULL;
	int nrComponents;

	//code    
	imageData = SOIL_load_image(path, &width, &height, &nrComponents, 0);

	if (imageData)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, imageData);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		SOIL_free_image_data(imageData);

	}
	else
	{
		SOIL_free_image_data(imageData);
	}

	return(textureID);

}

bool loadGLTexture(GLuint* texture, TCHAR resourceID[])
{
	//variable declaration

	bool bResult = false;
	HBITMAP sk_hBitmap = NULL;
	BITMAP sk_bmp;

	//code
	sk_hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), resourceID, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);    //Load Resource (LR) DIB - Device Independent Bitmap

	if (sk_hBitmap)
	{
		bResult = true;

		GetObject(sk_hBitmap, sizeof(BITMAP), &sk_bmp);


		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glGenTextures(1, texture);

		glBindTexture(GL_TEXTURE_2D, *texture);

		//setting texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		//PUSHING DATA INTO GRAPHIC MEMORY USING GRAPHIC DRIVER

		//gluBuild2DMipmaps(GL_TEXTURE_2D, 3, sk_bmp.bmWidth, sk_bmp.bmHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, sk_bmp.bmBits);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, sk_bmp.bmWidth, sk_bmp.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, sk_bmp.bmBits);
		glGenerateMipmap(GL_TEXTURE_2D);

		DeleteObject(sk_hBitmap);

	}

	return(bResult);

}

void Resize(int width, int height)
{
	//code
	if (height == 0)
		height = 1;

	gWidth = width;
	gHeight = height;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	perspectiveProjectionMatrix = perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void Display(void)
{
	//code
	mat4 modelMatrix;
	mat4 viewMatrix;
	mat4 projectionMatrix;
	mat4 translateMatrix;
	mat4 rotateXMatrix;
	mat4 rotateYMatrix;
	mat4 rotateZMatrix;
	mat4 scaleMatrix;

	//glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//pass1
	//light source colored with black scene objects (silhouette pass)

	//bind fbo
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_pass1);

	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	//glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, (GLsizei)WIN_WIDTH, (GLsizei)WIN_HEIGHT);
	//glViewport(0, 0, gWidth, gHeight);

	//start using OpenGL program object
	glUseProgram(sceneShaderProgramObject);

	//OpenGL Drawing

	glUniform1i(passUniformS, 1);     //pass 1

	//SUN TRANSFORMATION	
	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	rotateXMatrix = mat4::identity();
	rotateYMatrix = mat4::identity();
	rotateZMatrix = mat4::identity();
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	projectionMatrix = mat4::identity();

	translateMatrix = translate(sunPosX, sunPosY, -3.0f);

	modelMatrix = translateMatrix;
	projectionMatrix = perspectiveProjectionMatrix;

	glUniformMatrix4fv(modelMatrixUniformS, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniformS, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniformS, 1, GL_FALSE, projectionMatrix);

	glVertexAttrib3f(SPK_ATTRIBUTE_COLOR, 1.0f, 1.0f, 1.0f);

	drawSphere();


	//SQUARE TRANSFORMATION	
	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	rotateXMatrix = mat4::identity();
	rotateYMatrix = mat4::identity();
	rotateZMatrix = mat4::identity();
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	projectionMatrix = mat4::identity();

	glVertexAttrib3f(SPK_ATTRIBUTE_COLOR, 0.0f, 0.0f, 0.0f);

	translateMatrix = translate(1.0f, 0.0f, -3.0f);
	scaleMatrix = scale(0.5f, 0.5f, 0.0f);

	modelMatrix = translateMatrix * scaleMatrix;
	projectionMatrix = perspectiveProjectionMatrix;

	glUniformMatrix4fv(modelMatrixUniformS, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniformS, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniformS, 1, GL_FALSE, projectionMatrix);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, pngTexture);
	glUniform1i(textureSamplerUniformS, 0);

	//bind vao_rectangle
	glBindVertexArray(vao_square);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	//unbind vao_rectangle
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);

	//stop using OpenGL program object
	glUseProgram(0);

	//reset color
	//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	//unbind fbo
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//pass2
	//no light source with colored scene objects

	//bind fbo
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_pass2);

	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	//glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, (GLsizei)WIN_WIDTH, (GLsizei)WIN_HEIGHT);

	//start using OpenGL program object
	glUseProgram(sceneShaderProgramObject);

	//SQUARE TRANSFORMATION	
	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	rotateXMatrix = mat4::identity();
	rotateYMatrix = mat4::identity();
	rotateZMatrix = mat4::identity();
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	projectionMatrix = mat4::identity();

	glUniform1i(passUniformS, 2);     //pass 2

	glUniform3f(lAUniformS, 1.0f, 1.0f, 1.0f);
	glUniform3f(lDUniformS, 1.0f, 1.0f, 1.0f);
	glUniform3f(lSUniformS, 1.0f, 1.0f, 1.0f);
	glUniform4f(lightPositionUniformS, 100.0f, 100.0f, 100.0f, 1.0f);

	glUniform1f(kShininessUniformS, 128.0);

	translateMatrix = translate(1.0f, 0.0f, -3.0f);
	scaleMatrix = scale(0.5f, 0.5f, 0.0f);

	modelMatrix = translateMatrix * scaleMatrix;
	projectionMatrix = perspectiveProjectionMatrix;

	glUniformMatrix4fv(modelMatrixUniformS, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniformS, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniformS, 1, GL_FALSE, projectionMatrix);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, pngTexture);
	glUniform1i(textureSamplerUniformS, 0);

	//bind vao_rectangle
	glBindVertexArray(vao_square);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	//unbind vao_rectangle
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);

	//stop using OpenGL program object
	glUseProgram(0);

	//reset color
	//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	//unbind fbo
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//pass3
	//apply godrays

	//bind fbo
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_godrays);

	glDisable(GL_DEPTH_TEST);

	glClear(GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, (GLsizei)WIN_WIDTH, (GLsizei)WIN_HEIGHT);

	//start using OpenGL program object
	glUseProgram(godraysShaderProgramObject);

	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();

	translateMatrix = translate(0.0f, 0.0f, -3.0f);
	modelMatrix = translateMatrix;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, silhouetteTexture_pass1);

	glUniform4f(lightPositionUniformG, sunPosX, sunPosY, 0.0f, 1.0f);
	glUniform1f(densityUniformG, 1.0f);
	glUniform1f(weightUniformG, 0.01f);
	glUniform1f(decayUniformG, 1.0f);
	glUniform1f(exposureUniformG, 1.0f);
	glUniform1i(numSamplesUniformG, 100);

	glUniform1i(occlusionTextureSamplerUniformG, 0);

	glUniformMatrix4fv(modelMatrixUniformG, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniformG, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniformG, 1, GL_FALSE, projectionMatrix);

	//bind vao_rectangle
	glBindVertexArray(vao_square);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	//unbind vao_rectangle
	glBindVertexArray(0);

	//stop using OpenGL program object
	glUseProgram(0);

	//unbind fbo
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//default fbo
	//blend textures

	//start using OpenGL program object
	glUseProgram(finalPassShaderProgramObject);

	glEnable(GL_DEPTH_TEST);
	//glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, (GLsizei)gWidth, (GLsizei)gHeight);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorTexture_pass2);
	glUniform1i(pass1TextureSamplerUniformF, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, colorBuffer);
	//glBindTexture(GL_TEXTURE_2D, silhouetteTexture_pass1);
	glUniform1i(pass2TextureSamplerUniformF, 1);

	//bind vao_rectangle
	glBindVertexArray(vao_square);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	//unbind vao_rectangle
	glBindVertexArray(0);

	//stop using OpenGL program object
	glUseProgram(0);

	SwapBuffers(ghdc);
}

void Update(void)
{
	//code

	sunPosY += 0.001f;


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

	//sun
	deallocate();

	//square
	if (vao_square)
	{
		glDeleteVertexArrays(1, &vao_square);
		vao_square = 0;
	}

	if (vbo_position_square)
	{
		glDeleteBuffers(1, &vbo_position_square);
		vbo_position_square = 0;
	}

	if (vbo_texture_square)
	{
		glDeleteBuffers(1, &vbo_texture_square);
		vbo_texture_square = 0;
	}

	if (vbo_normal_square)
	{
		glDeleteBuffers(1, &vbo_normal_square);
		vbo_normal_square = 0;
	}



	/*****SAFE SHADER CLEAN-UP*****/

	if (sceneShaderProgramObject)
	{
		glUseProgram(sceneShaderProgramObject);
		GLsizei shaderCount;
		glGetProgramiv(sceneShaderProgramObject, GL_ATTACHED_SHADERS, &shaderCount);

		GLuint* pShaders = NULL;
		pShaders = (GLuint*)malloc(sizeof(GLuint) * shaderCount);

		glGetAttachedShaders(sceneShaderProgramObject, shaderCount, &shaderCount, pShaders);

		for (GLsizei i = 0; i < shaderCount; i++)
		{
			glDetachShader(sceneShaderProgramObject, pShaders[i]);
			glDeleteShader(pShaders[i]);
			pShaders[i] = 0;
		}
		free(pShaders);

		glDeleteProgram(sceneShaderProgramObject);
		sceneShaderProgramObject = 0;
		glUseProgram(0);

	}

	if (godraysShaderProgramObject)
	{
		glUseProgram(godraysShaderProgramObject);
		GLsizei shaderCount;
		glGetProgramiv(godraysShaderProgramObject, GL_ATTACHED_SHADERS, &shaderCount);

		GLuint* pShaders = NULL;
		pShaders = (GLuint*)malloc(sizeof(GLuint) * shaderCount);

		glGetAttachedShaders(godraysShaderProgramObject, shaderCount, &shaderCount, pShaders);

		for (GLsizei i = 0; i < shaderCount; i++)
		{
			glDetachShader(godraysShaderProgramObject, pShaders[i]);
			glDeleteShader(pShaders[i]);
			pShaders[i] = 0;
		}
		free(pShaders);

		glDeleteProgram(godraysShaderProgramObject);
		godraysShaderProgramObject = 0;
		glUseProgram(0);

	}

	if (finalPassShaderProgramObject)
	{
		glUseProgram(finalPassShaderProgramObject);
		GLsizei shaderCount;
		glGetProgramiv(finalPassShaderProgramObject, GL_ATTACHED_SHADERS, &shaderCount);

		GLuint* pShaders = NULL;
		pShaders = (GLuint*)malloc(sizeof(GLuint) * shaderCount);

		glGetAttachedShaders(finalPassShaderProgramObject, shaderCount, &shaderCount, pShaders);

		for (GLsizei i = 0; i < shaderCount; i++)
		{
			glDetachShader(finalPassShaderProgramObject, pShaders[i]);
			glDeleteShader(pShaders[i]);
			pShaders[i] = 0;
		}
		free(pShaders);

		glDeleteProgram(finalPassShaderProgramObject);
		finalPassShaderProgramObject = 0;
		glUseProgram(0);

	}




	//
	glDeleteTextures(1, &pngTexture);


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

