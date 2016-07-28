#ifndef H_DATASTRUCTURES
#define H_DATASTRUCTURES

#include <DirectXMath.h>
#include <d3d11.h>
#include <map>

#include "..\DirectXToolkit\Inc\WICTextureLoader.h"

using namespace DirectX;
using namespace std;

struct Vertex {
	XMFLOAT3 position;
	XMFLOAT2 uv;	
};

struct IndexedVertexBuffer {
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
};

struct StaticObject {
	UINT indexCount;
	IndexedVertexBuffer buffer;
};

struct ThesisData {
	map<string, ID3D11Buffer*> constantBuffers;
	map<string, ID3D11InputLayout*> inputLayouts;
	map<string, ID3D11VertexShader*> vertexShaders;
	map<string, ID3D11DomainShader*> domainShaders;
	map<string, ID3D11HullShader*> hullShaders;
	map<string, ID3D11GeometryShader*> geometryShaders;
	map<string, ID3D11PixelShader*> pixelShaders;
	map<string, ID3D11ComputeShader*> computeShaders;
	map<string, ID3D11RenderTargetView*> renderTargetViews;
	map<string, ID3D11ShaderResourceView*> shaderResourceViews;
	map<string, ID3D11Texture2D*> textures;
	map<string, ID3D11SamplerState*> samplerStates;
	~ThesisData() {
		for (map<string, ID3D11Buffer*>::iterator it = constantBuffers.begin(); it != constantBuffers.end(); ++it)
			if (it->second != NULL) it->second->Release();
		for (map<string, ID3D11InputLayout*>::iterator it = inputLayouts.begin(); it != inputLayouts.end(); ++it)
			if (it->second != NULL) it->second->Release();
		for (map<string, ID3D11VertexShader*>::iterator it = vertexShaders.begin(); it != vertexShaders.end(); ++it)
			if (it->second != NULL) it->second->Release();
		for (map<string, ID3D11DomainShader*>::iterator it = domainShaders.begin(); it != domainShaders.end(); ++it)
			if (it->second != NULL) it->second->Release();
		for (map<string, ID3D11HullShader*>::iterator it = hullShaders.begin(); it != hullShaders.end(); ++it)
			if (it->second != NULL) it->second->Release();
		for (map<string, ID3D11GeometryShader*>::iterator it = geometryShaders.begin(); it != geometryShaders.end(); ++it)
			if (it->second != NULL) it->second->Release();
		for (map<string, ID3D11PixelShader*>::iterator it = pixelShaders.begin(); it != pixelShaders.end(); ++it)
			if (it->second != NULL) it->second->Release();
		for (map<string, ID3D11ComputeShader*>::iterator it = computeShaders.begin(); it != computeShaders.end(); ++it)
			if (it->second != NULL) it->second->Release();
		for (map<string, ID3D11RenderTargetView*>::iterator it = renderTargetViews.begin(); it != renderTargetViews.end(); ++it)
			if (it->second != NULL) it->second->Release();
		for (map<string, ID3D11ShaderResourceView*>::iterator it = shaderResourceViews.begin(); it != shaderResourceViews.end(); ++it)
			if (it->second != NULL) it->second->Release();
		for (map<string, ID3D11Texture2D*>::iterator it = textures.begin(); it != textures.end(); ++it)
			if (it->second != NULL) it->second->Release();
		for (map<string, ID3D11SamplerState*>::iterator it = samplerStates.begin(); it != samplerStates.end(); ++it)
			if (it->second != NULL) it->second->Release();
	}
};

enum Users {
	TEXT,
	COMPOSITING,
	TONEMAPPING,
	ANTIALIASING,
	LIGHTNING,
	ALL
};

#endif