//header files
#include<windows.h>
#include<stdio.h>

#include<gl\glew.h>

#include<GL/GL.h>
#include "Resources.h"
#include "vmath.h"
#include "CircleVector.h"


//macro functions
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "OpenGL32.lib")

#define PI 3.1415

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

//global variable declaration
DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
bool gbFullscreen = false;
HWND ghwnd = NULL;
bool gbActiveWindow = false;
FILE* gpFile = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;

GLuint shaderProgramObject;

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

GLuint in_num_vertices = 0;
GLuint ex_num_vertices = 0;

GLuint mvpMatrixUniform;
mat4 perspectiveProjectionMatrix;   //4x4 matrix

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
	void drawCircle(GLfloat, GLfloat, GLfloat, GLfloat, GLint);

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
	const GLchar *vertexShaderSourceCode =
		"#version 450 core \n" \
		"\n" \
		"in vec4 vPosition; \n" \
		"in vec4 vColor; \n" \
		"uniform mat4 u_mvpMatrix; \n" \
		"out vec4 out_color; \n" \
		"void main(void) \n" \
		"{ \n" \
		"	gl_Position = u_mvpMatrix * vPosition; \n" \
		"	out_color = vColor; \n" \
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
		"in vec4 out_color; \n" \
		"out vec4 FragColor; \n" \
		"void main(void) \n" \
		"{ \n" \
		"	FragColor = out_color; \n" \
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
				fprintf(gpFile, "\nShader Program Link Log : %s\n", szBuffer);
				free(szBuffer);
				DestroyWindow(ghwnd);
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
	fprintf(gpFile, "\nsize = %d\ninNumVertices = %d\nexNumVertices = %d\n", excircle->size, in_num_vertices, ex_num_vertices);


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
	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_CULL_FACE);

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

	//OpenGL Drawing

	mat4 translateMatrix;
	mat4 rotateZMatrix;
	mat4 modelViewMatrix;
	mat4 modelViewProjectionMatrix;

	//VERTICAL LINES TRANSFORMATION

	translateMatrix = mat4::identity();
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();

	for (GLfloat x = 0.05f; x <= 1.05f; x += 0.05f)
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

	for (GLfloat x = -0.05f; x >= -1.05f; x -= 0.05f)
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

	for (GLfloat y = 0.05f; y <= 1.05f; y += 0.05f)
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

	for (GLfloat y = -0.05f; y >= -1.05f; y -= 0.05f)
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

	SwapBuffers(ghdc);
}

void Uninitialize(void)
{
	//code

	fprintf(gpFile, "Uninit\n");
	if (gbFullscreen == true)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);

		SetWindowLong(ghwnd, GWL_STYLE, (dwStyle | WS_OVERLAPPEDWINDOW));
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);

		ShowCursor(true);
	}

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

