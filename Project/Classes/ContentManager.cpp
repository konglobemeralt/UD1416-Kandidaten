#include "ContentManager.h"

ContentManager::ContentManager(ID3D11Device* device, ID3D11DeviceContext* deviceContext) {
	gDevice = device;
	gDeviceContext = deviceContext;
}

ContentManager::~ContentManager() {

}

void ContentManager::LoadContent() {
	//staticObjects.push_back(CreateStaticObject());
}

vector<StaticObject>* ContentManager::getStaticObjects() {
	return &staticObjects;
}

StaticObject ContentManager::CreateStaticObject() {
	StaticObject object;
	vector<UINT> indices = { 0, 1, 2, 3, 7, 1, 5, 4, 7, 6, 2, 4, 0, 1 };

	vector<Vertex> vertices = {
		Vertex{
		XMFLOAT3(-1.0, -1.0,  1.0),
		XMFLOAT2(0.000000, 0.000000) },

		Vertex{
		XMFLOAT3(1.0, -1.0,  1.0),
		XMFLOAT2(1.000000, 0.000000) },

		Vertex{
		XMFLOAT3(-1.0,  1.0,  1.0),
		XMFLOAT2(0.000000, 1.000000) },

		Vertex{
		XMFLOAT3(1.0,  1.0,  1.0),
		XMFLOAT2(1.000000, 1.000000) },

		Vertex{
		XMFLOAT3(-1.0, -1.0, -1.0),
		XMFLOAT2(0.000000, 0.000000) },

		Vertex{
		XMFLOAT3(1.0, -1.0, -1.0),
		XMFLOAT2(1.000000, 0.000000) },

		Vertex{
		XMFLOAT3(-1.0,  1.0, -1.0),
		XMFLOAT2(0.000000, 1.000000) },

		Vertex{
		XMFLOAT3(1.0,  1.0, -1.0),
		XMFLOAT2(1.000000, 1.000000) }
	};

	object.indexCount = indices.size();
	object.buffer = CreateIVBuffer(vertices, indices);
	
	return object;
}

IndexedVertexBuffer ContentManager::CreateIVBuffer(vector<Vertex> vertices, vector<UINT> indices) {
	IndexedVertexBuffer buffer;
	
	// Vertex buffer
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(Vertex) * indices.size();
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	
	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = vertices.data();
	
	gDevice->CreateBuffer(&bufferDesc, &data, &buffer.vertexBuffer);

	// Index buffer
	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(UINT) * indices.size();
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	
	gDevice->CreateBuffer(&indexBufferDesc, &indexData, &buffer.indexBuffer);

	return buffer;
}