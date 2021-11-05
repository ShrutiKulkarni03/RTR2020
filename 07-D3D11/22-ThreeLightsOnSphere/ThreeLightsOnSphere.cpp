//header files
#include<windows.h>
#include<stdio.h>

#include<d3d11.h>
#include<d3dcompiler.h>

#include<math.h>

#include "Sphere.h"
#include "Resources.h"

#pragma warning(disable:4838)
#include "XNAMath_204/xnamath.h"

//macro functions
#pragma comment(lib, "d3d11.lib")
#pragma comment (lib, "D3dcompiler.lib")
#pragma comment(lib,"dxgi.lib")    //Directx Graphics Infrastructure      [anologous to wgl]
#pragma comment(lib, "Sphere.lib")

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

//global function declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HRESULT Resize(int, int);
HRESULT Initialize(void);
HRESULT d3dInfo(void);
void Display(void);
void Update(void);
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

ID3D11DepthStencilView* gpID3D11DepthStencilView = NULL;

ID3D11VertexShader* gpID3D11VertexShaderPV = NULL;
ID3D11PixelShader* gpID3D11PixelShaderPV = NULL;
ID3D11VertexShader* gpID3D11VertexShaderPP = NULL;
ID3D11PixelShader* gpID3D11PixelShaderPP = NULL;

ID3D11Buffer* gpID3D11Buffer_VertexBuffer_position_cube = NULL;
ID3D11Buffer* gpID3D11Buffer_VertexBuffer_normal_cube = NULL;
ID3D11Buffer* gpID3D11Buffer_IndexBuffer = NULL;


ID3D11InputLayout* gpID3D11InputLayoutPV = NULL;
ID3D11InputLayout* gpID3D11InputLayoutPP = NULL;

ID3D11Buffer* gpID3D11Buffer_ConstantBufferPV = NULL;
ID3D11Buffer* gpID3D11Buffer_ConstantBufferPP = NULL;

//culling
ID3D11RasterizerState* gpID3D11RasterizerState = NULL;

struct CBUFFER
{
	XMMATRIX WorldMatrix;
	XMMATRIX ViewMatrix;
	XMMATRIX ProjectionMatrix;

	XMVECTOR LA[3];
	XMVECTOR LD[3];
	XMVECTOR LS[3];
	XMVECTOR LightPosition[3];

	XMVECTOR KA;
	XMVECTOR KD;
	XMVECTOR KS;
	float MaterialShininess;
};

bool PVLighting = true;

XMMATRIX gPerspectiveProjectionMatrix;

float gClearColor[4];

//sphere
float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];
unsigned int numVertices;
unsigned short numElements;

//float lightAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
//float lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
//float lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
//float lightPosition[] = { 100.0f, 100.0f, -100.0f, 1.0f };
//
//float materialAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
//float materialDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
//float materialSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
float materialShininess = 50.0f;

float lightAngle0 = 0.0f;
float lightAngle1 = 0.0f;
float lightAngle2 = 0.0f;


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
				Update();
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

	case WM_CHAR:
		switch (wParam)
		{
		case 'V':
		case 'v':
			PVLighting = true;
			break;

		case 'P':
		case 'p':
			PVLighting = false;
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

		if (d3dDriverType == D3D_DRIVER_TYPE_HARDWARE)
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

		/**********************************************************************************************************************************************************************************/

		/*VERTEX SHADER*/

		const char* vertexShaderSourceCodePV =
			"cbuffer ConstantBuffer\n" \
			"{\n" \
			"	float4x4 worldMatrix;\n" \
			"	float4x4 viewMatrix;\n" \
			"	float4x4 projectionMatrix;\n" \
			"	float4 lA[3];\n" \
			"	float4 lD[3];\n" \
			"	float4 lS[3];\n" \
			"	float4 lightPosition[3];\n" \
			"	float4 kA;\n" \
			"	float4 kD;\n" \
			"	float4 kS;\n" \
			"	float materialShininess;\n" \
			"}\n" \

			"struct vertex_output \n" \
			"{ \n" \
			"	float4 position: SV_POSITION; \n" \
			"	float3 phong_ads_light: COLOR; \n" \
			"}; \n" \

			"vertex_output main(float4 pos : POSITION, float4 normal : NORMAL)\n" \
			"{\n" \
			"	vertex_output output;\n" \
			"	output.phong_ads_light = float3(0.0, 0.0, 0.0); \n" \
			"	float3 ambient[3]; \n" \
			"	float3 diffuse[3]; \n" \
			"	float3 specular[3]; \n" \
			"	float3 reflection_vector[3]; \n" \
			"	float3 light_direction[3]; \n" \

			"	float4 eyePosition = mul(worldMatrix, pos);\n" \
			"	eyePosition = mul(viewMatrix, eyePosition);\n" \
			"	float3 transformed_normals = (float3)normalize(mul((float3x3)worldMatrix, (float3)normal));\n" \
			"	float3 view_vector = normalize(-eyePosition.xyz);\n" \

			"	for(int i=0; i<3; i++) \n" \
			"	{\n" \
			"		light_direction[i] = (float3)normalize(lightPosition[i] - eyePosition);\n" \
			"		ambient[i] = lA[i] * kA;\n" \
			"		diffuse[i] = lD[i] * kD * max(dot(light_direction[i], transformed_normals), 0.0);\n" \
			"		reflection_vector[i] = reflect(-light_direction[i], transformed_normals);\n" \
			"		specular[i] = lS[i] * kS * pow(max(dot(reflection_vector[i], view_vector), 0.0), materialShininess);\n" \
			"		output.phong_ads_light += (float3)(ambient[i] + diffuse[i] + specular[i]);\n" \
			"	}\n" \

			"	float4 position = mul(worldMatrix, pos);\n" \
			"	position = mul(viewMatrix, position);\n" \
			"	position = mul(projectionMatrix, position);\n" \
			"	output.position = position;\n" \
			"	return(output);" \
			"}";

		//compile the source-code
		ID3DBlob* pID3DBlob_VertexShaderCodePV = NULL;
		ID3DBlob* pID3DBlob_Error = NULL;

		hr = D3DCompile(vertexShaderSourceCodePV,
			lstrlenA(vertexShaderSourceCodePV) + 1,
			"VS",
			NULL,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"main",
			"vs_5_0",
			0,
			0,
			&pID3DBlob_VertexShaderCodePV,
			&pID3DBlob_Error);

		if (FAILED(hr))
		{
			if (pID3DBlob_Error != NULL)
			{
				fopen_s(&gpFile, "RenderLog.txt", "a+");
				fprintf(gpFile, "D3DCompile() failed for vertex shader PV : %s\n\n", (char*)pID3DBlob_Error->GetBufferPointer());
				fclose(gpFile);
				pID3DBlob_Error->Release();
				pID3DBlob_Error = NULL;
				return(hr);
				//Uninitialize();
			}
			else
			{
				fopen_s(&gpFile, "RenderLog.txt", "a+");
				fprintf(gpFile, "COM error for vertex shader PV\n\n");
				fclose(gpFile);
				return(hr);
				//Uninitialize();
			}

		}
		else
		{
			fopen_s(&gpFile, "RenderLog.txt", "a+");
			fprintf(gpFile, ("D3DCompile() successful for vertex shader PV\n\n"));
			fclose(gpFile);
		}


		//create shader
		hr = gpID3D11Device->CreateVertexShader(pID3DBlob_VertexShaderCodePV->GetBufferPointer(), pID3DBlob_VertexShaderCodePV->GetBufferSize(), NULL, &gpID3D11VertexShaderPV);
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


		//dont release vertex shader code blob here, after setting vertex shader

		/*PIXEL SHADER*/

		const char* pixelShaderSourceCodePV =
			"struct vertex_output \n" \
			"{ \n" \
			"	float4 position: SV_POSITION; \n" \
			"	float3 phong_ads_light: COLOR; \n" \
			"}; \n" \

			"float4 main(vertex_output input) : SV_TARGET\n" \
			"{\n" \
			"	float4 color = float4(input.phong_ads_light, 1.0); \n" \
			"	return(color); \n" \
			"}\n";

		//compile the source-code
		ID3DBlob* pID3DBlob_PixelShaderCodePV = NULL;
		pID3DBlob_Error = NULL;

		hr = D3DCompile(pixelShaderSourceCodePV,
			lstrlenA(pixelShaderSourceCodePV) + 1,
			"PS",
			NULL,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"main",
			"ps_5_0",
			0,
			0,
			&pID3DBlob_PixelShaderCodePV,
			&pID3DBlob_Error);

		if (FAILED(hr))
		{
			if (pID3DBlob_Error != NULL)
			{
				fopen_s(&gpFile, "RenderLog.txt", "a+");
				fprintf(gpFile, "D3DCompile() failed for pixel shader PV: %s\n\n", (char*)pID3DBlob_Error->GetBufferPointer());
				fclose(gpFile);
				pID3DBlob_Error->Release();
				pID3DBlob_Error = NULL;
				return(hr);
				//Uninitialize();
			}
			else
			{
				fopen_s(&gpFile, "RenderLog.txt", "a+");
				fprintf(gpFile, "COM error for pixel shader PV\n\n");
				fclose(gpFile);
				Uninitialize();
			}

		}
		else
		{
			fopen_s(&gpFile, "RenderLog.txt", "a+");
			fprintf(gpFile, ("D3DCompile() successful for pixel shader PV\n\n"));
			fclose(gpFile);
		}

		//create shader
		hr = gpID3D11Device->CreatePixelShader(pID3DBlob_PixelShaderCodePV->GetBufferPointer(), pID3DBlob_PixelShaderCodePV->GetBufferSize(), NULL, &gpID3D11PixelShaderPV);
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


		//can release pixel shader code blob here, after setting pixel shader
		pID3DBlob_PixelShaderCodePV->Release();
		pID3DBlob_PixelShaderCodePV = NULL;

		/**********************************************************************************************************************************************************************************/


		/**********************************************************************************************************************************************************************************/
		/*VERTEX SHADER*/

		const char* vertexShaderSourceCodePP =
			"cbuffer ConstantBuffer\n" \
			"{\n" \
			"	float4x4 worldMatrix;\n" \
			"	float4x4 viewMatrix;\n" \
			"	float4x4 projectionMatrix;\n" \
			"	float4 lA[3];\n" \
			"	float4 lD[3];\n" \
			"	float4 lS[3];\n" \
			"	float4 lightPosition[3];\n" \
			"	float4 kA;\n" \
			"	float4 kD;\n" \
			"	float4 kS;\n" \
			"	float materialShininess;\n" \
			"}\n" \

			"struct vertex_output \n" \
			"{ \n" \
			"	float4 position: SV_POSITION; \n" \
			"	float3 transformed_normals : NORMAL0; \n" \
			"	float3 light_direction[3] : NORMAL1; \n" \
			"	float3 view_vector : NORMAL4; \n" \
			"}; \n" \

			"vertex_output main(float4 pos : POSITION, float4 normal : NORMAL)\n" \
			"{\n" \
			"	vertex_output output;\n" \
			"	float4 eyePosition = mul(worldMatrix, pos);\n" \
			"	eyePosition = mul(viewMatrix, eyePosition);\n" \
			"	output.transformed_normals = mul((float3x3)worldMatrix, (float3)normal);\n" \
			
			"	for(int i=0; i<3; i++) \n" \
			"	{\n" \
			"		output.light_direction[i] = (float3)lightPosition[i] - eyePosition.xyz;\n" \
			"	}\n" \

			"	output.view_vector = -eyePosition.xyz;\n" \
			"	float4 position = mul(worldMatrix, pos);\n" \
			"	position = mul(viewMatrix, position);\n" \
			"	position = mul(projectionMatrix, position);\n" \
			"	output.position = position;\n" \
			"	return(output);" \
			"}";

		//compile the source-code
		ID3DBlob* pID3DBlob_VertexShaderCodePP = NULL;
		pID3DBlob_Error = NULL;

		hr = D3DCompile(vertexShaderSourceCodePP,
			lstrlenA(vertexShaderSourceCodePP) + 1,
			"VS",
			NULL,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"main",
			"vs_5_0",
			0,
			0,
			&pID3DBlob_VertexShaderCodePP,
			&pID3DBlob_Error);

		if (FAILED(hr))
		{
			if (pID3DBlob_Error != NULL)
			{
				fopen_s(&gpFile, "RenderLog.txt", "a+");
				fprintf(gpFile, "D3DCompile() failed for vertex shader PP : %s\n\n", (char*)pID3DBlob_Error->GetBufferPointer());
				fclose(gpFile);
				pID3DBlob_Error->Release();
				pID3DBlob_Error = NULL;
				return(hr);
				//Uninitialize();
			}
			else
			{
				fopen_s(&gpFile, "RenderLog.txt", "a+");
				fprintf(gpFile, "COM error for vertex shader PP\n\n");
				fclose(gpFile);
				return(hr);
				//Uninitialize();
			}

		}
		else
		{
			fopen_s(&gpFile, "RenderLog.txt", "a+");
			fprintf(gpFile, ("D3DCompile() successful for vertex shader PP\n\n"));
			fclose(gpFile);
		}


		//create shader
		hr = gpID3D11Device->CreateVertexShader(pID3DBlob_VertexShaderCodePP->GetBufferPointer(), pID3DBlob_VertexShaderCodePP->GetBufferSize(), NULL, &gpID3D11VertexShaderPP);
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

		
		//dont release vertex shader code blob here, after setting vertex shader

		/*PIXEL SHADER*/

		const char* pixelShaderSourceCodePP =
			"cbuffer ConstantBuffer\n" \
			"{\n" \
			"	float4x4 worldMatrix;\n" \
			"	float4x4 viewMatrix;\n" \
			"	float4x4 projectionMatrix;\n" \
			"	float4 lA[3];\n" \
			"	float4 lD[3];\n" \
			"	float4 lS[3];\n" \
			"	float4 lightPosition[3];\n" \
			"	float4 kA;\n" \
			"	float4 kD;\n" \
			"	float4 kS;\n" \
			"	float materialShininess;\n" \
			"}\n" \

			"struct vertex_output \n" \
			"{ \n" \
			"	float4 position: SV_POSITION; \n" \
			"	float3 transformed_normals : NORMAL0; \n" \
			"	float3 light_direction[3] : NORMAL1; \n" \
			"	float3 view_vector : NORMAL4; \n" \
			"}; \n" \

			"float4 main(vertex_output input) : SV_TARGET\n" \
			"{\n" \
			"	float3 ambient[3]; \n" \
			"	float3 diffuse[3]; \n" \
			"	float3 specular[3]; \n" \
			"	float3 normalized_light_direction[3]; \n" \
			"	float3 reflection_vector[3]; \n" \
			"	float3 phong_ads_light = float3(0.0, 0.0, 0.0); \n" \
			"	float3 normalized_transformed_normals = normalize(input.transformed_normals);\n" \
			"	float3 normalized_view_vector = normalize(input.view_vector);\n" \
			
			"	for(int i=0; i<3; i++) \n" \
			"	{\n" \
			"		normalized_light_direction[i] = normalize(input.light_direction[i]);\n" \
			"		ambient[i] = (float3)(lA[i] * kA);\n" \
			"		diffuse[i] = (float3)(lD[i] * kD) * max(dot(normalized_light_direction[i], normalized_transformed_normals), 0.0);\n" \
			"		reflection_vector[i] = reflect(-normalized_light_direction[i], normalized_transformed_normals);\n" \
			"		specular[i] = (float3)(lS[i] * kS) * pow(max(dot(reflection_vector[i], normalized_view_vector), 0.0), materialShininess);\n" \
			"		phong_ads_light += (float3)(ambient[i] + diffuse[i] + specular[i]);\n" \
			"	}\n" \
			"	float4 color = float4(phong_ads_light, 1.0); \n" \
			"	return(color); \n" \
			"}\n";

		//compile the source-code
		ID3DBlob* pID3DBlob_PixelShaderCodePP = NULL;
		pID3DBlob_Error = NULL;

		hr = D3DCompile(pixelShaderSourceCodePP,
			lstrlenA(pixelShaderSourceCodePP) + 1,
			"PS",
			NULL,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"main",
			"ps_5_0",
			0,
			0,
			&pID3DBlob_PixelShaderCodePP,
			&pID3DBlob_Error);

		if (FAILED(hr))
		{
			if (pID3DBlob_Error != NULL)
			{
				fopen_s(&gpFile, "RenderLog.txt", "a+");
				fprintf(gpFile, "D3DCompile() failed for pixel shader PP : %s\n\n", (char*)pID3DBlob_Error->GetBufferPointer());
				fclose(gpFile);
				pID3DBlob_Error->Release();
				pID3DBlob_Error = NULL;
				return(hr);
				//Uninitialize();
			}
			else
			{
				fopen_s(&gpFile, "RenderLog.txt", "a+");
				fprintf(gpFile, "COM error for pixel shader PP\n\n");
				fclose(gpFile);
				Uninitialize();
			}

		}
		else
		{
			fopen_s(&gpFile, "RenderLog.txt", "a+");
			fprintf(gpFile, ("D3DCompile() successful for pixel shader PP\n\n"));
			fclose(gpFile);
		}

		//create shader
		hr = gpID3D11Device->CreatePixelShader(pID3DBlob_PixelShaderCodePP->GetBufferPointer(), pID3DBlob_PixelShaderCodePP->GetBufferSize(), NULL, &gpID3D11PixelShaderPP);
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

		
		//can release pixel shader code blob here, after setting pixel shader
		pID3DBlob_PixelShaderCodePP->Release();
		pID3DBlob_PixelShaderCodePP = NULL;

		/**********************************************************************************************************************************************************************************/
		
		
		

		//initialize, create and set input layout


		/*QUAD*/

		D3D11_INPUT_ELEMENT_DESC inputElementDescQuad[2];
		//ZeroMemory((void*)&inputElementDesc, sizeof(D3D11_INPUT_ELEMENT_DESC));

		inputElementDescQuad[0].SemanticName = "POSITION";
		inputElementDescQuad[0].SemanticIndex = 0;
		inputElementDescQuad[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputElementDescQuad[0].InputSlot = 0;
		inputElementDescQuad[0].AlignedByteOffset = 0;
		inputElementDescQuad[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputElementDescQuad[0].InstanceDataStepRate = 0;

		inputElementDescQuad[1].SemanticName = "NORMAL";
		inputElementDescQuad[1].SemanticIndex = 0;
		inputElementDescQuad[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputElementDescQuad[1].InputSlot = 1;
		inputElementDescQuad[1].AlignedByteOffset = 0;
		inputElementDescQuad[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputElementDescQuad[1].InstanceDataStepRate = 0;

		hr = gpID3D11Device->CreateInputLayout(inputElementDescQuad, _ARRAYSIZE(inputElementDescQuad), pID3DBlob_VertexShaderCodePV->GetBufferPointer(), pID3DBlob_VertexShaderCodePV->GetBufferSize(), &gpID3D11InputLayoutPV);

		if (FAILED(hr))
		{
			fopen_s(&gpFile, "RenderLog.txt", "a+");
			fprintf(gpFile, ("ID3D11Device::CreateInputLayout() failed PV\n\n"));
			fclose(gpFile);
			Uninitialize();

			if (pID3DBlob_VertexShaderCodePP)
			{
				pID3DBlob_VertexShaderCodePP->Release();
				pID3DBlob_VertexShaderCodePP = NULL;
			}
		}
		else
		{
			fopen_s(&gpFile, "RenderLog.txt", "a+");
			fprintf(gpFile, ("ID3D11Device::CreateInputLayout() successful PV\n\n"));
			fclose(gpFile);

			/*if (pID3DBlob_VertexShaderCode)
			{
				pID3DBlob_VertexShaderCode->Release();
				pID3DBlob_VertexShaderCode = NULL;
			}*/
		}

		if (pID3DBlob_VertexShaderCodePV)
		{
			pID3DBlob_VertexShaderCodePV->Release();
			pID3DBlob_VertexShaderCodePV = NULL;
		}



		hr = gpID3D11Device->CreateInputLayout(inputElementDescQuad, _ARRAYSIZE(inputElementDescQuad), pID3DBlob_VertexShaderCodePP->GetBufferPointer(), pID3DBlob_VertexShaderCodePP->GetBufferSize(), &gpID3D11InputLayoutPP);

		if (FAILED(hr))
		{
			fopen_s(&gpFile, "RenderLog.txt", "a+");
			fprintf(gpFile, ("ID3D11Device::CreateInputLayout() failed PP\n\n"));
			fclose(gpFile);
			Uninitialize();

			if (pID3DBlob_VertexShaderCodePP)
			{
				pID3DBlob_VertexShaderCodePP->Release();
				pID3DBlob_VertexShaderCodePP = NULL;
			}
		}
		else
		{
			fopen_s(&gpFile, "RenderLog.txt", "a+");
			fprintf(gpFile, ("ID3D11Device::CreateInputLayout() successful PP\n\n"));
			fclose(gpFile);

			/*if (pID3DBlob_VertexShaderCode)
			{
				pID3DBlob_VertexShaderCode->Release();
				pID3DBlob_VertexShaderCode = NULL;
			}*/
		}

		if (pID3DBlob_VertexShaderCodePP)
		{
			pID3DBlob_VertexShaderCodePP->Release();
			pID3DBlob_VertexShaderCodePP = NULL;
		}
		
				
		//set created input layout
		

		//declare vertices, colors, normals, textures, coordinates of geometry [left-hand rule... coordinates are CW]

		//sphere data
		getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
		numVertices = getNumberOfSphereVertices();
		numElements = getNumberOfSphereElements();


		//initialize buffer description structure & create buffer
		D3D11_BUFFER_DESC d3d11BufferDesc;

		//POSITION
		ZeroMemory((void*)&d3d11BufferDesc, sizeof(D3D11_BUFFER_DESC));
		d3d11BufferDesc.ByteWidth = numVertices * 3 * sizeof(float);
		d3d11BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;   //glBindBuffer();
		d3d11BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		d3d11BufferDesc.Usage = D3D11_USAGE_DYNAMIC;

		hr = gpID3D11Device->CreateBuffer(&d3d11BufferDesc, NULL, &gpID3D11Buffer_VertexBuffer_position_cube);
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
		gpID3D11DeviceContext->Map(gpID3D11Buffer_VertexBuffer_position_cube, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3d11MappedSubResource);
		memcpy(d3d11MappedSubResource.pData, sphere_vertices, numVertices * 3 * sizeof(float));
		gpID3D11DeviceContext->Unmap(gpID3D11Buffer_VertexBuffer_position_cube, 0);


		//NORMALS
		ZeroMemory((void*)&d3d11BufferDesc, sizeof(D3D11_BUFFER_DESC));
		d3d11BufferDesc.ByteWidth = numVertices * 3 * sizeof(float);
		d3d11BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;   //glBindBuffer();
		d3d11BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		d3d11BufferDesc.Usage = D3D11_USAGE_DYNAMIC;

		hr = gpID3D11Device->CreateBuffer(&d3d11BufferDesc, NULL, &gpID3D11Buffer_VertexBuffer_normal_cube);
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
		gpID3D11DeviceContext->Map(gpID3D11Buffer_VertexBuffer_normal_cube, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3d11MappedSubResource);
		memcpy(d3d11MappedSubResource.pData, sphere_normals, numVertices * 3 * sizeof(float));
		gpID3D11DeviceContext->Unmap(gpID3D11Buffer_VertexBuffer_normal_cube, 0);


		//INDEX BUFFER
		ZeroMemory((void*)&d3d11BufferDesc, sizeof(D3D11_BUFFER_DESC));
		d3d11BufferDesc.ByteWidth = numElements * sizeof(short);
		d3d11BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;   //glBindBuffer();
		d3d11BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		d3d11BufferDesc.Usage = D3D11_USAGE_DYNAMIC;

		hr = gpID3D11Device->CreateBuffer(&d3d11BufferDesc, NULL, &gpID3D11Buffer_IndexBuffer);
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
		gpID3D11DeviceContext->Map(gpID3D11Buffer_IndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3d11MappedSubResource);
		memcpy(d3d11MappedSubResource.pData, sphere_elements, numElements * sizeof(short));
		gpID3D11DeviceContext->Unmap(gpID3D11Buffer_IndexBuffer, 0);



		//initialize and create another buffer to hold uniforms called as constant buffers
		//set in pipeline
		//you can use same variable for constant buffer
		ZeroMemory((void*)&d3d11BufferDesc, sizeof(D3D11_BUFFER_DESC));
		d3d11BufferDesc.ByteWidth = sizeof(CBUFFER);
		d3d11BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		d3d11BufferDesc.CPUAccessFlags = 0;
		d3d11BufferDesc.Usage = D3D11_USAGE_DEFAULT;

		hr = gpID3D11Device->CreateBuffer(&d3d11BufferDesc, NULL, &gpID3D11Buffer_ConstantBufferPV);
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


		hr = gpID3D11Device->CreateBuffer(&d3d11BufferDesc, NULL, &gpID3D11Buffer_ConstantBufferPP);
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
				

		//create and set rasterizer state [backface_culling]
		D3D11_RASTERIZER_DESC d3d11RasterizerDesc;
		ZeroMemory((void*)&d3d11RasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
		d3d11RasterizerDesc.AntialiasedLineEnable = false;
		d3d11RasterizerDesc.CullMode = D3D11_CULL_NONE;
		d3d11RasterizerDesc.DepthBias = 0;
		d3d11RasterizerDesc.DepthBiasClamp = 0.0f;
		d3d11RasterizerDesc.DepthClipEnable = true;
		d3d11RasterizerDesc.FillMode = D3D11_FILL_SOLID;     //shaded or wireframe
		d3d11RasterizerDesc.FrontCounterClockwise = false;
		d3d11RasterizerDesc.MultisampleEnable = false;
		d3d11RasterizerDesc.ScissorEnable = false;
		d3d11RasterizerDesc.SlopeScaledDepthBias = 0.0f;

		hr = gpID3D11Device->CreateRasterizerState(&d3d11RasterizerDesc, &gpID3D11RasterizerState);
		if (FAILED(hr))
		{
			fopen_s(&gpFile, "RenderLog.txt", "a+");
			fprintf(gpFile, ("ID3D11Device::CreateRasterizerState() failed\n\n"));
			fclose(gpFile);
			Uninitialize();
		}
		else
		{
			fopen_s(&gpFile, "RenderLog.txt", "a+");
			fprintf(gpFile, ("ID3D11Device::CreateRasterizerState() successful\n\n"));
			fclose(gpFile);
		}

		gpID3D11DeviceContext->RSSetState(gpID3D11RasterizerState);


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

	//depth related code here:
	//just like RTV, DSV also needs render buffer
	D3D11_TEXTURE2D_DESC d3d11Texture2dDesc;
	ZeroMemory((void*)&d3d11Texture2dDesc, sizeof(D3D11_TEXTURE2D_DESC));
	d3d11Texture2dDesc.Width = (UINT)width;
	d3d11Texture2dDesc.Height = (UINT)height;
	d3d11Texture2dDesc.Format = DXGI_FORMAT_D32_FLOAT;
	d3d11Texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
	d3d11Texture2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	d3d11Texture2dDesc.SampleDesc.Count = 1;    //1 to 4
	d3d11Texture2dDesc.SampleDesc.Quality = 0;
	d3d11Texture2dDesc.ArraySize = 1;
	d3d11Texture2dDesc.MipLevels = 1;
	d3d11Texture2dDesc.CPUAccessFlags = 0;
	d3d11Texture2dDesc.MiscFlags = 0;

	ID3D11Texture2D* pID3D11Texture2D_depthbuffer = NULL;

	hr = gpID3D11Device->CreateTexture2D(&d3d11Texture2dDesc, NULL, &pID3D11Texture2D_depthbuffer);
	if (FAILED(hr))
	{
		fopen_s(&gpFile, "RenderLog.txt", "a+");
		fprintf(gpFile, ("CreateTexture2D() failed\n\n"));
		fclose(gpFile);
		Uninitialize();
	}
	else
	{
		fopen_s(&gpFile, "RenderLog.txt", "a+");
		fprintf(gpFile, ("CreateTexture2D() successful\n\n"));
		fclose(gpFile);
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC d3d11DepthStencilViewDesc;
	ZeroMemory((void*)&d3d11DepthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	d3d11DepthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	d3d11DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;    //MS: Multi Sample

	hr = gpID3D11Device->CreateDepthStencilView(pID3D11Texture2D_depthbuffer, &d3d11DepthStencilViewDesc, &gpID3D11DepthStencilView);
	if (FAILED(hr))
	{
		fopen_s(&gpFile, "RenderLog.txt", "a+");
		fprintf(gpFile, ("CreateDepthStencilView() failed\n\n"));
		fclose(gpFile);
		Uninitialize();
	}
	else
	{
		fopen_s(&gpFile, "RenderLog.txt", "a+");
		fprintf(gpFile, ("CreateDepthStencilView() successful\n\n"));
		fclose(gpFile);
	}



	gpID3D11DeviceContext->OMSetRenderTargets(1, &gpID3D11RenderTargetView, gpID3D11DepthStencilView);

	D3D11_VIEWPORT d3d11ViewPort;
	ZeroMemory((void*)&d3d11ViewPort, sizeof(D3D11_VIEWPORT));
	d3d11ViewPort.TopLeftX = 0;
	d3d11ViewPort.TopLeftY = 0;
	d3d11ViewPort.Width = (float)width;
	d3d11ViewPort.Height = (float)height;
	d3d11ViewPort.MinDepth = 0.0f;
	d3d11ViewPort.MaxDepth = 1.0f;
	gpID3D11DeviceContext->RSSetViewports(1, &d3d11ViewPort);

	//set perspective matrix

	gPerspectiveProjectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), (float)width / (float)height, 0.1f, 100.0f);

	return(hr);
}

void Display(void)
{
	float r = 5.0f;

	//code	
	gpID3D11DeviceContext->ClearRenderTargetView(gpID3D11RenderTargetView, gClearColor);
	gpID3D11DeviceContext->ClearDepthStencilView(gpID3D11DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);    //ClearDepthStencilView(gpID3D11DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 1); FOR STENCIL

	if (PVLighting == true)
	{
		gpID3D11DeviceContext->VSSetShader(gpID3D11VertexShaderPV, 0, 0);
		gpID3D11DeviceContext->PSSetShader(gpID3D11PixelShaderPV, 0, 0);
		gpID3D11DeviceContext->IASetInputLayout(gpID3D11InputLayoutPV);
		gpID3D11DeviceContext->VSSetConstantBuffers(0, 1, &gpID3D11Buffer_ConstantBufferPV);
	}
	else
	{
		gpID3D11DeviceContext->VSSetShader(gpID3D11VertexShaderPP, 0, 0);
		gpID3D11DeviceContext->PSSetShader(gpID3D11PixelShaderPP, 0, 0);
		gpID3D11DeviceContext->IASetInputLayout(gpID3D11InputLayoutPP);
		gpID3D11DeviceContext->VSSetConstantBuffers(0, 1, &gpID3D11Buffer_ConstantBufferPP);
		gpID3D11DeviceContext->PSSetConstantBuffers(0, 1, &gpID3D11Buffer_ConstantBufferPP);
	}


	//set vertex buffer in pipeline

	/*QUAD*/

	//quad position
	UINT stride = sizeof(float) * 3;
	UINT offset = 0;
	gpID3D11DeviceContext->IASetVertexBuffers(0, 1, &gpID3D11Buffer_VertexBuffer_position_cube, &stride, &offset);

	stride = sizeof(float) * 3;
	offset = 0;
	gpID3D11DeviceContext->IASetVertexBuffers(1, 1, &gpID3D11Buffer_VertexBuffer_normal_cube, &stride, &offset);

	gpID3D11DeviceContext->IASetIndexBuffer(gpID3D11Buffer_IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	//set primitive topology
	gpID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//transformations
	XMMATRIX worldMatrix = XMMatrixIdentity();
	XMMATRIX viewMatrix = XMMatrixIdentity();

	worldMatrix = XMMatrixTranslation(0.0f, 0.0f, 3.0f);

	//push transformation matrices in shader
	CBUFFER constantBuffer;
	//ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));

	constantBuffer.LA[0] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	constantBuffer.LD[0] = XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f);
	constantBuffer.LS[0] = XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f);
	constantBuffer.LightPosition[0] = XMVectorSet(0.0f, r * sin(lightAngle0), -r * cos(lightAngle0), 1.0f);

	constantBuffer.LA[1] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	constantBuffer.LD[1] = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
	constantBuffer.LS[1] = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
	constantBuffer.LightPosition[1] = XMVectorSet(r * sin(lightAngle1), 0.0f, -r * cos(lightAngle1), 1.0f);

	constantBuffer.LA[2] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	constantBuffer.LD[2] = XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);
	constantBuffer.LS[2] = XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);
	constantBuffer.LightPosition[2] = XMVectorSet(r * sin(lightAngle2), r * cos(lightAngle2), 0.0f, 1.0f);

	constantBuffer.KA = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	constantBuffer.KD = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	constantBuffer.KS = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	constantBuffer.MaterialShininess = materialShininess;

	constantBuffer.WorldMatrix = worldMatrix;
	constantBuffer.ViewMatrix = viewMatrix;
	constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

	if (PVLighting == true)
		gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBufferPV, 0, NULL, &constantBuffer, 0, 0);   //glUniformMatrix4fv();
	else
		gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBufferPP, 0, NULL, &constantBuffer, 0, 0);   //glUniformMatrix4fv();
		

	//draw
	gpID3D11DeviceContext->DrawIndexed(numElements, 0, 0);   //0-6, 6-6, 12-6, 18-6, 24-6, 30-6  

	gpIDXGISwapChain->Present(1, 0);
}

void Update(void)
{
	//code
	lightAngle0 += 0.01f;
	if (lightAngle0 >= 360.0f)
	{
		lightAngle0 = 0.0f;
	}

	lightAngle1 += 0.01f;
	if (lightAngle1 >= 360.0f)
	{
		lightAngle1 = 0.0f;
	}

	lightAngle2 += 0.01f;
	if (lightAngle2 >= 360.0f)
	{
		lightAngle2 = 0.0f;
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

	if (gpID3D11DepthStencilView)
	{
		gpID3D11DepthStencilView->Release();
		gpID3D11DepthStencilView = NULL;
	}

	if (gpID3D11RasterizerState)
	{
		gpID3D11RasterizerState->Release();
		gpID3D11RasterizerState = NULL;
	}

	if (gpID3D11Buffer_ConstantBufferPP)
	{
		gpID3D11Buffer_ConstantBufferPP->Release();
		gpID3D11Buffer_ConstantBufferPP = NULL;
	}

	if (gpID3D11Buffer_ConstantBufferPV)
	{
		gpID3D11Buffer_ConstantBufferPV->Release();
		gpID3D11Buffer_ConstantBufferPV = NULL;
	}

	if (gpID3D11InputLayoutPP)
	{
		gpID3D11InputLayoutPP->Release();
		gpID3D11InputLayoutPP = NULL;
	}

	if (gpID3D11InputLayoutPV)
	{
		gpID3D11InputLayoutPV->Release();
		gpID3D11InputLayoutPV = NULL;
	}

	if (gpID3D11Buffer_IndexBuffer)
	{
		gpID3D11Buffer_IndexBuffer->Release();
		gpID3D11Buffer_IndexBuffer = NULL;
	}

	if (gpID3D11Buffer_VertexBuffer_position_cube)
	{
		gpID3D11Buffer_VertexBuffer_position_cube->Release();
		gpID3D11Buffer_VertexBuffer_position_cube = NULL;
	}

	if (gpID3D11Buffer_VertexBuffer_normal_cube)
	{
		gpID3D11Buffer_VertexBuffer_normal_cube->Release();
		gpID3D11Buffer_VertexBuffer_normal_cube = NULL;
	}

	if (gpID3D11PixelShaderPP)
	{
		gpID3D11PixelShaderPP->Release();
		gpID3D11PixelShaderPP = NULL;
	}

	if (gpID3D11VertexShaderPP)
	{
		gpID3D11VertexShaderPP->Release();
		gpID3D11VertexShaderPP = NULL;
	}

	if (gpID3D11PixelShaderPV)
	{
		gpID3D11PixelShaderPV->Release();
		gpID3D11PixelShaderPV = NULL;
	}

	if (gpID3D11VertexShaderPV)
	{
		gpID3D11VertexShaderPV->Release();
		gpID3D11VertexShaderPV = NULL;
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
