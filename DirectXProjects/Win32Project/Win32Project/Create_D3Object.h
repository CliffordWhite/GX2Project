#pragma once
#include "MathLib.h"
#include <DirectXMath.h>
using namespace DirectX;

#include <iostream>
#include <ctime>
#include <d3d11.h>
#pragma comment (lib, "d3d11.lib")
#include <atlcomcli.h>
#include <vector>

#define SAFE_RELEASE(p) {if(p){p->Release(); p = nullptr;}}
#define SAFE_DELETE(p) {if(p){delete p; p = nullptr;}}
struct SIMPLE_VERTEX
{
	XMFLOAT3 Verts;
	XMFLOAT2 UV;
	XMFLOAT3 NRM;

};
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


	bool loadOBJ(const char * path,std::vector < XMFLOAT3 > & out_vertices,std::vector < XMFLOAT2 > & out_uvs,std::vector < XMFLOAT3 > & out_normals);

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

bool Create_D3Object::loadOBJ(const char * path,std::vector < XMFLOAT3 > & out_vertices,std::vector < XMFLOAT2 > & out_uvs,std::vector < XMFLOAT3 > & out_normals)
{
	std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
	std::vector< XMFLOAT3 > temp_vertices;
	std::vector< XMFLOAT2 > temp_uvs;
	std::vector< XMFLOAT3 > temp_normals;

	FILE * file = fopen(path, "r");
	if (file == NULL){
		printf("Impossible to open the file !\n");
		return false;
	}

	while (1){

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader
		if (strcmp(lineHeader, "v") == 0){
			XMFLOAT3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0){
			XMFLOAT2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0){
			XMFLOAT3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0){
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9){
				printf("File can't be read by our simple parser : ( Try exporting with other options\n");
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
	}
			// For each vertex of each triangle
			for (unsigned int i = 0; i < vertexIndices.size(); i++){
				unsigned int vertexIndex = vertexIndices[i];
				XMFLOAT3 vertex = temp_vertices[vertexIndex - 1];
				out_vertices.push_back(vertex);
			}
			for (unsigned int i = 0; i < uvIndices.size(); i++){
				unsigned int UVIndex = uvIndices[i];
				XMFLOAT2 uvvertex = temp_uvs[UVIndex - 1];
				out_uvs.push_back(uvvertex);
			}
			for (unsigned int i = 0; i < normalIndices.size(); i++){
				unsigned int nmvertexIndex = normalIndices[i];
				XMFLOAT3 nrmvertex = temp_normals[nmvertexIndex - 1];
				out_normals.push_back(nrmvertex);
			}
	return true;
}




