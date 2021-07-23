//header files
#include<windows.h>
#include<stdio.h>

#include<gl\glew.h>

#include<GL/GL.h>
#include<CL/opencl.h>
#include "Resources.h"
#include "vmath.h"


//macro functions
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "OpenCL.lib")


#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#define MY_ARRAY_SIZE meshWidth * meshHeight * 4

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
void launchCPUKernel(unsigned int, unsigned int, float);

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

GLuint vao;                          //vertex array object
GLuint vbo_cpu;                          //vertex buffer object
GLuint vbo_gpu;
GLuint mvpMatrixUniform;

//for sine wave
const unsigned int meshWidth = 1024;
const unsigned int meshHeight = 1024;
float pos[meshWidth][meshHeight][4];

//interop
cl_int clSuccess;
cl_mem cl_graphics_resource;
cl_device_id clDeviceID;
cl_context clContext;
cl_command_queue clCommandQueue;
cl_program clProgram;
cl_kernel clKernel;

char* clSourceCode = NULL;
size_t sizeKernelCodeLength;

bool bOnGPU = false;

float animationTime = 0.0f;

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
		case 'C':
		case 'c':
			bOnGPU = false;
			break;

		case 'G':
		case 'g':
			bOnGPU = true;
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
	char* loadOclKernelSource(const char*, const char*, size_t*);

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
		fflush(gpFile);
		DestroyWindow(ghwnd);
	}

	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
	{
		fprintf(gpFile, ("SetPixelFormat() Failed.\n"));
		fflush(gpFile);
		DestroyWindow(ghwnd);
	}

	ghrc = wglCreateContext(ghdc);

	if (ghrc == NULL)
	{
		fprintf(gpFile, ("wglCreateContext() Failed.\n"));
		fflush(gpFile);
		DestroyWindow(ghwnd);
	}

	if (wglMakeCurrent(ghdc, ghrc) == FALSE)
	{
		fprintf(gpFile, ("wglMakeCurrent() Failed\n"));
		fflush(gpFile);
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


	/*INTEROP*/
	cl_platform_id clPlatformID;
	cl_device_id* clDeviceIDs;
	cl_uint devCount;

	clSuccess = clGetPlatformIDs(1, &clPlatformID, NULL);
	if (clSuccess != CL_SUCCESS)
	{
		fprintf(gpFile, "clGetPlatformIDs() unsuccessfull\n");
		fflush(gpFile);
		exit(EXIT_FAILURE);
	}

	clSuccess = clGetDeviceIDs(clPlatformID, CL_DEVICE_TYPE_GPU, 0, NULL, &devCount);
	if (clSuccess != CL_SUCCESS)
	{
		fprintf(gpFile, "clGetDeviceIDs() unsuccessfull\n");
		fflush(gpFile);
		exit(EXIT_FAILURE);
	}
	else if (devCount == 0)
	{
		fprintf(gpFile, "devCount = 0\n");
		fflush(gpFile);
		exit(EXIT_FAILURE);
	}
	else
	{
		clDeviceIDs = (cl_device_id*)malloc(sizeof(cl_device_id) * devCount);
		clSuccess = clGetDeviceIDs(clPlatformID, CL_DEVICE_TYPE_GPU, devCount, clDeviceIDs, NULL);
		if (clSuccess != CL_SUCCESS)
		{
			fprintf(gpFile, "clGetDeviceIDs() unsuccessfull\n");
			fflush(gpFile);
			exit(EXIT_FAILURE);
		}

		clDeviceID = clDeviceIDs[0];

		free(clDeviceIDs);
	}

	cl_context_properties context_properties[] = { CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(),
												   CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(),
												   CL_CONTEXT_PLATFORM, (cl_context_properties)clPlatformID, 0 };

	clContext = clCreateContext(context_properties, 1, &clDeviceID, NULL, NULL, &clSuccess);
	if (clSuccess != CL_SUCCESS)
	{
		fprintf(gpFile, "clCreateContext() unsuccessfull\n");
		fflush(gpFile);
		exit(EXIT_FAILURE);
	}

	clCommandQueue = clCreateCommandQueueWithProperties(clContext, clDeviceID, 0, &clSuccess);
	if (clSuccess != CL_SUCCESS)
	{
		fprintf(gpFile, "clCreateCommandQueueWithProperties() unsuccessfull\n");
		fflush(gpFile);
		exit(EXIT_FAILURE);
	}

	clSourceCode = loadOclKernelSource("SineWave.cl", "", &sizeKernelCodeLength);
	clProgram = clCreateProgramWithSource(clContext, 1, (const char**)&clSourceCode, &sizeKernelCodeLength, &clSuccess);
	if (clSuccess != CL_SUCCESS)
	{
		fprintf(gpFile, "clCreateProgramWithSource() unsuccessfull\n");
		fflush(gpFile);
		exit(EXIT_FAILURE);
	}

	clSuccess = clBuildProgram(clProgram, 0, NULL, "-cl-fast-relaxed-math", NULL, NULL);
	if (clSuccess != CL_SUCCESS)
	{
		fprintf(gpFile, "clBuildProgram() unsuccessfull\n");
		fflush(gpFile);
		exit(EXIT_FAILURE);
	}

	clKernel = clCreateKernel(clProgram, "SineWave_kernel", &clSuccess);
	if (clSuccess != CL_SUCCESS)
	{
		fprintf(gpFile, "clCreateKernel() unsuccessfull\n");
		fflush(gpFile);
		exit(EXIT_FAILURE);
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
		"uniform mat4 u_mvpMatrix; \n" \
		"void main(void) \n" \
		"{ \n" \
		"	gl_Position = u_mvpMatrix * vPosition; \n" \
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
				fflush(gpFile);
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
		"out vec4 FragColor; \n" \
		"void main(void) \n" \
		"{ \n" \
		"	FragColor = vec4(1.0f, 0.5f, 0.0f, 1.0f); \n" \
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
				fflush(gpFile);
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
				fflush(gpFile);
				free(szBuffer);
				DestroyWindow(ghwnd);
			}
		}
	}

	//get MVP uniform location
	mvpMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_mvpMatrix");


	//vertices array declaration

	for (int i = 0; i < meshWidth; i++)
	{
		for (int j = 0; j < meshHeight; j++)
		{
			for (int k = 0; k < 4; k++)
			{
				pos[i][j][k] = 0.0f;
			}
		}
	}


	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo_cpu);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_cpu);
	glBufferData(GL_ARRAY_BUFFER, MY_ARRAY_SIZE * sizeof(float), NULL, GL_DYNAMIC_DRAW);	
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vbo_gpu);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_gpu);
	glBufferData(GL_ARRAY_BUFFER, MY_ARRAY_SIZE * sizeof(float), NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	cl_graphics_resource = clCreateFromGLBuffer(clContext, CL_MEM_WRITE_ONLY, vbo_gpu, &clSuccess);
	if (clSuccess != CL_SUCCESS)
	{
		fprintf(gpFile, "clCreateFromGLBuffer() unsuccessfull\n");
		fflush(gpFile);
		exit(EXIT_FAILURE);
	}


	glBindVertexArray(0);

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

	perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void launchCPUKernel(unsigned int meshWidth, unsigned int meshHeight, float animationTime)
{
	for (int i = 0; i < meshWidth; i++)
	{
		for (int j = 0; j < meshHeight; j++)
		{
			for (int k = 0; k < 4; k++)
			{
				float u = i / (float)meshWidth;
				float v = j / (float)meshHeight;

				u = u * 2.0f - 1.0f;
				v = v * 2.0f - 1.0f;

				float frequency = 4.0f;

				float w = sinf(u * frequency + animationTime) * cosf(v * frequency + animationTime) * 0.5f;

				if (k == 0)
					pos[i][j][k] = u;

				if (k == 1)
					pos[i][j][k] = w;

				if (k == 2)
					pos[i][j][k] = v;

				if (k == 3)
					pos[i][j][k] = 1.0;
				
			}
		}
	}
}

void Display(void)
{
	//variables
	size_t globalWorkSize[2];

	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//start using OpenGL program object
	glUseProgram(shaderProgramObject);

	//OpenGL Drawing

	mat4 modelViewMatrix;
	mat4 modelViewProjectionMatrix;
	
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();

	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;  //pre-multiplication of matrices

	glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	//bind vao
	glBindVertexArray(vao);

	if (bOnGPU == true)
	{
		clSuccess = clSetKernelArg(clKernel, 0, sizeof(cl_mem), (void*)&cl_graphics_resource);
		if (clSuccess != CL_SUCCESS)
		{
			fprintf(gpFile, "clSetKernelArg() for cl_graphics_resource is unsuccessfull\n");
			fflush(gpFile);
			exit(EXIT_FAILURE);
		}
	
		clSuccess = clSetKernelArg(clKernel, 1, sizeof(unsigned int), (void*)&meshWidth);
		if (clSuccess != CL_SUCCESS)
		{
			fprintf(gpFile, "clSetKernelArg() for meshWidth is unsuccessfull\n");
			fflush(gpFile);
			exit(EXIT_FAILURE);
		}

		clSuccess = clSetKernelArg(clKernel, 2, sizeof(unsigned int), (void*)&meshHeight);
		if (clSuccess != CL_SUCCESS)
		{
			fprintf(gpFile, "clSetKernelArg() for meshHeight is unsuccessfull\n");
			fflush(gpFile);
			exit(EXIT_FAILURE);
		}

		clSuccess = clSetKernelArg(clKernel, 3, sizeof(float), (void*)&animationTime);
		if (clSuccess != CL_SUCCESS)
		{
			fprintf(gpFile, "clSetKernelArg() for animationTime is unsuccessfull\n");
			fflush(gpFile);
			exit(EXIT_FAILURE);
		}

		clSuccess = clEnqueueAcquireGLObjects(clCommandQueue, 1, &cl_graphics_resource, 0, NULL, NULL);
		if (clSuccess != CL_SUCCESS)
		{
			fprintf(gpFile, "clEnqueueAcquireGLObjects() unsuccessfull\n");
			fflush(gpFile);
			exit(EXIT_FAILURE);
		}

		globalWorkSize[0] = meshWidth;
		globalWorkSize[1] = meshHeight;

		clSuccess = clEnqueueNDRangeKernel(clCommandQueue, clKernel, 2, NULL, globalWorkSize, 0, 0, NULL, NULL);
		if (clSuccess != CL_SUCCESS)
		{
			fprintf(gpFile, "clEnqueueNDRangeKernel() unsuccessfull\n");
			fflush(gpFile);
			exit(EXIT_FAILURE);
		}

		clSuccess = clEnqueueReleaseGLObjects(clCommandQueue, 1, &cl_graphics_resource, 0, NULL, NULL);
		if (clSuccess != CL_SUCCESS)
		{
			fprintf(gpFile, "clEnqueueReleaseGLObjects() unsuccessfull\n");
			fflush(gpFile);
			exit(EXIT_FAILURE);
		}

		clFinish(clCommandQueue);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_gpu);
	
	}
	else
	{
		launchCPUKernel(meshWidth, meshHeight, animationTime);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_cpu);
		glBufferData(GL_ARRAY_BUFFER, MY_ARRAY_SIZE * sizeof(float), pos, GL_DYNAMIC_DRAW);
	}
		
	glVertexAttribPointer(SPK_ATTRIBUTE_POSITION, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SPK_ATTRIBUTE_POSITION);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//glPointSize(2.0f);
	glDrawArrays(GL_POINTS, 0, meshWidth * meshHeight);

	animationTime += 0.01f;	

	//unbind vao
	glBindVertexArray(0);

	//stop using OpenGL program object
	glUseProgram(0);


	SwapBuffers(ghdc);
}

char* loadOclKernelSource(const char* filename, const char* preamble, size_t* sizeFinalLength)
{
	//locals
	FILE* pFile = NULL;
	size_t sizeSourceLength;

	pFile = fopen(filename, "rb");
	if (pFile == NULL)
		return(NULL);

	size_t sizePreambleLength = (size_t)strlen(preamble);

	//get the length of the source code
	fseek(pFile, 0, SEEK_END);
	sizeSourceLength = ftell(pFile);
	fseek(pFile, 0, SEEK_SET); //reset to beginning

	//allocate a buffer for the source code string and read it in
	char* sourceString = (char*)malloc(sizeSourceLength + sizePreambleLength + 1);
	memcpy(sourceString, preamble, sizePreambleLength);

	if (fread((sourceString)+sizePreambleLength, sizeSourceLength, 1, pFile) != 1)
	{
		fclose(pFile);
		free(sourceString);
		return(0);
	}

	//close the file and return the total length of the combined (preamble + source) string

	fclose(pFile);
	if (sizeFinalLength != 0)
	{
		*sizeFinalLength = sizeSourceLength + sizePreambleLength;
	}

	sourceString[sizeSourceLength + sizePreambleLength] = '\0';

	return(sourceString);


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

	if (clSourceCode)
	{
		free((void*)clSourceCode);
		clSourceCode = NULL;
	}

	if (cl_graphics_resource)
	{
		clReleaseMemObject(cl_graphics_resource);
		cl_graphics_resource = NULL;
	}

	if (clKernel)
	{
		clReleaseKernel(clKernel);
		clKernel = NULL;
	}

	if (clProgram)
	{
		clReleaseProgram(clProgram);
		clProgram = NULL;
	}

	if (clCommandQueue)
	{
		clReleaseCommandQueue(clCommandQueue);
		clCommandQueue = NULL;
	}

	if (clContext)
	{
		clReleaseContext(clContext);
		clContext = NULL;
	}

	if (vao)
	{
		glDeleteVertexArrays(1, &vao);
		vao = 0;
	}

	if (vbo_cpu)
	{
		glDeleteBuffers(1, &vbo_cpu);
		vbo_cpu = 0;
	}

	if (vbo_gpu)
	{
		glDeleteBuffers(1, &vbo_gpu);
		vbo_gpu = 0;
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

