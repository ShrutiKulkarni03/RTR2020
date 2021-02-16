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


//global function declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void enqueue(void(*FunctionPointer)(void));
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
GLuint nFontListMarathi;
GLuint nFontListEnglish;
GLuint nFontListPresents;
GLuint nFontListSir;
//bool PlaySong = false;


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
		TEXT("FFP Project - SHRUTI KULKARNI"),
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

struct node* Peek(void)
{
	return front;
}

void Initialize(void)
{
	//function prototype
	void Resize(int, int);
	bool loadGLTexture(GLuint*, TCHAR[]);

	//variable declaration
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;

	HFONT hFontMarathi;
	LOGFONT logfontMarathi;

	HFONT hFontEnglish;
	LOGFONT logfontEnglish;

	HFONT hFontPresents;
	LOGFONT logfontPresents;

	HFONT hFontSir;
	LOGFONT logfontSir;

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


	//MARATHI

	logfontMarathi.lfHeight = 50;
	logfontMarathi.lfWidth = 0;
	logfontMarathi.lfEscapement = 0;
	logfontMarathi.lfOrientation = 0;
	logfontMarathi.lfWeight = FALSE;
	logfontMarathi.lfItalic = FALSE;
	logfontMarathi.lfUnderline = FALSE;
	logfontMarathi.lfStrikeOut = FALSE;
	logfontMarathi.lfCharSet = ANSI_CHARSET;
	logfontMarathi.lfOutPrecision = OUT_DEFAULT_PRECIS;
	logfontMarathi.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	logfontMarathi.lfQuality = DEFAULT_QUALITY;
	logfontMarathi.lfPitchAndFamily = DEFAULT_PITCH;
	strcpy(logfontMarathi.lfFaceName, "Kruti Dev 100");

	hFontMarathi = CreateFontIndirect(&logfontMarathi);
	SelectObject(ghdc, hFontMarathi);

	nFontListMarathi = glGenLists(128);

	wglUseFontBitmaps(ghdc, 0, 128, nFontListMarathi);

	DeleteObject(hFontMarathi);


	//SIR

	logfontSir.lfHeight = 55;
	logfontSir.lfWidth = 0;
	logfontSir.lfEscapement = 0;
	logfontSir.lfOrientation = 0;
	logfontSir.lfWeight = TRUE;
	logfontSir.lfItalic = FALSE;
	logfontSir.lfUnderline = FALSE;
	logfontSir.lfStrikeOut = FALSE;
	logfontSir.lfCharSet = ANSI_CHARSET;
	logfontSir.lfOutPrecision = OUT_DEFAULT_PRECIS;
	logfontSir.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	logfontSir.lfQuality = DEFAULT_QUALITY;
	logfontSir.lfPitchAndFamily = DEFAULT_PITCH;
	strcpy(logfontSir.lfFaceName, "Kruti Dev 100");

	hFontSir = CreateFontIndirect(&logfontSir);
	SelectObject(ghdc, hFontSir);

	nFontListSir = glGenLists(128);

	wglUseFontBitmaps(ghdc, 0, 128, nFontListSir);

	DeleteObject(hFontSir);


	//ENGLISH

	logfontEnglish.lfHeight = 70;
	logfontEnglish.lfWidth = 0;
	logfontEnglish.lfEscapement = 0;
	logfontEnglish.lfOrientation = 0;
	logfontEnglish.lfWeight = FALSE;
	logfontEnglish.lfItalic = FALSE;
	logfontEnglish.lfUnderline = FALSE;
	logfontEnglish.lfStrikeOut = FALSE;
	logfontEnglish.lfCharSet = ANSI_CHARSET;
	logfontEnglish.lfOutPrecision = OUT_DEFAULT_PRECIS;
	logfontEnglish.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	logfontEnglish.lfQuality = DEFAULT_QUALITY;
	logfontEnglish.lfPitchAndFamily = DEFAULT_PITCH;
	strcpy(logfontEnglish.lfFaceName, "Fredericka the Great");

	hFontEnglish = CreateFontIndirect(&logfontEnglish);
	SelectObject(ghdc, hFontEnglish);

	nFontListEnglish = glGenLists(128);

	wglUseFontBitmaps(ghdc, 0, 128, nFontListEnglish);

	DeleteObject(hFontEnglish);


	//PRESENTS

	logfontPresents.lfHeight = 60;
	logfontPresents.lfWidth = 0;
	logfontPresents.lfEscapement = 0;
	logfontPresents.lfOrientation = 0;
	logfontPresents.lfWeight = FALSE;
	logfontPresents.lfItalic = FALSE;
	logfontPresents.lfUnderline = FALSE;
	logfontPresents.lfStrikeOut = FALSE;
	logfontPresents.lfCharSet = ANSI_CHARSET;
	logfontPresents.lfOutPrecision = OUT_DEFAULT_PRECIS;
	logfontPresents.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	logfontPresents.lfQuality = DEFAULT_QUALITY;
	logfontPresents.lfPitchAndFamily = DEFAULT_PITCH;
	strcpy(logfontPresents.lfFaceName, "Fredericka the Great");

	hFontPresents = CreateFontIndirect(&logfontPresents);
	SelectObject(ghdc, hFontPresents);

	nFontListPresents = glGenLists(128);

	wglUseFontBitmaps(ghdc, 0, 128, nFontListPresents);

	DeleteObject(hFontPresents);



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
	loadGLTexture(&front_wall_door_closed_texture, MAKEINTRESOURCE(FRONT_WALL_DOOR_CLOSED_BITMAP));
	loadGLTexture(&front_wall_door_opened_texture, MAKEINTRESOURCE(FRONT_WALL_DOOR_OPENED_BITMAP));
	loadGLTexture(&railing_texture, MAKEINTRESOURCE(RAILING_BITMAP));
	loadGLTexture(&roof_texture, MAKEINTRESOURCE(ROOF_BITMAP));

	glEnable(GL_TEXTURE_2D);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//SetClearColor
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);    //black


	enqueue(Scene0);
	enqueue(Scene1);
	enqueue(Scene2);
	enqueue(Scene3);
	enqueue(Scene4);
	enqueue(Scene5);
	enqueue(Scene6);


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
	void Stars(void);


	//variable declaration
	GLfloat RailingX = 0.0f;
	GLfloat RailingY = 0.0f;
	GLfloat RailingZ = 0.0f;

	GLfloat StarsX;
	GLfloat StarsY;

	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//gluLookAt(0.0f, 0.0, 15.0f, 0.0f, 0.0, 0.0f, 0.0f, 1.0f, 0.0f);     //trials
	gluLookAt(0.0f, LookAtTranslateY, 15.0f, 0.0f, LookAtTranslateY, 0.0f, 0.0f, 1.0f, 0.0f);     //MainCamera
	//gluLookAt(0.0f, 25.0f, 15.0f, 0.0f, 25.0f, 0.0f, 0.0f, 1.0f, 0.0f);  //EndCreditsFrame
	//gluLookAt(0.0f, 30.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f);    //top view

	glPushMatrix();

	//Audio

	if (PlaySong == true)
	{
		PlaySound(MAKEINTRESOURCE(AUDIO), GetModuleHandle(NULL), SND_RESOURCE | SND_ASYNC);
		PlaySong = false;
	}


	/******DRAW OBJECTS******/


	/*****SUN*****/

	glTranslatef(SunTranslate.X, SunTranslate.Y, SunTranslate.Z);
	glScalef(3.0f, 3.0f, 0.0f);

	Sun();


	glPopMatrix();
	glPushMatrix();


	/*****SKY*****/


	Sky();


	/****FONT[END CREDITS]****/


	//glColor3f(1.0f, 1.0f, 1.0f);
	glColor4f(1.0f, 1.0f, 1.0f, endCreditsAlpha);

	glRasterPos3f(-1.6, 26.0, 0.0);
	glListBase(nFontListSir);
	glCallLists(17, GL_UNSIGNED_BYTE, "v'kh ik[kjs ;srh!");


	glRasterPos3f(-2.1, 25.0, 0.0);
	glListBase(nFontListMarathi);
	glCallLists(21, GL_UNSIGNED_BYTE, "doh & eaxs'k ikMxkodj");


	glRasterPos3f(-2.17, 24.0, 0.0);
	glListBase(nFontListMarathi);
	glCallLists(22, GL_UNSIGNED_BYTE, "ewG xk;d & lq/khj QMds");

	


	///******THANK YOU FONT******/


	glColor4f(1.0f, 1.0f, 1.0f, TYCreditsAlpha);
	//glColor4f(1.0f, 1.0f, 1.0f, 1.0f);


	glRasterPos3f(-0.85, 28.0, 0.0);
	glListBase(nFontListMarathi);
	glCallLists(10, GL_UNSIGNED_BYTE, "Xkzqi yhMj");


	glRasterPos3f(-1.2, 27.0, 0.0);
	glListBase(nFontListMarathi);
	glCallLists(14, GL_UNSIGNED_BYTE, "_fRod pkS?kqys");





	glRasterPos3f(-0.8, 25.0, 0.0);
	glListBase(nFontListMarathi);
	glCallLists(8, GL_UNSIGNED_BYTE, "/kU;okn!");



	glRasterPos3f(-1.7, 24.0, 0.0);
	glListBase(nFontListMarathi);
	glCallLists(21, GL_UNSIGNED_BYTE, "MkW- jek xks[kys eWMe");


	glRasterPos3f(-1.5, 23.0, 0.0);
	glListBase(nFontListMarathi);
	glCallLists(17, GL_UNSIGNED_BYTE, "izKk xks[kys eWMe");




	glRasterPos3f(-1.95, 21.5, 0.0);
	glListBase(nFontListSir);
	glCallLists(20, GL_UNSIGNED_BYTE, "MkW- fot; xks[kys lj");


	glPopMatrix();
	glPushMatrix();


	/*****STARS*****/



	/*for (StarsX = -5.0f, StarsY = 0.0f; StarsX >= 5.0f, StarsY >= 20.0f; StarsX += 5.0f, StarsY += 5.0f)
	{
		glTranslatef(StarsX, StarsY, -25.0f);

		Stars();

		glPopMatrix();
		glPushMatrix();
	}*/




	



	glTranslatef(-9.0f, 10.0f, -25.0f);
	glScalef(3.0f, 3.0f, 3.0f);

	Stars();

	glPopMatrix();
	glPushMatrix();



	glTranslatef(9.0f, 10.0f, -25.0f);
	glScalef(3.0f, 3.0f, 3.0f);

	Stars();

	glPopMatrix();
	glPushMatrix();

		



	glTranslatef(-7.0f, 20.0f, -25.0f);
	glScalef(4.0f, 4.0f, 4.0f);

	Stars();

	glPopMatrix();
	glPushMatrix();



	glTranslatef(7.0f, 20.0f, -25.0f);
	glScalef(4.0f, 4.0f, 4.0f);

	Stars();

	glPopMatrix();
	glPushMatrix();




	glTranslatef(-9.0f, 35.0f, -25.0f);
	glScalef(3.0f, 3.0f, 3.0f);

	Stars();

	glPopMatrix();
	glPushMatrix();



	glTranslatef(9.0f, 35.0f, -25.0f);
	glScalef(3.0f, 3.0f, 3.0f);

	Stars();

	glPopMatrix();
	glPushMatrix();


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




	glTranslatef(-4.0f, 0.0f, -24.9f);
	glScalef(18.0f, 7.0f, 8.0f);
	glRotatef(-7.0, 0.0f, 1.0f, 0.0f);

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

	glLoadIdentity();
	gluLookAt(0.0f, LookAtTranslateY, 15.0f, 0.0f, LookAtTranslateY, 0.0f, 0.0f, 1.0f, 0.0f);     //MainCamera

	/****GIRL2****/

	if (isGirl2Visible == true)
	{
		Girl2();
	}
		

	glPopMatrix();
	glPushMatrix();


	glLoadIdentity();
	gluLookAt(0.0f, LookAtTranslateY, 15.0f, 0.0f, LookAtTranslateY, 0.0f, 0.0f, 1.0f, 0.0f);     //MainCamera


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


	glLoadIdentity();
	gluLookAt(0.0f, LookAtTranslateY, 15.0f, 0.0f, LookAtTranslateY, 0.0f, 0.0f, 1.0f, 0.0f);     //MainCamera



	


	/*****FADE-IN QUAD*****/

	glColor4f(0.0f, 0.0f, 0.0f, FadeInQuadAlpha);

	FadeInQuad();

	glColor3f(1.0f, 1.0f, 1.0f);

	glPopMatrix();
	glPushMatrix();


	
	/******INTRO FONT******/


	glColor4f(1.0f, 1.0f, 1.0f, introTypoAlpha);
	//glColor3f(1.0f, 1.0f, 1.0f);

	glRasterPos3f(-0.085, 0.03, 14.5);
	glListBase(nFontListEnglish);
	glCallLists(15, GL_UNSIGNED_BYTE, "AstroMediComp's");


	glRasterPos3f(-0.09, -0.015, 14.5);
	glListBase(nFontListEnglish);
	glCallLists(12, GL_UNSIGNED_BYTE, "DOMAIN GROUP");


	glRasterPos3f(-0.037, -0.055, 14.5);
	glListBase(nFontListPresents);
	glCallLists(12, GL_UNSIGNED_BYTE, "Presents");


	glPopMatrix();
	glPushMatrix();


	


	

	
	
	

	SwapBuffers(ghdc);
}

void Update(void)
{


	//SUN

	if (isSunTranslate == true)
	{
		SunTranslate.Y += SunTranslateSpeed;
		if (SunTranslate.Y >= 50.0f)
			SunTranslate.Y = 50.0f;

		SunColor.R -= SunColorAnimateSpeed;
		if (SunColor.R <= 0.96f)
		{
			SunColor.R = 0.96f;
		}

		SunColor.G += SunColorAnimateSpeed;
		if (SunColor.G >= 0.7f)
		{
			SunColor.G = 0.7f;
		}

		SunColor.B += SunColorAnimateSpeed;
		if (SunColor.B >= 0.18f)
		{
			SunColor.B = 0.18f;
		}

		//SKY GRADIENT

		static int SkyGradient = 0;

		switch (SkyGradient)
		{

			//Dawn to Morning

		case 0:

			SkyMid.R += SkyColorAnimateSpeed;
			if (SkyMid.R >= 0.89f)
			{
				SkyMid.R = 0.89f;
			}

			SkyMid.G += SkyColorAnimateSpeed;
			if (SkyMid.G >= 1.0f)
			{
				SkyMid.G = 1.0f;
				SkyGradient++;
			}

			SkyMid.B += SkyColorAnimateSpeed;
			if (SkyMid.B >= 1.0f)
			{
				SkyMid.B = 1.0f;
			}



			/*SkyBottom.R -= SkyColorAnimateSpeed;
			if (SkyBottom.R <= 0.78f)
			{
				SkyBottom.R = 0.78f;
			}*/

			SkyBottom.G += SkyColorAnimateSpeed;
			if (SkyBottom.G >= 0.86f)
			{
				SkyBottom.G = 0.86f;
			}

			SkyBottom.B += SkyColorAnimateSpeed;
			if (SkyBottom.B >= 0.12f)
			{
				SkyBottom.B = 0.12f;

			}

			break;


			//Morning to Noon

		case 1:

			SkyMid.R -= SkyColorAnimateSpeed;
			if (SkyMid.R <= 0.36f)
			{
				SkyMid.R = 0.36f;
			}

			/*SkyMid.G += SkyColorAnimateSpeed;
			if (SkyMid.G >= 1.0f)
			{
				SkyMid.G = 1.0f;
			}

			SkyMid.B += SkyColorAnimateSpeed;
			if (SkyMid.B >= 1.0f)
			{
				SkyMid.B = 1.0f;
			}*/



			/*SkyBottom.R -= SkyColorAnimateSpeed;
			if (SkyBottom.R <= 0.78f)
			{
				SkyBottom.R = 0.78f;
			}*/

			SkyBottom.G += SkyColorAnimateSpeed;
			if (SkyBottom.G >= 0.92f)
			{
				SkyBottom.G = 0.92f;
			}

			SkyBottom.B += SkyColorAnimateSpeed;
			if (SkyBottom.B >= 0.67f)
			{
				SkyBottom.B = 0.67f;
				SkyGradient++;
			}

			break;


			//Noon to Evening

		case 2:

			SkyMid.R += SkyColorAnimateSpeed - 0.0001;
			if (SkyMid.R >= 0.6f)
			{
				SkyMid.R = 0.6f;
			}

			SkyMid.G -= SkyColorAnimateSpeed - 0.0001;
			if (SkyMid.G <= 0.59f)
			{
				SkyMid.G = 0.59f;
			}

			SkyMid.B -= SkyColorAnimateSpeed - 0.0001;
			if (SkyMid.B <= 0.89f)
			{
				SkyMid.B = 0.89f;
			}



			/*SkyBottom.R += SkyColorAnimateSpeed;
			if (SkyBottom.R >= 0.91f)
			{
				SkyBottom.R = 0.91f;
			}*/

			SkyBottom.G -= SkyColorAnimateSpeed - 0.0001;
			if (SkyBottom.G <= 0.56f)
			{
				SkyBottom.G = 0.56f;
			}

			SkyBottom.B -= SkyColorAnimateSpeed - 0.0001;
			if (SkyBottom.B <= 0.11f)
			{
				SkyBottom.B = 0.11f;
				isStarsVisible = true;
				SkyGradient++;
			}

			break;


			//Evening to Night

		case 3:

			SkyMid.R -= SkyColorAnimateSpeed + 0.0004;
			if (SkyMid.R <= 0.12f)
			{
				SkyMid.R = 0.12f;
			}

			SkyMid.G -= SkyColorAnimateSpeed + 0.0004;
			if (SkyMid.G <= 0.17f)
			{
				SkyMid.G = 0.17f;
			}

			SkyMid.B -= SkyColorAnimateSpeed + 0.0004;
			if (SkyMid.B <= 0.23f)
			{
				SkyMid.B = 0.23f;
			}



			SkyBottom.R -= SkyColorAnimateSpeed + 0.0004;
			if (SkyBottom.R <= 0.31f)
			{
				SkyBottom.R = 0.31f;
				SkyGradient++;
			}

			SkyBottom.G -= SkyColorAnimateSpeed + 0.0004;
			if (SkyBottom.G <= 0.39f)
			{
				SkyBottom.G = 0.39f;
			}

			SkyBottom.B += SkyColorAnimateSpeed + 0.0004;
			if (SkyBottom.B >= 0.48f)
			{
				SkyBottom.B = 0.48f;

			}

			break;

		}


		//STARS

		if (isStarsVisible == true)
		{
			static int StarsFadeIn = 0;

			switch (StarsFadeIn)
			{
			case 0:
				/*Pt1Alpha += 0.003f;
				if (Pt1Alpha >= 1.0f)
				{
					Pt1Alpha = 1.0f;
				}*/


				Pt2Alpha += 0.003f;
				if (Pt2Alpha >= 1.0f)
				{
					Pt2Alpha = 1.0f;
				}


				Pt3Alpha += 0.003f;
				if (Pt3Alpha >= 1.0f)
				{
					Pt3Alpha = 1.0f;
					StarsFadeIn++;
				}
				break;



			case 1:

				/*static int StarsPt1 = 0;

				switch (StarsPt1)
				{
				case 0:
					Pt1Alpha += 0.015f;
					if (Pt1Alpha >= 1.0f)
					{
						Pt1Alpha = 1.0f;
						StarsPt1++;
					}

					break;

				case 1:

					Pt1Alpha -= 0.015f;
					if (Pt1Alpha <= 0.0f)
					{
						Pt1Alpha = 0.0f;
						StarsPt1--;
					}

					break;

				}*/



				static int StarsPt2 = 0;

				switch (StarsPt2)
				{
				case 0:
					Pt2Alpha += 0.015f;
					if (Pt2Alpha >= 1.0f)
					{
						Pt2Alpha = 1.0f;
						StarsPt2++;
					}

					break;

				case 1:

					Pt2Alpha -= 0.015f;
					if (Pt2Alpha <= 0.0f)
					{
						Pt2Alpha = 0.0f;
						StarsPt2--;
					}

					break;

				}



				static int StarsPt3 = 0;

				switch (StarsPt3)
				{
				case 0:
					Pt3Alpha += 0.01f;
					if (Pt3Alpha >= 1.0f)
					{
						Pt3Alpha = 1.0f;
						StarsPt3++;
					}

					break;

				case 1:

					Pt3Alpha -= 0.01f;
					if (Pt3Alpha <= 0.0f)
					{
						Pt3Alpha = 0.0f;
						StarsPt3--;
					}

					break;

				}


				break;
			}



		}


	}

	

	//SCENES


	struct node* tempFront = Peek();
	tempFront->FunctionPointer();


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
	glDeleteTextures(1, &front_wall_door_closed_texture);
	glDeleteTextures(1, &front_wall_door_opened_texture);
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

