//header files
#include<windows.h>
#include<stdio.h>
#include<stdlib.h>
#include<GL/GL.h>
#include<GL/GLU.h>
#include<math.h>
#include "SPK.h"

//macro functions
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "GLU32.lib")

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

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

void Display(void)
{
	//code
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(0.0f, 0.0f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	glPointSize(3.0f);

	glBegin(GL_POINTS);

	glColor3f(1.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);

	glEnd();

	glBegin(GL_LINES);	
	
	glColor3f(0.0f, 1.0f, 0.0f);	

	glVertex3f(0.0f, 2.0f, 0.0f);	
	glVertex3f(0.0f, -2.0f, 0.0f);

	//positive x axis

	glColor3f(0.0f, 0.0f, 1.0f);

	glVertex3f(0.05f, 2.0f, 0.0f);
	glVertex3f(0.05f, -2.0f, 0.0f);

	glVertex3f(0.1f, 2.0f, 0.0f);
	glVertex3f(0.1f, -2.0f, 0.0f);

	glVertex3f(0.15f, 2.0f, 0.0f);
	glVertex3f(0.15f, -2.0f, 0.0f);

	glVertex3f(0.2f, 2.0f, 0.0f);
	glVertex3f(0.2f, -2.0f, 0.0f);

	glVertex3f(0.25f, 2.0f, 0.0f);
	glVertex3f(0.25f, -2.0f, 0.0f);

	glVertex3f(0.3f, 2.0f, 0.0f);
	glVertex3f(0.3f, -2.0f, 0.0f);

	glVertex3f(0.35f, 2.0f, 0.0f);
	glVertex3f(0.35f, -2.0f, 0.0f);

	glVertex3f(0.4f, 2.0f, 0.0f);
	glVertex3f(0.4f, -2.0f, 0.0f);

	glVertex3f(0.45f, 2.0f, 0.0f);
	glVertex3f(0.45f, -2.0f, 0.0f);

	glVertex3f(0.5f, 2.0f, 0.0f);
	glVertex3f(0.5f, -2.0f, 0.0f);

	glVertex3f(0.55f, 2.0f, 0.0f);
	glVertex3f(0.55f, -2.0f, 0.0f);

	glVertex3f(0.6f, 2.0f, 0.0f);
	glVertex3f(0.6f, -2.0f, 0.0f);

	glVertex3f(0.65f, 2.0f, 0.0f);
	glVertex3f(0.65f, -2.0f, 0.0f);

	glVertex3f(0.7f, 2.0f, 0.0f);
	glVertex3f(0.7f, -2.0f, 0.0f);

	glVertex3f(0.75f, 2.0f, 0.0f);
	glVertex3f(0.75f, -2.0f, 0.0f);

	glVertex3f(0.8f, 2.0f, 0.0f);
	glVertex3f(0.8f, -2.0f, 0.0f);

	glVertex3f(0.85f, 2.0f, 0.0f);
	glVertex3f(0.85f, -2.0f, 0.0f);

	glVertex3f(0.9f, 2.0f, 0.0f);
	glVertex3f(0.9f, -2.0f, 0.0f);

	glVertex3f(0.95f, 2.0f, 0.0f);
	glVertex3f(0.95f, -2.0f, 0.0f);

	glVertex3f(1.0f, 2.0f, 0.0f);
	glVertex3f(1.0f, -2.0f, 0.0f);

	glVertex3f(1.05f, 2.0f, 0.0f);
	glVertex3f(1.05f, -2.0f, 0.0f);

	glVertex3f(1.1f, 2.0f, 0.0f);
	glVertex3f(1.1f, -2.0f, 0.0f);

	glVertex3f(1.15f, 2.0f, 0.0f);
	glVertex3f(1.15f, -2.0f, 0.0f);

	glVertex3f(1.2f, 2.0f, 0.0f);
	glVertex3f(1.2f, -2.0f, 0.0f);

	glVertex3f(1.25f, 2.0f, 0.0f);
	glVertex3f(1.25f, -2.0f, 0.0f);

	glVertex3f(1.3f, 2.0f, 0.0f);
	glVertex3f(1.3f, -2.0f, 0.0f);

	glVertex3f(1.35f, 2.0f, 0.0f);
	glVertex3f(1.35f, -2.0f, 0.0f);

	glVertex3f(1.4f, 2.0f, 0.0f);
	glVertex3f(1.4f, -2.0f, 0.0f);

	glVertex3f(1.45f, 2.0f, 0.0f);
	glVertex3f(1.45f, -2.0f, 0.0f);

	glVertex3f(1.5f, 2.0f, 0.0f);
	glVertex3f(1.5f, -2.0f, 0.0f);

	glVertex3f(1.55f, 2.0f, 0.0f);
	glVertex3f(1.55f, -2.0f, 0.0f);

	glVertex3f(1.6f, 2.0f, 0.0f);
	glVertex3f(1.6f, -2.0f, 0.0f);

	glVertex3f(1.65f, 2.0f, 0.0f);
	glVertex3f(1.65f, -2.0f, 0.0f);

	glVertex3f(1.7f, 2.0f, 0.0f);
	glVertex3f(1.7f, -2.0f, 0.0f);

	glVertex3f(1.75f, 2.0f, 0.0f);
	glVertex3f(1.75f, -2.0f, 0.0f);

	glVertex3f(1.8f, 2.0f, 0.0f);
	glVertex3f(1.8f, -2.0f, 0.0f);

	glVertex3f(1.85f, 2.0f, 0.0f);
	glVertex3f(1.85f, -2.0f, 0.0f);

	glVertex3f(1.9f, 2.0f, 0.0f);
	glVertex3f(1.9f, -2.0f, 0.0f);

	glVertex3f(1.95f, 2.0f, 0.0f);
	glVertex3f(1.95f, -2.0f, 0.0f);

	glVertex3f(2.0f, 2.0f, 0.0f);
	glVertex3f(2.0f, -2.0f, 0.0f);

	glVertex3f(2.05f, 2.0f, 0.0f);
	glVertex3f(2.05f, -2.0f, 0.0f);

	glVertex3f(2.1f, 2.0f, 0.0f);
	glVertex3f(2.1f, -2.0f, 0.0f);

	glVertex3f(2.15f, 2.0f, 0.0f);
	glVertex3f(2.15f, -2.0f, 0.0f);

	glVertex3f(2.2f, 2.0f, 0.0f);
	glVertex3f(2.2f, -2.0f, 0.0f);

	//negative x axis

	glVertex3f(-0.05f, 2.0f, 0.0f);
	glVertex3f(-0.05f, -2.0f, 0.0f);

	glVertex3f(-0.1f, 2.0f, 0.0f);
	glVertex3f(-0.1f, -2.0f, 0.0f);

	glVertex3f(-0.15f, 2.0f, 0.0f);
	glVertex3f(-0.15f, -2.0f, 0.0f);

	glVertex3f(-0.2f, 2.0f, 0.0f);
	glVertex3f(-0.2f, -2.0f, 0.0f);

	glVertex3f(-0.25f, 2.0f, 0.0f);
	glVertex3f(-0.25f, -2.0f, 0.0f);

	glVertex3f(-0.3f, 2.0f, 0.0f);
	glVertex3f(-0.3f, -2.0f, 0.0f);

	glVertex3f(-0.35f, 2.0f, 0.0f);
	glVertex3f(-0.35f, -2.0f, 0.0f);

	glVertex3f(-0.4f, 2.0f, 0.0f);
	glVertex3f(-0.4f, -2.0f, 0.0f);

	glVertex3f(-0.45f, 2.0f, 0.0f);
	glVertex3f(-0.45f, -2.0f, 0.0f);

	glVertex3f(-0.5f, 2.0f, 0.0f);
	glVertex3f(-0.5f, -2.0f, 0.0f);

	glVertex3f(-0.55f, 2.0f, 0.0f);
	glVertex3f(-0.55f, -2.0f, 0.0f);

	glVertex3f(-0.6f, 2.0f, 0.0f);
	glVertex3f(-0.6f, -2.0f, 0.0f);

	glVertex3f(-0.65f, 2.0f, 0.0f);
	glVertex3f(-0.65f, -2.0f, 0.0f);

	glVertex3f(-0.7f, 2.0f, 0.0f);
	glVertex3f(-0.7f, -2.0f, 0.0f);

	glVertex3f(-0.75f, 2.0f, 0.0f);
	glVertex3f(-0.75f, -2.0f, 0.0f);

	glVertex3f(-0.8f, 2.0f, 0.0f);
	glVertex3f(-0.8f, -2.0f, 0.0f);

	glVertex3f(-0.85f, 2.0f, 0.0f);
	glVertex3f(-0.85f, -2.0f, 0.0f);

	glVertex3f(-0.9f, 2.0f, 0.0f);
	glVertex3f(-0.9f, -2.0f, 0.0f);

	glVertex3f(-0.95f, 2.0f, 0.0f);
	glVertex3f(-0.95f, -2.0f, 0.0f);

	glVertex3f(-1.0f, 2.0f, 0.0f);
	glVertex3f(-1.0f, -2.0f, 0.0f);

	glVertex3f(-1.05f, 2.0f, 0.0f);
	glVertex3f(-1.05f, -2.0f, 0.0f);

	glVertex3f(-1.1f, 2.0f, 0.0f);
	glVertex3f(-1.1f, -2.0f, 0.0f);

	glVertex3f(-1.15f, 2.0f, 0.0f);
	glVertex3f(-1.15f, -2.0f, 0.0f);

	glVertex3f(-1.2f, 2.0f, 0.0f);
	glVertex3f(-1.2f, -2.0f, 0.0f);

	glVertex3f(-1.25f, 2.0f, 0.0f);
	glVertex3f(-1.25f, -2.0f, 0.0f);

	glVertex3f(-1.3f, 2.0f, 0.0f);
	glVertex3f(-1.3f, -2.0f, 0.0f);

	glVertex3f(-1.35f, 2.0f, 0.0f);
	glVertex3f(-1.35f, -2.0f, 0.0f);

	glVertex3f(-1.4f, 2.0f, 0.0f);
	glVertex3f(-1.4f, -2.0f, 0.0f);

	glVertex3f(-1.45f, 2.0f, 0.0f);
	glVertex3f(-1.45f, -2.0f, 0.0f);

	glVertex3f(-1.5f, 2.0f, 0.0f);
	glVertex3f(-1.5f, -2.0f, 0.0f);

	glVertex3f(-1.55f, 2.0f, 0.0f);
	glVertex3f(-1.55f, -2.0f, 0.0f);

	glVertex3f(-1.6f, 2.0f, 0.0f);
	glVertex3f(-1.6f, -2.0f, 0.0f);

	glVertex3f(-1.65f, 2.0f, 0.0f);
	glVertex3f(-1.65f, -2.0f, 0.0f);

	glVertex3f(-1.7f, 2.0f, 0.0f);
	glVertex3f(-1.7f, -2.0f, 0.0f);

	glVertex3f(-1.75f, 2.0f, 0.0f);
	glVertex3f(-1.75f, -2.0f, 0.0f);

	glVertex3f(-1.8f, 2.0f, 0.0f);
	glVertex3f(-1.8f, -2.0f, 0.0f);

	glVertex3f(-1.85f, 2.0f, 0.0f);
	glVertex3f(-1.85f, -2.0f, 0.0f);

	glVertex3f(-1.9f, 2.0f, 0.0f);
	glVertex3f(-1.9f, -2.0f, 0.0f);

	glVertex3f(-1.95f, 2.0f, 0.0f);
	glVertex3f(-1.95f, -2.0f, 0.0f);

	glVertex3f(-2.0f, 2.0f, 0.0f);
	glVertex3f(-2.0f, -2.0f, 0.0f);

	glVertex3f(-2.05f, 2.0f, 0.0f);
	glVertex3f(-2.05f, -2.0f, 0.0f);

	glVertex3f(-2.1f, 2.0f, 0.0f);
	glVertex3f(-2.1f, -2.0f, 0.0f);

	glVertex3f(-2.15f, 2.0f, 0.0f);
	glVertex3f(-2.15f, -2.0f, 0.0f);

	glVertex3f(-2.2f, 2.0f, 0.0f);
	glVertex3f(-2.2f, -2.0f, 0.0f);

	glEnd();
	
	SwapBuffers(ghdc);
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
}

