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
#include "DirectionalLight_PS.csh"
#include "SpotLight.csh"
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

	D3D11_VIEWPORT ViewPort[2];

	XMMATRIX SV_ViewMatrix;
	XMMATRIX SV_ViewMatrix_2;
	XMMATRIX SV_Projection;



	struct CONSTANTSCENE
	{
		XMMATRIX viewMatrix;
		XMMATRIX projectionMatrix;
	};


	CONSTANTSCENE ConstantScene;
	XMMATRIX WorldMatrixObject;


	ID3D11Texture2D *DepthBuffer = nullptr;//cleared
	ID3D11Texture2D *TextureBuffer = nullptr;//cleared
	ID3D11DepthStencilView *DepthStencilView = nullptr;//cleared


	ID3D11Buffer *AmbientLightBuffer = nullptr;//cleared

	ID3D11Buffer *DirectionalLightBuffer = nullptr;//Cleared

	ID3D11Buffer *SpotLightBuffer = nullptr;//Cleared


	struct DLIGHT
	{
		XMFLOAT4 Col;
		XMFLOAT4 Direction;
	};

	DLIGHT Dlight;

	struct SLIGHT
	{
		XMFLOAT4 Col;
		XMFLOAT3 Dir;
		float MotherOfOne;//padding
		XMFLOAT4 Pos;
		float Cratio;
		XMFLOAT3 MotherOfThree;//padding
	};

	SLIGHT SLight;

	ID3D11BlendState *BlendState = nullptr;//cleared
	ID3D11BlendState *BlendStateNULL = nullptr;//cleared
	ID3D11SamplerState *SamplerState = nullptr;//Cleared
	ID3D11DepthStencilState *pDSState = nullptr;//cleared

	struct SEND_TO_VRAM
	{
		float constantColor[4];
		float constantOffset[2];
		float padding[2];
	};
	SEND_TO_VRAM ToShader;



	//Camera
	float MoveCameraZ = -1.0f;
	float MoveCameraY = 0.0f;
	float MoveCameraX = 0.0f;
	POINT currPos, PrevPos;

#pragma region Objects
	Create_D3Object Cube_Object;
	Create_D3Object Star_Object;
	Create_D3Object Model_Object;
	Create_D3Object Plane_Object;

#pragma endregion
public:

	DEMO_APP(HINSTANCE hinst, WNDPROC proc);
	bool Run();
	bool ShutDown();
	void MoveCamera(float MoveSpeed, XMMATRIX &ViewMatrix);
};

void DEMO_APP::MoveCamera(float MoveSpeed, XMMATRIX &ViewMatrix)
{
	//camera

	ViewMatrix = XMMatrixInverse(nullptr, ViewMatrix);
	//Get input from user for Z axis
	if (GetAsyncKeyState(0x57) && 0x1)
	{

		XMMATRIX trans = XMMatrixTranslation(0.0f, 0.0f, 0.0001f * MoveSpeed);
		ViewMatrix = XMMatrixMultiply(trans, ViewMatrix);
	}
	if (GetAsyncKeyState(0x53) && 0x1)
	{
		XMMATRIX trans = XMMatrixTranslation(0.0f, 0.0f, -0.0001f* MoveSpeed);
		ViewMatrix = XMMatrixMultiply(trans, ViewMatrix);
	}
	//Get input from user for X axis
	if (GetAsyncKeyState(0x41) && 0x1)
	{
		XMMATRIX trans = XMMatrixTranslation(-0.0001f* MoveSpeed, 0.0f, 0.0f);
		ViewMatrix = XMMatrixMultiply(trans, ViewMatrix);

	}
	if (GetAsyncKeyState(0x44) && 0x1)
	{
		XMMATRIX trans = XMMatrixTranslation(0.0001f* MoveSpeed, 0.0f, 0.0f);
		ViewMatrix = XMMatrixMultiply(trans, ViewMatrix);

	}
	//Get input from user for Y axis
	if (GetAsyncKeyState(0x51) && 0x1)
	{
		XMMATRIX trans = XMMatrixTranslation(0.0f, -0.0001f* MoveSpeed, 0.0f);
		ViewMatrix = XMMatrixMultiply(trans, ViewMatrix);

	}
	if (GetAsyncKeyState(0x45) && 0x1)
	{
		XMMATRIX trans = XMMatrixTranslation(0.0f, 0.0001f* MoveSpeed, 0.0f);
		ViewMatrix = XMMatrixMultiply(trans, ViewMatrix);
	}

	//Translation & Rotation
	if (GetAsyncKeyState(VK_RBUTTON))
	{
		//	ViewMatrix = XMMatrixInverse(nullptr, ViewMatrix);
		float Dx = (FLOAT)(currPos.x - PrevPos.x);
		float Dy = (FLOAT)(currPos.y - PrevPos.y);

		XMMATRIX x_rot = XMMatrixIdentity();
		XMMATRIX y_rot = XMMatrixIdentity();
		y_rot = XMMatrixRotationY(Dx * (FLOAT)(TotalTimeLoop.Delta()) * 10);
		x_rot = XMMatrixRotationX(Dy * (FLOAT)(TotalTimeLoop.Delta()) * 10);

		XMVECTOR SavePositon = { ViewMatrix.r[3].m128_f32[0], ViewMatrix.r[3].m128_f32[1], ViewMatrix.r[3].m128_f32[2] };

		//XMMATRIX tempMatrix;
		//	tempMatrix.r[3].m128_f32[0];  // x of the 4th row //

		ViewMatrix.r[3].m128_f32[0] = 0; //x
		ViewMatrix.r[3].m128_f32[1] = 0; //y
		ViewMatrix.r[3].m128_f32[2] = 0; //z

		ViewMatrix = XMMatrixMultiply(x_rot, ViewMatrix);
		ViewMatrix = XMMatrixMultiply(ViewMatrix, y_rot);

		ViewMatrix.r[3].m128_f32[0] = SavePositon.m128_f32[0];
		ViewMatrix.r[3].m128_f32[1] = SavePositon.m128_f32[1];
		ViewMatrix.r[3].m128_f32[2] = SavePositon.m128_f32[2];

		//ViewMatrix = XMMatrixInverse(nullptr, ViewMatrix);
	}


	ViewMatrix = XMMatrixInverse(nullptr, ViewMatrix);
}


//************************************************************
//************ CREATION OF OBJECTS & RESOURCES ***************
//************************************************************

DEMO_APP::DEMO_APP(HINSTANCE hinst, WNDPROC proc)
{
#pragma region WINDOWS CODE
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
#pragma endregion

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
	ViewPort[0].MinDepth = 0;
	ViewPort[0].MaxDepth = 1;
	ViewPort[0].Height = (FLOAT)(des.BufferDesc.Height / 2);
	ViewPort[0].Width = (FLOAT)(des.BufferDesc.Width);
	ViewPort[0].TopLeftX = 0;
	ViewPort[0].TopLeftY = 0;


	ViewPort[1].MinDepth = 0;
	ViewPort[1].MaxDepth = 1;
	ViewPort[1].Height = (FLOAT)(des.BufferDesc.Height / 2);
	ViewPort[1].Width = (FLOAT)(des.BufferDesc.Width);
	ViewPort[1].TopLeftX = 0;
	ViewPort[1].TopLeftY = (FLOAT)(des.BufferDesc.Height / 2);

	ID3D11Texture2D *My_Texture2D;

	// TODO: PART 1 STEP 3b
	hr = I_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&My_Texture2D);
	hr = I_Device->CreateRenderTargetView(My_Texture2D, NULL, &I_RenderTargetView);
	My_Texture2D->Release();

	XMVECTOR CameraPOS = { 0.0f, 0.0f, -1.0f };
	XMVECTOR CameraPOSLookdirection = { 0.0f, 0.0f, 0.0f };
	XMVECTOR CameraPOSUP = { 0.0f, 1.0f, 0.0f };
	SV_ViewMatrix = XMMatrixLookAtLH(CameraPOS, CameraPOSLookdirection, CameraPOSUP);
	SV_ViewMatrix_2 = XMMatrixLookAtLH(CameraPOS, CameraPOSLookdirection, CameraPOSUP);

#pragma endregion

#pragma region Create and Set Tron Cube

#pragma region Create Cube(tron)
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

	//ID3D11ShaderResourceView // ask about null. // samplerstate?

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

	I_Device->CreatePixelShader(Transform_PS, sizeof(Transform_PS), nullptr, &Cube_Object.PixelShader[0]);

	I_Device->CreatePixelShader(DirectionalLight_PS, sizeof(DirectionalLight_PS), nullptr, &Cube_Object.PixelShader[1]);

	I_Device->CreatePixelShader(SpotLight, sizeof(SpotLight), nullptr, &Cube_Object.PixelShader[2]);




#pragma endregion

#pragma endregion

#pragma region Create and Set Star
#pragma region Create Star
	VERTEX4 geometry[12];
	for (int i = 1; i < 11; i++)
	{
		if (i == 1 || i == 3 || i == 5 || i == 7 || i == 9)
		{
			geometry[i].xyzw[0] = sin(Degrees_To_Radians(i * 36.0f)) * .5f;
			geometry[i].xyzw[1] = cos(Degrees_To_Radians(i * 36.0f)) * .5f;
			geometry[i].RGBA[0] = 1.0f;
			geometry[i].RGBA[1] = 0.0f;
			geometry[i].RGBA[2] = 0.0f;
		}
		else
		{
			geometry[i].xyzw[0] = sin(Degrees_To_Radians(i * 36.0f));
			geometry[i].xyzw[1] = cos(Degrees_To_Radians(i * 36.0f));
			geometry[i].RGBA[0] = 0.0f;
			geometry[i].RGBA[1] = 1.0f;
			geometry[i].RGBA[2] = 1.0f;
		}
		geometry[i].xyzw[2] = 0.0f;
	}
	geometry[0].xyzw[0] = 0;
	geometry[0].xyzw[1] = 0;
	geometry[0].xyzw[2] = -.5f;
	geometry[0].RGBA[0] = 0.0f;
	geometry[0].RGBA[1] = 1.0f;
	geometry[0].RGBA[2] = 0.0f;



	geometry[11].xyzw[0] = 0;
	geometry[11].xyzw[1] = 0;
	geometry[11].xyzw[2] = .5f;
	geometry[11].RGBA[0] = 0.0f;
	geometry[11].RGBA[1] = 1.0f;
	geometry[11].RGBA[2] = 0.0f;


	UINT Index_Buffer[60] = { 0, 1, 2,
		0, 2, 3,
		0, 3, 4,
		0, 4, 5,
		0, 5, 6,
		0, 6, 7,
		0, 7, 8,
		0, 8, 9,
		0, 9, 10,
		0, 10, 1,
		11, 1, 10,
		11, 10, 9,
		11, 9, 8,
		11, 8, 7,
		11, 7, 6,
		11, 6, 5,
		11, 5, 4,
		11, 4, 3,
		11, 3, 2,
		11, 2, 1 };

	for (size_t i = 0; i < 12; i++)
	{
		geometry[i].nrm[0] = 0;
		geometry[i].nrm[1] = 0;
		geometry[i].nrm[2] = 0;
	}



	// Fill in a buffer description.
	D3D11_BUFFER_DESC StarDesc;
	StarDesc.ByteWidth = sizeof(geometry);
	StarDesc.Usage = D3D11_USAGE_IMMUTABLE;
	StarDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	StarDesc.CPUAccessFlags = NULL;
	StarDesc.MiscFlags = 0;
	StarDesc.StructureByteStride = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA Star_SubresourceData;
	Star_SubresourceData.pSysMem = geometry;
	Star_SubresourceData.SysMemPitch = 0;
	Star_SubresourceData.SysMemSlicePitch = 0;

	unsigned int NumberOfVerts = 12;

	hr = I_Device->CreateBuffer(&StarDesc, &Star_SubresourceData, &Star_Object.ConstantBuffer);

	// Fill in a buffer description.
	D3D11_BUFFER_DESC StarIndexBufferDesc;
	StarIndexBufferDesc.ByteWidth = sizeof(Index_Buffer);
	StarIndexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	StarIndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	StarIndexBufferDesc.CPUAccessFlags = NULL;
	StarIndexBufferDesc.MiscFlags = 0;
	StarIndexBufferDesc.StructureByteStride = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA Star_Index_Subresource_data;
	Star_Index_Subresource_data.pSysMem = Index_Buffer;
	Star_Index_Subresource_data.SysMemPitch = 0;
	Star_Index_Subresource_data.SysMemSlicePitch = 0;

	unsigned int NumOIndexVerts = 11;

	hr = I_Device->CreateBuffer(&StarIndexBufferDesc, &Star_Index_Subresource_data, &Star_Object.IndexBuffer);


#pragma endregion

#pragma region Vertex/Pixel shader and layout

	D3D11_INPUT_ELEMENT_DESC Star_Layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	I_Device->CreateVertexShader(Transform_VS, sizeof(Transform_VS), nullptr, &Star_Object.VertexShader);

	I_Device->CreatePixelShader(Transform_PS, sizeof(Transform_PS), nullptr, &Star_Object.PixelShader[0]);


	I_Device->CreatePixelShader(DirectionalLight_PS, sizeof(DirectionalLight_PS), nullptr, &Star_Object.PixelShader[1]);
	I_Device->CreatePixelShader(SpotLight, sizeof(SpotLight), nullptr, &Star_Object.PixelShader[2]);



	I_Device->CreateInputLayout(Star_Layout, 3, Transform_VS, sizeof(Transform_VS), &Star_Object.InputLayout);
#pragma endregion

#pragma endregion

#pragma region Create and Set Model(Loaded)
#pragma region Create Model
	vector<XMFLOAT3> Model_Vertices;
	vector<XMFLOAT2> Model_UVS;
	vector<XMFLOAT3> Model_Normals;
	Model_Object.loadOBJ("tri.obj", Model_Vertices, Model_UVS, Model_Normals);
	SIMPLE_VERTEX Model_Pyramid[36];
	for (size_t i = 0; i < 36; i++)
	{
		Model_Pyramid[i].Verts.x = Model_Vertices[i].x;
		Model_Pyramid[i].Verts.y = Model_Vertices[i].y;
		Model_Pyramid[i].Verts.z = Model_Vertices[i].z;
		Model_Pyramid[i].UV.x = Model_UVS[i].x;
		Model_Pyramid[i].UV.y = Model_UVS[i].y;
		Model_Pyramid[i].NRM.x = Model_Normals[i].x;
		Model_Pyramid[i].NRM.y = Model_Normals[i].y;
		Model_Pyramid[i].NRM.z = Model_Normals[i].z;
	}

	// Fill in a buffer description.
	D3D11_BUFFER_DESC ModelDesc;
	ModelDesc.ByteWidth = sizeof(SIMPLE_VERTEX) * 36;
	ModelDesc.Usage = D3D11_USAGE_IMMUTABLE;
	ModelDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	ModelDesc.CPUAccessFlags = NULL;
	ModelDesc.MiscFlags = 0;
	ModelDesc.StructureByteStride = sizeof(SIMPLE_VERTEX);

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA Model_SubresourceData;
	Model_SubresourceData.pSysMem = Model_Pyramid;
	Model_SubresourceData.SysMemPitch = 0;
	Model_SubresourceData.SysMemSlicePitch = 0;


	hr = I_Device->CreateBuffer(&ModelDesc, &Model_SubresourceData, &Model_Object.ConstantBuffer);




#pragma endregion
#pragma region Vertex/Pixel shader and layout
	D3D11_INPUT_ELEMENT_DESC Model_Layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	I_Device->CreateVertexShader(Transform_VS, sizeof(Transform_VS), nullptr, &Model_Object.VertexShader);

	I_Device->CreatePixelShader(Transform_PS, sizeof(Transform_PS), nullptr, &Model_Object.PixelShader[0]);


	I_Device->CreatePixelShader(DirectionalLight_PS, sizeof(DirectionalLight_PS), nullptr, &Model_Object.PixelShader[1]);

	I_Device->CreatePixelShader(SpotLight, sizeof(SpotLight), nullptr, &Model_Object.PixelShader[2]);


	I_Device->CreateInputLayout(Model_Layout, 3, Transform_VS, sizeof(Transform_VS), &Model_Object.InputLayout);

#pragma endregion
#pragma endregion

#pragma region Create and Set Plane
#pragma region Create Plane
	vector<XMFLOAT3> Plane_Vertices;
	vector<XMFLOAT2> Plane_UVS;
	vector<XMFLOAT3> Plane_Normals;
	Plane_Object.loadOBJ("Plane.obj", Plane_Vertices, Plane_UVS, Plane_Normals);
	SIMPLE_VERTEX Plane_Pyramid[12];
	for (size_t i = 0; i < 12; i++)
	{
		Plane_Pyramid[i].Verts.x = Plane_Vertices[i].x;
		Plane_Pyramid[i].Verts.y = Plane_Vertices[i].y;
		Plane_Pyramid[i].Verts.z = Plane_Vertices[i].z;
		Plane_Pyramid[i].UV.x = Plane_UVS[i].x;
		Plane_Pyramid[i].UV.y = Plane_UVS[i].y;
		Plane_Pyramid[i].NRM.x = Plane_Normals[i].x;
		Plane_Pyramid[i].NRM.y = Plane_Normals[i].y;
		Plane_Pyramid[i].NRM.z = Plane_Normals[i].z;
	}

	// Fill in a buffer description.
	D3D11_BUFFER_DESC PlaneDesc;
	PlaneDesc.ByteWidth = sizeof(SIMPLE_VERTEX) * 12;
	PlaneDesc.Usage = D3D11_USAGE_IMMUTABLE;
	PlaneDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	PlaneDesc.CPUAccessFlags = NULL;
	PlaneDesc.MiscFlags = 0;
	PlaneDesc.StructureByteStride = sizeof(SIMPLE_VERTEX);

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA Plane_SubresourceData;
	Plane_SubresourceData.pSysMem = Plane_Pyramid;
	Plane_SubresourceData.SysMemPitch = 0;
	Plane_SubresourceData.SysMemSlicePitch = 0;


	hr = I_Device->CreateBuffer(&PlaneDesc, &Plane_SubresourceData, &Plane_Object.ConstantBuffer);




#pragma endregion
#pragma region Vertex/Pixel shader and layout
	D3D11_INPUT_ELEMENT_DESC Plane_Layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	I_Device->CreateVertexShader(Transform_VS, sizeof(Transform_VS), nullptr, &Plane_Object.VertexShader);

	I_Device->CreatePixelShader(Transform_PS, sizeof(Transform_PS), nullptr, &Plane_Object.PixelShader[0]);


	I_Device->CreatePixelShader(DirectionalLight_PS, sizeof(DirectionalLight_PS), nullptr, &Plane_Object.PixelShader[1]);

	I_Device->CreatePixelShader(SpotLight, sizeof(SpotLight), nullptr, &Plane_Object.PixelShader[2]);



	I_Device->CreateInputLayout(Plane_Layout, 3, Transform_VS, sizeof(Transform_VS), &Plane_Object.InputLayout);

#pragma endregion
#pragma endregion

#pragma region Sampler and Blend State;

	D3D11_BLEND_DESC BlendDesc;
	ZeroMemory(&BlendDesc, sizeof(D3D11_BLEND_DESC));
	BlendDesc.RenderTarget[0].BlendEnable = TRUE;
	BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;


	I_Device->CreateBlendState(&BlendDesc, &BlendState);

	D3D11_SAMPLER_DESC SamplerDesc = CD3D11_SAMPLER_DESC(CD3D11_DEFAULT());

	I_Device->CreateSamplerState(&SamplerDesc, &SamplerState);

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
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

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

	I_Device->CreateDepthStencilState(&dsDesc, &pDSState);

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));

	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;

	// Create the depth stencil view
	hr = I_Device->CreateDepthStencilView(DepthBuffer, &descDSV, &DepthStencilView);  // [out] Depth stencil view

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

#pragma region Scene Constant Buffer and Ambient/Directional Light
	//Scene
	// Fill in a buffer description.
	D3D11_BUFFER_DESC SceneConstantBufferDesc;

	SceneConstantBufferDesc.ByteWidth = sizeof(CONSTANTSCENE);
	SceneConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	SceneConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	SceneConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	SceneConstantBufferDesc.MiscFlags = 0;
	SceneConstantBufferDesc.StructureByteStride = 0;

	hr = I_Device->CreateBuffer(&SceneConstantBufferDesc, nullptr, &SceneConstantBuffer);


	D3D11_BUFFER_DESC AmbientLightConstantBufferDesc;

	AmbientLightConstantBufferDesc.ByteWidth = sizeof(XMFLOAT4);
	AmbientLightConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	AmbientLightConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	AmbientLightConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	AmbientLightConstantBufferDesc.MiscFlags = 0;
	AmbientLightConstantBufferDesc.StructureByteStride = 0;


	hr = I_Device->CreateBuffer(&AmbientLightConstantBufferDesc, nullptr, &AmbientLightBuffer);

	D3D11_BUFFER_DESC DLightDesc;
	DLightDesc.ByteWidth = sizeof(DLIGHT);
	DLightDesc.Usage = D3D11_USAGE_DYNAMIC;
	DLightDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	DLightDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	DLightDesc.MiscFlags = 0;
	DLightDesc.StructureByteStride = 0;

	hr = I_Device->CreateBuffer(&DLightDesc, nullptr, &DirectionalLightBuffer);


	//forward and looking down(should be looking at an angle)
	Dlight.Direction.x = 0.0f;
	Dlight.Direction.y = -1.0f;
	Dlight.Direction.z = 1.0f;
	Dlight.Direction.w = 0.0f;

	XMVECTOR DVect = XMLoadFloat4(&Dlight.Direction);
	XMVector3Normalize(DVect);
	XMStoreFloat4(&Dlight.Direction, DVect);

	//SpotLight
	D3D11_BUFFER_DESC SLightDesc;
	SLightDesc.ByteWidth = sizeof(SLIGHT);
	SLightDesc.Usage = D3D11_USAGE_DYNAMIC;
	SLightDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	SLightDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	SLightDesc.MiscFlags = 0;
	SLightDesc.StructureByteStride = 0;

	hr = I_Device->CreateBuffer(&SLightDesc, nullptr, &SpotLightBuffer);

	SLight.Col.x = 0.75f;
	SLight.Col.y = 0.25f;
	SLight.Col.z = 0.75f;
	SLight.Col.w = 1.0f;

	SLight.Cratio = .5f;

	SLight.Dir.x = 0.0f;
	SLight.Dir.y = -1.0f;
	SLight.Dir.z = 0.0f;

	SLight.Pos.x = 0.0f;
	SLight.Pos.y = 2.0f;
	SLight.Pos.z = 1.0f;
	SLight.Pos.w = 1.0f;

	SLight.MotherOfThree.x = 0.0f;
	SLight.MotherOfThree.y = 0.0f;
	SLight.MotherOfThree.z = 0.0f;

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
	if (GetKeyState(VK_F6) & 0x1)
	{
		MoveCamera(30, SV_ViewMatrix);
	}
	else
	{
		MoveCamera(30, SV_ViewMatrix_2);
	}

	SV_Projection = XMMatrixIdentity();

	FLOAT ZNear = 0.1f;
	FLOAT ZFar = 100.0f;
	FLOAT FOV = Degrees_To_Radians(75);
	FLOAT AspectRatio = BACKBUFFER_WIDTH / BACKBUFFER_HEIGHT;
	FLOAT Yscale = FLOAT(1 / tan(FOV * 0.5f));
	FLOAT Xscale = Yscale * AspectRatio;

	XMMATRIX PerspectiveProjection = XMMatrixPerspectiveFovLH(FOV, AspectRatio, ZNear, ZFar);



	ConstantScene.projectionMatrix = SV_Projection = PerspectiveProjection;

	PrevPos = currPos;

#pragma endregion

#pragma region ViewPorts
	//How to change view ports.
	I_Context->OMSetRenderTargets(1, &I_RenderTargetView, DepthStencilView);

	float ColorRGBA[4] = { 0, 0, 1, 1 };
	I_Context->ClearRenderTargetView(I_RenderTargetView, ColorRGBA);
	I_Context->ClearDepthStencilView(DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
#pragma endregion

#pragma region Ambient Light

	XMFLOAT4 LightColor;
	ZeroMemory(&LightColor, sizeof(XMFLOAT4));
	if (GetKeyState(VK_F3) & 0x1)
	{
		LightColor.x = 0.4f;
		LightColor.y = 0.4f;
		LightColor.z = 0.4f;
		LightColor.w = 1.0f;
	}
#pragma endregion

#pragma region Directional Light
	ZeroMemory(&Dlight.Col, sizeof(XMFLOAT4));
	if (GetKeyState(VK_F4) & 0x1)
	{
		Dlight.Col.x = 1.0f;
		Dlight.Col.y = 0.7f;
		Dlight.Col.z = 0.2f;
		Dlight.Col.w = 1.0f;

	}
	if (GetKeyState(VK_F5) & 0x8000)
	{
		Dlight.Direction.y = -1.0f * (Dlight.Direction.y);
		Dlight.Direction.z = -1.0f * (Dlight.Direction.y);
	}
#pragma endregion

#pragma region Spot Light
	if (GetKeyState(VK_F7) & 0x1)
	{
		SLight.Dir.x = 0.0f;
		SLight.Dir.y = 0.0f;
		SLight.Dir.z = -1.0f;

		SLight.Pos.x = 0.0f;
		SLight.Pos.y = 1.0f;
		SLight.Pos.z = 3.0f;
		SLight.Pos.w = 1.0f;

	}
	else
	{
		SLight.Dir.x = 0.0f;
		SLight.Dir.y = -1.0f;
		SLight.Dir.z = 0.0f;

		SLight.Pos.x = 0.0f;
		SLight.Pos.y = 2.0f;
		SLight.Pos.z = 1.0f;
		SLight.Pos.w = 1.0f;
	}
#pragma endregion

	I_Context->OMSetDepthStencilState(pDSState, 0); // set so I can do more then one draw call and have lights show.

#pragma region For loop for drawing
	for (size_t j = 0; j < 2; j++)
	{
		if (j == 1)
		{
			ConstantScene.viewMatrix = SV_ViewMatrix;
			I_Context->RSSetViewports(1, &ViewPort[j]);

		}
		else
		{
			ConstantScene.viewMatrix = SV_ViewMatrix_2;
			I_Context->RSSetViewports(1, &ViewPort[j]);
		}
		for (size_t i = 0; i < 3; i++) //for loop to loop through ambient and spot light.
		{

#pragma region Switch case for PixelShaders
			switch (i)
			{
			case 0:
				I_Context->PSSetConstantBuffers(0, 1, &AmbientLightBuffer);
				I_Context->OMSetBlendState(nullptr, nullptr, 0xffffffff);
				break;
			case 1:
				I_Context->PSSetConstantBuffers(0, 1, &DirectionalLightBuffer);
				I_Context->OMSetBlendState(BlendState, nullptr, 0xffffffff);
				break;
			case 2:
				I_Context->PSSetConstantBuffers(0, 1, &SpotLightBuffer);
				break;
			}
#pragma endregion

#pragma region Draw Tron Cube

			Cube_Object.SetWorldMatrix(XMMatrixIdentity());

			Cube_Object.SetWorldMatrix(XMMatrixMultiply(Cube_Object.WorldMatrix, XMMatrixTranslation(0.0f, 0.0f, 1.0f)));

			WorldMatrixObject = Cube_Object.WorldMatrix = XMMatrixMultiply(XMMatrixRotationY((FLOAT)(TotalTimeLoop.TotalTime())), Cube_Object.WorldMatrix);

			// Set vertex buffer
			UINT stride = sizeof(_OBJ_VERT_);
			UINT offset = 0;
			I_Context->IASetVertexBuffers(0, 1, &Cube_Object.ConstantBuffer, &stride, &offset);

			I_Context->IASetIndexBuffer(Cube_Object.IndexBuffer, DXGI_FORMAT_R32_UINT, offset);

			I_Context->VSSetShader(Cube_Object.VertexShader, NULL, 0);

			I_Context->PSSetShader(Cube_Object.PixelShader[i], NULL, 0);

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

			memcpy(Local.pData, &ConstantScene, sizeof(CONSTANTSCENE));

			I_Context->Unmap(SceneConstantBuffer, 0);



			//Map DLight
			I_Context->Map(DirectionalLightBuffer, NULL, D3D11_MAP_WRITE_DISCARD, 0, &Local);

			memcpy(Local.pData, &Dlight, sizeof(DLIGHT));

			I_Context->Unmap(DirectionalLightBuffer, 0);


			//Map ALight
			I_Context->Map(AmbientLightBuffer, NULL, D3D11_MAP_WRITE_DISCARD, 0, &Local);

			memcpy(Local.pData, &LightColor, sizeof(XMFLOAT4));

			I_Context->Unmap(AmbientLightBuffer, 0);

			//Map SLight
			I_Context->Map(SpotLightBuffer, NULL, D3D11_MAP_WRITE_DISCARD, 0, &Local);

			memcpy(Local.pData, &SLight, sizeof(SLIGHT));

			I_Context->Unmap(SpotLightBuffer, 0);


			I_Context->VSSetConstantBuffers(0, 1, &WorldConstantBuffer);

			I_Context->VSSetConstantBuffers(1, 1, &SceneConstantBuffer);

			I_Context->PSSetSamplers(0, 1, &SamplerState);

			I_Context->DrawIndexed(1692, 0, 0);// drawing cube

#pragma endregion

#pragma region Draw Star

			Star_Object.SetWorldMatrix(XMMatrixIdentity());

			Star_Object.SetWorldMatrix(XMMatrixMultiply(Star_Object.WorldMatrix, XMMatrixTranslation(2.0f, 2.0f, 1.0f)));

			WorldMatrixObject = Star_Object.WorldMatrix = XMMatrixMultiply(XMMatrixRotationY((FLOAT)(TotalTimeLoop.TotalTime())), Star_Object.WorldMatrix);

			// Set vertex buffer
			UINT Star_stride = sizeof(VERTEX4);
			UINT Star_offset = 0;
			I_Context->IASetVertexBuffers(0, 1, &Star_Object.ConstantBuffer, &Star_stride, &Star_offset);

			I_Context->IASetIndexBuffer(Star_Object.IndexBuffer, DXGI_FORMAT_R32_UINT, Star_offset);

			I_Context->VSSetShader(Star_Object.VertexShader, NULL, 0);

			I_Context->PSSetShader(Star_Object.PixelShader[i], NULL, 0);

			I_Context->IASetInputLayout(Star_Object.InputLayout);

			I_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			Star_Object.ShaderResourceView = nullptr;

			I_Context->PSSetShaderResources(0, 1, &Star_Object.ShaderResourceView);


			//mapping local to constantbuffer
			D3D11_MAPPED_SUBRESOURCE Star_Local;
			I_Context->Map(ConstantBuffer, NULL, D3D11_MAP_WRITE_DISCARD, 0, &Star_Local);

			memcpy(Star_Local.pData, &ToShader, sizeof(SEND_TO_VRAM));

			I_Context->Unmap(ConstantBuffer, 0);

			//World Constant buffer mapping
			I_Context->Map(WorldConstantBuffer, NULL, D3D11_MAP_WRITE_DISCARD, 0, &Star_Local);

			memcpy(Star_Local.pData, &WorldMatrixObject, sizeof(XMMATRIX));

			I_Context->Unmap(WorldConstantBuffer, 0);


			//Scene Constant buffer mapping
			I_Context->Map(SceneConstantBuffer, NULL, D3D11_MAP_WRITE_DISCARD, 0, &Star_Local);

			memcpy(Star_Local.pData, &ConstantScene, sizeof(CONSTANTSCENE));

			I_Context->Unmap(SceneConstantBuffer, 0);



			I_Context->VSSetConstantBuffers(0, 1, &WorldConstantBuffer);
			I_Context->VSSetConstantBuffers(1, 1, &SceneConstantBuffer);

			I_Context->DrawIndexed(60, 0, 0);
#pragma endregion

#pragma region Draw Model

			Model_Object.SetWorldMatrix(XMMatrixIdentity());

			Model_Object.SetWorldMatrix(XMMatrixMultiply(Model_Object.WorldMatrix, XMMatrixTranslation(-3.0f, 2.0f, 1.0f)));

			WorldMatrixObject = Model_Object.WorldMatrix = XMMatrixMultiply(XMMatrixRotationY((FLOAT)(TotalTimeLoop.TotalTime())), Model_Object.WorldMatrix);

			// Set vertex buffer
			UINT Model_stride = sizeof(SIMPLE_VERTEX);
			UINT Model_offset = 0;
			I_Context->IASetVertexBuffers(0, 1, &Model_Object.ConstantBuffer, &Model_stride, &Model_offset);

			//I_Context->IASetIndexBuffer(Model_Object.IndexBuffer, DXGI_FORMAT_R32_UINT, Model_offset);

			I_Context->VSSetShader(Model_Object.VertexShader, NULL, 0);

			I_Context->PSSetShader(Model_Object.PixelShader[i], NULL, 0);

			I_Context->IASetInputLayout(Model_Object.InputLayout);
			//mapping local to constantbuffer
			D3D11_MAPPED_SUBRESOURCE Model_Local;
			I_Context->Map(ConstantBuffer, NULL, D3D11_MAP_WRITE_DISCARD, 0, &Model_Local);

			memcpy(Model_Local.pData, &ToShader, sizeof(SEND_TO_VRAM));

			I_Context->Unmap(ConstantBuffer, 0);

			//World Constant buffer mapping
			I_Context->Map(WorldConstantBuffer, NULL, D3D11_MAP_WRITE_DISCARD, 0, &Model_Local);

			memcpy(Model_Local.pData, &WorldMatrixObject, sizeof(XMMATRIX));

			I_Context->Unmap(WorldConstantBuffer, 0);


			//Scene Constant buffer mapping
			I_Context->Map(SceneConstantBuffer, NULL, D3D11_MAP_WRITE_DISCARD, 0, &Model_Local);

			memcpy(Model_Local.pData, &ConstantScene, sizeof(CONSTANTSCENE));

			I_Context->Unmap(SceneConstantBuffer, 0);

			I_Context->VSSetConstantBuffers(0, 1, &WorldConstantBuffer);
			I_Context->VSSetConstantBuffers(1, 1, &SceneConstantBuffer);

			I_Context->Draw(36, 0);


#pragma endregion

#pragma region Draw Plane

			Plane_Object.SetWorldMatrix(XMMatrixIdentity());

			Plane_Object.SetWorldMatrix(XMMatrixMultiply(Plane_Object.WorldMatrix, XMMatrixTranslation(0.0f, -1.0f, 0.0f)));

			WorldMatrixObject = Plane_Object.WorldMatrix;// = XMMatrixMultiply(XMMatrixRotationY((FLOAT)(TotalTimeLoop.TotalTime())), Plane_Object.WorldMatrix);

			// Set vertex buffer
			UINT Plane_stride = sizeof(SIMPLE_VERTEX);
			UINT Plane_offset = 0;
			I_Context->IASetVertexBuffers(0, 1, &Plane_Object.ConstantBuffer, &Plane_stride, &Plane_offset);

			//I_Context->IASetIndexBuffer(Plane_Object.IndexBuffer, DXGI_FORMAT_R32_UINT, Plane_offset);

			I_Context->VSSetShader(Plane_Object.VertexShader, NULL, 0);

			I_Context->PSSetShader(Plane_Object.PixelShader[i], NULL, 0);

			I_Context->IASetInputLayout(Plane_Object.InputLayout);
			//mapping local to constantbuffer
			D3D11_MAPPED_SUBRESOURCE Plane_Local;
			I_Context->Map(ConstantBuffer, NULL, D3D11_MAP_WRITE_DISCARD, 0, &Plane_Local);

			memcpy(Plane_Local.pData, &ToShader, sizeof(SEND_TO_VRAM));

			I_Context->Unmap(ConstantBuffer, 0);

			//World Constant buffer mapping
			I_Context->Map(WorldConstantBuffer, NULL, D3D11_MAP_WRITE_DISCARD, 0, &Plane_Local);

			memcpy(Plane_Local.pData, &WorldMatrixObject, sizeof(XMMATRIX));

			I_Context->Unmap(WorldConstantBuffer, 0);


			//Scene Constant buffer mapping
			I_Context->Map(SceneConstantBuffer, NULL, D3D11_MAP_WRITE_DISCARD, 0, &Plane_Local);

			memcpy(Plane_Local.pData, &ConstantScene, sizeof(CONSTANTSCENE));

			I_Context->Unmap(SceneConstantBuffer, 0);

			I_Context->VSSetConstantBuffers(0, 1, &WorldConstantBuffer);
			I_Context->VSSetConstantBuffers(1, 1, &SceneConstantBuffer);

			I_Context->Draw(12, 0);


#pragma endregion

		}//end of for loop
	}//end of forloop for viewports
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
	SAFE_RELEASE(AmbientLightBuffer);//Cleared
	SAFE_RELEASE(BlendState);//Cleared
	SAFE_RELEASE(BlendStateNULL)//Cleared
		SAFE_RELEASE(SamplerState);//Cleared
	SAFE_RELEASE(DirectionalLightBuffer);//Cleared
	SAFE_RELEASE(SpotLightBuffer);//Cleared
	SAFE_RELEASE(pDSState);//Cleared

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