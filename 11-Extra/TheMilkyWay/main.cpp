//header files
#include <windows.h>
#include <stdio.h>

#include "inc/glew.h"
#include "inc/SOIL.h"
#include <GL/GL.h>

#include "inc/al.h"
#include "inc/alc.h"

#include "Resources.h"
#include "inc/wavhelper.h"
#include "inc/vmath.h"
#include "inc/SphereMesh.h"
#include "inc/TorusMesh.h"
#include "inc/CircleVector.h"
#include "inc/stack.h"
#include "inc/GlobalVariableDeclarations.h"
#include "inc/shaders.h"
#include "inc/scenes.h"

#include "ft2build.h"
#include FT_FREETYPE_H
#include<map>
#include<string>

//macro functions
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "OpenAL32.lib")
#pragma comment(lib, "soil.lib")
#pragma comment(lib, "freetyped.lib")

using namespace vmath;

//global function declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void Initialize(void);
void Display(void);
void Uninitialize(void);
GLuint LoadImageAsTexture(const char*);
void Update(void);

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

		/*case 0x46:
		case 0x66:
			ToggleFullscreen();
			break;*/

		case VK_UP:
			if (frameCount >= 2800)
				orbitsXRot -= 0.5f;
			break;

		case VK_DOWN:
			if (frameCount >= 2800)
				orbitsXRot += 0.5f;
			break;

		case VK_LEFT:
			if (frameCount >= 2800)
				orbitsZRot += 0.5f;
			break;

		case VK_RIGHT:
			if (frameCount >= 2800)
				orbitsZRot -= 0.5f;
			break;

		case 48:
		case VK_NUMPAD0:
			break;

		case 49:
		case VK_NUMPAD1:
			if (frameCount >= 2800)
			{
				bTextViewport = true;
				isMercuryClicked = true;
				isVenusClicked = false;
				isEarthClicked = false;
				isMarsClicked = false;
				isJupiterClicked = false;
				isSaturnClicked = false;
				isUranusClicked = false;
				isNeptuneClicked = false;
				isPlutoClicked = false;
			}
			break;

		case 50:
		case VK_NUMPAD2:
			if (frameCount >= 2800)
			{
				bTextViewport = true;
				isMercuryClicked = false;
				isVenusClicked = true;
				isEarthClicked = false;
				isMarsClicked = false;
				isJupiterClicked = false;
				isSaturnClicked = false;
				isUranusClicked = false;
				isNeptuneClicked = false;
				isPlutoClicked = false;
			}
			break;

		case 51:
		case VK_NUMPAD3:
			if (frameCount >= 2800)
			{
				bTextViewport = true;
				isMercuryClicked = false;
				isVenusClicked = false;
				isEarthClicked = true;
				isMarsClicked = false;
				isJupiterClicked = false;
				isSaturnClicked = false;
				isUranusClicked = false;
				isNeptuneClicked = false;
				isPlutoClicked = false;
			}
			break;

		case 52:
		case VK_NUMPAD4:
			if (frameCount >= 2800)
			{
				bTextViewport = true;
				isMercuryClicked = false;
				isVenusClicked = false;
				isEarthClicked = false;
				isMarsClicked = true;
				isJupiterClicked = false;
				isSaturnClicked = false;
				isUranusClicked = false;
				isNeptuneClicked = false;
				isPlutoClicked = false;
			}
			break;

		case 53:
		case VK_NUMPAD5:
			if (frameCount >= 2800)
			{
				bTextViewport = true;
				isMercuryClicked = false;
				isVenusClicked = false;
				isEarthClicked = false;
				isMarsClicked = false;
				isJupiterClicked = true;
				isSaturnClicked = false;
				isUranusClicked = false;
				isNeptuneClicked = false;
				isPlutoClicked = false;
			}
			break;

		case 54:
		case VK_NUMPAD6:
			if (frameCount >= 2800)
			{
				bTextViewport = true;
				isMercuryClicked = false;
				isVenusClicked = false;
				isEarthClicked = false;
				isMarsClicked = false;
				isJupiterClicked = false;
				isSaturnClicked = true;
				isUranusClicked = false;
				isNeptuneClicked = false;
				isPlutoClicked = false;
			}
			break;

		case 55:
		case VK_NUMPAD7:
			if (frameCount >= 2800)
			{
				bTextViewport = true;
				isMercuryClicked = false;
				isVenusClicked = false;
				isEarthClicked = false;
				isMarsClicked = false;
				isJupiterClicked = false;
				isSaturnClicked = false;
				isUranusClicked = true;
				isNeptuneClicked = false;
				isPlutoClicked = false;
			}
			break;

		case 56:
		case VK_NUMPAD8:
			if (frameCount >= 2800)
			{
				bTextViewport = true;
				isMercuryClicked = false;
				isVenusClicked = false;
				isEarthClicked = false;
				isMarsClicked = false;
				isJupiterClicked = false;
				isSaturnClicked = false;
				isUranusClicked = false;
				isNeptuneClicked = true;
				isPlutoClicked = false;
			}
			break;

		case 57:
		case VK_NUMPAD9:
			if (frameCount >= 2800)
			{
				bTextViewport = true;
				isMercuryClicked = false;
				isVenusClicked = false;
				isEarthClicked = false;
				isMarsClicked = false;
				isJupiterClicked = false;
				isSaturnClicked = false;
				isUranusClicked = false;
				isNeptuneClicked = false;
				isPlutoClicked = true;
			}
			break;

		default:
			if (frameCount >= 2800)
			{
				bTextViewport = false;
				isMercuryClicked = false;
				isVenusClicked = false;
				isEarthClicked = false;
				isMarsClicked = false;
				isJupiterClicked = false;
				isSaturnClicked = false;
				isUranusClicked = false;
				isNeptuneClicked = false;
				isPlutoClicked = false;
			}
			break;
		}
		break;

	case WM_CHAR:
		switch (wParam)
		{
		case 'F':
		case 'f':
			ToggleFullscreen();
			break;

		case 'R':
		case 'r':
			orbitsXRot = 30.0f;
			orbitsZRot = 0.0f;
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
	/*if (gbFullscreen == false)
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
		ShowCursor(true);
		gbFullscreen = true;
	}
	else
	{
		SetWindowLong(ghwnd, GWL_STYLE, (dwStyle | WS_OVERLAPPEDWINDOW));
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);

		ShowCursor(true);
		gbFullscreen = false;
	}*/

	dwStyle = GetWindowLong(ghwnd, GWL_STYLE);

	if (dwStyle & WS_OVERLAPPEDWINDOW)
	{
		if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
		{
			SetWindowLong(ghwnd, GWL_STYLE, (dwStyle & ~WS_OVERLAPPEDWINDOW));
			SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
		}

	}
	ShowCursor(true);
}

void initFreetype(void)
{
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
	{
		fprintf(gpFile, "Could not init FT Lib\n");
		fflush(gpFile);
	}

	std::string font_name = "res//Roboto-Bold.ttf";
	if (font_name.empty())
	{
		fprintf(gpFile, "Could not load font_name\n");
		fflush(gpFile);
	}

	FT_Face face;
	if (FT_New_Face(ft, font_name.c_str(), 0, &face))
	{
		fprintf(gpFile, "Could not load font\n");
		fflush(gpFile);
	}
	else
	{
		FT_Set_Pixel_Sizes(face, 0, 48);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		for (unsigned char c = 0; c < 128; c++)
		{
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				fprintf(gpFile, "Could not load glyph\n");
				fflush(gpFile);
				continue;
			}
			//generate texture
			unsigned int tex;
			glGenTextures(1, &tex);
			glBindTexture(GL_TEXTURE_2D, tex);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			Character character = { tex,
									vec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
									vec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
									static_cast<unsigned int>(face->glyph->advance.x) };
			characters.insert(std::pair<char, Character>(c, character));
		}
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	FT_Done_Face(face);
	FT_Done_FreeType(ft);

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

		//alData = loadWav("D:/SHRUTI/AstroMediComp/RTR/RTR2020/MyCodes/_Extra/00-Demos/TheMilkyWay/res/SlowMotion.wav", &channel, &sampleRate, &beatsPerSecond, &size);
		alData = loadWav("res//SlowMotion.wav", &channel, &sampleRate, &beatsPerSecond, &size);

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

void Initialize(void)
{
	//function prototype
	void Resize(int, int);
	bool loadGLTexture(GLuint*, TCHAR[]);
	GLuint cubemapLoadTexture(const char**);
	void alInitialize(void);
	void initFreetype(void);

	//variable declaration
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;

	//code
	ToggleFullscreen();

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

	/****INIT FREETYPE****/
	initFreetype();

	/********SHADERS********/

	initSceneLightingShader();
	initGodraysShader();
	initFinalPassForGodraysShader();
	initSkyboxShader();
	initCubemapCubeShader();
	initTextShader();
	initStarfieldShader();
	//initFullScreenQuad();

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	//vertices array declaration

	//create stack
	pStack = Create();

	//SPHERE_SUN
	makeSphere(0.5, 100, 100);
	fprintf(gpFile, "\nnumVertices = %d\nnumElements = %d\n", numVertices, numElements);

	//SATURN_RING
	makeTorus(0.75f, 0.13f, 40, 40);

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

	//cube vertices
	const GLfloat skyboxVertices[] = { -1.0f,  1.0f, -1.0f,     //front (NZ)
									-1.0f, -1.0f, -1.0f,
									 1.0f, -1.0f, -1.0f,
									 1.0f, -1.0f, -1.0f,
									 1.0f,  1.0f, -1.0f,
									-1.0f,  1.0f, -1.0f,

									-1.0f, -1.0f,  1.0f,     //left (NX)
									-1.0f, -1.0f, -1.0f,
									-1.0f,  1.0f, -1.0f,
									-1.0f,  1.0f, -1.0f,
									-1.0f,  1.0f,  1.0f,
									-1.0f, -1.0f,  1.0f,

									 1.0f, -1.0f, -1.0f,     //right (PX)
									 1.0f, -1.0f,  1.0f,
									 1.0f,  1.0f,  1.0f,
									 1.0f,  1.0f,  1.0f,
									 1.0f,  1.0f, -1.0f,
									 1.0f, -1.0f, -1.0f,

									-1.0f, -1.0f,  1.0f,     //
									-1.0f,  1.0f,  1.0f,
									 1.0f,  1.0f,  1.0f,
									 1.0f,  1.0f,  1.0f,
									 1.0f, -1.0f,  1.0f,
									-1.0f, -1.0f,  1.0f,

									-1.0f,  1.0f, -1.0f,     //
									 1.0f,  1.0f, -1.0f,
									 1.0f,  1.0f,  1.0f,
									 1.0f,  1.0f,  1.0f,
									-1.0f,  1.0f,  1.0f,
									-1.0f,  1.0f, -1.0f,

									-1.0f, -1.0f, -1.0f,
									-1.0f, -1.0f,  1.0f,
									 1.0f, -1.0f, -1.0f,
									 1.0f, -1.0f, -1.0f,
									-1.0f, -1.0f,  1.0f,
									 1.0f, -1.0f,  1.0f };


	//cube vertices
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




	//init all ellipses
	initMercuryEllipse();
	initVenusEllipse();
	initEarthEllipse();
	initMarsEllipse();
	initJupiterEllipse();
	initSaturnEllipse();
	initSaturnRingEllipse();
	initUranusEllipse();
	initNeptuneEllipse();
	initPlutoEllipse();

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

	/*****SKYBOX VAO*****/

	glGenVertexArrays(1, &vao_skybox);
	glBindVertexArray(vao_skybox);

	glGenBuffers(1, &vbo_skybox);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_skybox);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(SPK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SPK_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	/*****STAR VAO*****/

	glGenVertexArrays(1, &vao_star);
	glBindVertexArray(vao_star);

	struct star_t
	{
		vec3 position;
		vec3 color;
	};

	glGenBuffers(1, &vbo_star);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_star);
	glBufferData(GL_ARRAY_BUFFER, NUM_STARS * sizeof(star_t), NULL, GL_STATIC_DRAW);

	star_t* star = (star_t*)glMapBufferRange(GL_ARRAY_BUFFER, 0, NUM_STARS * sizeof(star_t), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

	for (int i = 0; i < 1000; i++)
	{
		star[i].position[0] = (random_float() * 2.0f - 1.0f) * 100.0f;
		star[i].position[1] = (random_float() * 2.0f - 1.0f) * 100.0f;
		star[i].position[2] = random_float();

		star[i].color[0] = 0.8f + random_float() * 0.2f;
		star[i].color[1] = 0.8f + random_float() * 0.2f;
		star[i].color[2] = 0.8f + random_float() * 0.2f;
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);

	glVertexAttribPointer(SPK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(star_t), NULL);
	glEnableVertexAttribArray(SPK_ATTRIBUTE_POSITION);
	glVertexAttribPointer(SPK_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, sizeof(star_t), (void*)sizeof(vec3));
	glEnableVertexAttribArray(SPK_ATTRIBUTE_COLOR);

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

	/*FBO for CUBEMAP*/
	glGenFramebuffers(1, &fbo_starfield);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_starfield);

	glGenTextures(1, &starfieldTextureColorBuffer);
	glBindTexture(GL_TEXTURE_2D, starfieldTextureColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIN_WIDTH, WIN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, starfieldTextureColorBuffer, 0);

	glGenRenderbuffers(1, &rbo_starfield);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo_starfield);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WIN_WIDTH, WIN_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo_starfield);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	//depth
	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	//glEnable(GL_CULL_FACE);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_POINT_SPRITE);

	/*glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);*/


	//Loading Textures
	//loadGLTexture(&smiley_texture, MAKEINTRESOURCE(SMILEY_BITMAP));

	const char* faces[] = { "res//px.png", 
							"res//nx.png", 
							"res//py.png", 
							"res//ny.png", 
							"res//pz.png", 
							"res//nz.png" };
	cubemap_texture = cubemapLoadTexture(faces);
	if (cubemap_texture != 0)
	{
		fprintf(gpFile, "Cubemap texLoad successful\n");
	}

	star_texture = LoadImageAsTexture("res//star.png");
	sun_texture = LoadImageAsTexture("res//sunMap.jpg");
	mercury_texture = LoadImageAsTexture("res//mercuryMap.jpg");
	venus_texture = LoadImageAsTexture("res//venusMap.jpg");
	earth_texture = LoadImageAsTexture("res//earthMap.jpg");
	earthMoon_texture = LoadImageAsTexture("res//moonMap.jpg");
	mars_texture = LoadImageAsTexture("res//marsMap.jpg");
	marsMoon_texture = LoadImageAsTexture("res//phobosMoonMap.jpg");
	jupiter_texture = LoadImageAsTexture("res//jupiterMap.jpg");
	jupiterMoon_texture = LoadImageAsTexture("res//europaMoonMap.jpg");
	saturn_texture = LoadImageAsTexture("res//saturnMap.jpg");
	//saturn_ring_texture = LoadImageAsTexture("res//saturnRingColor.jpg");
	saturn_ring_texture = LoadImageAsTexture("res//saturnRingAlphaColor.png");
	saturnMoon_texture = LoadImageAsTexture("res//titanMoonMap.jpg");
	uranus_texture = LoadImageAsTexture("res//uranusMap.jpg");
	uranusMoon_texture = LoadImageAsTexture("res//arielMoonMap.jpg");
	neptune_texture = LoadImageAsTexture("res//neptuneMap.jpg");
	neptuneMoon_texture = LoadImageAsTexture("res//tritonMoonMap.jpg");
	pluto_texture = LoadImageAsTexture("res//plutoMap.jpg");

	//OpenAL
	alInitialize();

	//SetClearColor
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	//set perspective matrix to identity matrix
	perspectiveProjectionMatrix = mat4::identity();

	//Resize(WIN_WIDTH, WIN_HEIGHT);
}

GLuint cubemapLoadTexture(const char** f)
{
	int width, height;
	//unsigned char* imageData = NULL;
	int nrComponents;
	//GLenum format = GL_RGB;

	//code    
	glGenTextures(1, &cubemapTextureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTextureID);

	for (int i = 0; i < 6; i++)
	{

		unsigned char* data = SOIL_load_image(f[i], &width, &height, &nrComponents, 0);

		if (data)
		{
			GLenum format;
			if (nrComponents == 1)
				format = GL_RED;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;

			glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			SOIL_free_image_data(data);
			fprintf(gpFile, "%d tex success\n", i);
		}
		else
		{
			fprintf(gpFile, "%d tex failed\n", i);
		}
	}

	//glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	//glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	fprintf(gpFile, "textureID = %d\n", cubemapTextureID);
	return(cubemapTextureID);
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

	perspectiveProjectionMatrix = perspective(50.0f, (GLfloat)width / (GLfloat)height, 0.1f, 1000.0f);
}

void Display(void)
{
	//camera
	//camPos = vec3(0.0f, 5.0f, 0.0f);
	//camTarget = vec3(0.0f, 0.0f, 0.0f);
	//camUpAxis = vec3(0.0f, 0.0f, -1.0f);
	//camera = lookat(camPos, camTarget, camUpAxis);

	camPos = vec3(camXPos, camYPos, camZPos);
	camTarget = vec3(0.0f, 0.0f, 0.0f);
	camUpAxis = vec3(0.0f, 1.0f, 0.0f);
	camera = lookat(camPos, camTarget, camUpAxis);

	//code

	/**********************************DRAW GODRAYS*********************************/

	//draw sun + godrays
	//drawSunGodrays();

	/**********************************DRAW STARFIELD IN FBO*********************************/
	//bind fbo
	//glBindFramebuffer(GL_FRAMEBUFFER, fbo_starfield);

	////glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glViewport(0, 0, (GLsizei)WIN_WIDTH, (GLsizei)WIN_HEIGHT);
	
	//drawStarfield();

	//unbind fbo
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);

	/**********************************DEFAULT FRAMEBUFFER*********************************/
	glDisable(GL_DEPTH_TEST);
	//glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	perspectiveProjectionMatrix = perspective(50.0f, (GLfloat)gWidth / (GLfloat)gHeight, 0.1f, 1000.0f);
	glViewport(0, 0, (GLsizei)gWidth, (GLsizei)gHeight);
	
	//DRAW SKYBOX [2D TEXTURE]
	//drawSkybox();

	//DRAW CUBE FOR CUBEMAP [STARFIELD]
	//drawCubeForCubemap();

	//DRAW STARFIELD IN DEFAULT FB
	drawStarfield();

	glEnable(GL_DEPTH_TEST);


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	//DRAW SOLAR SYSTEM
	drawSolarSystem();


	/**********************************DRAW FOCUSED PLANETS & TEXT IN NEW VIEWPORT*********************************/

	if (bTextViewport == true)
	{
		//planets
		drawFocusedPlanets();

		//text
		glViewport(5 * gWidth / 6, 0, (GLsizei)gWidth / 6, (GLsizei)gHeight);
		perspectiveProjectionMatrix = perspective(45.0f, ((GLfloat)gWidth / 6) / ((GLfloat)gHeight), 0.1f, 100.0f);

		glUseProgram(sceneShaderProgramObject);

		glUniform1i(passUniformS, 1);
		mat4 modelMatrix;
		mat4 viewMatrix;
		mat4 projectionMatrix;
		mat4 translateMatrix;

		translateMatrix = mat4::identity();
		modelMatrix = mat4::identity();
		viewMatrix = mat4::identity();
		projectionMatrix = mat4::identity();

		translateMatrix = translate(0.0f, 0.0f, -2.0f);

		modelMatrix = translateMatrix;
		projectionMatrix = perspectiveProjectionMatrix;

		glUniformMatrix4fv(modelMatrixUniformS, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(viewMatrixUniformS, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionMatrixUniformS, 1, GL_FALSE, projectionMatrix);

		glVertexAttrib4f(SPK_ATTRIBUTE_COLOR, 0.3f, 0.3f, 0.3f, 0.5f);

		//bind vao_rectangle
		glBindVertexArray(vao_square);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		//unbind vao_rectangle
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glVertexAttrib4f(SPK_ATTRIBUTE_COLOR, 0.0f, 0.0f, 0.0f, 1.0f);

		glUseProgram(0);

		//render text

		if (isMercuryClicked == true)
			renderTextMercury();

		if (isVenusClicked == true)
			renderTextVenus();

		if (isEarthClicked == true)
			renderTextEarth();

		if (isMarsClicked == true)
			renderTextMars();

		if (isJupiterClicked == true)
			renderTextJupiter();

		if (isSaturnClicked == true)
			renderTextSaturn();

		if (isUranusClicked == true)
			renderTextUranus();

		if (isNeptuneClicked == true)
			renderTextNeptune();

		if (isPlutoClicked == true)
			renderTextPluto();

		//RenderText(textShaderProgramObject, "not-to-scale", -0.045f, -0.4f, 0.0003f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	}
	
	/**********************************NOT-TO-SCALE*********************************/

	perspectiveProjectionMatrix = perspective(45.0f, (GLfloat)gWidth / (GLfloat)gHeight, 0.1f, 100.0f);
	glViewport(0, 0, (GLsizei)gWidth, (GLsizei)gHeight);
	RenderText(textShaderProgramObject, "not-to-scale", 0.57f, -0.4f, 0.00025f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	
	if (frameCount >= 2800)
	{
		RenderText(textShaderProgramObject, "NAVIGATION: Arrow Keys", -0.7f, 0.38f, 0.00025f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
		RenderText(textShaderProgramObject, "INFO: Numpad 1-9", -0.7f, 0.36f, 0.00025f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
		RenderText(textShaderProgramObject, "RESET: Key 'R'", -0.7f, 0.34f, 0.00025f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	}

	SwapBuffers(ghdc);
}

void Update(void)
{
	//code
	frameCount++;
	currentTime++;

	//sunPosY+=0.001f;

	//camera
	camZPos += 0.0013f;
	if (camZPos >= 4.0f)
		camZPos = 4.0f;

	//beginning translations
	//mercury
	if (frameCount >= 664)
	{
		mercuryXPos += 0.003f;
		if (mercuryXPos >= 0.3f)
		{
			mercuryXPos = 0.3f;
			isMercuryInOrbit = true;
		}
	}

	if (isMercuryInOrbit == true)
	{
		mercuryRevAngle -= 0.05f;
		if (mercuryRevAngle <= -2 * PI)
			mercuryRevAngle = 0.0f;

		mercuryRotAngle -= 1.0f;
		if (mercuryRotAngle <= -360.0f)
			mercuryRotAngle = 0.0f;
	}
	
	//venus
	if (frameCount >= 846)
	{
		venusXPos += 0.003f;
		if (venusXPos >= 0.45f)
		{
			venusXPos = 0.45f;
			isVenusInOrbit = true;
		}
	}

	if (isVenusInOrbit == true)
	{
		venusRevAngle += 0.03f;
		if (venusRevAngle >= 2 * PI)
			venusRevAngle = 0.0f;

		venusRotAngle += 1.0f;
		if (venusRotAngle >= 360.0f)
			venusRotAngle = 0.0f;
	}

	//earth
	if (frameCount >= 1026)
	{
		earthXPos += 0.004f;
		if (earthXPos >= 0.6f)
		{
			earthXPos = 0.6f;
			isEarthInOrbit = true;
		}
	}

	if (isEarthInOrbit == true)
	{
		earthRevAngle -= 0.03f;
		if (earthRevAngle <= -2 * PI)
			earthRevAngle = 0.0f;

		earthRotAngle -= 1.0f;
		if (earthRotAngle <= -360.0f)
			earthRotAngle = 0.0f;
	}

	//mars
	if (frameCount >= 1396)
	{
		marsXPos += 0.004f;
		if (marsXPos >= 0.75f)
		{
			marsXPos = 0.75f;
			isMarsInOrbit = true;
		}
	}

	if (isMarsInOrbit == true)
	{
		marsRevAngle -= 0.02f;
		if (marsRevAngle <= -2 * PI)
			marsRevAngle = 0.0f;

		marsRotAngle -= 1.0f;
		if (marsRotAngle <= -360.0f)
			marsRotAngle = 0.0f;
	}
	
	//jupiter
	if (frameCount >= 1586)
	{
		jupiterXPos += 0.006f;
		if (jupiterXPos >= 1.2f)
		{
			jupiterXPos = 1.2f;
			isJupiterInOrbit = true;
		}
	}

	if (isJupiterInOrbit == true)
	{
		jupiterRevAngle -= 0.008f;
		if (jupiterRevAngle <= -2 * PI)
			jupiterRevAngle = 0.0f;

		jupiterRotAngle -= 1.0f;
		if (jupiterRotAngle <= -360.0f)
			jupiterRotAngle = 0.0f;
	}

	//saturn
	if (frameCount >= 1794)
	{
		saturnXPos += 0.006f;
		if (saturnXPos >= 1.5f)
		{
			saturnXPos = 1.5f;
			isSaturnInOrbit = true;
		}
	}

	if (isSaturnInOrbit == true)
	{
		saturnRevAngle -= 0.008f;
		if (saturnRevAngle <= -2 * PI)
			saturnRevAngle = 0.0f;

		saturnRotAngle -= 1.0f;
		if (saturnRotAngle <= -360.0f)
			saturnRotAngle = 0.0f;
	}

	//uranus
	if (frameCount >= 2186)
	{
		uranusXPos += 0.011f;
		if (uranusXPos >= 1.8f)
		{
			uranusXPos = 1.8f;
			isUranusInOrbit = true;
		}
	}

	if (isUranusInOrbit == true)
	{
		uranusRevAngle -= 0.006f;
		if (uranusRevAngle <= -2 * PI)
			uranusRevAngle = 0.0f;

		uranusRotAngle -= 1.0f;
		if (uranusRotAngle <= -360.0f)
			uranusRotAngle = 0.0f;
	}

	//neptune
	if (frameCount >= 2346)
	{
		neptuneXPos += 0.013f;
		if (neptuneXPos >= 2.1f)
		{
			neptuneXPos = 2.1f;
			isNeptuneInOrbit = true;
		}
	}

	if (isNeptuneInOrbit == true)
	{
		neptuneRevAngle -= 0.004f;
		if (neptuneRevAngle <= -2 * PI)
			neptuneRevAngle = 0.0f;

		neptuneRotAngle -= 1.0f;
		if (neptuneRotAngle <= -360.0f)
			neptuneRotAngle = 0.0f;
	}

	//pluto
	if (frameCount >= 2526)
	{
		plutoXPos += 0.01f;
		if (plutoXPos >= 2.4f)
		{
			plutoXPos = 2.4f;
			isPlutoInOrbit = true;
		}
	}

	if (isPlutoInOrbit == true)
	{
		plutoRevAngle -= 0.002f;
		if (plutoRevAngle <= -2 * PI)
			plutoRevAngle = 0.0f;

		plutoRotAngle -= 1.0f;
		if (plutoRotAngle <= -360.0f)
			plutoRotAngle = 0.0f;
	}

	//moons
	/*earthMoonRevAngle -= 2.0f;
	if (earthMoonRevAngle <= -360.0f)
		earthMoonRevAngle = 0.0f;*/

	earthMoonRotAngle += 2.0f;
	if (earthMoonRotAngle >= 360.0f)
		earthMoonRotAngle = 0.0f;

	/*marsMoonRevAngle -= 2.0f;
	if (marsMoonRevAngle <= -360.0f)
		marsMoonRevAngle = 0.0f;*/

	marsMoonRotAngle += 2.0f;
	if (marsMoonRotAngle >= 360.0f)
		marsMoonRotAngle = 0.0f;

	/*jupiterMoonRevAngle -= 2.0f;
	if (jupiterMoonRevAngle <= -360.0f)
		jupiterMoonRevAngle = 0.0f;*/

	jupiterMoonRotAngle += 2.0f;
	if (jupiterMoonRotAngle >= 360.0f)
		jupiterMoonRotAngle = 0.0f;

	/*saturnMoonRevAngle -= 2.0f;
	if (saturnMoonRevAngle <= -360.0f)
		saturnMoonRevAngle = 0.0f;*/

	saturnMoonRotAngle += 2.0f;
	if (saturnMoonRotAngle >= 360.0f)
		saturnMoonRotAngle = 0.0f;

	/*uranusMoonRevAngle -= 2.0f;
	if (uranusMoonRevAngle <= -360.0f)
		uranusMoonRevAngle = 0.0f;*/

	uranusMoonRotAngle += 2.0f;
	if (uranusMoonRotAngle >= 360.0f)
		uranusMoonRotAngle = 0.0f;

	//neptuneMoonRevAngle -= 2.0f;
	//if (neptuneMoonRevAngle <= -360.0f)
	//	neptuneMoonRevAngle = 0.0f;

	neptuneMoonRotAngle += 2.0f;
	if (neptuneMoonRotAngle >= 360.0f)
		neptuneMoonRotAngle = 0.0f;



	////last animation (planet focused)

	//////mercury

	//if (isMercuryClicked == true)
	//{
	//	if (mercuryXPos > 0.0f)
	//	{
	//		focusedMercuryXPos -= 0.1f;
	//		if (focusedMercuryXPos <= 0.0f)
	//			focusedMercuryXPos = 0.0f;

	//		focusedMercuryZPos += 0.1f;
	//		if (focusedMercuryZPos >= 2.5f)
	//			focusedMercuryZPos = 2.5f;
	//	}
	//	else if (mercuryXPos < 0.0f)
	//	{
	//		focusedMercuryXPos += 0.1f;
	//		if (focusedMercuryXPos >= 0.0f)
	//			focusedMercuryXPos = 0.0f;

	//		focusedMercuryZPos += 0.1f;
	//		if (focusedMercuryZPos >= 2.5f)
	//			focusedMercuryZPos = 2.5f;
	//	}
	//	else
	//	{
	//		focusedMercuryZPos += 0.1f;
	//		if (focusedMercuryZPos >= 2.5f)
	//			focusedMercuryZPos = 2.5f;
	//	}
	//}


	////saturn
	//if (isSaturnClicked == true)
	//{
	//	if (saturnXPos > 0.0f)
	//	{
	//		focusedSaturnXPos -= 0.1f;
	//		if (focusedSaturnXPos <= 0.0f)
	//			focusedSaturnXPos = 0.0f;

	//		focusedSaturnZPos += 0.1f;
	//		if (focusedSaturnZPos >= 2.5f)
	//			focusedSaturnZPos = 2.5f;
	//	}
	//	else if (saturnXPos < 0.0f)
	//	{
	//		focusedSaturnXPos += 0.1f;
	//		if (focusedSaturnXPos >= 0.0f)
	//			focusedSaturnXPos = 0.0f;

	//		focusedSaturnZPos += 0.1f;
	//		if (focusedSaturnZPos >= 2.5f)
	//			focusedSaturnZPos = 2.5f;
	//	}
	//	else
	//	{
	//		focusedSaturnZPos += 0.1f;
	//		if (focusedSaturnZPos >= 2.5f)
	//			focusedSaturnZPos = 2.5f;
	//	}
	//}

	/*if (frameCount >= 4324 && frameCount <= 4866)
	{
		bTextViewport = true;
		isMercuryClicked = true;
		isVenusClicked = false;
		isEarthClicked = false;
		isMarsClicked = false;
		isJupiterClicked = false;
		isSaturnClicked = false;
		isUranusClicked = false;
		isNeptuneClicked = false;
		isPlutoClicked = false;
	}

	else if (frameCount >= 5044 && frameCount <= 5574)
	{
		bTextViewport = true;
		isMercuryClicked = false;
		isVenusClicked = true;
		isEarthClicked = false;
		isMarsClicked = false;
		isJupiterClicked = false;
		isSaturnClicked = false;
		isUranusClicked = false;
		isNeptuneClicked = false;
		isPlutoClicked = false;
	}

	else if (frameCount >= 5764 && frameCount <= 6298)
	{
		bTextViewport = true;
		isMercuryClicked = false;
		isVenusClicked = false;
		isEarthClicked = true;
		isMarsClicked = false;
		isJupiterClicked = false;
		isSaturnClicked = false;
		isUranusClicked = false;
		isNeptuneClicked = false;
		isPlutoClicked = false;
	}

	else if (frameCount >= 6484 && frameCount <= 7036)
	{
		bTextViewport = true;
		isMercuryClicked = false;
		isVenusClicked = false;
		isEarthClicked = false;
		isMarsClicked = true;
		isJupiterClicked = false;
		isSaturnClicked = false;
		isUranusClicked = false;
		isNeptuneClicked = false;
		isPlutoClicked = false;
	}

	else if (frameCount >= 7204 && frameCount <= 7744)
	{
		bTextViewport = true;
		isMercuryClicked = false;
		isVenusClicked = false;
		isEarthClicked = false;
		isMarsClicked = false;
		isJupiterClicked = true;
		isSaturnClicked = false;
		isUranusClicked = false;
		isNeptuneClicked = false;
		isPlutoClicked = false;
	}
	
	else if (frameCount >= 7924 && frameCount <= 8466)
	{
		bTextViewport = true;
		isMercuryClicked = false;
		isVenusClicked = false;
		isEarthClicked = false;
		isMarsClicked = false;
		isJupiterClicked = false;
		isSaturnClicked = true;
		isUranusClicked = false;
		isNeptuneClicked = false;
		isPlutoClicked = false;
	}

	else if (frameCount >= 8644 && frameCount <= 9186)
	{
		bTextViewport = true;
		isMercuryClicked = false;
		isVenusClicked = false;
		isEarthClicked = false;
		isMarsClicked = false;
		isJupiterClicked = false;
		isSaturnClicked = false;
		isUranusClicked = true;
		isNeptuneClicked = false;
		isPlutoClicked = false;
	}

	else if (frameCount >= 9362 && frameCount <= 9926)
	{
		bTextViewport = true;
		isMercuryClicked = false;
		isVenusClicked = false;
		isEarthClicked = false;
		isMarsClicked = false;
		isJupiterClicked = false;
		isSaturnClicked = false;
		isUranusClicked = false;
		isNeptuneClicked = true;
		isPlutoClicked = false;
	}
		
	else if (frameCount >= 10076 && frameCount <= 10626)
	{
		bTextViewport = true;
		isMercuryClicked = false;
		isVenusClicked = false;
		isEarthClicked = false;
		isMarsClicked = false;
		isJupiterClicked = false;
		isSaturnClicked = false;
		isUranusClicked = false;
		isNeptuneClicked = false;
		isPlutoClicked = true;
	}

	else
	{
		bTextViewport = false;
		isMercuryClicked = false;
		isVenusClicked = false;
		isEarthClicked = false;
		isMarsClicked = false;
		isJupiterClicked = false;
		isSaturnClicked = false;
		isUranusClicked = false;
		isNeptuneClicked = false;
		isPlutoClicked = false;
	}*/
	

}

void RenderText(GLuint shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, vec4 color)
{
	mat4 translateMatrix;
	mat4 modelViewMatrix;
	mat4 modelViewProjectionMatrix;

	glUseProgram(shader);

	glUniform4f(textColorUniform, color[0], color[1], color[2], color[3]);

	translateMatrix = mat4::identity();
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();

	translateMatrix = translate(0.0f, 0.0f, -1.0f);
	modelViewMatrix = translateMatrix;
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;  //pre-multiplication of matrices
	glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(vao_text);

	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = characters[*c];

		float xpos = x + ch.bearing[0] * scale;
		float ypos = y - (ch.size[1] - ch.bearing[1]) * scale;

		float w = ch.size[0] * scale;
		float h = ch.size[1] * scale;

		float vertices[6][4] = {
			{xpos, ypos + h, 0.0f, 0.0f},
			{xpos, ypos, 0.0f, 1.0f},
			{xpos + w, ypos, 1.0f, 1.0f},

			{xpos, ypos + h, 0.0f, 0.0f},
			{xpos + w, ypos, 1.0f, 1.0f},
			{xpos + w, ypos + h, 1.0f, 0.0f}
		};

		glBindTexture(GL_TEXTURE_2D, ch.textTextureID);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_text);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		x += (ch.advance >> 6) * scale;

	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
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

	//sphere
	deallocate();
	uninitTorus();

	if (vao_star)
	{
		glDeleteVertexArrays(1, &vao_star);
		vao_star = 0;
	}

	if (vbo_star)
	{
		glDeleteBuffers(1, &vbo_star);
		vbo_star = 0;
	}

	//cube
	if (vao_skybox)
	{
		glDeleteVertexArrays(1, &vao_skybox);
		vao_skybox = 0;
	}

	if (vbo_skybox)
	{
		glDeleteBuffers(1, &vbo_skybox);
		vbo_skybox = 0;
	}

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

	//spheres
	if (vao_mercury_ellipse)
	{
		glDeleteVertexArrays(1, &vao_mercury_ellipse);
		vao_mercury_ellipse = 0;
	}

	if (vbo_position_mercury_ellipse)
	{
		glDeleteBuffers(1, &vbo_position_mercury_ellipse);
		vbo_position_mercury_ellipse = 0;
	}

	if (vao_venus_ellipse)
	{
		glDeleteVertexArrays(1, &vao_venus_ellipse);
		vao_venus_ellipse = 0;
	}

	if (vbo_position_venus_ellipse)
	{
		glDeleteBuffers(1, &vbo_position_venus_ellipse);
		vbo_position_venus_ellipse = 0;
	}

	if (vao_earth_ellipse)
	{
		glDeleteVertexArrays(1, &vao_earth_ellipse);
		vao_earth_ellipse = 0;
	}

	if (vbo_position_earth_ellipse)
	{
		glDeleteBuffers(1, &vbo_position_earth_ellipse);
		vbo_position_earth_ellipse = 0;
	}

	if (vao_mars_ellipse)
	{
		glDeleteVertexArrays(1, &vao_mars_ellipse);
		vao_mars_ellipse = 0;
	}

	if (vbo_position_mars_ellipse)
	{
		glDeleteBuffers(1, &vbo_position_mars_ellipse);
		vbo_position_mars_ellipse = 0;
	}

	if (vao_jupiter_ellipse)
	{
		glDeleteVertexArrays(1, &vao_jupiter_ellipse);
		vao_jupiter_ellipse = 0;
	}

	if (vbo_position_jupiter_ellipse)
	{
		glDeleteBuffers(1, &vbo_position_jupiter_ellipse);
		vbo_position_jupiter_ellipse = 0;
	}

	if (vao_saturn_ellipse)
	{
		glDeleteVertexArrays(1, &vao_saturn_ellipse);
		vao_saturn_ellipse = 0;
	}

	if (vbo_position_saturn_ellipse)
	{
		glDeleteBuffers(1, &vbo_position_saturn_ellipse);
		vbo_position_saturn_ellipse = 0;
	}

	if (vao_uranus_ellipse)
	{
		glDeleteVertexArrays(1, &vao_uranus_ellipse);
		vao_uranus_ellipse = 0;
	}

	if (vbo_position_uranus_ellipse)
	{
		glDeleteBuffers(1, &vbo_position_uranus_ellipse);
		vbo_position_uranus_ellipse = 0;
	}

	if (vao_neptune_ellipse)
	{
		glDeleteVertexArrays(1, &vao_neptune_ellipse);
		vao_neptune_ellipse = 0;
	}

	if (vbo_position_neptune_ellipse)
	{
		glDeleteBuffers(1, &vbo_position_neptune_ellipse);
		vbo_position_neptune_ellipse = 0;
	}

	if (vao_pluto_ellipse)
	{
		glDeleteVertexArrays(1, &vao_pluto_ellipse);
		vao_pluto_ellipse = 0;
	}

	if (vbo_position_pluto_ellipse)
	{
		glDeleteBuffers(1, &vbo_position_pluto_ellipse);
		vbo_position_pluto_ellipse = 0;
	}

	//text
	if (vao_text)
	{
		glDeleteVertexArrays(1, &vao_text);
		vao_text = 0;
	}

	if (vbo_text)
	{
		glDeleteBuffers(1, &vbo_text);
		vbo_text = 0;
	}


	//OpenAL
	alDeleteSources(1, &source);
	alDeleteBuffers(1, &buffer);
	context = alcGetCurrentContext();
	device = alcGetContextsDevice(context);
	alcMakeContextCurrent(NULL);
	alcDestroyContext(context);
	alcCloseDevice(device);

	/*****SAFE SHADER CLEAN-UP*****/

	uninitSceneLightingShader();
	uninitGodraysShader();
	uninitFinalPassForGodraysShader();
	uninitSkyboxShader();
	uninitCubemapCubeShader();
	uninitTextShader();
	uninitStarfieldShader();
	//uninitFullScreenQuad();

	
	//delete textures
	glDeleteTextures(1, &sun_texture);
	glDeleteTextures(1, &mercury_texture);
	glDeleteTextures(1, &venus_texture);
	glDeleteTextures(1, &earth_texture);
	glDeleteTextures(1, &earthMoon_texture);
	glDeleteTextures(1, &mars_texture);
	glDeleteTextures(1, &marsMoon_texture);
	glDeleteTextures(1, &jupiter_texture);
	glDeleteTextures(1, &jupiterMoon_texture);
	glDeleteTextures(1, &saturn_texture);
	glDeleteTextures(1, &saturnMoon_texture);
	glDeleteTextures(1, &uranus_texture);
	glDeleteTextures(1, &uranusMoon_texture);
	glDeleteTextures(1, &neptune_texture);
	glDeleteTextures(1, &neptuneMoon_texture);
	glDeleteTextures(1, &pluto_texture);
	glDeleteTextures(1, &star_texture);
	glDeleteTextures(1, &cubemap_texture);


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


