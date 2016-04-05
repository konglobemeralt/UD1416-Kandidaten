#ifndef H_DATASTRUCTURES
#define H_DATASTRUCTURES

#include <DirectXMath.h>

using namespace DirectX;

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

#endif