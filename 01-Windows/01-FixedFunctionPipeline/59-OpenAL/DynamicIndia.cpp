//header files
#include<windows.h>
#include<stdio.h>
#include<stdlib.h>
#include<GL/GL.h>
#include<GL/GLU.h>
#include<al.h>
#include<alc.h>
#include<math.h>

#include "Resource.h"
#include "wavhelper.h"

#include"DrawObjects.h"

//macro functions
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "GLU32.lib")
#pragma comment(lib, "OpenAL32.lib")
//#pragma comment(lib, "Winmm.lib")

#define WIN_WIDTH 1280
#define WIN_HEIGHT 720


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

//bool PlaySong = false;

//OpenAL
ALCdevice* device = NULL;
ALCcontext* context = NULL;
unsigned int buffer;
unsigned int source;
ALsizei alSize, alFrequency;
ALenum alFormat;
ALvoid* alData;
ALboolean alLoop = AL_FALSE;


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

	x = (width / 2) - 640;
	y = (height / 2) - 360;

	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(FLAG_ICON));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(FLAG_ICON));

	RegisterClassEx(&wndclass);

	//CreateWindow
	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szAppName,
		TEXT("Dynamic India"),
		(WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE),
		0,
		0,
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
	void alInitialize(void);

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

	////Audio

	//if (PlaySong == false)
	//{
	//	PlaySound(MAKEINTRESOURCE(AUDIO), GetModuleHandle(NULL), SND_RESOURCE | SND_ASYNC);
	//	PlaySong = true;
	//}

	//OpenAL
	alInitialize();

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

	//Function Declaration
	void bgRect(void);
	void D(void);
	void Flag_A(void);

	//code	 

	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	//BG

	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	bgRect();


	//INDIA


	//Flag_A

	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	glScalef(India_scale_x, India_scale_y, 0.0f);

	Flag_A();


	//I

	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	glTranslatef(I1_x, 0.0f, 0.0f);
	glScalef(India_scale_x, India_scale_y, 0.0f);

	I1();


	//A

	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	glTranslatef(A_x, 0.0f, 0.0f);
	glScalef(India_scale_x, India_scale_y, 0.0f);

	A();


	//N

	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	glTranslatef(0.0f, N_y, 0.0f);
	glScalef(India_scale_x, India_scale_y, 0.0f);

	N();


	//I

	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	glTranslatef(0.0f, I2_y, 0.0f);
	glScalef(India_scale_x, India_scale_y, 0.0f);

	I2();


	//D

	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	glTranslatef(0.0f, 0.0f, 0.0f);
	glScalef(India_scale_x, India_scale_y, 0.0f);

	D();


	//Fighter Jets


	//1

	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	glTranslatef(1.25f * cos(Jet1_angle1) + r1_x, 1.25f * sin(Jet1_angle1) + r1_y, 0.0f);
	glTranslatef(Jet1_x, 0.0f, 0.0f);
	glRotatef(Jet1_rot, 0.0f, 0.0f, 1.0f);
	glScalef(0.8f, 0.8f, 0.8f);

	FighterJet1();


	//3

	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	glTranslatef(1.25f * cos(Jet3_angle1) + r3_x, 1.25f * sin(Jet3_angle1) + r3_y, 0.0f);
	glTranslatef(Jet3_x, 0.0f, 0.0f);
	glRotatef(Jet3_rot, 0.0f, 0.0f, 1.0f);
	glScalef(0.8f, 0.8f, 0.8f);

	FighterJet3();


	//2

	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	glTranslatef(Jet2_x, 0.0f, 0.0f);
	glScalef(0.8f, 0.8f, 0.8f);

	FighterJet2();

	//Smoke Tricolor
	glBegin(GL_QUADS);

	//saffron

	glColor3f(Smoke_light_Saffron_R, Smoke_light_Saffron_G, Smoke_light_Saffron_B);

	glVertex3f(-0.225f, 0.0067f, 0.0f);
	glVertex3f(-0.225f, 0.02f, 0.0f);

	glColor3f(Smoke_dark_Saffron_R, Smoke_dark_Saffron_G, Smoke_dark_Saffron_B);

	glVertex3f(-0.35f, 0.02f, 0.0f);
	glVertex3f(-0.35f, 0.0067f, 0.0f);

	//White

	glColor3f(Smoke_light_White_R, Smoke_light_White_G, Smoke_light_White_B);

	glVertex3f(-0.225f, -0.0067f, 0.0f);
	glVertex3f(-0.225f, 0.0067f, 0.0f);

	glColor3f(Smoke_dark_White_R, Smoke_dark_White_G, Smoke_dark_White_B);

	glVertex3f(-0.35f, 0.0067f, 0.0f);
	glVertex3f(-0.35f, -0.0067f, 0.0f);

	//Green

	glColor3f(Smoke_light_Green_R, Smoke_light_Green_G, Smoke_light_Green_B);

	glVertex3f(-0.225f, -0.02f, 0.0f);
	glVertex3f(-0.225f, -0.0067f, 0.0f);

	glColor3f(Smoke_dark_Green_R, Smoke_dark_Green_G, Smoke_dark_Green_B);

	glVertex3f(-0.35f, -0.0067f, 0.0f);
	glVertex3f(-0.35f, -0.02f, 0.0f);

	//glColor3f(0.0f, 0.0f, 0.082f);	

	glEnd();


	SwapBuffers(ghdc);


}

void Update(void)
{
	static int frame = 0;

	switch (frame)
	{

	case 0:

		//bgRect

		if (BG_blue <= 0.14f)
		{
			BG_blue += 0.0006f;
		}
		else
		{
			BG_blue = 0.14f;
		}


		//Flag_A

		if (Flag_A_Saffron_B <= 0.081)
		{
			Flag_A_Saffron_B += 0.0003f;
		}
		else
		{
			Flag_A_Saffron_B = 0.081f;
		}


		if (Flag_A_White_B <= 0.081)
		{
			Flag_A_White_B += 0.0003f;
		}
		else
		{
			Flag_A_White_B = 0.081f;
		}


		if (Flag_A_Green_B <= 0.081)
		{
			Flag_A_Green_B += 0.0003f;
		}
		else
		{
			Flag_A_Green_B = 0.081f;
		}


		//D

		if (D_SaffronB <= 0.081f)
		{
			D_SaffronB += 0.0003f;
		}
		else
		{
			D_SaffronB = 0.081f;
			frame++;
		}



		if (D_GreenB <= 0.064f)
		{
			D_GreenB += 0.0003f;
		}
		else
		{
			D_GreenB = 0.064f;
		}
		break;


		//I

	case 1:
		if (I1_x <= 0.0f)
		{
			I1_x += 0.0055f;
		}
		else
		{
			I1_x = 0.0f;
			frame++;
		}
		break;


		//A

	case 2:
		if (A_x >= 0.0f)
		{
			A_x -= 0.0056f;
		}
		else
		{
			A_x = 0.0f;
			frame++;
		}
		break;


		//N

	case 3:
		if (N_y >= 0.0f)
		{
			N_y -= 0.0053f;
		}
		else
		{
			N_y = 0.0f;
			frame++;
		}
		break;


		//I

	case 4:
		if (I2_y <= 0.0f)
		{
			I2_y += 0.005f;
		}
		else
		{
			I2_y = 0.0f;
			frame++;
		}
		break;


		//D

	case 5:
		if (D_SaffronR <= 1.0f)
		{
			D_SaffronR += 0.0028f;
		}
		else
		{
			D_SaffronR = 1.0f;
			frame++;
		}

		if (D_SaffronG <= 0.6f)
		{
			D_SaffronG += 0.0028f;
		}
		else
		{
			D_SaffronG = 0.6f;
		}

		if (D_SaffronB <= 0.2f)
		{
			D_SaffronB += 0.0028f;
		}
		else
		{
			D_SaffronB = 0.2f;
		}


		if (D_GreenR <= 0.07f)
		{
			D_GreenR += 0.0028f;
		}
		else
		{
			D_GreenR = 0.07f;
		}

		if (D_GreenG <= 0.53f)
		{
			D_GreenG += 0.0028f;
		}
		else
		{
			D_GreenG = 0.53f;
		}

		if (D_GreenB <= 0.03f)
		{
			D_GreenB += 0.0028f;
		}
		else
		{
			D_GreenB = 0.03f;
		}
		break;


		//INDIA Scale-up

	case 6:

		if (India_scale_x < 0.9f)
		{
			India_scale_x += 0.00032f;
		}
		else
		{
			India_scale_x = 0.9f;
		}

		if (India_scale_y < 0.9f)
		{
			India_scale_y += 0.00032f;
		}
		else
		{
			India_scale_y = 0.9f;
			frame++;
		}
		break;


	case 7:

		//Jet2

		if (Jet2_x < -1.0f)
		{
			Jet2_x += 0.0055f;
		}
		else
		{
			Jet2_x = -1.0f;
			//frame++;
		}


		//Jet1

		if (Jet1_angle1 <= 3 * PI / 2)
		{
			Jet1_angle1 += 0.0065f;
		}
		else
		{
			Jet1_angle1 = 3 * PI / 2;
		}

		if (Jet1_rot <= 360.0f)
		{
			Jet1_rot += 0.4f;
		}
		else
		{
			Jet1_rot = 360.0f;
		}



		//Jet3

		if (Jet3_angle1 >= PI / 2)
		{
			Jet3_angle1 -= 0.0065f;
		}
		else
		{
			Jet3_angle1 = PI / 2;
			frame++;
		}

		if (Jet3_rot >= 0.0f)
		{
			Jet3_rot -= 0.4f;
		}
		else
		{
			Jet3_rot = 0.0f;
		}
		break;


	case 8:

		//Smoke Tricolor

		//saffron

		//light

		if (Smoke_light_Saffron_R <= 1.0f)
		{
			Smoke_light_Saffron_R += 0.01f;
		}
		else
		{
			Smoke_light_Saffron_R = 1.0f;

		}

		if (Smoke_light_Saffron_G >= 0.6f)
		{
			Smoke_light_Saffron_G -= 0.01f;
		}
		else
		{
			Smoke_light_Saffron_G = 0.6f;

		}

		if (Smoke_light_Saffron_B >= 0.2f)
		{
			Smoke_light_Saffron_B -= 0.01f;
		}
		else
		{
			Smoke_light_Saffron_B = 0.2f;

		}

		//dark

		if (Smoke_dark_Saffron_R <= 1.0f)
		{
			Smoke_dark_Saffron_R += 0.01f;
		}
		else
		{
			Smoke_dark_Saffron_R = 1.0f;

		}

		if (Smoke_dark_Saffron_G <= 0.6f)
		{
			Smoke_dark_Saffron_G += 0.01f;
		}
		else
		{
			Smoke_dark_Saffron_G = 0.6f;

		}

		if (Smoke_dark_Saffron_B <= 0.2f)
		{
			Smoke_dark_Saffron_B += 0.01f;
		}
		else
		{
			Smoke_dark_Saffron_B = 0.2f;

		}

		//white

		//light

		if (Smoke_light_White_R <= 1.0f)
		{
			Smoke_light_White_R += 0.01f;
		}
		else
		{
			Smoke_light_White_R = 1.0f;

		}

		if (Smoke_light_White_G <= 1.0f)
		{
			Smoke_light_White_G += 0.01f;
		}
		else
		{
			Smoke_light_White_G = 1.0f;

		}

		if (Smoke_light_White_B <= 1.0f)
		{
			Smoke_light_White_B += 0.01f;
		}
		else
		{
			Smoke_light_White_B = 1.0f;

		}

		//dark

		if (Smoke_dark_White_R <= 1.0f)
		{
			Smoke_dark_White_R += 0.01f;
		}
		else
		{
			Smoke_dark_White_R = 1.0f;

		}

		if (Smoke_dark_White_G <= 1.0f)
		{
			Smoke_dark_White_G += 0.01f;
		}
		else
		{
			Smoke_dark_White_G = 1.0f;

		}

		if (Smoke_dark_White_B <= 1.0f)
		{
			Smoke_dark_White_B += 0.01f;
		}
		else
		{
			Smoke_dark_White_B = 1.0f;

		}


		//Green

		//light

		if (Smoke_light_Green_R <= 0.07f)
		{
			Smoke_light_Green_R += 0.01f;
		}
		else
		{
			Smoke_light_Green_R = 0.07f;

		}

		if (Smoke_light_Green_G >= 0.53f)
		{
			Smoke_light_Green_G -= 0.01f;
		}
		else
		{
			Smoke_light_Green_G = 0.53f;

		}

		if (Smoke_light_Green_B >= 0.03f)
		{
			Smoke_light_Green_B -= 0.01f;
		}
		else
		{
			Smoke_light_Green_B = 0.03f;

		}

		//dark

		if (Smoke_dark_Green_R <= 0.07f)
		{
			Smoke_dark_Green_R += 0.01f;
		}
		else
		{
			Smoke_dark_Green_R = 0.07f;

		}

		if (Smoke_dark_Green_G <= 0.53f)
		{
			Smoke_dark_Green_G += 0.01f;
		}
		else
		{
			Smoke_dark_Green_G = 0.53f;

		}

		if (Smoke_dark_Green_B <= 0.03f)
		{
			Smoke_dark_Green_B += 0.01f;
		}
		else
		{
			Smoke_dark_Green_B = 0.03f;

		}


		//Jet1

		if (Jet1_x <= 2.0f)
		{
			Jet1_x += 0.0055f;
		}
		else
		{
			Jet1_x = 2.0f;
			//frame++;
		}


		//Jet2

		if (Jet2_x < 1.0f)
		{
			Jet2_x += 0.0055f;
		}
		else
		{
			Jet2_x = 1.0f;
		}


		//Jet3

		if (Jet3_x < 2.0f)
		{
			Jet3_x += 0.0055f;
		}
		else
		{
			Jet3_x = 2.0f;
			frame++;
		}

		if (Jet2_x >= 0.5f)
		{
			//saffron
			if (Flag_A_Saffron_R <= 1.0f)
			{
				Flag_A_Saffron_R += 0.1f;
			}
			else
			{
				Flag_A_Saffron_R = 1.0f;
			}

			if (Flag_A_Saffron_G <= 0.6f)
			{
				Flag_A_Saffron_G += 0.1f;
			}
			else
			{
				Flag_A_Saffron_G = 0.6f;
			}

			if (Flag_A_Saffron_B <= 0.2f)
			{
				Flag_A_Saffron_B += 0.1f;
			}
			else
			{
				Flag_A_Saffron_B = 0.2f;
			}


			//white
			if (Flag_A_White_R <= 1.0f)
			{
				Flag_A_White_R += 0.1f;
			}
			else
			{
				Flag_A_White_R = 1.0f;
			}

			if (Flag_A_White_G <= 1.0f)
			{
				Flag_A_White_G += 0.1f;
			}
			else
			{
				Flag_A_White_G = 1.0f;
			}

			if (Flag_A_White_B <= 1.0f)
			{
				Flag_A_White_B += 0.1f;
			}
			else
			{
				Flag_A_White_B = 1.0f;
			}


			//green
			if (Flag_A_Green_R <= 0.07f)
			{
				Flag_A_Green_R += 0.1f;
			}
			else
			{
				Flag_A_Green_R = 0.07f;
			}

			if (Flag_A_Green_G <= 0.53f)
			{
				Flag_A_Green_G += 0.1f;
			}
			else
			{
				Flag_A_Green_G = 0.53f;
			}

			if (Flag_A_Green_B >= 0.03f)
			{
				Flag_A_Green_B -= 0.1f;
			}
			else
			{
				Flag_A_Green_B = 0.03f;
			}



			/*GLfloat Flag_A_Saffron_R = 0.0f;
			GLfloat Flag_A_Saffron_G = 0.0f;
			GLfloat Flag_A_Saffron_B = 0.0f;

			GLfloat Flag_A_White_R = 0.0f;
			GLfloat Flag_A_White_G = 0.0f;
			GLfloat Flag_A_White_B = 0.0f;

			GLfloat Flag_A_Green_R = 0.0f;
			GLfloat Flag_A_Green_G = 0.0f;
			GLfloat Flag_A_Green_B = 0.0f;*/
		}
		break;


	case 9:


		//Smoke Tricolor

		//saffron

		//light

		if (Smoke_light_Saffron_R <= 1.0f)
		{
			Smoke_light_Saffron_R += 0.05f;
		}
		else
		{
			Smoke_light_Saffron_R = 1.0f;

		}

		if (Smoke_light_Saffron_G <= 1.0f)
		{
			Smoke_light_Saffron_G += 0.05f;
		}
		else
		{
			Smoke_light_Saffron_G = 1.0f;

		}

		if (Smoke_light_Saffron_B <= 1.0f)
		{
			Smoke_light_Saffron_B += 0.05f;
		}
		else
		{
			Smoke_light_Saffron_B = 1.0f;

		}

		//dark

		if (Smoke_dark_Saffron_R >= 0.0f)
		{
			Smoke_dark_Saffron_R -= 0.05f;
		}
		else
		{
			Smoke_dark_Saffron_R = 0.0f;

		}

		if (Smoke_dark_Saffron_G >= 0.0f)
		{
			Smoke_dark_Saffron_G -= 0.05f;
		}
		else
		{
			Smoke_dark_Saffron_G = 0.0f;

		}

		if (Smoke_dark_Saffron_B >= 0.082f)
		{
			Smoke_dark_Saffron_B -= 0.05f;
		}
		else
		{
			Smoke_dark_Saffron_B = 0.082f;

		}

		//white

		//light

		if (Smoke_light_White_R <= 1.0f)
		{
			Smoke_light_White_R += 0.05f;
		}
		else
		{
			Smoke_light_White_R = 1.0f;

		}

		if (Smoke_light_White_G <= 1.0f)
		{
			Smoke_light_White_G += 0.05f;
		}
		else
		{
			Smoke_light_White_G = 1.0f;

		}

		if (Smoke_light_White_B <= 1.0f)
		{
			Smoke_light_White_B += 0.05f;
		}
		else
		{
			Smoke_light_White_B = 1.0f;
		}

		//dark

		if (Smoke_dark_White_R >= 0.0f)
		{
			Smoke_dark_White_R -= 0.05f;
		}
		else
		{
			Smoke_dark_White_R = 0.0f;

		}

		if (Smoke_dark_White_G >= 0.0f)
		{
			Smoke_dark_White_G -= 0.05f;
		}
		else
		{
			Smoke_dark_White_G = 0.0f;

		}

		if (Smoke_dark_White_B >= 0.082f)
		{
			Smoke_dark_White_B -= 0.05f;
		}
		else
		{
			Smoke_dark_White_B = 0.082f;

		}


		//Green

		//light

		if (Smoke_light_Green_R <= 1.0f)
		{
			Smoke_light_Green_R += 0.05f;
		}
		else
		{
			Smoke_light_Green_R = 1.0f;

		}

		if (Smoke_light_Green_G <= 1.0f)
		{
			Smoke_light_Green_G += 0.05f;
		}
		else
		{
			Smoke_light_Green_G = 1.0f;

		}

		if (Smoke_light_Green_B <= 1.0f)
		{
			Smoke_light_Green_B += 0.05f;
		}
		else
		{
			Smoke_light_Green_B = 1.0f;

		}

		//dark

		if (Smoke_dark_Green_R >= 0.0f)
		{
			Smoke_dark_Green_R -= 0.05f;
		}
		else
		{
			Smoke_dark_Green_R = 0.0f;

		}

		if (Smoke_dark_Green_G >= 0.0f)
		{
			Smoke_dark_Green_G -= 0.05f;
		}
		else
		{
			Smoke_dark_Green_G = 0.0f;

		}

		if (Smoke_dark_Green_B <= 0.082f)
		{
			Smoke_dark_Green_B += 0.05f;
		}
		else
		{
			Smoke_dark_Green_B = 0.082f;

		}


		//Jet1

		if (Jet1_angle1 <= 2.05 * PI)
		{
			Jet1_angle1 += 0.0065f;
		}
		else
		{
			Jet1_angle1 = 2.05 * PI;
		}

		if (Jet1_rot <= 450.0f)
		{
			Jet1_rot += 0.4f;
		}
		else
		{
			Jet1_rot = 450.0f;
		}


		//Jet3

		if (Jet3_angle1 >= -PI / 18)
		{
			Jet3_angle1 -= 0.0065f;
		}
		else
		{
			Jet3_angle1 = -PI / 18;
			frame++;
		}

		if (Jet3_rot >= -270.0f)
		{
			Jet3_rot -= 0.4f;
		}
		else
		{
			Jet3_rot = -270.0f;

		}

		//Jet2

		if (Jet2_x < 2.5f)
		{
			Jet2_x += 0.0055f;
		}
		else
		{
			Jet2_x = 2.5f;
		}
		break;



		//INDIA Scale-up

	case 10:

		if (India_scale_x < 1.0f)
		{
			India_scale_x += 0.00032f;
		}
		else
		{
			India_scale_x = 1.0f;
		}

		if (India_scale_y < 1.0f)
		{
			India_scale_y += 0.00032f;
		}
		else
		{
			India_scale_y = 1.0f;
			frame++;
		}
		break;


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

	/*if (PlaySong == true)
	{
		PlaySound(NULL, NULL, NULL);
	}*/

	//OpenAL

	alDeleteSources(1, &source);
	alDeleteBuffers(1, &buffer);

	context = alcGetCurrentContext();
	device = alcGetContextsDevice(context);
	alcMakeContextCurrent(NULL);
	alcDestroyContext(context);
	alcCloseDevice(device);

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

void alInitialize(void)
{
	int channel, sampleRate, beatsPerSecond, size;
	device = alcOpenDevice(NULL);

	if (device)
	{
		fprintf(gpFile, "Device created!\n");
		context = alcCreateContext(device, NULL);

		if (context != NULL)
		{
			fprintf(gpFile, "Context created!\n");
		}

		alcMakeContextCurrent(context);

		alGenBuffers(1, &buffer);
		alGenSources(1, &source);

		if (source != NULL)
		{
			fprintf(gpFile, "Source Generated!\n");
		}

		alData = loadWav("AbTumhareHawaleVatanSathiyon.wav", &channel, &sampleRate, &beatsPerSecond, &size);

		if (alData)
		{
			fprintf(gpFile, "Wave File Loaded!\n");
		}

		if (channel == 1)
		{
			if (beatsPerSecond == 8)
			{
				alFormat = AL_FORMAT_MONO8;
				fprintf(gpFile, "mono8\n");
			}
			else
			{
				alFormat = AL_FORMAT_MONO16;
				fprintf(gpFile, "mono16\n");
			}
		}
		else
		{
			if (beatsPerSecond == 8)
			{
				alFormat = AL_FORMAT_STEREO8;
				fprintf(gpFile, "stereo8\n");
			}
			else
			{
				alFormat = AL_FORMAT_STEREO16;
				fprintf(gpFile, "stereo16\n");
			}
		}



		alBufferData(buffer, alFormat, alData, size, sampleRate);

		alSourcei(source, AL_BUFFER, buffer);

		alSourcePlay(source);

		
	}

}
