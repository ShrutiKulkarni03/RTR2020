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
//BITMAP sk_bmp;

//shader variables

GLuint shaderProgramObjectPyramid;
GLuint shaderProgramObjectCube;


//vao for pyramid
GLuint vao_pyramid;                          //vertex array object
GLuint vbo_position_pyramid;                 //vertex buffer object
GLuint vbo_texture_pyramid;

//vao for cube
GLuint vao_cube;
GLuint vbo_position_cube;
GLuint vbo_texture_cube;

GLuint mvpMatrixUniformPyramid;
GLuint textureSamplerUniformPyramid;

GLuint mvpMatrixUniformCube;
GLuint textureSamplerUniformCube;

mat4 perspectiveProjectionMatrix;   //4x4 matrix

//update variables

GLfloat pyramidAngle = 0.0f;
GLfloat cubeAngle = 0.0f;

//texture variables

GLuint sk_stone_texture;

//framebuffer variables
GLuint fbo;                         //frame buffer object
GLuint textureRenderBuffer; 
GLuint rbo;                        //render buffer object

int fbWidth;
int fbHeight;


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
	bool SKloadGLTexture(GLuint*, TCHAR[]);

	//variable declaration
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;
	GLuint vertexShaderObjectPyramid;
	GLuint fragmentShaderObjectPyramid;

	GLuint vertexShaderObjectCube;
	GLuint fragmentShaderObjectCube;


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


	/*****VERTEX SHADER PYRAMID*****/

	//create shader
	vertexShaderObjectPyramid = glCreateShader(GL_VERTEX_SHADER);

	//provide source code to vertex shader
	const GLchar* vertexShaderSourceCodePyramid =
		"#version 450 core \n" \
		"\n" \
		"in vec4 vPosition; \n" \
		"in vec2 vTexcoord; \n" \
		"uniform mat4 u_mvpMatrix; \n" \
		"out vec2 out_texcoord; \n" \
		"void main(void) \n" \
		"{ \n" \
		"	gl_Position = u_mvpMatrix * vPosition; \n" \
		"	out_texcoord = vTexcoord; \n" \
		"} \n";

	glShaderSource(vertexShaderObjectPyramid, 1, (const GLchar**)&vertexShaderSourceCodePyramid, NULL);

	//compile shader
	glCompileShader(vertexShaderObjectPyramid);

	//vertex shader compilation error checking
	GLint infoLogLength = 0;
	GLint shaderCompiledStatus = 0;
	char* szBuffer = NULL;

	glGetShaderiv(vertexShaderObjectPyramid, GL_COMPILE_STATUS, &shaderCompiledStatus);

	if (shaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObjectPyramid, GL_INFO_LOG_LENGTH, &infoLogLength);

		if (infoLogLength > 0)
		{
			szBuffer = (char*)malloc(infoLogLength);
			if (szBuffer != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(vertexShaderObjectPyramid, infoLogLength, &written, szBuffer);
				fprintf(gpFile, "\nVertex Shader Compilation Log : %s\n", szBuffer);
				free(szBuffer);
				DestroyWindow(ghwnd);
			}

		}
	}


	/*****FRAGMENT SHADER PYRAMID*****/

	//create shader
	fragmentShaderObjectPyramid = glCreateShader(GL_FRAGMENT_SHADER);

	//provide source code to fragment shader
	const GLchar* fragmentShaderSourceCodePyramid =
		"#version 450 core \n" \
		"\n" \
		"in vec2 out_texcoord; \n" \
		"uniform sampler2D u_texture_sampler; \n" \
		"out vec4 FragColor; \n" \
		"void main(void) \n" \
		"{ \n" \
		"	FragColor = texture(u_texture_sampler, out_texcoord); \n" \
		"} \n";

	glShaderSource(fragmentShaderObjectPyramid, 1, (const GLchar**)&fragmentShaderSourceCodePyramid, NULL);

	//compile shader
	glCompileShader(fragmentShaderObjectPyramid);

	//fragment shader compilation error checking
	infoLogLength = 0;
	shaderCompiledStatus = 0;
	szBuffer = NULL;

	glGetShaderiv(fragmentShaderObjectPyramid, GL_COMPILE_STATUS, &shaderCompiledStatus);

	if (shaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(fragmentShaderObjectPyramid, GL_INFO_LOG_LENGTH, &infoLogLength);

		if (infoLogLength > 0)
		{
			szBuffer = (char*)malloc(infoLogLength);
			if (szBuffer != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(fragmentShaderObjectPyramid, infoLogLength, &written, szBuffer);
				fprintf(gpFile, "\nFragment Shader Compilation Log : %s\n", szBuffer);
				free(szBuffer);
				DestroyWindow(ghwnd);

			}
		}
	}


	/*****SHADER PROGRAM PYRAMID*****/

	//create
	shaderProgramObjectPyramid = glCreateProgram();

	//attach vertex shader to shader program
	glAttachShader(shaderProgramObjectPyramid, vertexShaderObjectPyramid);

	//attach fragment shader to shader program
	glAttachShader(shaderProgramObjectPyramid, fragmentShaderObjectPyramid);

	//pre-linking binding
	glBindAttribLocation(shaderProgramObjectPyramid, SPK_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(shaderProgramObjectPyramid, SPK_ATTRIBUTE_TEXCOORD, "vTexcoord");

	//link shader
	glLinkProgram(shaderProgramObjectPyramid);

	//shader linking error checking
	infoLogLength = 0;
	GLint shaderProgramLinkStatus;
	szBuffer = NULL;

	glGetProgramiv(shaderProgramObjectPyramid, GL_LINK_STATUS, &shaderProgramLinkStatus);

	if (shaderProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObjectPyramid, GL_INFO_LOG_LENGTH, &infoLogLength);

		if (infoLogLength > 0)
		{
			szBuffer = (char*)malloc(infoLogLength);

			if (szBuffer != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(shaderProgramObjectPyramid, infoLogLength, &written, szBuffer);
				fprintf(gpFile, "\nShader Program Link Log : %s\n", szBuffer);
				free(szBuffer);
				DestroyWindow(ghwnd);
			}
		}
	}

	//get MVP uniform location
	mvpMatrixUniformPyramid = glGetUniformLocation(shaderProgramObjectPyramid, "u_mvpMatrix");
	textureSamplerUniformPyramid = glGetUniformLocation(shaderProgramObjectPyramid, "u_texture_sampler");




	/*****VERTEX SHADER CUBE*****/

	//create shader
	vertexShaderObjectCube = glCreateShader(GL_VERTEX_SHADER);

	//provide source code to vertex shader
	const GLchar* vertexShaderSourceCodeCube =
		"#version 450 core \n" \
		"\n" \
		"in vec4 vPosition; \n" \
		"in vec2 vTexcoord; \n" \
		"uniform mat4 u_mvpMatrix; \n" \
		"out vec2 out_texcoord; \n" \
		"void main(void) \n" \
		"{ \n" \
		"	gl_Position = u_mvpMatrix * vPosition; \n" \
		"	out_texcoord = vTexcoord; \n" \
		"} \n";

	glShaderSource(vertexShaderObjectCube, 1, (const GLchar**)&vertexShaderSourceCodeCube, NULL);

	//compile shader
	glCompileShader(vertexShaderObjectCube);

	//vertex shader compilation error checking
	infoLogLength = 0;
	shaderCompiledStatus = 0;
	szBuffer = NULL;

	glGetShaderiv(vertexShaderObjectCube, GL_COMPILE_STATUS, &shaderCompiledStatus);

	if (shaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObjectCube, GL_INFO_LOG_LENGTH, &infoLogLength);

		if (infoLogLength > 0)
		{
			szBuffer = (char*)malloc(infoLogLength);
			if (szBuffer != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(vertexShaderObjectCube, infoLogLength, &written, szBuffer);
				fprintf(gpFile, "\nVertex Shader Compilation Log : %s\n", szBuffer);
				free(szBuffer);
				DestroyWindow(ghwnd);
			}

		}
	}


	/*****FRAGMENT SHADER CUBE*****/

	//create shader
	fragmentShaderObjectCube = glCreateShader(GL_FRAGMENT_SHADER);

	//provide source code to fragment shader
	const GLchar* fragmentShaderSourceCodeCube =
		"#version 450 core \n" \
		"\n" \
		"in vec2 out_texcoord; \n" \
		"uniform sampler2D u_texture_sampler; \n" \
		"out vec4 FragColor; \n" \
		"void main(void) \n" \
		"{ \n" \
		"	vec3 col = texture(u_texture_sampler, out_texcoord).rgb; \n" \
		"	FragColor = vec4(col, 1.0); \n" \
		"} \n";

	glShaderSource(fragmentShaderObjectCube, 1, (const GLchar**)&fragmentShaderSourceCodeCube, NULL);

	//compile shader
	glCompileShader(fragmentShaderObjectCube);

	//fragment shader compilation error checking
	infoLogLength = 0;
	shaderCompiledStatus = 0;
	szBuffer = NULL;

	glGetShaderiv(fragmentShaderObjectCube, GL_COMPILE_STATUS, &shaderCompiledStatus);

	if (shaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(fragmentShaderObjectCube, GL_INFO_LOG_LENGTH, &infoLogLength);

		if (infoLogLength > 0)
		{
			szBuffer = (char*)malloc(infoLogLength);
			if (szBuffer != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(fragmentShaderObjectCube, infoLogLength, &written, szBuffer);
				fprintf(gpFile, "\nFragment Shader Compilation Log : %s\n", szBuffer);
				free(szBuffer);
				DestroyWindow(ghwnd);

			}
		}
	}


	/*****SHADER PROGRAM CUBE*****/

	//create
	shaderProgramObjectCube = glCreateProgram();

	//attach vertex shader to shader program
	glAttachShader(shaderProgramObjectCube, vertexShaderObjectPyramid);

	//attach fragment shader to shader program
	glAttachShader(shaderProgramObjectCube, fragmentShaderObjectPyramid);

	//pre-linking binding
	glBindAttribLocation(shaderProgramObjectCube, SPK_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(shaderProgramObjectCube, SPK_ATTRIBUTE_TEXCOORD, "vTexcoord");

	//link shader
	glLinkProgram(shaderProgramObjectCube);

	//shader linking error checking
	infoLogLength = 0;
	shaderProgramLinkStatus;
	szBuffer = NULL;

	glGetProgramiv(shaderProgramObjectCube, GL_LINK_STATUS, &shaderProgramLinkStatus);

	if (shaderProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObjectCube, GL_INFO_LOG_LENGTH, &infoLogLength);

		if (infoLogLength > 0)
		{
			szBuffer = (char*)malloc(infoLogLength);

			if (szBuffer != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(shaderProgramObjectCube, infoLogLength, &written, szBuffer);
				fprintf(gpFile, "\nShader Program Link Log : %s\n", szBuffer);
				free(szBuffer);
				DestroyWindow(ghwnd);
			}
		}
	}

	//get MVP uniform location
	mvpMatrixUniformCube = glGetUniformLocation(shaderProgramObjectCube, "u_mvpMatrix");
	textureSamplerUniformCube = glGetUniformLocation(shaderProgramObjectCube, "u_texture_sampler");


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

	/******FBO******/

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glGenTextures(1, &textureRenderBuffer);
	glBindTexture(GL_TEXTURE_2D, textureRenderBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIN_WIDTH, WIN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureRenderBuffer, 0);

	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WIN_WIDTH, WIN_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);



	//depth
	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	//glEnable(GL_CULL_FACE);

	//Loading Textures
	SKloadGLTexture(&sk_stone_texture, MAKEINTRESOURCE(STONE_BITMAP));
	glEnable(GL_TEXTURE_2D);

	//SetClearColor
	//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	//set perspective matrix to identity matrix
	perspectiveProjectionMatrix = mat4::identity();

	Resize(WIN_WIDTH, WIN_HEIGHT);
}

bool SKloadGLTexture(GLuint* texture, TCHAR resourceID[])
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

	fbWidth = width;
	fbHeight = height;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	perspectiveProjectionMatrix = perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void Display(void)
{
	//code

	//bind fbo
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, (GLsizei)WIN_WIDTH, (GLsizei)WIN_HEIGHT);

	//start using OpenGL program object
	glUseProgram(shaderProgramObjectPyramid);

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

	translateMatrix = translate(0.0f, 0.0f, -3.0f);
	rotateYMatrix = rotate(pyramidAngle, 0.0f, 1.0f, 0.0f);

	modelViewMatrix = translateMatrix * rotateYMatrix;

	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;  //pre-multiplication of matrices

	glUniformMatrix4fv(mvpMatrixUniformPyramid, 1, GL_FALSE, modelViewProjectionMatrix);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sk_stone_texture);
	glUniform1i(textureSamplerUniformPyramid, 0);

	//bind vao_pyramid
	glBindVertexArray(vao_pyramid);

	glDrawArrays(GL_TRIANGLES, 0, 12);

	//unbind vao_triangle
	glBindVertexArray(0);

	//stop using OpenGL program object
	glUseProgram(0);

	//reset color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	//unbind fbo
	glBindFramebuffer(GL_FRAMEBUFFER, 0);



	//CUBE TRANSFORMATION
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, (GLsizei)fbWidth, (GLsizei)fbHeight);

	//start using OpenGL program object
	glUseProgram(shaderProgramObjectCube);


	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	rotateXMatrix = mat4::identity();
	rotateYMatrix = mat4::identity();
	rotateZMatrix = mat4::identity();
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();

	translateMatrix = translate(0.0f, 0.0f, -3.0f);
	scaleMatrix = scale(0.45f, 0.45f, 0.45f);
	rotateXMatrix = rotate(cubeAngle, 1.0f, 0.0f, 0.0f);
	rotateYMatrix = rotate(cubeAngle, 0.0f, 1.0f, 0.0f);
	rotateZMatrix = rotate(cubeAngle, 0.0f, 0.0f, 1.0f);


	modelViewMatrix = translateMatrix * scaleMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix;

	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	glUniformMatrix4fv(mvpMatrixUniformCube, 1, GL_FALSE, modelViewProjectionMatrix);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureRenderBuffer);
	glUniform1i(textureSamplerUniformCube, 0);

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


	SwapBuffers(ghdc);
}

void Update(void)
{
	//code
	pyramidAngle += 0.5f;
	if (pyramidAngle >= 360.0f)
		pyramidAngle = 0.0f;

	cubeAngle -= 0.5f;
	if (cubeAngle <= -360.0f)
		cubeAngle = 0.0f;
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



	/*****SAFE SHADER CLEAN-UP*****/

	if (shaderProgramObjectPyramid)
	{
		glUseProgram(shaderProgramObjectPyramid);
		GLsizei shaderCount;
		glGetProgramiv(shaderProgramObjectPyramid, GL_ATTACHED_SHADERS, &shaderCount);

		GLuint* pShaders = NULL;
		pShaders = (GLuint*)malloc(sizeof(GLuint) * shaderCount);

		glGetAttachedShaders(shaderProgramObjectPyramid, shaderCount, &shaderCount, pShaders);

		for (GLsizei i = 0; i < shaderCount; i++)
		{
			glDetachShader(shaderProgramObjectPyramid, pShaders[i]);
			glDeleteShader(pShaders[i]);
			pShaders[i] = 0;
		}
		free(pShaders);

		glDeleteProgram(shaderProgramObjectPyramid);
		shaderProgramObjectPyramid = 0;
		glUseProgram(0);

	}

	if (shaderProgramObjectCube)
	{
		glUseProgram(shaderProgramObjectCube);
		GLsizei shaderCount;
		glGetProgramiv(shaderProgramObjectCube, GL_ATTACHED_SHADERS, &shaderCount);

		GLuint* pShaders = NULL;
		pShaders = (GLuint*)malloc(sizeof(GLuint) * shaderCount);

		glGetAttachedShaders(shaderProgramObjectCube, shaderCount, &shaderCount, pShaders);

		for (GLsizei i = 0; i < shaderCount; i++)
		{
			glDetachShader(shaderProgramObjectCube, pShaders[i]);
			glDeleteShader(pShaders[i]);
			pShaders[i] = 0;
		}
		free(pShaders);

		glDeleteProgram(shaderProgramObjectCube);
		shaderProgramObjectCube = 0;
		glUseProgram(0);

	}



	//
	glDeleteTextures(1, &sk_stone_texture);

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

