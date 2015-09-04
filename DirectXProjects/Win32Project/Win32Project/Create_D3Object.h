#pragma once
#include "MathLib.h"
#include <DirectXMath.h>
using namespace DirectX;

#include <iostream>
#include <ctime>
#include <d3d11.h>
#pragma comment (lib, "d3d11.lib")
#include <atlcomcli.h>

#define SAFE_RELEASE(p) {if(p){p->Release(); p = nullptr;}}
#define SAFE_DELETE(p) {if(p){delete p; p = nullptr;}}

class Create_D3Object
{

public:

	ID3D11Buffer *ConstantBuffer = nullptr;
	ID3D11Buffer *IndexBuffer = nullptr;
	ID3D11Buffer *VertexBuffer = nullptr;

	//Position of the Object;
	XMMATRIX WorldMatrix; // ZeroMemory called in Constructor;

	ID3D11VertexShader *VertexShader = nullptr;
	ID3D11PixelShader *PixelShader[2];
	ID3D11InputLayout *InputLayout = nullptr;

	//could be more then one(consider array)
	ID3D11ShaderResourceView *ShaderResourceView = nullptr;
	ID3D11Texture2D *Texture2DBuffer = nullptr;

	ID3D11SamplerState *SamplerState = nullptr;

	Create_D3Object();
	~Create_D3Object();

#pragma region Getters and Setters
#pragma region Getters
	ID3D11Buffer* GetConstantBuffer(){ return ConstantBuffer; };
	ID3D11Buffer* GetIndexBuffer(){ return IndexBuffer; };
	ID3D11Buffer* GetVertexBuffer(){ return VertexBuffer; };
	ID3D11VertexShader* GetVertexShader(){ return VertexShader; };
	ID3D11InputLayout* GetInputLayout(){ return InputLayout; };
	ID3D11ShaderResourceView* GetShaderResourceView(){ return ShaderResourceView; };
	ID3D11Texture2D* GetTexture2DBuffer(){ return Texture2DBuffer; };
	XMMATRIX GetWorldMatrix(){ return WorldMatrix; };
#pragma endregion
#pragma region Setters
	void SetConstantBuffer(ID3D11Buffer *NewConstantBuffer){ ConstantBuffer = NewConstantBuffer; };
	void SetIndexBuffer(ID3D11Buffer *NewIndexBuffer){ IndexBuffer = NewIndexBuffer; };
	void SetVertexBuffer(ID3D11Buffer *NewVertexBuffer){ VertexBuffer = NewVertexBuffer; };
	void SetVertexShader(ID3D11VertexShader *NewVertexShader){ VertexShader = NewVertexShader; };
	void SetInputLayout(ID3D11InputLayout *NewInputLayout){ InputLayout = NewInputLayout; };
	void SetShaderResourceView(ID3D11ShaderResourceView *NewShaderResourceView){ ShaderResourceView = NewShaderResourceView; };
	void SetTexture2DBuffer(ID3D11Texture2D *NewTextureBuffer){ Texture2DBuffer = NewTextureBuffer; };
	void SetWorldMatrix(XMMATRIX NewWorldMatrix){ WorldMatrix = NewWorldMatrix; };
#pragma endregion
#pragma endregion

};


Create_D3Object::Create_D3Object()
{
	ZeroMemory(&WorldMatrix, sizeof(WorldMatrix));
	WorldMatrix = XMMatrixIdentity();
	ZeroMemory(PixelShader, sizeof(PixelShader));


};

Create_D3Object::~Create_D3Object()
{
	SAFE_RELEASE(ConstantBuffer);
	SAFE_DELETE(ConstantBuffer);
	
	SAFE_RELEASE(IndexBuffer);
	SAFE_DELETE(IndexBuffer);
	
	SAFE_RELEASE(VertexBuffer);
	SAFE_DELETE(VertexBuffer);
	
	SAFE_RELEASE(VertexShader);
	SAFE_DELETE(VertexShader);
	
	SAFE_RELEASE(PixelShader[0]);
	SAFE_DELETE(PixelShader[0]);

	SAFE_RELEASE(PixelShader[1]);
	SAFE_DELETE(PixelShader[1]);

	SAFE_RELEASE(InputLayout);
	SAFE_DELETE(InputLayout);

	SAFE_RELEASE(ShaderResourceView);
	SAFE_DELETE(ShaderResourceView);

	SAFE_RELEASE(Texture2DBuffer);
	SAFE_DELETE(Texture2DBuffer);
	
	SAFE_RELEASE(SamplerState);
	SAFE_DELETE(SamplerState);

};






