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
	~IndexedVertexBuffer() {
		vertexBuffer->Release();
		indexBuffer->Release();
	}
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
	map<string, ID3D11SamplerState*> samplerStates;

	~ThesisData() {
		for (auto cbit = constantBuffers.begin(); cbit != constantBuffers.end(); ++cbit) 
			cbit->second->Release();
		for (auto ilit = inputLayouts.begin(); ilit != inputLayouts.end(); ++ilit)
			ilit->second->Release();
		for (auto vsit = vertexShaders.begin(); vsit != vertexShaders.end(); vsit++)
			vsit->second->Release();
		for (auto dsit = domainShaders.begin(); dsit != domainShaders.end(); dsit++) 
			dsit->second->Release();
		for (auto hsit = hullShaders.begin(); hsit != hullShaders.end(); hsit++) 
			hsit->second->Release();
		for (auto gsit = geometryShaders.begin(); gsit != geometryShaders.end(); gsit++) 
			gsit->second->Release();
		for (auto psit = pixelShaders.begin(); psit != pixelShaders.end(); psit++) 
			psit->second->Release();
		for (auto csit = computeShaders.begin(); csit != computeShaders.end(); csit++) 
			csit->second->Release();
		for (auto rtvit = renderTargetViews.begin(); rtvit != renderTargetViews.end(); rtvit++) 
			rtvit->second->Release();
		for (auto srvit = shaderResourceViews.begin(); srvit != shaderResourceViews.end(); srvit++) 
			srvit->second->Release();
		for (auto ssit = samplerStates.begin(); ssit != samplerStates.end(); ssit++) 
			ssit->second->Release();

		constantBuffers.clear();
		inputLayouts.clear();
		vertexShaders.clear();
		domainShaders.clear();
		hullShaders.clear();
		geometryShaders.clear();
		pixelShaders.clear();
		computeShaders.clear();
		renderTargetViews.clear();
		shaderResourceViews.clear();
		samplerStates.clear();
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