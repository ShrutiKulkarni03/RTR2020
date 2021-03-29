//header files
#include<windows.h>
#include<stdio.h>
#include<stdlib.h>
#include<GL/GL.h>
#include<GL/GLU.h>
#include<math.h>
#include "Resources.h"

//macro functions
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "GLU32.lib")



//symbolic constants

#define TRUE 1
#define FALSE 0

#define BUFFER_SIZE 256
#define S_EQUAL 0

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#define NR_POINT_COORDS 3
#define NR_TEXTURE_COORDS 2
#define NR_NORMAL_COORDS 3
#define NR_FACE_TOKENS 3
#define NR_TRIANGLE_VERTICES 3

typedef struct vec_2d_int
{
	GLint** pp_arr;
	size_t size;
}vec_2d_int_t;

typedef struct vec_2d_float
{
	GLfloat** pp_arr;
	size_t size;
}vec_2d_float_t;



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
FILE* gpMeshFile = NULL;
//FILE* gfpLogfile = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;

GLfloat g_rotate;

vec_2d_float_t* gp_vertices = NULL;
vec_2d_float_t* gp_texture;
vec_2d_float_t* gp_normals;
vec_2d_int_t* gp_face_tri, *gp_face_texture, *gp_face_normals;


char g_line[BUFFER_SIZE];


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

	//function prototype
	void Update(void);

	if (fopen_s(&gpFile, "RenderLog.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("Cannot Open The Desired File\n"), TEXT("Error"), MB_OK);
		exit(0);
	}
	else
	{
		fprintf(gpFile, ("Log File Created Successfully, Program Started Successfully.\n"));
		fflush(gpFile);
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

	Uninitialize();
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
	void LoadMeshData(void);


	//variable declaration
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;

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

	//SetClearColor
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);    //black


	//depth
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	LoadMeshData();

	Resize(WIN_WIDTH, WIN_HEIGHT);


}

void Resize(int width, int height)
{
	//code
	if (height == 0)
		height = 1;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

}

void LoadMeshData(void)
{

	//function prototype
	vec_2d_int_t* create_vec_2d_int(void);
	vec_2d_float_t* create_vec_2d_float(void);

	void push_back_vec_2d_int(vec_2d_int_t *p_vec, int *p_arr);
	void push_back_vec_2d_float(vec_2d_float_t *p_vec, float *p_arr);

	void *xcalloc(int, size_t);

	//code

	gpMeshFile = fopen("MonkeyHeadMeshObj.obj", "r");


	if (!gpMeshFile)
	{
		fprintf(gpFile, "MonkeyHead LOG file unsuccessful.\n");
		Uninitialize();

	}
	else
	{
		fprintf(gpFile, "MonkeyHead LOG file successful.\n");
	}

	gp_vertices = create_vec_2d_float();

	gp_texture = create_vec_2d_float();
	gp_normals = create_vec_2d_float();

	gp_face_tri = create_vec_2d_int();
	gp_face_texture = create_vec_2d_int();
	gp_face_normals = create_vec_2d_int();

	//separator strings
	char* sep_space = " ";
	char* sep_fslash = "/";

	//token pointers
	char* first_token = NULL;
	char* token = NULL;

	char* face_tokens[NR_FACE_TOKENS];
	int nr_tokens;

	char* token_vertex_index = NULL;
	char* token_texture_index = NULL;
	char* token_normal_index = NULL;

	while (fgets(g_line, BUFFER_SIZE, gpMeshFile) != NULL)
	{

		first_token = strtok(g_line, sep_space);


		if (strcmp(first_token, "v") == S_EQUAL)
		{

			GLfloat* pvec_point_coord = (GLfloat*)xcalloc(NR_POINT_COORDS, sizeof(GLfloat));

			for (int i = 0; i != NR_POINT_COORDS; i++)
			{
				pvec_point_coord[i] = atof(strtok(NULL, sep_space));
			}

			push_back_vec_2d_float(gp_vertices, pvec_point_coord);
		}

		else if (strcmp(first_token, "vt") == S_EQUAL)
		{

			GLfloat* pvec_texture_coord = (GLfloat*)xcalloc(NR_TEXTURE_COORDS, sizeof(GLfloat));

			for (int i = 0; i != NR_TEXTURE_COORDS; i++)
			{
				pvec_texture_coord[i] = atof(strtok(NULL, sep_space));
			}

			push_back_vec_2d_float(gp_texture, pvec_texture_coord);
		}

		else if (strcmp(first_token, "vn") == S_EQUAL)
		{
			
			GLfloat* pvec_normal_coord = (GLfloat*)xcalloc(NR_NORMAL_COORDS, sizeof(GLfloat));

			for (int i = 0; i != NR_NORMAL_COORDS; i++)
			{
				pvec_normal_coord[i] = atof(strtok(NULL, sep_space));
			}

			push_back_vec_2d_float(gp_normals, pvec_normal_coord);
		}

		else if (strcmp(first_token, "f") == S_EQUAL)
		{

			GLint *pvec_vertex_indices = (GLint*)xcalloc(3, sizeof(GLint));
			GLint *pvec_texture_indices = (GLint*)xcalloc(3, sizeof(GLint));
			GLint *pvec_normal_indices = (GLint*)xcalloc(3, sizeof(GLint));
			
			memset((void*)face_tokens, 0, NR_FACE_TOKENS);

			nr_tokens = 0;

			while (token = strtok(NULL, sep_space))
			{

				if (strlen(token) < 3)
				{
					break;
				}

				face_tokens[nr_tokens] = token;
				nr_tokens++; 
			}

			for (int i = 0; i != NR_FACE_TOKENS; ++i)
			{
				token_vertex_index = strtok(face_tokens[i], sep_fslash);
				token_texture_index = strtok(NULL, sep_fslash);
				token_normal_index = strtok(NULL, sep_fslash);

				pvec_vertex_indices[i] = atoi(token_vertex_index);
				pvec_texture_indices[i] = atoi(token_texture_index);
				pvec_normal_indices[i] = atoi(token_normal_index);
			}

			push_back_vec_2d_int(gp_face_tri, pvec_vertex_indices);
			push_back_vec_2d_int(gp_face_texture, pvec_texture_indices);
			push_back_vec_2d_int(gp_face_normals, pvec_normal_indices);

		}

		memset((void*)g_line, (int)'\0', BUFFER_SIZE);
	}

	if (gpMeshFile)
	{
		fclose(gpMeshFile);
		gpMeshFile = NULL;
	}
	

	fprintf(gpFile, "gp_vertices->size : %zu gp_texture->size: %zu g_normals: %zu g_face_tri: %zu\n", gp_vertices->size, gp_texture->size, gp_normals->size, gp_face_tri->size);

}

void Display(void)
{

	//GLfloat x, y;

	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(0.0f, 0.0f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	glRotatef(g_rotate, 0.0f, 1.0f, 0.0f);
	glScalef(0.7f, 0.7f, 0.7f);

	glFrontFace(GL_CCW);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	for (int i = 0; i < gp_face_tri->size; ++i)
	{

		glBegin(GL_TRIANGLES);

		for (int j = 0; j < NR_TRIANGLE_VERTICES; j++)
		{

			int vi = gp_face_tri->pp_arr[i][j] - 1;
						
			glVertex3f(gp_vertices->pp_arr[vi][0], gp_vertices->pp_arr[vi][1], gp_vertices->pp_arr[vi][2]);

		}

		glEnd();

	}
		
	SwapBuffers(ghdc);

}

void Update(void)
{
	g_rotate += 0.1f;
	if (g_rotate >= 360.0f)
	{
		g_rotate = 0.0f;
	}
}

void Uninitialize(void)
{
	//code

	void clean_vec_2d_int(vec_2d_int_t **pp_vec);
	void clean_vec_2d_float(vec_2d_float_t **pp_vec);

	if (gbFullscreen == true)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);

		SetWindowLong(ghwnd, GWL_STYLE, (dwStyle | WS_OVERLAPPEDWINDOW));
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);

		ShowCursor(true);
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
		fprintf(gpFile, ("Log File Closed Successfully, Program Completed Successfully.\n"));
		fclose(gpFile);
		gpFile = NULL;
	}

	clean_vec_2d_float(&gp_normals);
	clean_vec_2d_float(&gp_texture);
	clean_vec_2d_float(&gp_vertices);

	clean_vec_2d_int(&gp_face_normals);
	clean_vec_2d_int(&gp_face_texture);
	clean_vec_2d_int(&gp_face_tri);


}

vec_2d_int_t* create_vec_2d_int(void)
{
	void* xcalloc(int nr_elements, size_t size_per_element);
	return(vec_2d_int_t*)xcalloc(1, sizeof(vec_2d_int_t));
}

vec_2d_float_t* create_vec_2d_float(void)
{
	void* xcalloc(int nr_elements, size_t size_per_element);
	return(vec_2d_float_t*)xcalloc(1, sizeof(vec_2d_float_t));
}

void push_back_vec_2d_int(vec_2d_int_t* p_vec, GLint* p_arr)
{
	void* xrealloc(void* p, size_t new_size);
	p_vec->pp_arr = (GLint**)xrealloc(p_vec->pp_arr, (p_vec->size + 1) * sizeof(int**));
	p_vec->size++;
	p_vec->pp_arr[p_vec->size - 1] = p_arr;
}

void push_back_vec_2d_float(vec_2d_float_t* p_vec, GLfloat* p_arr)
{
	void* xrealloc(void* p, size_t new_size);
	p_vec->pp_arr = (GLfloat**)xrealloc(p_vec->pp_arr, (p_vec->size + 1) * sizeof(GLfloat**));
	p_vec->size++;
	p_vec->pp_arr[p_vec->size - 1] = p_arr;
}

void clean_vec_2d_int(vec_2d_int_t** pp_vec)
{
	vec_2d_int_t* p_vec = *pp_vec;
	for (size_t i = 0; i != p_vec->size; i++)
	{
		free(p_vec->pp_arr[i]);
	}
	free(p_vec);
	*pp_vec = NULL;
}

void clean_vec_2d_float(vec_2d_float_t** pp_vec)
{
	vec_2d_float_t* p_vec = *pp_vec;
	for (size_t i = 0; i != p_vec->size; i++)
	{
		free(p_vec->pp_arr[i]);
	}
	free(p_vec);
	*pp_vec = NULL;
}

void* xcalloc(int nr_elements, size_t size_per_element)
{
	void* p = calloc(nr_elements, size_per_element);

	if (!p)
	{
		fprintf(gpFile, "\ncalloc:fatal:out of memory\n");
		Uninitialize();
	}
	return(p);
}

void* xrealloc(void * p, size_t new_size)
{
	void* ptr = realloc(p, new_size);

	if (!ptr)
	{
		fprintf(gpFile, "\nrealloc:fatal:out of memory\n");
		Uninitialize();
	}
	return(ptr);
}





