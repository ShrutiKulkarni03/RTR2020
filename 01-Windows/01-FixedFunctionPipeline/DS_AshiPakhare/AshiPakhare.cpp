//header files
#include<windows.h>
#include<stdio.h>
#include<stdlib.h>
#include<GL/GL.h>
#include<GL/GLU.h>
#include<math.h>
#include<malloc.h>
#include "Resources.h"
#include "SceneObjects.h"

//macro functions
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "GLU32.lib")
#pragma comment(lib, "Winmm.lib")

#define WIN_WIDTH 1280
#define WIN_HEIGHT 720


struct node
{
	void (*FunctionPointer)(void);
	struct node* next;
}*front = NULL, *rear = NULL;



//global function declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void enqueue(void(*FunctionPointer)(void));
void dequeue(void);
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
GLuint nFontList;

bool PlaySong = false;




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

void enqueue(void (*FunctionPointer) (void))
{
	struct node* newnode;
	newnode = (struct node*)malloc(sizeof(struct node));
	if (newnode == NULL)
	{
		
		return;
	}

	newnode->FunctionPointer = FunctionPointer;
	newnode->next = NULL;

	if (front == NULL && rear == NULL)
	{
		front = rear = newnode;
	}
	else
	{
		rear->next = newnode;
		rear = newnode;
	}

}

void dequeue(void)
{
	//front->FunctionPointer();
	front = front->next;
}

void Initialize(void)
{
	//function prototype
	void Resize(int, int);
	bool loadGLTexture(GLuint*, TCHAR[]);

	//variable declaration
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;

	HFONT hFont;
	LOGFONT logfont;

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


	logfont.lfHeight = 60;
	logfont.lfWidth = 0;
	logfont.lfEscapement = 0;
	logfont.lfOrientation = 0;
	logfont.lfWeight = FALSE;
	logfont.lfItalic = FALSE;
	logfont.lfUnderline = FALSE;
	logfont.lfStrikeOut = FALSE;
	logfont.lfCharSet = ANSI_CHARSET;
	logfont.lfOutPrecision = OUT_DEFAULT_PRECIS;
	logfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	logfont.lfQuality = DEFAULT_QUALITY;
	logfont.lfPitchAndFamily = DEFAULT_PITCH;
	strcpy(logfont.lfFaceName, "Kruti Dev 100");

	hFont = CreateFontIndirect(&logfont);
	SelectObject(ghdc, hFont);

	nFontList = glGenLists(128);

	wglUseFontBitmaps(ghdc, 0, 128, nFontList);

	DeleteObject(hFont);




	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);


	//Loading Textures
	loadGLTexture(&grass_texture, MAKEINTRESOURCE(GRASS_BITMAP));
	loadGLTexture(&tree1_texture, MAKEINTRESOURCE(TREE1_BITMAP));
	loadGLTexture(&tree2_texture, MAKEINTRESOURCE(TREE2_BITMAP));
	loadGLTexture(&tree_bark1_texture, MAKEINTRESOURCE(TREE_BARK1_BITMAP));
	loadGLTexture(&tree_bark2_texture, MAKEINTRESOURCE(TREE_BARK2_BITMAP));
	loadGLTexture(&top1_texture, MAKEINTRESOURCE(TOP_FABRIC1_BITMAP));
	loadGLTexture(&skirt1_texture, MAKEINTRESOURCE(SKIRT_FABRIC1_BITMAP));
	loadGLTexture(&skirt2_texture, MAKEINTRESOURCE(SKIRT_FABRIC2_BITMAP));
	loadGLTexture(&top2_texture, MAKEINTRESOURCE(TOP_FABRIC2_BITMAP));
	loadGLTexture(&mountain_texture, MAKEINTRESOURCE(MOUNTAINS_BITMAP));
	loadGLTexture(&concrete_texture, MAKEINTRESOURCE(CONCRETE_BITMAP));
	loadGLTexture(&wall_texture, MAKEINTRESOURCE(WALL_BITMAP));
	loadGLTexture(&front_wall_texture, MAKEINTRESOURCE(FRONT_WALL_BITMAP));
	loadGLTexture(&railing_texture, MAKEINTRESOURCE(RAILING_BITMAP));
	loadGLTexture(&roof_texture, MAKEINTRESOURCE(ROOF_BITMAP));

	glEnable(GL_TEXTURE_2D);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//SetClearColor
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);    //black

	//Audio

	if (PlaySong == false)
	{
		PlaySound(MAKEINTRESOURCE(AUDIO), GetModuleHandle(NULL), SND_RESOURCE | SND_ASYNC);
		PlaySong = true;
	}

	enqueue(Scene1);
	enqueue(Scene2);
	enqueue(Scene3);
	enqueue(Scene4);
	enqueue(Scene5);

	Resize(WIN_WIDTH, WIN_HEIGHT);
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


		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

		glGenTextures(1, texture);

		glBindTexture(GL_TEXTURE_2D, *texture);

		//setting texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		//PUSHING DATA INTO GRAPHIC MEMORY USING GRAPHIC DRIVER

		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, sk_bmp.bmWidth, sk_bmp.bmHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, sk_bmp.bmBits);

		DeleteObject(sk_hBitmap);

	}

	return(bResult);

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
	//function prototype
	void House(void);
	void Sky(void);
	void Floor(void);
	void Mountains(void);
	void Sun(void);
	void RailingBlock(void);
	void Tree(void);
	void Girl1(void);
	void Girl2(void);
	void Bird1(void);
	void Bird2(void);
	void Bird3(void);
	void Bird4(void);
	void Rice(void);
	void FadeInQuad(void);


	//variable declaration
	GLfloat RailingX = 0.0f;
	GLfloat RailingY = 0.0f;
	GLfloat RailingZ = 0.0f;

	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//gluLookAt(0.0f, 0.0, 15.0f, 0.0f, 0.0, 0.0f, 0.0f, 1.0f, 0.0f);     //trials
	gluLookAt(0.0f, LookAtTranslateY, 15.0f, 0.0f, LookAtTranslateY, 0.0f, 0.0f, 1.0f, 0.0f);     //MainCamera
	//gluLookAt(0.0f, 25.0f, 15.0f, 0.0f, 25.0f, 0.0f, 0.0f, 1.0f, 0.0f);  //EndCreditsFrame
	//gluLookAt(0.0f, 30.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f);    //top view

	glPushMatrix();

	/******DRAW OBJECTS******/


	/*****SUN*****/

	glTranslatef(SunTranslate.X, SunTranslate.Y, SunTranslate.Z);
	glScalef(3.0f, 3.0f, 0.0f);

	Sun();

	glPopMatrix();
	glPushMatrix();


	/*****SKY*****/


	Sky();


	/*****GROUND*****/


	Floor();



	/****RICE****/

	glTranslatef(2.0f, -2.0f, 1.0f);

	glColor4f(1.0f, 1.0f, 1.0f, RiceAlpha);

	Rice();

	glPopMatrix();
	glPushMatrix();



	/*****HOUSE*****/


	glTranslatef(-5.0f, -2.0f, 0.0f);
	glScalef(1.2f, 1.3f, 1.2f);
	glRotatef(50.0f, 0.0f, 1.0f, 0.0f);

	House();

	glPopMatrix();
	glPushMatrix();



	/*****RAILING*****/

	/******RIGHT RAILING HORIZONTAL BLOCKS*****/

	for (RailingY = -2.0f; RailingY <= -1.0f; RailingY += 0.5f)
	{

		glRotatef(50.0f, 0.0f, 1.0f, 0.0f);
		glTranslatef(0.0f, RailingY, -15.0);
		glScalef(0.8f, 0.15f, 96.0f);



		RailingBlock();

		glPopMatrix();
		glPushMatrix();

	}


	/******LEFT RAILING HORIZONTAL BLOCKS*****/

	for (RailingY = -2.0f; RailingY <= -1.0f; RailingY += 0.5f)
	{

		glRotatef(50.0f, 0.0f, 1.0f, 0.0f);
		glTranslatef(-18.5f, RailingY, 0.0);
		glScalef(0.8f, 0.15f, 45.0f);



		RailingBlock();

		glPopMatrix();
		glPushMatrix();

	}


	/******BACK RAILING HORIZONTAL BLOCKS*****/

	for (RailingY = -2.0f; RailingY <= -1.0f; RailingY += 0.5f)
	{

		glRotatef(140.0f, 0.0f, 1.0f, 0.0f);
		glTranslatef(6.82f, RailingY, -9.0);
		glScalef(0.8f, 0.15f, 85.0f);



		RailingBlock();

		glPopMatrix();
		glPushMatrix();

	}


	/******FRONT RAILING HORIZONTAL BLOCKS*****/

	for (RailingY = -2.0f; RailingY <= -1.0f; RailingY += 0.5f)
	{

		glRotatef(160.0f, 0.0f, 1.0f, 0.0f);
		glTranslatef(-14.65f, RailingY, -10.0);
		glScalef(0.8f, 0.15f, 80.0f);



		RailingBlock();

		glPopMatrix();
		glPushMatrix();

	}


	/******RIGHT RAILING VERTICAL BLOCKS*****/

	for (RailingZ = -14.0f; RailingZ <= 3.0f; RailingZ += 1.0f)
	{

		glRotatef(50.0f, 0.0f, 1.0f, 0.0f);
		glTranslatef(0.0f, -2.0f, RailingZ);
		glScalef(0.8f, 0.8f, 0.8f);



		RailingBlock();

		glPopMatrix();
		glPushMatrix();

	}


	/******FRONT RAILING VERTICAL BLOCKS*****/

	for (RailingX = -2.0f; RailingX >= -17.0f; RailingX -= 1.0f)
	{

		glRotatef(70.0f, 0.0f, 1.0f, 0.0f);
		glTranslatef(RailingX, -2.0f, 6.5f);
		glScalef(0.8f, 0.8f, 0.8f);

		RailingBlock();

		glPopMatrix();
		glPushMatrix();
	}


	/******LEFT RAILING VERTICAL BLOCKS*****/

	for (RailingZ = 0.0f; RailingZ <= 7.0f; RailingZ += 1.0f)
	{

		glRotatef(50.0f, 0.0f, 1.0f, 0.0f);
		glTranslatef(-18.5f, -2.0f, RailingZ);
		glScalef(0.8f, 0.8f, 0.8f);

		RailingBlock();

		glPopMatrix();
		glPushMatrix();
	}


	/******BACK RAILING VERTICAL BLOCKS*****/

	for (RailingX = 0.0f; RailingX >= -17.0f; RailingX -= 1.0f)
	{

		glRotatef(50.0f, 0.0f, 1.0f, 0.0f);
		glTranslatef(RailingX, -2.0f, -15.0f);
		glScalef(0.8f, 0.8f, 0.8f);

		RailingBlock();

		glPopMatrix();
		glPushMatrix();
	}


	//glPopMatrix();
	//glPushMatrix();



	/*****MOUNTAINS*****/


	glTranslatef(-20.0f, 0.0f, -25.0f);
	glScalef(25.0f, 6.0f, 8.0f);
	glRotatef(20, 0.0f, 1.0f, 0.0f);

	Mountains();

	glPopMatrix();
	glPushMatrix();




	glTranslatef(-4.0f, 0.0f, -24.5f);
	glScalef(18.0f, 7.0f, 8.0f);
	glRotatef(-9.0, 0.0f, 1.0f, 0.0f);

	Mountains();

	glPopMatrix();
	glPushMatrix();




	glTranslatef(7.0f, 0.0f, -24.0f);
	glScalef(30.0f, 10.0f, 8.0f);
	glRotatef(-10.0, 0.0f, 1.0f, 0.0f);

	Mountains();

	glPopMatrix();
	glPushMatrix();




	glTranslatef(20.0f, 0.0f, -24.5f);
	glScalef(35.0f, 8.0f, 8.0f);
	glRotatef(6.0, 0.0f, 1.0f, 0.0f);

	Mountains();

	glPopMatrix();
	glPushMatrix();



	/******TREES*****/


	Tree();

	glPopMatrix();
	glPushMatrix();



	/****GIRL1****/


	if (isGirl1Visible == true)
	{
		Girl1();
	}


	glPopMatrix();
	glPushMatrix();



	/****GIRL2****/

	if (isGirl2Visible == true)
	{
		Girl2();
	}
		

	glPopMatrix();
	glPushMatrix();



	/****BIRD1****/

	Bird1();

	glPopMatrix();
	glPushMatrix();


	/****BIRD2****/

	Bird2();

	glPopMatrix();
	glPushMatrix();


	/****BIRD3****/

	Bird3();

	glPopMatrix();
	glPushMatrix();


	/****BIRD4****/

	Bird4();

	glPopMatrix();
	glPushMatrix();



	/****FONT[END CREDITS]****/


	glColor3f(1.0f, 1.0f, 1.0f);

	glRasterPos3f(-2.0, 26.0, 0.0);
	glListBase(nFontList);


	glCallLists(17, GL_UNSIGNED_BYTE, "v'kh ik[kjs ;srh!");


	glRasterPos3f(-3.0, 25.0, 0.0);
	glListBase(nFontList);


	glCallLists(21, GL_UNSIGNED_BYTE, "doh & eaxs'k ikMxkodj");


	glRasterPos3f(-3.1, 24, 0.0);
	glListBase(nFontList);


	glCallLists(22, GL_UNSIGNED_BYTE, "ewG xk;d & lq/khj QMds");

	glPopMatrix();
	glPushMatrix();


	/*****FADE-IN QUAD*****/

	glColor4f(0.0f, 0.0f, 0.0f, FadeInQuadAlpha);

	FadeInQuad();

	glColor3f(1.0f, 1.0f, 1.0f);

	SwapBuffers(ghdc);
}

/*void Update(void)
{

	
	Timer += TimerSpeed;

	SunTranslate.Y += SunTranslateSpeed;
	if (SunTranslate.Y >= 22.0f)
		SunTranslate.Y = 22.0f;

	FadeInQuadAlpha -= FadeInQuadAlphaSpeed;
	if (FadeInQuadAlpha <= 0.0f)
	{
		FadeInQuadAlpha = 0.0f;
	}

	
	//Scene5();
	//Scene4();
	//Scene3();
	//Scene2();
	//Scene1();

}*/

void Update(void)
{
	Timer += TimerSpeed;

	SunTranslate.Y += SunTranslateSpeed;
	if (SunTranslate.Y >= 50.0f)
		SunTranslate.Y = 50.0f;

	FadeInQuadAlpha -= FadeInQuadAlphaSpeed;
	if (FadeInQuadAlpha <= 0.0f)
	{
		FadeInQuadAlpha = 0.0f;
	}


	static int SkyGradient = 0;

	switch (SkyGradient)
	{
		//Morning to Noon

	case 0:

		SkyMid.R -= SkyColorAnimateSpeed;
		if (SkyMid.R <= 0.29f)
		{
			SkyMid.R = 0.29f;
		}

		SkyMid.G -= SkyColorAnimateSpeed;
		if (SkyMid.G <= 0.45f)
		{
			SkyMid.G = 0.45f;
		}

		SkyMid.B -= SkyColorAnimateSpeed;
		if (SkyMid.B <= 0.63f)
		{
			SkyMid.B = 0.63f;
		}



		SkyBottom.R -= SkyColorAnimateSpeed;
		if (SkyBottom.R <= 0.78f)
		{
			SkyBottom.R = 0.78f;
		}

		SkyBottom.G += SkyColorAnimateSpeed;
		if (SkyBottom.G >= 0.84f)
		{
			SkyBottom.G = 0.84f;
		}

		SkyBottom.B += SkyColorAnimateSpeed;
		if (SkyBottom.B >= 0.9f)
		{
			SkyBottom.B = 0.9f;
			SkyGradient++;
		}

		break;


		//Noon to Evening

	case 1:

		SkyMid.R += SkyColorAnimateSpeed;
		if (SkyMid.R >= 0.47f)
		{
			SkyMid.R = 0.47f;
		}

		SkyMid.G += SkyColorAnimateSpeed;
		if (SkyMid.G >= 0.56f)
		{
			SkyMid.G = 0.56f;
		}

		SkyMid.B += SkyColorAnimateSpeed;
		if (SkyMid.B >= 0.96f)
		{
			SkyMid.B = 0.96f;
		}



		SkyBottom.R += SkyColorAnimateSpeed;
		if (SkyBottom.R >= 0.91f)
		{
			SkyBottom.R = 0.91f;
		}

		SkyBottom.G -= SkyColorAnimateSpeed;
		if (SkyBottom.G <= 0.73f)
		{
			SkyBottom.G = 0.73f;
		}

		SkyBottom.B -= SkyColorAnimateSpeed;
		if (SkyBottom.B <= 0.74f)
		{
			SkyBottom.B = 0.74f;
			SkyGradient++;
		}

		break;


		//Evening to Night

	case 2:

		SkyMid.R -= SkyColorAnimateSpeed;
		if (SkyMid.R <= 0.12f)
		{
			SkyMid.R = 0.12f;
		}

		SkyMid.G -= SkyColorAnimateSpeed;
		if (SkyMid.G <= 0.17f)
		{
			SkyMid.G = 0.17f;
		}

		SkyMid.B -= SkyColorAnimateSpeed;
		if (SkyMid.B <= 0.28f)
		{
			SkyMid.B = 0.28f;
		}



		SkyBottom.R -= SkyColorAnimateSpeed;
		if (SkyBottom.R <= 0.54f)
		{
			SkyBottom.R = 0.54f;
		}

		SkyBottom.G -= SkyColorAnimateSpeed;
		if (SkyBottom.G <= 0.58f)
		{
			SkyBottom.G = 0.58f;
		}

		SkyBottom.B -= SkyColorAnimateSpeed;
		if (SkyBottom.B <= 0.58f)
		{
			SkyBottom.B = 0.58f;
			SkyGradient++;
		}

		break;

	}


	switch (Scene)
	{

		//SCENE 1



	case 0:


		if (SunTranslate.Y >= 0.0f)
		{
			isBird2Still = false;
			isScene1 = true;
		}

		if (isScene1 == true)
		{
			//Bird2 (red)

			switch (B2WingsRot)
			{
			case 0:
				Bird2WingsRotate -= BirdsWingsSpeed;
				if (Bird2WingsRotate <= WingSwingMin)
				{
					Bird2WingsRotate = WingSwingMin;
					B2WingsRot++;
				}
				break;

			case 1:
				Bird2WingsRotate += BirdsWingsSpeed;
				if (Bird2WingsRotate >= WingSwingMax)
				{
					Bird2WingsRotate = WingSwingMax;
					B2WingsRot--;
				}
				break;
			}


			Bird2Translate.X -= BirdsTranslateSpeed - 0.005f;
			if (Bird2Translate.X <= 6.5f)
			{
				Bird2Translate.X = 6.5f;
			}

			Bird2Translate.Y -= BirdsTranslateSpeed - 0.005f;
			if (Bird2Translate.Y <= -0.5f)
			{
				Bird2Translate.Y = -0.5f;
			}

			Bird2Translate.Z -= BirdsTranslateSpeed - 0.005f;
			if (Bird2Translate.Z <= 1.0f)
			{
				Bird2Translate.Z = 1.0f;
				isBird2Still = true;
			}


			if (isBird2Still == true)
			{
				Bird2WingsRotate = 165.0f;
			}


			//Bird3 (purple)


			if (SunTranslate.Y >= 0.0f)
			{
				isBird3Still = false;
			}


			if (isBird3Still == false)
			{

				static int B3WingsRot = 0;
				switch (B3WingsRot)
				{
				case 0:
					Bird3WingsRotate -= BirdsWingsSpeed;
					if (Bird3WingsRotate <= WingSwingMin)
					{
						Bird3WingsRotate = WingSwingMin;
						B3WingsRot++;
					}
					break;

				case 1:
					Bird3WingsRotate += BirdsWingsSpeed;
					if (Bird3WingsRotate >= WingSwingMax)
					{
						Bird3WingsRotate = WingSwingMax;
						B3WingsRot--;
					}
					break;
				}

				Bird3Translate.X += BirdsTranslateSpeed;
				if (Bird3Translate.X >= -3.6f)
				{
					Bird3Translate.X = -3.6f;
				}

				Bird3Translate.Y -= BirdsTranslateSpeed - 0.013175f;
				if (Bird3Translate.Y <= 2.8f)
				{
					Bird3Translate.Y = 2.8f;
					isBird3Still = true;
					isGirl1Visible = true;
					isGirl1Still = false;
					isScene2 = true;
					Scene++;
				}

				Bird3Translate.Z -= BirdsTranslateSpeed;
				if (Bird3Translate.Z <= -0.9f)
				{
					Bird3Translate.Z = -0.9f;
				}

			}

			if (isBird3Still == true)
			{
				Bird3WingsRotate = 165.0f;
			}

		}

		break;



				
		//SCENE2



	case 1:


		if (isScene2 == true)
		{




			switch (Girl1Movements)
			{
			case 0:



				switch (RightHandLoop)
				{

				case 0:
					Girl1RtShoulderRotate -= GirlsHandsRotateSpeed;
					if (Girl1RtShoulderRotate <= HandSwingMin)
					{
						Girl1RtShoulderRotate = HandSwingMin;
						RightHandLoop++;
					}
					break;

				case 1:
					Girl1RtShoulderRotate += GirlsHandsRotateSpeed;
					if (Girl1RtShoulderRotate >= HandSwingMax)
					{
						Girl1RtShoulderRotate = HandSwingMax;
						RightHandLoop--;
					}
					break;
				}




				switch (LeftHandLoop)
				{

				case 0:
					Girl1LtShoulderRotate += GirlsHandsRotateSpeed;
					if (Girl1LtShoulderRotate >= HandSwingMax)
					{
						Girl1LtShoulderRotate = HandSwingMax;
						LeftHandLoop++;
					}
					break;

				case 1:
					Girl1LtShoulderRotate -= GirlsHandsRotateSpeed;
					if (Girl1LtShoulderRotate <= HandSwingMin)
					{
						Girl1LtShoulderRotate = HandSwingMin;
						LeftHandLoop--;
					}
					break;
				}




				Girl1Translate.X += GirlsTranslateSpeed;
				if (Girl1Translate.X >= -1.0f)
				{
					Girl1Translate.X = -1.0f;
					//isGirl1Still = true;
					Girl1Movements++;
				}

				Girl1Translate.Z -= GirlsTranslateSpeed - 0.0036f;
				if (Girl1Translate.Z <= 0.8f)
				{
					Girl1Translate.Z = 0.8f;

				}

				break;



			case 1:

				Girl1RtShoulderRotate = 270.0f;
				Girl1LtShoulderRotate = 270.0f;
				Girl1Movements++;

				break;



			case 2:

				if (Girl1RtShoulderRotate < 340.0f)
				{
					Girl1RtShoulderRotate += 0.8f; // GirlsHandsRotateSpeed;
				}
				else
				{
					Girl1RtShoulderRotate = 340.0f;
					Girl1Movements++;
				}
				break;


			case 3:
				RiceAlpha += RiceAlphaSpeed;
				if (RiceAlpha >= 1.0f)
				{
					RiceAlpha = 1.0f;
					isRiceFeed = false;
					isBird1Still = false;
					isBird2Still = false;
					isBird3Still = false;
					isBird4Still = false;
					Girl1Movements++;
				}
				break;

			case 4:

				if (Girl1RtShoulderRotate > 270.0f)
				{
					Girl1RtShoulderRotate -= 1.0f; // GirlsHandsRotateSpeed;
				}
				else
				{
					Girl1RtShoulderRotate = 270.0f;
					isGirl2Visible = true;
					Girl1Movements++;
					//Scene++;
				}
				break;


			case 5:
				//Bird1 blue

				if (isBird1Still == false)
				{


					switch (B2WingsRot)
					{
					case 0:
						Bird1WingsRotate -= BirdsWingsSpeed;
						if (Bird1WingsRotate <= WingSwingMin)
						{
							Bird1WingsRotate = WingSwingMin;
							B2WingsRot++;
						}
						break;

					case 1:
						Bird1WingsRotate += BirdsWingsSpeed;
						if (Bird1WingsRotate >= WingSwingMax)
						{
							Bird1WingsRotate = WingSwingMax;
							B2WingsRot--;
						}
						break;
					}

					Bird1RotateAngle += BirdsRotateSpeed + 0.3f;
					if (Bird1RotateAngle >= 90.0f)
					{
						Bird1RotateAngle = 90.0f;
					}



					Bird1Translate.X -= BirdsTranslateSpeed +0.015f;
					if (Bird1Translate.X <= 2.0f)
					{
						Bird1Translate.X = 2.0f;
						isBird1Still = true;
					}

					Bird1Translate.Y -= BirdsTranslateSpeed;
					if (Bird1Translate.Y <= -1.75f)
					{
						Bird1Translate.Y = -1.75f;
						//isBird1Still = true;
					}

					Bird1Translate.Z += BirdsTranslateSpeed +0.01f;
					if (Bird1Translate.Z >= 0.0f)
					{
						Bird1Translate.Z = 0.0f;
						//isBird1Still = true;
					}

				}

				if (isBird1Still == true)
				{
					Bird1WingsRotate = 165.0f;
				}



				//Bird2 red



				if (isBird2Still == false)
				{


					switch (B2WingsRot)
					{
					case 0:
						Bird2WingsRotate -= BirdsWingsSpeed;
						if (Bird2WingsRotate <= WingSwingMin)
						{
							Bird2WingsRotate = WingSwingMin;
							B2WingsRot++;
						}
						break;

					case 1:
						Bird2WingsRotate += BirdsWingsSpeed;
						if (Bird2WingsRotate >= WingSwingMax)
						{
							Bird2WingsRotate = WingSwingMax;
							B2WingsRot--;
						}
						break;
					}



					Bird2RotateAngle += BirdsRotateSpeed;
					if (Bird2RotateAngle >= 90.0f)
					{
						Bird2RotateAngle = 90.0f;
					}


					Bird2Translate.X -= BirdsTranslateSpeed + 0.03f;
					if (Bird2Translate.X <= 3.0f)
					{
						Bird2Translate.X = 3.0f;
						isBird2Still = true;
					}

					Bird2Translate.Y -= BirdsTranslateSpeed;
					if (Bird2Translate.Y <= -1.75f)
					{
						Bird2Translate.Y = -1.75f;

					}

					Bird2Translate.Z += BirdsTranslateSpeed + 0.035f;
					if (Bird2Translate.Z >= 0.3f)
					{
						Bird2Translate.Z = 0.3f;

					}

				}

				if (isBird2Still == true)
				{
					Bird2WingsRotate = 165.0f;
				}




				//Bird3 purple


				if (isBird3Still == false)
				{


					switch (B2WingsRot)
					{
					case 0:
						Bird3WingsRotate -= BirdsWingsSpeed;
						if (Bird3WingsRotate <= WingSwingMin)
						{
							Bird3WingsRotate = WingSwingMin;
							B2WingsRot++;
						}
						break;

					case 1:
						Bird3WingsRotate += BirdsWingsSpeed;
						if (Bird3WingsRotate >= WingSwingMax)
						{
							Bird3WingsRotate = WingSwingMax;
							B2WingsRot--;
						}
						break;
					}


					Bird3RotateAngle -= BirdsRotateSpeed;
					if (Bird3RotateAngle <= 110.0f)
					{
						Bird3RotateAngle = 110.0f;
					}


					Bird3Translate.X += BirdsTranslateSpeed;
					if (Bird3Translate.X >= 1.2f)
					{
						Bird3Translate.X = 1.2f;
						isBird3Still = true;
						Scene++;
					}

					Bird3Translate.Y -= BirdsTranslateSpeed - 0.00125f;
					if (Bird3Translate.Y <= -1.75f)
					{
						Bird3Translate.Y = -1.75f;
						//isBird3Still = true;
						//Scene++;
					}

					Bird3Translate.Z += BirdsTranslateSpeed;
					if (Bird3Translate.Z >= 0.2f)
					{
						Bird3Translate.Z = 0.2f;
						//isBird3Still = true;
					}

				}

				if (isBird3Still == true)
				{
					Bird3WingsRotate = 165.0f;
					isScene3 = true;
				}



				break;


			}
		}

		break;




		//SCENE 3



	case 2:


		if (isScene3 == true)
		{
			switch (Girl2Movements)
			{
			case 0:


				//Bird4 orange



				if (isBird4Still == false)
				{


					switch (B2WingsRot)
					{
					case 0:
						Bird4WingsRotate -= BirdsWingsSpeed;
						if (Bird4WingsRotate <= WingSwingMin)
						{
							Bird4WingsRotate = WingSwingMin;
							B2WingsRot++;
						}
						break;

					case 1:
						Bird4WingsRotate += BirdsWingsSpeed;
						if (Bird4WingsRotate >= WingSwingMax)
						{
							Bird4WingsRotate = WingSwingMax;
							B2WingsRot--;
						}
						break;
					}



					Bird4RotateAngle -= BirdsRotateSpeed;
					if (Bird4RotateAngle <= 0.0f)
					{
						Bird4RotateAngle = 0.0f;
					}


					Bird4Translate.X -= BirdsTranslateSpeed + 0.00375f;
					if (Bird4Translate.X <= 4.0f)
					{
						Bird4Translate.X = 4.0f;
						//isBird4Still = true;
					}

					Bird4Translate.Y -= BirdsTranslateSpeed;
					if (Bird4Translate.Y <= -1.75f)
					{
						Bird4Translate.Y = -1.75f;
						isBird4Still = true;
					}

					Bird4Translate.Z -= BirdsTranslateSpeed + 0.00175f;
					if (Bird4Translate.Z <= 1.0f)
					{
						Bird4Translate.Z = 1.0f;

					}

				}

				if (isBird4Still == true)
				{
					Bird4WingsRotate = 165.0f;
				}




				//Girl1


				Girl1RotateAngle -= GirlsRotateSpeed;
				if (Girl1RotateAngle <= 10.0f)
				{
					Girl1RotateAngle = 10.0f;
					//Girl2Movements++;
				}




				Girl2Translate.X -= GirlsTranslateSpeed;
				if (Girl2Translate.X <= -2.0f)
				{
					Girl2Translate.X = -2.0f;
					//isGirl1Still = true;
					Girl2Movements++;
				}



				Girl2Translate.Z -= GirlsTranslateSpeed + 0.0073;
				if (Girl2Translate.Z <= 4.0f)
				{
					Girl2Translate.Z = 4.0f;

				}



				switch (RightHandLoop)
				{

				case 0:
					Girl2RtShoulderRotate -= GirlsHandsRotateSpeed;
					if (Girl2RtShoulderRotate <= HandSwingMin)
					{
						Girl2RtShoulderRotate = HandSwingMin;
						RightHandLoop++;
					}
					break;

				case 1:
					Girl2RtShoulderRotate += GirlsHandsRotateSpeed;
					if (Girl2RtShoulderRotate >= HandSwingMax)
					{
						Girl2RtShoulderRotate = HandSwingMax;
						RightHandLoop--;
					}
					break;
				}




				switch (LeftHandLoop)
				{

				case 0:
					Girl2LtShoulderRotate += GirlsHandsRotateSpeed;
					if (Girl2LtShoulderRotate >= HandSwingMax)
					{
						Girl2LtShoulderRotate = HandSwingMax;
						LeftHandLoop++;
					}
					break;

				case 1:
					Girl2LtShoulderRotate -= GirlsHandsRotateSpeed;
					if (Girl2LtShoulderRotate <= HandSwingMin)
					{
						Girl2LtShoulderRotate = HandSwingMin;
						LeftHandLoop--;
					}
					break;
				}



				if (Girl2Translate.X <= 1.0f)
				{
					Girl1RotateAngle -= GirlsRotateSpeed;
					if (Girl1RotateAngle <= 290.0f)
					{
						Girl1RotateAngle = 290.0f;
						//Girl2Movements++;
					}

					Girl1Translate.X -= GirlsTranslateSpeed;
					if (Girl1Translate.X <= -4.0f)
					{
						Girl1Translate.X = -4.0f;
						//isGirl1Still = true;
						isGirl1Visible = false;
					}

					Girl1Translate.Z += GirlsTranslateSpeed - 0.002f;
					if (Girl1Translate.Z >= 2.0f)
					{
						Girl1Translate.Z = 2.0f;
					}




					switch (RightHandLoop)
					{

					case 0:
						Girl1RtShoulderRotate -= GirlsHandsRotateSpeed;
						if (Girl1RtShoulderRotate <= HandSwingMin)
						{
							Girl1RtShoulderRotate = HandSwingMin;
							RightHandLoop++;
						}
						break;

					case 1:
						Girl1RtShoulderRotate += GirlsHandsRotateSpeed;
						if (Girl1RtShoulderRotate >= HandSwingMax)
						{
							Girl1RtShoulderRotate = HandSwingMax;
							RightHandLoop--;
						}
						break;
					}




					switch (LeftHandLoop)
					{

					case 0:
						Girl1LtShoulderRotate += GirlsHandsRotateSpeed;
						if (Girl1LtShoulderRotate >= HandSwingMax)
						{
							Girl1LtShoulderRotate = HandSwingMax;
							LeftHandLoop++;
						}
						break;

					case 1:
						Girl1LtShoulderRotate -= GirlsHandsRotateSpeed;
						if (Girl1LtShoulderRotate <= HandSwingMin)
						{
							Girl1LtShoulderRotate = HandSwingMin;
							LeftHandLoop--;
						}
						break;
					}


				}



				break;



			case 1:

				Girl2Translate.Y += GirlsTranslateSpeed; //- 0.001f;
				if (Girl2Translate.Y >= -0.05f)
				{
					Girl2Translate.Y = -0.05f;
					Girl2Movements++;
				}
				break;


			case 2:


				Girl2Translate.X -= GirlsTranslateSpeed;
				if (Girl2Translate.X <= -4.0f)
				{
					Girl2Translate.X = -4.0f;
					Girl2Movements++;
					isGirl2Still = true;
				}


				Girl2Translate.Z -= GirlsTranslateSpeed;
				if (Girl2Translate.Z <= 2.0f)
				{
					Girl2Translate.Z = 2.0f;

				}



				switch (RightHandLoop)
				{

				case 0:
					Girl2RtShoulderRotate -= GirlsHandsRotateSpeed;
					if (Girl2RtShoulderRotate <= HandSwingMin)
					{
						Girl2RtShoulderRotate = HandSwingMin;
						RightHandLoop++;
					}
					break;

				case 1:
					Girl2RtShoulderRotate += GirlsHandsRotateSpeed;
					if (Girl2RtShoulderRotate >= HandSwingMax)
					{
						Girl2RtShoulderRotate = HandSwingMax;
						RightHandLoop--;
					}
					break;
				}




				switch (LeftHandLoop)
				{

				case 0:
					Girl2LtShoulderRotate += GirlsHandsRotateSpeed;
					if (Girl2LtShoulderRotate >= HandSwingMax)
					{
						Girl2LtShoulderRotate = HandSwingMax;
						LeftHandLoop++;
					}
					break;

				case 1:
					Girl2LtShoulderRotate -= GirlsHandsRotateSpeed;
					if (Girl2LtShoulderRotate <= HandSwingMin)
					{
						Girl2LtShoulderRotate = HandSwingMin;
						LeftHandLoop--;
					}
					break;
				}

				break;


			case 3:
				if (isGirl2Still == true)
				{
					isGirl2Visible = false;
					Girl2RotateAngle = 30.0f;
					isBird1Still = false;
					isBird2Still = false;
					isBird3Still = false;
					isBird4Still = false;
					isScene4 = true;

					Scene++;
				}

				break;

			}
			
		}

		break;

		


		//SCENE 4



	case 3:

		if (isScene4 == true)
		{
			//Bird3 purple


			if (isBird3Still == false)
			{


				switch (B2WingsRot)
				{
				case 0:
					Bird3WingsRotate -= BirdsWingsSpeed;
					if (Bird3WingsRotate <= WingSwingMin)
					{
						Bird3WingsRotate = WingSwingMin;
						B2WingsRot++;
					}
					break;

				case 1:
					Bird3WingsRotate += BirdsWingsSpeed;
					if (Bird3WingsRotate >= WingSwingMax)
					{
						Bird3WingsRotate = WingSwingMax;
						B2WingsRot--;
					}
					break;
				}


				Bird3RotateAngle += BirdsRotateSpeed + 4.0f;
				if (Bird3RotateAngle >= 360.0f)
				{
					Bird3RotateAngle = 360.0f;
				}


				Bird3Translate.X -= BirdsTranslateSpeed;
				if (Bird3Translate.X <= -13.0f)
				{
					Bird3Translate.X = -13.0f;
					isBird3Still = true;
					//BirdBlockings++;

				}

				Bird3Translate.Y += BirdsTranslateSpeed;
				if (Bird3Translate.Y >= 7.0f)
				{
					Bird3Translate.Y = 7.0f;
					//isBird3Still = true;
					//Scene++;
				}

				Bird3Translate.Z -= BirdsTranslateSpeed;
				if (Bird3Translate.Z <= 0.0f)
				{
					Bird3Translate.Z = 0.0f;
					//isBird3Still = true;
				}

			}

			if (isBird3Still == true)
			{
				Bird3WingsRotate = 165.0f;
			}





			//Bird4 orange



			if (isBird4Still == false)
			{


				switch (B2WingsRot)
				{
				case 0:
					Bird4WingsRotate -= BirdsWingsSpeed;
					if (Bird4WingsRotate <= WingSwingMin)
					{
						Bird4WingsRotate = WingSwingMin;
						B2WingsRot++;
					}
					break;

				case 1:
					Bird4WingsRotate += BirdsWingsSpeed;
					if (Bird4WingsRotate >= WingSwingMax)
					{
						Bird4WingsRotate = WingSwingMax;
						B2WingsRot--;
					}
					break;
				}



				Bird4RotateAngle -= BirdsRotateSpeed;
				if (Bird4RotateAngle <= 0.0f)
				{
					Bird4RotateAngle = 0.0f;
				}


				Bird4Translate.X -= BirdsTranslateSpeed + 0.0015f;
				if (Bird4Translate.X <= -13.0f)
				{
					Bird4Translate.X = -13.0f;
					//isBird4Still = true;

				}

				Bird4Translate.Y += BirdsTranslateSpeed;
				if (Bird4Translate.Y >= 7.0f)
				{
					Bird4Translate.Y = 7.0f;
					isBird4Still = true;
					isBird1Still = false;
					isGirl2Still = false;
					isGirl2Visible = true;
					isScene5 = true;
					Scene++;

				}

				Bird4Translate.Z -= BirdsTranslateSpeed + 0.0015f;
				if (Bird4Translate.Z <= 0.0f)
				{
					Bird4Translate.Z = 0.0f;


				}

			}

			if (isBird4Still == true)
			{
				Bird4WingsRotate = 165.0f;


				Bird1RotateAngle += BirdsRotateSpeed + 0.8f;
				if (Bird1RotateAngle >= 300.0f)
				{
					Bird1RotateAngle = 300.0f;
				}
			}



		}

		break;




		//SCENE 5



	case 4:

		if (isScene5 == true)
		{
			//Bird1 blue

			if (isBird1Still == false)
			{


				switch (B2WingsRot)
				{
				case 0:
					Bird1WingsRotate -= BirdsWingsSpeed;
					if (Bird1WingsRotate <= WingSwingMin)
					{
						Bird1WingsRotate = WingSwingMin;
						B2WingsRot++;
					}
					break;

				case 1:
					Bird1WingsRotate += BirdsWingsSpeed;
					if (Bird1WingsRotate >= WingSwingMax)
					{
						Bird1WingsRotate = WingSwingMax;
						B2WingsRot--;
					}
					break;
				}





				Bird1Translate.X += BirdsTranslateSpeed + 0.006f;
				if (Bird1Translate.X >= 5.3f)
				{
					Bird1Translate.X = 5.3f;

				}

				Bird1Translate.Y += BirdsTranslateSpeed;
				if (Bird1Translate.Y >= 0.1f)
				{
					Bird1Translate.Y = 0.1f;
					//isBird1Still = true;
				}

				Bird1Translate.Z -= BirdsTranslateSpeed + 0.004f;
				if (Bird1Translate.Z <= -3.1f)
				{
					Bird1Translate.Z = -3.1f;
					//isBird1Still = true;
				}

			}

			if (isBird1Still == true)
			{
				Bird1WingsRotate = 165.0f;

				Bird1RotateAngle -= BirdsRotateSpeed + 0.3f;
				if (Bird1RotateAngle <= 40.0f)
				{
					Bird1RotateAngle = 40.0f;
				}
			}




			//Bird2 red



			if (isBird2Still == false)
			{


				switch (B2WingsRot)
				{
				case 0:
					Bird2WingsRotate -= BirdsWingsSpeed;
					if (Bird2WingsRotate <= WingSwingMin)
					{
						Bird2WingsRotate = WingSwingMin;
						B2WingsRot++;
					}
					break;

				case 1:
					Bird2WingsRotate += BirdsWingsSpeed;
					if (Bird2WingsRotate >= WingSwingMax)
					{
						Bird2WingsRotate = WingSwingMax;
						B2WingsRot--;
					}
					break;
				}



				Bird2RotateAngle -= BirdsRotateSpeed;
				if (Bird2RotateAngle <= 210.0f)
				{
					Bird2RotateAngle = 210.0f;
				}


				Bird2Translate.X += BirdsTranslateSpeed;
				if (Bird2Translate.X >= 9.0f)
				{
					Bird2Translate.X = 9.0f;
					isBird2Still = true;

				}

				Bird2Translate.Y += BirdsTranslateSpeed;
				if (Bird2Translate.Y >= 5.5f)
				{
					Bird2Translate.Y = 5.5f;

				}

				Bird2Translate.Z += BirdsTranslateSpeed;
				if (Bird2Translate.Z >= 4.0f)
				{
					Bird2Translate.Z = 4.0f;

				}

			}

			if (isBird2Still == true)
			{
				Bird2WingsRotate = 165.0f;
				//BirdBlockings++;

				//Scene++;
			}

			RiceAlpha -= RiceAlphaSpeed;
			if (RiceAlpha >= 0.0f)
			{
				RiceAlpha = 0.0f;
				isRiceFeed = false;

			}






			switch (Girl2Movements)
			{
			case 3:

				Girl2Translate.X += GirlsTranslateSpeed + 0.002f;
				if (Girl2Translate.X >= -2.0f)
				{
					Girl2Translate.X = -2.0f;
					Girl2Movements++;
					//isGirl2Still = true;
				}


				Girl2Translate.Z += GirlsTranslateSpeed + 0.001f;
				if (Girl2Translate.Z >= 4.0f)
				{
					Girl2Translate.Z = 4.0f;

				}



				switch (RightHandLoop)
				{

				case 0:
					Girl2RtShoulderRotate -= GirlsHandsRotateSpeed;
					if (Girl2RtShoulderRotate <= HandSwingMin)
					{
						Girl2RtShoulderRotate = HandSwingMin;
						RightHandLoop++;
					}
					break;

				case 1:
					Girl2RtShoulderRotate += GirlsHandsRotateSpeed;
					if (Girl2RtShoulderRotate >= HandSwingMax)
					{
						Girl2RtShoulderRotate = HandSwingMax;
						RightHandLoop--;
					}
					break;
				}




				switch (LeftHandLoop)
				{

				case 0:
					Girl2LtShoulderRotate += GirlsHandsRotateSpeed;
					if (Girl2LtShoulderRotate >= HandSwingMax)
					{
						Girl2LtShoulderRotate = HandSwingMax;
						LeftHandLoop++;
					}
					break;

				case 1:
					Girl2LtShoulderRotate -= GirlsHandsRotateSpeed;
					if (Girl2LtShoulderRotate <= HandSwingMin)
					{
						Girl2LtShoulderRotate = HandSwingMin;
						LeftHandLoop--;
					}
					break;
				}


				break;





			case 4:

				Girl2Translate.Y -= GirlsTranslateSpeed; //- 0.001f;
				if (Girl2Translate.Y <= -0.3f)
				{
					Girl2Translate.Y = -0.3f;
					Girl2Movements++;
				}


				break;





			case 5:

				Girl2Translate.X += GirlsTranslateSpeed;
				if (Girl2Translate.X >= 2.0f)
				{
					Girl2Translate.X = 2.0f;
					//isGirl1Still = true;
					//isGirl1Visible = false;
					
				}

				Girl2Translate.Z += GirlsTranslateSpeed + 0.006;
				if (Girl2Translate.Z >= 15.0f)
				{
					Girl2Translate.Z = 15.0f;
					isScene6 = true;
					Scene++;
				}


				switch (RightHandLoop)
				{

				case 0:
					Girl2RtShoulderRotate -= GirlsHandsRotateSpeed;
					if (Girl2RtShoulderRotate <= HandSwingMin)
					{
						Girl2RtShoulderRotate = HandSwingMin;
						RightHandLoop++;
					}
					break;

				case 1:
					Girl2RtShoulderRotate += GirlsHandsRotateSpeed;
					if (Girl2RtShoulderRotate >= HandSwingMax)
					{
						Girl2RtShoulderRotate = HandSwingMax;
						RightHandLoop--;
					}
					break;
				}




				switch (LeftHandLoop)
				{

				case 0:
					Girl2LtShoulderRotate += GirlsHandsRotateSpeed;
					if (Girl2LtShoulderRotate >= HandSwingMax)
					{
						Girl2LtShoulderRotate = HandSwingMax;
						LeftHandLoop++;
					}
					break;

				case 1:
					Girl2LtShoulderRotate -= GirlsHandsRotateSpeed;
					if (Girl2LtShoulderRotate <= HandSwingMin)
					{
						Girl2LtShoulderRotate = HandSwingMin;
						LeftHandLoop--;
					}
					break;
				}


			}
		}
		
		break;
		
			


		//SCENE 6



	case 5:

		if (isScene6 == true)
		{
			LookAtTranslateY += LookAtTranslateSpeed;
			if (LookAtTranslateY >= 25.0f)
			{
				LookAtTranslateY = 25.0f;
			}
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

	//Audio 

	if (PlaySong == true)
	{
		PlaySound(NULL, NULL, NULL);
	}



	//Delete Textures

	glDeleteTextures(1, &grass_texture);
	glDeleteTextures(1, &tree1_texture);
	glDeleteTextures(1, &tree2_texture);
	glDeleteTextures(1, &tree_bark1_texture);
	glDeleteTextures(1, &tree_bark2_texture);
	glDeleteTextures(1, &top1_texture);
	glDeleteTextures(1, &skirt1_texture);
	glDeleteTextures(1, &skirt2_texture);
	glDeleteTextures(1, &top2_texture);
	glDeleteTextures(1, &mountain_texture);
	glDeleteTextures(1, &concrete_texture);
	glDeleteTextures(1, &wall_texture);
	glDeleteTextures(1, &front_wall_texture);
	glDeleteTextures(1, &railing_texture);
	glDeleteTextures(1, &roof_texture);



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

	gluDeleteQuadric(quadric);


}

