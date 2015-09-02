// D3D LAB 1a "Line Land".
// Author: L.Norri CD DRX, FullSail University

// Introduction:
// Welcome to the first lab of the Direct3D Graphics Programming class.
// This is the ONLY guided lab in this course! 
// Future labs will demand the habits & foundation you develop right now!  
// It is CRITICAL that you follow each and every step. ESPECIALLY THE READING!!!

// TO BEGIN: Open the word document that acompanies this lab and start from the top.

//************************************************************
//************ INCLUDES & DEFINES ****************************
//************************************************************

#include <iostream>
#include <windows.h>
#include <ctime>
#include "XTime.h"
#include "Trivial_VS.csh"
#include "Trivial_PS.csh"
#include "Transform_VS.csh"
#include "Transform_PS.csh"
#include "Cube.h"
#include "Tron.h"

using namespace std;
// BEGIN PART 1
// TODO: PART 1 STEP 1a
#include <d3d11.h>
#pragma comment (lib, "d3d11.lib")
#include <atlcomcli.h>
// TODO: PART 1 STEP 1b
//#include "MathLib.h"
#include "Create_D3Object.h"
// TODO: PART 2 STEP 6

#define BACKBUFFER_WIDTH	500
#define BACKBUFFER_HEIGHT	500


//************************************************************
//************ SIMPLE WINDOWS APP CLASS **********************
//************************************************************

class DEMO_APP
{
	HINSTANCE						application;
	WNDPROC							appWndProc;
	HWND							window;

	// TODO: PART 1 STEP 2
	IDXGISwapChain *I_SwapChain = nullptr;//Responsible for presenting, bliting//cleared
	ID3D11Device  *I_Device = nullptr;//Use to create//cleared
	ID3D11DeviceContext *I_Context = nullptr;//Use to bind pipeline objects.//cleared
	ID3D11RenderTargetView *I_RenderTargetView = nullptr;//cleared
	

	ID3D11Buffer *ConstantBuffer = nullptr;//cleared
	ID3D11Buffer *WorldConstantBuffer = nullptr;//cleared
	ID3D11Buffer *SceneConstantBuffer = nullptr;//cleared

	XTime TotalTimeLoop;

	D3D11_VIEWPORT ViewPort;


	XMMATRIX SV_WorldMatrix;
	XMMATRIX SV_ViewMatrix;
	XMMATRIX SV_Projection;

	Create_D3Object Cube_Object;


	struct SEND_TO_VRAM
	{
		float constantColor[4];
		float constantOffset[2];
		float padding[2];
	};
	SEND_TO_VRAM ToShader;



	XMMATRIX viewMatrix;
	XMMATRIX projectionMatrix;
	XMMATRIX WorldMatrixObject;


	ID3D11Texture2D *DepthBuffer = nullptr;//cleared
	ID3D11Texture2D *TextureBuffer = nullptr;//cleared
	ID3D11DepthStencilView *DepthStencilView = nullptr;//cleared

	//Camera
	float MoveCameraZ = -1.0f;
	float MoveCameraY = 0.0f;
	float MoveCameraX = 0.0f;
	POINT currPos, PrevPos;

public:
	// BEGIN PART 2
	// TODO: PART 2 STEP 1

	DEMO_APP(HINSTANCE hinst, WNDPROC proc);
	bool Run();
	bool ShutDown();
	void MoveCamera(float MoveSpeed);
};

void DEMO_APP::MoveCamera(float MoveSpeed)
{
	//camera


	//Get input from user for Z axis
	if (GetAsyncKeyState(0x57) && 0x1)
	{

		XMMATRIX trans = XMMatrixTranslation(0.0f, 0.0f, 0.0001f * MoveSpeed);
		viewMatrix = SV_ViewMatrix = XMMatrixMultiply(trans, SV_ViewMatrix);
	}
	if (GetAsyncKeyState(0x53) && 0x1)
	{
		XMMATRIX trans = XMMatrixTranslation(0.0f, 0.0f, -0.0001f* MoveSpeed);
		viewMatrix = SV_ViewMatrix = XMMatrixMultiply(trans, SV_ViewMatrix);
	}
	//Get input from user for X axis
	if (GetAsyncKeyState(0x41) && 0x1)
	{
		XMMATRIX trans = XMMatrixTranslation(-0.0001f* MoveSpeed, 0.0f, 0.0f);
		viewMatrix = SV_ViewMatrix = XMMatrixMultiply(trans, SV_ViewMatrix);

	}
	if (GetAsyncKeyState(0x44) && 0x1)
	{
		XMMATRIX trans = XMMatrixTranslation(0.0001f* MoveSpeed, 0.0f, 0.0f);
		viewMatrix = SV_ViewMatrix = XMMatrixMultiply(trans, SV_ViewMatrix);

	}
	//Get input from user for Y axis
	if (GetAsyncKeyState(0x51) && 0x1)
	{
		XMMATRIX trans = XMMatrixTranslation(0.0f, -0.0001f* MoveSpeed, 0.0f);
		viewMatrix = SV_ViewMatrix = XMMatrixMultiply(trans, SV_ViewMatrix);

	}
	if (GetAsyncKeyState(0x45) && 0x1)
	{
		XMMATRIX trans = XMMatrixTranslation(0.0f, 0.0001f* MoveSpeed, 0.0f);
		viewMatrix = SV_ViewMatrix = XMMatrixMultiply(trans, SV_ViewMatrix);
	}



}


//************************************************************
//************ CREATION OF OBJECTS & RESOURCES ***************
//************************************************************

DEMO_APP::DEMO_APP(HINSTANCE hinst, WNDPROC proc)
{
	// ****************** BEGIN WARNING ***********************// 
	// WINDOWS CODE, I DON'T TEACH THIS YOU MUST KNOW IT ALREADY! 
	application = hinst;
	appWndProc = proc;

	WNDCLASSEX  wndClass;
	ZeroMemory(&wndClass, sizeof(wndClass));
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.lpfnWndProc = appWndProc;
	wndClass.lpszClassName = L"DirectXApplication";
	wndClass.hInstance = application;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOWFRAME);
	//wndClass.hIcon			= LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_FSICON));
	RegisterClassEx(&wndClass);

	RECT window_size = { 0, 0, BACKBUFFER_WIDTH, BACKBUFFER_HEIGHT };
	AdjustWindowRect(&window_size, WS_OVERLAPPEDWINDOW, false);

	window = CreateWindow(L"DirectXApplication", L"Lab 1a Line Land", WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MAXIMIZEBOX),
		CW_USEDEFAULT, CW_USEDEFAULT, window_size.right - window_size.left, window_size.bottom - window_size.top,
		NULL, NULL, application, this);

	ShowWindow(window, SW_SHOW);
	//********************* END WARNING ************************//

#pragma region SwapChain and ViewPort

	DXGI_SWAP_CHAIN_DESC des;//research
	ZeroMemory(&des, sizeof(des));
	des.BufferCount = 1;
	des.BufferDesc.Width = BACKBUFFER_WIDTH;
	des.BufferDesc.Height = BACKBUFFER_HEIGHT;
	des.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	des.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	des.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	des.OutputWindow = window;
	des.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	des.Windowed = true;
	des.SampleDesc.Count = 1;

	HRESULT hr;

	hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_DEBUG, nullptr,
		0, D3D11_SDK_VERSION, &des, &I_SwapChain, &I_Device, nullptr, &I_Context);

	I_SwapChain->GetDesc(&des);


	//viewport
	ViewPort.MinDepth = 0;
	ViewPort.MaxDepth = 1;
	ViewPort.Height = (FLOAT)(des.BufferDesc.Width);
	ViewPort.Width = (FLOAT)(des.BufferDesc.Height);
	ViewPort.TopLeftX = 0;
	ViewPort.TopLeftY = 0;


	ID3D11Texture2D *My_Texture2D;

	// TODO: PART 1 STEP 3b
	hr = I_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&My_Texture2D);
	hr = I_Device->CreateRenderTargetView(My_Texture2D, NULL, &I_RenderTargetView);
	My_Texture2D->Release();

#pragma endregion

#pragma region Cube
	// Fill in a buffer description.
	D3D11_BUFFER_DESC CubeBufferDesc;
	CubeBufferDesc.ByteWidth = sizeof(Cube_data);
	CubeBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	CubeBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	CubeBufferDesc.CPUAccessFlags = NULL;
	CubeBufferDesc.MiscFlags = 0;
	CubeBufferDesc.StructureByteStride = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA CubeSubResourceData;
	CubeSubResourceData.pSysMem = Cube_data;
	CubeSubResourceData.SysMemPitch = 0;
	CubeSubResourceData.SysMemSlicePitch = 0;


	hr = I_Device->CreateBuffer(&CubeBufferDesc, &CubeSubResourceData, &Cube_Object.ConstantBuffer); // putting cube buffer into Device

	// Fill in a buffer description.
	D3D11_BUFFER_DESC CubeIndexBufferDesc;
	CubeIndexBufferDesc.ByteWidth = sizeof(Cube_indicies);
	CubeIndexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	CubeIndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	CubeIndexBufferDesc.CPUAccessFlags = NULL;
	CubeIndexBufferDesc.MiscFlags = 0;
	CubeIndexBufferDesc.StructureByteStride = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA CubeIndexSubResourceData;
	CubeIndexSubResourceData.pSysMem = Cube_indicies;
	CubeIndexSubResourceData.SysMemPitch = 0;
	CubeIndexSubResourceData.SysMemSlicePitch = 0;

	//unsigned int NumOIndexVerts = 1692;

	hr = I_Device->CreateBuffer(&CubeIndexBufferDesc, &CubeIndexSubResourceData, &Cube_Object.IndexBuffer);

	//depth buffer desc
	D3D11_TEXTURE2D_DESC Texture2DDesc;
	Texture2DDesc.Width = Tron_width;
	Texture2DDesc.Height = Tron_height;
	Texture2DDesc.MipLevels = Tron_numlevels;
	Texture2DDesc.ArraySize = 1;
	Texture2DDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	Texture2DDesc.SampleDesc.Count = 1;
	Texture2DDesc.SampleDesc.Quality = 0;
	Texture2DDesc.Usage = D3D11_USAGE_DEFAULT;
	Texture2DDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	Texture2DDesc.CPUAccessFlags = 0;
	Texture2DDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA Tron_Cube[Tron_numlevels];
	ZeroMemory(Tron_Cube, sizeof(Tron_Cube));

	for (size_t i = 0; i < Tron_numlevels; i++)
	{
		Tron_Cube[i].pSysMem = &Tron_pixels[Tron_leveloffsets[i]];
		Tron_Cube[i].SysMemPitch = (Tron_width >> i) * sizeof(unsigned int);
	}

	hr = I_Device->CreateTexture2D(&Texture2DDesc, Tron_Cube, &Cube_Object.Texture2DBuffer);

	//ID3D11ShaderResourceView // Ask Jon
	I_Device->CreateShaderResourceView(Cube_Object.Texture2DBuffer, NULL, &Cube_Object.ShaderResourceView);

#pragma endregion

#pragma region Vertex/Pixel for Cube Buffer
	// create input layout describing our geometry
	D3D11_INPUT_ELEMENT_DESC Cube_Layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	I_Device->CreateInputLayout(Cube_Layout, 3, Transform_VS, sizeof(Transform_VS), &Cube_Object.InputLayout);

	I_Device->CreateVertexShader(Transform_VS, sizeof(Transform_VS), nullptr, &Cube_Object.VertexShader);
	I_Device->CreatePixelShader(Transform_PS, sizeof(Transform_PS), nullptr, &Cube_Object.PixelShader);



#pragma endregion


#pragma region DepthBuffer
	D3D11_TEXTURE2D_DESC deDepth;
	deDepth.Width = BACKBUFFER_WIDTH;
	deDepth.Height = BACKBUFFER_HEIGHT;
	deDepth.MipLevels = 1;
	deDepth.ArraySize = 1;
	deDepth.Format = DXGI_FORMAT_D32_FLOAT;
	deDepth.SampleDesc.Count = 1;
	deDepth.SampleDesc.Quality = 0;
	deDepth.Usage = D3D11_USAGE_DEFAULT;
	deDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	deDepth.CPUAccessFlags = 0;
	deDepth.MiscFlags = 0;

	hr = I_Device->CreateTexture2D(&deDepth, NULL, &DepthBuffer);

	//stencil desc
	D3D11_DEPTH_STENCIL_DESC dsDesc;
	ZeroMemory(&dsDesc, sizeof(dsDesc));

	// Depth test parameters
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

	// Stencil test parameters
	dsDesc.StencilEnable = true;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create depth stencil state
	ID3D11DepthStencilState *pDSState;

	I_Device->CreateDepthStencilState(&dsDesc, &pDSState);

	pDSState->Release();

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));

	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;

	// Create the depth stencil view
	hr = I_Device->CreateDepthStencilView(DepthBuffer,&descDSV,&DepthStencilView);  // [out] Depth stencil view

#pragma endregion

#pragma region Constant Buffer
	//Const Buffer
	// Fill in a buffer description.
	D3D11_BUFFER_DESC ConstantBufferDesc;
	ConstantBufferDesc.ByteWidth = sizeof(SEND_TO_VRAM);
	ConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	ConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	ConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	ConstantBufferDesc.MiscFlags = 0;
	ConstantBufferDesc.StructureByteStride = 0;

	hr = I_Device->CreateBuffer(&ConstantBufferDesc, nullptr, &ConstantBuffer);

#pragma endregion

#pragma region World Constant Buffer
	///World
	// Fill in a buffer description.
	D3D11_BUFFER_DESC WorldConstantBufferDesc;
	WorldConstantBufferDesc.ByteWidth = sizeof(XMMATRIX);
	WorldConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	WorldConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	WorldConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	WorldConstantBufferDesc.MiscFlags = 0;
	WorldConstantBufferDesc.StructureByteStride = 0;


	hr = I_Device->CreateBuffer(&WorldConstantBufferDesc, nullptr, &WorldConstantBuffer);

#pragma endregion

#pragma region Scene Constant Buffer
	//Scene
	// Fill in a buffer description.
	D3D11_BUFFER_DESC SceneConstantBufferDesc;

	SceneConstantBufferDesc.ByteWidth = sizeof(XMMATRIX);
	SceneConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	SceneConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	SceneConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	SceneConstantBufferDesc.MiscFlags = 0;
	SceneConstantBufferDesc.StructureByteStride = 0;


	hr = I_Device->CreateBuffer(&SceneConstantBufferDesc, nullptr, &SceneConstantBuffer);

#pragma endregion

	TotalTimeLoop.Restart();

}

//************************************************************
//************ EXECUTION *************************************
//************************************************************

bool DEMO_APP::Run()
{


	TotalTimeLoop.Signal();

#pragma region Camera Movement
	GetCursorPos(&currPos);
	MoveCamera(10);
	//Translation & Rotation
	if (GetAsyncKeyState(VK_RBUTTON))
	{
		float Dx = (FLOAT)(currPos.x - PrevPos.x);
		float Dy = (FLOAT)(currPos.y - PrevPos.y);

		XMMATRIX x_rot = XMMatrixIdentity();
		XMMATRIX y_rot = XMMatrixIdentity();
		y_rot = XMMatrixRotationY(Dx * (FLOAT)(TotalTimeLoop.Delta()) * 10);
		x_rot = XMMatrixRotationX(Dy * (FLOAT)(TotalTimeLoop.Delta()) * 10);

		XMVECTOR SavePositon = { SV_ViewMatrix.r[3].m128_f32[0], SV_ViewMatrix.r[3].m128_f32[1], SV_ViewMatrix.r[3].m128_f32[2] };

		//XMMATRIX tempMatrix;
		//	tempMatrix.r[3].m128_f32[0];  // x of the 4th row // position row

		SV_ViewMatrix.r[3].m128_f32[0] = 0; //x
		SV_ViewMatrix.r[3].m128_f32[1] = 0; //y
		SV_ViewMatrix.r[3].m128_f32[2] = 0; //z

		SV_ViewMatrix = XMMatrixMultiply(x_rot, SV_ViewMatrix);
		SV_ViewMatrix = XMMatrixMultiply(SV_ViewMatrix, y_rot);

		SV_ViewMatrix.r[3].m128_f32[0] = SavePositon.m128_f32[0];
		SV_ViewMatrix.r[3].m128_f32[1] = SavePositon.m128_f32[1];
		SV_ViewMatrix.r[3].m128_f32[2] = SavePositon.m128_f32[2];
	}

	viewMatrix = SV_ViewMatrix;
	viewMatrix = XMMatrixInverse(nullptr,viewMatrix);//inverse
	SV_Projection = XMMatrixIdentity();

	FLOAT ZNear = 0.1f;
	FLOAT ZFar = 100.0f;
	FLOAT FOV = Degrees_To_Radians(75);
	FLOAT AspectRatio = BACKBUFFER_WIDTH / BACKBUFFER_HEIGHT;
	FLOAT Yscale = FLOAT(1 / tan(FOV * 0.5f));
	FLOAT Xscale = Yscale * AspectRatio;

	XMMATRIX PerspectiveProjection = {
		Xscale, 0, 0, 0,
		0, Yscale, 0, 0,
		0, 0, (ZFar / (ZFar - ZNear)), 1,
		0, 0, (-(ZFar * ZNear) / (ZFar - ZNear)), 0,
	};

	projectionMatrix = SV_Projection = PerspectiveProjection;
	
	PrevPos = currPos;

#pragma endregion



#pragma region ViewPorts
	//How to change view ports.
	I_Context->OMSetRenderTargets(1, &I_RenderTargetView, DepthStencilView);

	I_Context->RSSetViewports(1, &ViewPort);


	float ColorRGBA[4] = { 0, 0, 1, 1 };
	
	I_Context->ClearRenderTargetView(I_RenderTargetView, ColorRGBA);
	I_Context->ClearDepthStencilView(DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
#pragma endregion

#pragma region Cube Draw New

	Cube_Object.SetWorldMatrix(XMMatrixMultiply(Cube_Object.WorldMatrix, XMMatrixTranslation(2.0f, 2.0f, 1.0f)));

	WorldMatrixObject = Cube_Object.WorldMatrix = XMMatrixMultiply(XMMatrixRotationY((FLOAT)(TotalTimeLoop.TotalTime())), Cube_Object.WorldMatrix);

	// Set vertex buffer
	UINT stride = sizeof(_OBJ_VERT_);
	UINT offset = 0;
	I_Context->IASetVertexBuffers(0, 1, &Cube_Object.ConstantBuffer, &stride, &offset);

	I_Context->IASetIndexBuffer(Cube_Object.IndexBuffer, DXGI_FORMAT_R32_UINT, offset);

	I_Context->VSSetShader(Cube_Object.VertexShader, NULL, 0);

	I_Context->PSSetShader(Cube_Object.PixelShader, NULL, 0);

	I_Context->PSSetShaderResources(0, 1, &Cube_Object.ShaderResourceView);

	I_Context->IASetInputLayout(Cube_Object.InputLayout);


	I_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);



	//mapping local to constantbuffer
	D3D11_MAPPED_SUBRESOURCE Local;
	I_Context->Map(ConstantBuffer, NULL, D3D11_MAP_WRITE_DISCARD, 0, &Local);

	memcpy(Local.pData, &ToShader, sizeof(SEND_TO_VRAM));

	I_Context->Unmap(ConstantBuffer, 0);


	//World Constant buffer mapping
	I_Context->Map(WorldConstantBuffer, NULL, D3D11_MAP_WRITE_DISCARD, 0, &Local);

	memcpy(Local.pData, &WorldMatrixObject, sizeof(XMMATRIX));

	I_Context->Unmap(WorldConstantBuffer, 0);


	//Scene Constant buffer mapping
	I_Context->Map(SceneConstantBuffer, NULL, D3D11_MAP_WRITE_DISCARD, 0, &Local);

	memcpy(Local.pData, &SceneConstantBuffer, sizeof(XMMATRIX));

	I_Context->Unmap(SceneConstantBuffer, 0);



	I_Context->VSSetConstantBuffers(0, 1, &WorldConstantBuffer);
	I_Context->VSSetConstantBuffers(1, 1, &SceneConstantBuffer);

	I_Context->DrawIndexed(1692, 0, 0);// drawing cube

#pragma endregion
	

	I_SwapChain->Present(0, 0);
	return true;
}

//************************************************************
//************ DESTRUCTION ***********************************
//************************************************************

bool DEMO_APP::ShutDown()
{
	// TODO: PART 1 STEP 6
	SAFE_RELEASE(DepthStencilView);//cleared
	SAFE_RELEASE(ConstantBuffer);//cleared
	SAFE_RELEASE(I_Context);//cleared
	SAFE_RELEASE(I_Device);//cleared
	SAFE_RELEASE(I_SwapChain);//cleared
	SAFE_RELEASE(I_RenderTargetView);//cleared
	SAFE_RELEASE(SceneConstantBuffer);//cleared
	SAFE_RELEASE(WorldConstantBuffer);//cleared
	SAFE_RELEASE(TextureBuffer);//cleared
	SAFE_RELEASE(DepthBuffer);//cleared

	UnregisterClass(L"DirectXApplication", application);
	return true;
}

//************************************************************
//************ WINDOWS RELATED *******************************
//************************************************************

// ****************** BEGIN WARNING ***********************// 
// WINDOWS CODE, I DON'T TEACH THIS YOU MUST KNOW IT ALREADY!

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wparam, LPARAM lparam);
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR, int)
{
	srand(unsigned int(time(0)));
	DEMO_APP myApp(hInstance, (WNDPROC)WndProc);
	MSG msg; ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT && myApp.Run())
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	myApp.ShutDown();
	return 0;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (GetAsyncKeyState(VK_ESCAPE))
		message = WM_DESTROY;
	switch (message)
	{
	case (WM_DESTROY) : { PostQuitMessage(0); }
		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}
//********************* END WARNING ************************//