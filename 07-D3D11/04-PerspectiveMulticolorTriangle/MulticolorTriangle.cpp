//header files
#include<windows.h>
#include<stdio.h>

#include<d3d11.h>
#include<d3dcompiler.h>

#include<math.h>

#include "Resources.h"

#pragma warning(disable:4838)
#include "XNAMath_204/xnamath.h"

//macro functions
#pragma comment(lib, "d3d11.lib")
#pragma comment (lib, "D3dcompiler.lib")
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

ID3D11VertexShader* gpID3D11VertexShader = NULL;
ID3D11PixelShader* gpID3D11PixelShader = NULL;
ID3D11Buffer* gpID3D11Buffer_VertexBuffer_position = NULL;
ID3D11Buffer* gpID3D11Buffer_VertexBuffer_color = NULL;
ID3D11InputLayout* gpID3D11InputLayout = NULL;
ID3D11Buffer* gpID3D11Buffer_ConstantBuffer = NULL;

struct CBUFFER
{
	XMMATRIX WorldViewProjectionMatrix;
};

XMMATRIX gPerspectiveProjectionMatrix;

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

		//initialize shader, input layouts, constant buffers

		/*VERTEX SHADER*/

		const char* vertexShaderSourceCode =
			"cbuffer ConstantBuffer\n" \
			"{\n" \
			"	float4x4 worldViewProjectionMatrix;\n" \
			"}\n" \
			
			"struct vertex_output \n" \
			"{ \n" \
			"	float4 position: SV_POSITION; \n" \
			"	float4 color: COLOR; \n" \
			"}; \n" \

			"vertex_output main(float4 pos : POSITION, float4 col : COLOR) \n" \
			"{\n" \
			"	vertex_output output; \n" \
			"	output.position = mul(worldViewProjectionMatrix, pos);\n" \
			"	output.color = col; \n" \
			"	return(output); \n" \
			"}";

		//compile the source-code
		ID3DBlob* pID3DBlob_VertexShaderCode = NULL;
		ID3DBlob* pID3DBlob_Error = NULL;

		hr = D3DCompile(vertexShaderSourceCode,
			lstrlenA(vertexShaderSourceCode) + 1,
			"VS",
			NULL,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"main",
			"vs_5_0",
			0,
			0,
			&pID3DBlob_VertexShaderCode,
			&pID3DBlob_Error);

		if (FAILED(hr))
		{
			if (pID3DBlob_Error != NULL)
			{
				fopen_s(&gpFile, "RenderLog.txt", "a+");
				fprintf(gpFile, "D3DCompile() failed for vertex shader : %s\n\n", (char*)pID3DBlob_Error->GetBufferPointer());
				fclose(gpFile);
				pID3DBlob_Error->Release();
				pID3DBlob_Error = NULL;
				return(hr);
				//Uninitialize();
			}
			else
			{
				fopen_s(&gpFile, "RenderLog.txt", "a+");
				fprintf(gpFile, "COM error for vertex shader\n\n");
				fclose(gpFile);
				return(hr);
				//Uninitialize();
			}
			
		}
		else
		{
			fopen_s(&gpFile, "RenderLog.txt", "a+");
			fprintf(gpFile, ("D3DCompile() successful for vertex shader\n\n"));
			fclose(gpFile);
		}
		

		//create shader
		hr = gpID3D11Device->CreateVertexShader(pID3DBlob_VertexShaderCode->GetBufferPointer(), pID3DBlob_VertexShaderCode->GetBufferSize(), NULL, &gpID3D11VertexShader);
		if (FAILED(hr))
		{
			fopen_s(&gpFile, "RenderLog.txt", "a+");
			fprintf(gpFile, ("ID3D11Device::CreateVertexShader() failed\n\n"));
			fclose(gpFile);
			//Uninitialize();
		}
		else
		{
			fopen_s(&gpFile, "RenderLog.txt", "a+");
			fprintf(gpFile, ("ID3D11Device::CreateVertexShader() successful\n\n"));
			fclose(gpFile);
		}

		gpID3D11DeviceContext->VSSetShader(gpID3D11VertexShader, 0, 0);
		//dont release vertex shader code blob here, after setting vertex shader

		/*PIXEL SHADER*/

		const char* pixelShaderSourceCode =
			"struct vertex_output \n" \
			"{ \n" \
			"	float4 position: SV_POSITION; \n" \
			"	float4 color: COLOR; \n" \
			"}; \n" \

			"float4 main(vertex_output input) : SV_TARGET\n" \
			"{\n" \
			"	return(input.color); \n" \
			"}\n";

		//compile the source-code
		ID3DBlob* pID3DBlob_PixelShaderCode = NULL;
		pID3DBlob_Error = NULL;

		hr = D3DCompile(pixelShaderSourceCode,
			lstrlenA(pixelShaderSourceCode) + 1,
			"PS",
			NULL,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"main",
			"ps_5_0",
			0,
			0,
			&pID3DBlob_PixelShaderCode,
			&pID3DBlob_Error);

		if (FAILED(hr))
		{
			if (pID3DBlob_Error != NULL)
			{
				fopen_s(&gpFile, "RenderLog.txt", "a+");
				fprintf(gpFile, "D3DCompile() failed for pixel shader : %s\n\n", (char*)pID3DBlob_Error->GetBufferPointer());
				fclose(gpFile);
				pID3DBlob_Error->Release();
				pID3DBlob_Error = NULL;
				return(hr);
				//Uninitialize();
			}
			else
			{
				fopen_s(&gpFile, "RenderLog.txt", "a+");
				fprintf(gpFile, "COM error for pixel shader\n\n");
				fclose(gpFile);
				Uninitialize();
			}

		}
		else
		{
			fopen_s(&gpFile, "RenderLog.txt", "a+");
			fprintf(gpFile, ("D3DCompile() successful for pixel shader\n\n"));
			fclose(gpFile);
		}

		//create shader
		hr = gpID3D11Device->CreatePixelShader(pID3DBlob_PixelShaderCode->GetBufferPointer(), pID3DBlob_PixelShaderCode->GetBufferSize(), NULL, &gpID3D11PixelShader);
		if (FAILED(hr))
		{
			fopen_s(&gpFile, "RenderLog.txt", "a+");
			fprintf(gpFile, ("ID3D11Device::CreatePixelShader() failed\n\n"));
			fclose(gpFile);
			Uninitialize();
		}
		else
		{
			fopen_s(&gpFile, "RenderLog.txt", "a+");
			fprintf(gpFile, ("ID3D11Device::CreatePixelShader() successful\n\n"));
			fclose(gpFile);
		}

		//release pID3DBlob_Error
		if (pID3DBlob_Error)
		{
			pID3DBlob_Error->Release();
			pID3DBlob_Error = NULL;
		}

		gpID3D11DeviceContext->PSSetShader(gpID3D11PixelShader, 0, 0);
		//can release pixel shader code blob here, after setting pixel shader
		pID3DBlob_PixelShaderCode->Release();
		pID3DBlob_PixelShaderCode = NULL;


		//initialize, create and set input layout
		D3D11_INPUT_ELEMENT_DESC inputElementDesc[2];
		//ZeroMemory((void*)&inputElementDesc, sizeof(D3D11_INPUT_ELEMENT_DESC));

		inputElementDesc[0].SemanticName = "POSITION";
		inputElementDesc[0].SemanticIndex = 0;
		inputElementDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputElementDesc[0].InputSlot = 0;
		inputElementDesc[0].AlignedByteOffset = 0;
		inputElementDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputElementDesc[0].InstanceDataStepRate = 0;

		inputElementDesc[1].SemanticName = "COLOR";
		inputElementDesc[1].SemanticIndex = 0;
		inputElementDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputElementDesc[1].InputSlot = 1;
		inputElementDesc[1].AlignedByteOffset = 0;
		inputElementDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputElementDesc[1].InstanceDataStepRate = 0;


		hr = gpID3D11Device->CreateInputLayout(inputElementDesc, _ARRAYSIZE(inputElementDesc), pID3DBlob_VertexShaderCode->GetBufferPointer(), pID3DBlob_VertexShaderCode->GetBufferSize(), &gpID3D11InputLayout);
		if (FAILED(hr))
		{
			fopen_s(&gpFile, "RenderLog.txt", "a+");
			fprintf(gpFile, ("ID3D11Device::CreateInputLayout() failed\n\n"));
			fclose(gpFile);
			Uninitialize();

			if (pID3DBlob_VertexShaderCode)
			{
				pID3DBlob_VertexShaderCode->Release();
				pID3DBlob_VertexShaderCode = NULL;
			}
		}
		else
		{
			fopen_s(&gpFile, "RenderLog.txt", "a+");
			fprintf(gpFile, ("ID3D11Device::CreateInputLayout() successful\n\n"));
			fclose(gpFile);

			if (pID3DBlob_VertexShaderCode)
			{
				pID3DBlob_VertexShaderCode->Release();
				pID3DBlob_VertexShaderCode = NULL;
			}
		}

		//set created input layout
		gpID3D11DeviceContext->IASetInputLayout(gpID3D11InputLayout);
		
		//declare vertices, colors, normals, textures, coordinates of geometry [left-hand rule... coordinates are CW]
		const float vertices[] =
		{
			0.0f, 1.0f, 0.0f,       //apex
			1.0f, -1.0f, 0.0f,     //right
			-1.0f, -1.0f, 0.0f     //left
		};

		const float colors[] =
		{
			1.0f, 0.0f, 0.0f,       //apex
			0.0f, 0.0f, 1.0f,     //right
			0.0f, 1.0f, 0.0f     //left
		};

		//initialize buffer description structure & create buffer
		D3D11_BUFFER_DESC d3d11BufferDesc;

		//POSITION
		ZeroMemory((void*)&d3d11BufferDesc, sizeof(D3D11_BUFFER_DESC));
		d3d11BufferDesc.ByteWidth = sizeof(float) * _ARRAYSIZE(vertices);
		d3d11BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;   //glBindBuffer();
		d3d11BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		d3d11BufferDesc.Usage = D3D11_USAGE_DYNAMIC;

		hr = gpID3D11Device->CreateBuffer(&d3d11BufferDesc, NULL, &gpID3D11Buffer_VertexBuffer_position);
		if (FAILED(hr))
		{
			fopen_s(&gpFile, "RenderLog.txt", "a+");
			fprintf(gpFile, ("ID3D11Device::CreateBuffer() failed\n\n"));
			fclose(gpFile);
			Uninitialize();
		}
		else
		{
			fopen_s(&gpFile, "RenderLog.txt", "a+");
			fprintf(gpFile, ("ID3D11Device::CreateBuffer() successful\n\n"));
			fclose(gpFile);
		}

		D3D11_MAPPED_SUBRESOURCE d3d11MappedSubResource;
		ZeroMemory(&d3d11MappedSubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
		gpID3D11DeviceContext->Map(gpID3D11Buffer_VertexBuffer_position, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3d11MappedSubResource);
		memcpy(d3d11MappedSubResource.pData, vertices, sizeof(vertices));
		gpID3D11DeviceContext->Unmap(gpID3D11Buffer_VertexBuffer_position, 0);


		//COLOR
		ZeroMemory((void*)&d3d11BufferDesc, sizeof(D3D11_BUFFER_DESC));
		d3d11BufferDesc.ByteWidth = sizeof(float) * _ARRAYSIZE(colors);
		d3d11BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;   //glBindBuffer();
		d3d11BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		d3d11BufferDesc.Usage = D3D11_USAGE_DYNAMIC;

		hr = gpID3D11Device->CreateBuffer(&d3d11BufferDesc, NULL, &gpID3D11Buffer_VertexBuffer_color);
		if (FAILED(hr))
		{
			fopen_s(&gpFile, "RenderLog.txt", "a+");
			fprintf(gpFile, ("ID3D11Device::CreateBuffer() failed\n\n"));
			fclose(gpFile);
			Uninitialize();
		}
		else
		{
			fopen_s(&gpFile, "RenderLog.txt", "a+");
			fprintf(gpFile, ("ID3D11Device::CreateBuffer() successful\n\n"));
			fclose(gpFile);
		}

		//push the data in this buffer by Map(), memcpy() & Unmap() method
		//D3D11_MAPPED_SUBRESOURCE d3d11MappedSubResource;
		ZeroMemory(&d3d11MappedSubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
		gpID3D11DeviceContext->Map(gpID3D11Buffer_VertexBuffer_color, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3d11MappedSubResource);
		memcpy(d3d11MappedSubResource.pData, colors, sizeof(colors));
		gpID3D11DeviceContext->Unmap(gpID3D11Buffer_VertexBuffer_color, 0);
		//here we don't set buffer, we set it in draw();

		//initialize and create another buffer to hold uniforms called as constant buffers
		//set in pipeline
		//you can use same variable for constant buffer
		ZeroMemory((void*)&d3d11BufferDesc, sizeof(D3D11_BUFFER_DESC));
		d3d11BufferDesc.ByteWidth = sizeof(CBUFFER);
		d3d11BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		d3d11BufferDesc.CPUAccessFlags = 0;
		d3d11BufferDesc.Usage = D3D11_USAGE_DEFAULT;

		hr = gpID3D11Device->CreateBuffer(&d3d11BufferDesc, NULL, &gpID3D11Buffer_ConstantBuffer);
		if (FAILED(hr))
		{
			fopen_s(&gpFile, "RenderLog.txt", "a+");
			fprintf(gpFile, ("ID3D11Device::CreateBuffer() failed\n\n"));
			fclose(gpFile);
			Uninitialize();
		}
		else
		{
			fopen_s(&gpFile, "RenderLog.txt", "a+");
			fprintf(gpFile, ("ID3D11Device::CreateBuffer() successful\n\n"));
			fclose(gpFile);
		}

		gpID3D11DeviceContext->VSSetConstantBuffers(0, 1, &gpID3D11Buffer_ConstantBuffer);

		//d3d clear color
		gClearColor[0] = 0.0f;
		gClearColor[1] = 0.0f;
		gClearColor[2] = 0.0f;
		gClearColor[3] = 1.0f;

		//set projection matrix to identity matrix
		gPerspectiveProjectionMatrix = XMMatrixIdentity();

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

	if (height < 0)
		height = 1;

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

	//set orthographic matrix

	gPerspectiveProjectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), (float)width / (float)height, 0.1f, 100.0f);

	return(hr);
}

void Display(void)
{
	//code	
	gpID3D11DeviceContext->ClearRenderTargetView(gpID3D11RenderTargetView, gClearColor);

	//set vertex buffer in pipeline
	//position
	UINT stride = sizeof(float) * 3;
	UINT offset = 0;
	gpID3D11DeviceContext->IASetVertexBuffers(0, 1, &gpID3D11Buffer_VertexBuffer_position, &stride, &offset);

	//color
	stride = sizeof(float) * 3;
	offset = 0;
	gpID3D11DeviceContext->IASetVertexBuffers(1, 1, &gpID3D11Buffer_VertexBuffer_color, &stride, &offset);

	//set primitive topology
	gpID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//transformations
	XMMATRIX worldMatrix = XMMatrixTranslation(0.0f, 0.0f, 4.0f);
	XMMATRIX viewMatrix = XMMatrixIdentity();

	XMMATRIX wvpMatrix = worldMatrix * viewMatrix * gPerspectiveProjectionMatrix;

	//push transformation matrices in shader
	CBUFFER constantBuffer;
	ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
	constantBuffer.WorldViewProjectionMatrix = wvpMatrix;
	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer, 0, NULL, &constantBuffer, 0, 0);   //glUniformMatrix4fv();

	//draw
	gpID3D11DeviceContext->Draw(3, 0);

	gpIDXGISwapChain->Present(1, 0);
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

	if (gpID3D11Buffer_ConstantBuffer)
	{
		gpID3D11Buffer_ConstantBuffer->Release();
		gpID3D11Buffer_ConstantBuffer = NULL;
	}

	if (gpID3D11InputLayout)
	{
		gpID3D11InputLayout->Release();
		gpID3D11InputLayout = NULL;
	}

	if (gpID3D11Buffer_VertexBuffer_color)
	{
		gpID3D11Buffer_VertexBuffer_color->Release();
		gpID3D11Buffer_VertexBuffer_color = NULL;
	}

	if (gpID3D11Buffer_VertexBuffer_position)
	{
		gpID3D11Buffer_VertexBuffer_position->Release();
		gpID3D11Buffer_VertexBuffer_position = NULL;
	}

	if (gpID3D11PixelShader)
	{
		gpID3D11PixelShader->Release();
		gpID3D11PixelShader = NULL;
	}

	if (gpID3D11VertexShader)
	{
		gpID3D11VertexShader->Release();
		gpID3D11VertexShader = NULL;
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
