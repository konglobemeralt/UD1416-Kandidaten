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
	map<string, ID3D11SamplerState*> samplerStates;
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