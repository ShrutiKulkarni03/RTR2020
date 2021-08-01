//header files
#include<windows.h>
#include<stdio.h>

#include<d3d11.h>
#include<math.h>

#include "Resources.h"

//macro functions
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib,"dxgi.lib")    //Directx Graphics Infrastructure      [anologous to wgl]

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

//global function declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HRESULT Resize(int, int);
HRESULT Initialize(void);
HRESULT d3dInfo(void);
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
HRESULT hr;

//direct3D variables
IDXGIFactory* pIDXGIFactory = NULL;
IDXGIAdapter* pIDXGIAdapter = NULL;

IDXGISwapChain* gpIDXGISwapChain = NULL;
ID3D11Device* gpID3D11Device = NULL;
ID3D11DeviceContext* gpID3D11DeviceContext = NULL;
ID3D11RenderTargetView* gpID3D11RenderTargetView = NULL;

float gClearColor[4];


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
		fclose(gpFile);
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
	ShowWindow(hwnd, iCmdShow);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);
	hr = Initialize();

	if (FAILED(hr))
	{
		fopen_s(&gpFile, "RenderLog.txt", "a+");
		fprintf(gpFile, ("Initialize() failed\n\n"));
		fclose(gpFile);
		Uninitialize();
	}
	else
	{
		fopen_s(&gpFile, "RenderLog.txt", "a+");
		fprintf(gpFile, ("Initialize() successful\n\n"));
		fclose(gpFile);
	}

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
	//HRESULT Resize(int, int);

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
		if (gpID3D11DeviceContext)
		{
			hr = Resize(LOWORD(lParam), HIWORD(lParam));
			if (FAILED(hr))
			{
				fopen_s(&gpFile, "RenderLog.txt", "a+");
				fprintf(gpFile, ("WM_SIZE- resize() failed\n\n"));
				fclose(gpFile);
				Uninitialize();
			}
			else
			{
				fopen_s(&gpFile, "RenderLog.txt", "a+");
				fprintf(gpFile, ("WM_SIZE- resize() successful\n\n"));
				fclose(gpFile);
			}
		}
		
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

HRESULT Initialize(void)
{
	//variables
	D3D_DRIVER_TYPE d3dDriverType;
	D3D_DRIVER_TYPE d3dDriverTypes[] = { D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP, D3D_DRIVER_TYPE_REFERENCE };
	UINT numDriverTypes;

	D3D_FEATURE_LEVEL d3dFeatureLevel_required = D3D_FEATURE_LEVEL_11_0;
	D3D_FEATURE_LEVEL d3dFeatureLevel_acquired = D3D_FEATURE_LEVEL_10_0;
	UINT numFeatureLevels = 1;

	UINT createDeviceFlags = 0;


	//code	
	hr = d3dInfo();
	if (FAILED(hr))
	{
		fopen_s(&gpFile, "RenderLog.txt", "a+");
		fprintf(gpFile, ("\nd3dInfo() failed\n\n"));
		fclose(gpFile);
		Uninitialize();
	}
	else
	{
		fopen_s(&gpFile, "RenderLog.txt", "a+");
		fprintf(gpFile, ("\nd3dInfo() successful\n\n"));
		fclose(gpFile);
	}

	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	ZeroMemory((void*)&dxgiSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	dxgiSwapChainDesc.BufferCount = 1;
	dxgiSwapChainDesc.BufferDesc.Width = WIN_WIDTH;
	dxgiSwapChainDesc.BufferDesc.Height = WIN_HEIGHT;
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.OutputWindow = ghwnd;
	dxgiSwapChainDesc.SampleDesc.Count = 1;
	dxgiSwapChainDesc.SampleDesc.Quality = 0;  //default
	dxgiSwapChainDesc.Windowed = TRUE;

	numDriverTypes = sizeof(d3dDriverTypes) / sizeof(d3dDriverTypes[0]);
	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		d3dDriverType = d3dDriverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(
			NULL,                       //adapter
			d3dDriverType,              //driver type
			NULL,                       //software
			createDeviceFlags,          //flags
			&d3dFeatureLevel_required,  //feature levels
			numFeatureLevels,           //num feature levels
			D3D11_SDK_VERSION,          //SDK version
			&dxgiSwapChainDesc,         //swap chain desc
			&gpIDXGISwapChain,          //swap chain
			&gpID3D11Device,            //device
			&d3dFeatureLevel_acquired,  //feature level
			&gpID3D11DeviceContext);    //device context

		if (SUCCEEDED(hr))
			break;
	}

	if (FAILED(hr))
	{
		fopen_s(&gpFile, "RenderLog.txt", "a+");
		fprintf(gpFile, ("D3D11CreateDeviceAndSwapChain() failed\n\n"));
		fclose(gpFile);
		Uninitialize();
	}
	else
	{
		fopen_s(&gpFile, "RenderLog.txt", "a+");
		fprintf(gpFile, ("D3D11CreateDeviceAndSwapChain() successful\n\n"));

		fprintf(gpFile, ("The Chosen Driver is:\n"));

		if(d3dDriverType == D3D_DRIVER_TYPE_HARDWARE)
			fprintf(gpFile, ("Hardware Type chosen\n\n"));

		else if (d3dDriverType == D3D_DRIVER_TYPE_WARP)
			fprintf(gpFile, ("Warp Type chosen\n\n"));

		else if (d3dDriverType == D3D_DRIVER_TYPE_REFERENCE)
			fprintf(gpFile, ("Reference Type chosen\n\n"));

		else
			fprintf(gpFile, ("Unknown Type\n\n"));



		fprintf(gpFile, ("The Supported Highest Feature Level is:\n"));

		if (d3dFeatureLevel_acquired == D3D_FEATURE_LEVEL_11_0)
			fprintf(gpFile, ("11.0\n\n"));

		else if (d3dFeatureLevel_acquired == D3D_FEATURE_LEVEL_10_1)
			fprintf(gpFile, ("10.1\n\n"));

		else if (d3dFeatureLevel_acquired == D3D_FEATURE_LEVEL_10_0)
			fprintf(gpFile, ("10.0\n\n"));

		else
			fprintf(gpFile, ("Unknown Level\n\n"));


		fclose(gpFile);

		//d3d clear color
		gClearColor[0] = 0.0f;
		gClearColor[1] = 0.0f;
		gClearColor[2] = 1.0f;
		gClearColor[3] = 0.0f;

		hr = Resize(WIN_WIDTH, WIN_HEIGHT);
		if (FAILED(hr))
		{
			fopen_s(&gpFile, "RenderLog.txt", "a+");
			fprintf(gpFile, ("Resize() failed\n\n"));
			fclose(gpFile);
			Uninitialize();
		}
		else
		{
			fopen_s(&gpFile, "RenderLog.txt", "a+");
			fprintf(gpFile, ("Resize() successful\n\n"));
			fclose(gpFile);
		}


	}

	return(hr);


}

HRESULT d3dInfo(void)
{
	//variable declarations
	DXGI_ADAPTER_DESC dxgiAdapterDesc;
	char str[255];

	//code
	hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pIDXGIFactory);

	if (FAILED(hr))
	{
		if (FAILED(hr))
		{
			fopen_s(&gpFile, "RenderLog.txt", "a+");
			fprintf(gpFile, ("createDXGIFactory() failed\n\n"));
			fclose(gpFile);
			Uninitialize();
		}
		else
		{
			fopen_s(&gpFile, "RenderLog.txt", "a+");
			fprintf(gpFile, ("createDXGIFactory() successful\n\n"));
			fclose(gpFile);
		}
		//goto cleanup;  //labelled jumping
	}

	if (pIDXGIFactory->EnumAdapters(0, &pIDXGIAdapter) == DXGI_ERROR_NOT_FOUND)
	{
		fopen_s(&gpFile, "RenderLog.txt", "a+");
		fprintf(gpFile, ("DXGIAdapter failed\n\n"));
		fclose(gpFile);
		Uninitialize();
		//goto cleanup;
	}

	ZeroMemory((void*)&dxgiAdapterDesc, sizeof(DXGI_ADAPTER_DESC));

	pIDXGIAdapter->GetDesc(&dxgiAdapterDesc);

	WideCharToMultiByte(CP_ACP, 0, dxgiAdapterDesc.Description, 255, str, 255, NULL, NULL);

	fopen_s(&gpFile, "RenderLog.txt", "a+");
	fprintf(gpFile, "GPU Info Log: \n");
	fprintf(gpFile, "Graphic Card Name: %s\n", str);
	fprintf(gpFile, "Graphic Card Memory [VRAM]: %I64d bytes\n", (__int64)dxgiAdapterDesc.DedicatedVideoMemory);
	fprintf(gpFile, "VRAM (GB) = %d\n", (int)(ceil(dxgiAdapterDesc.DedicatedVideoMemory / 1024.0 / 1024.0 / 1024.0)));
	fclose(gpFile);

	return(hr);

}

HRESULT Resize(int width, int height)
{
	//code	
	hr = S_OK;

	if (gpID3D11RenderTargetView)
	{
		gpID3D11RenderTargetView->Release();
		gpID3D11RenderTargetView = NULL;
	}

	gpIDXGISwapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

	ID3D11Texture2D* pID3D11Texture2D_BackBuffer;

	gpIDXGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pID3D11Texture2D_BackBuffer);

	hr = gpID3D11Device->CreateRenderTargetView(pID3D11Texture2D_BackBuffer, NULL, &gpID3D11RenderTargetView);
	if (FAILED(hr))
	{
		fopen_s(&gpFile, "RenderLog.txt", "a+");
		fprintf(gpFile, ("CreateRenderTargetView() failed\n\n"));
		fclose(gpFile);
		Uninitialize();
	}
	else
	{
		fopen_s(&gpFile, "RenderLog.txt", "a+");
		fprintf(gpFile, ("CreateRenderTargetView() successful\n\n"));
		fclose(gpFile);
	}

	//release
	pID3D11Texture2D_BackBuffer->Release();
	pID3D11Texture2D_BackBuffer = NULL;

	gpID3D11DeviceContext->OMSetRenderTargets(1, &gpID3D11RenderTargetView, NULL);

	D3D11_VIEWPORT d3d11ViewPort;
	ZeroMemory((void*)&d3d11ViewPort, sizeof(D3D11_VIEWPORT));
	d3d11ViewPort.TopLeftX = 0;
	d3d11ViewPort.TopLeftY = 0;
	d3d11ViewPort.Width = (float)width;
	d3d11ViewPort.Height = (float)height;
	d3d11ViewPort.MinDepth = 0.0f;
	d3d11ViewPort.MaxDepth = 1.0f;
	gpID3D11DeviceContext->RSSetViewports(1, &d3d11ViewPort);

	return(hr);
}

void Display(void)
{
	//code	
	gpID3D11DeviceContext->ClearRenderTargetView(gpID3D11RenderTargetView, gClearColor);

	gpIDXGISwapChain->Present(0, 0);
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
	
	if (pIDXGIAdapter)
	{
		pIDXGIAdapter->Release();
		pIDXGIAdapter = NULL;
	}

	if (pIDXGIFactory)
	{
		pIDXGIFactory->Release();
		pIDXGIFactory = NULL;
	}

	if (gpID3D11RenderTargetView)
	{
		gpID3D11RenderTargetView->Release();
		gpID3D11RenderTargetView = NULL;
	}

	if (gpIDXGISwapChain)
	{
		gpIDXGISwapChain->Release();
		gpIDXGISwapChain = NULL;
	}

	if (gpID3D11DeviceContext)
	{
		gpID3D11DeviceContext->Release();
		gpID3D11DeviceContext = NULL;
	}

	if (gpID3D11Device)
	{
		gpID3D11Device->Release();
		gpID3D11Device = NULL;
	}

	//
	fopen_s(&gpFile, "RenderLog.txt", "a+");
	fprintf(gpFile, ("Log File Closed Successfully, Program Completed Successfully.\n"));
	fclose(gpFile);
}
