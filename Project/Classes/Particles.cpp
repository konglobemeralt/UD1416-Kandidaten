#include "Particles.h"

Particles::Particles(ID3D11Device* gDevice, ID3D11DeviceContext* gDeviceContext, int particleCount, XMFLOAT2 particleSize, XMFLOAT3 particleBox, XMFLOAT3 boxPosition){
	this->gDevice = gDevice;
	this->gDeviceContext = gDeviceContext;

	gPComputeShader = nullptr;
	gPVertexShader = nullptr;
	gPGeometryShader = nullptr;
	gPPixelShader = nullptr;
	gPVertexLayout = nullptr;

	this->particleCount = particleCount;
	this->particleSize = particleSize;
	this->particleBox = particleBox;
	this->boxPosition = boxPosition;

	InitParticles();
	CreateParticleShaders();
	CreateParticleBuffer();
}

void Particles::InitParticles(){
	if ((particleCount % 20) != 0)
		particleCount -= (particleCount % 20);

	srand((unsigned int)time(NULL));

	for (int i = 0; i < particleCount; i++){
		vertex.x = (float(rand()) / float(RAND_MAX) / 2.0f) + (float(rand() % int(particleBox.x)));
		vertex.y = (float(rand()) / float(RAND_MAX) / 2.0f) + particleBox.y;
		vertex.z = (float(rand()) / float(RAND_MAX) / 2.0f) + (float(rand() % int(particleBox.z)));
		vertex.nx = 0.0f;
		vertex.ny = 0.0f;
		vertex.nz = 1.0f;
		vertex.u = 0.5f;
		vertex.v = 0.5f;
		vertex.lifespan = (float(rand()) / float(RAND_MAX) / 1.0f) + particleBox.y;
		vertex.velocity = (float(rand()) / float(RAND_MAX) / 10.0f) + 0.2f;
		
		vertexArray.push_back(vertex);
	}
}

void Particles::CreateParticleShaders(){
	//Particle vertex-shader
	ID3DBlob* pPVS = nullptr;
	D3DCompileFromFile(L"PVertexShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PVS_main", "vs_5_0", 0, NULL, &pPVS, nullptr);
	gDevice->CreateVertexShader(pPVS->GetBufferPointer(), pPVS->GetBufferSize(), nullptr, &gPVertexShader);

	//Particle vertex-shader layout
	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	gDevice->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), pPVS->GetBufferPointer(), pPVS->GetBufferSize(), &gPVertexLayout);
	pPVS->Release();

	//Particle geometry-shader
	ID3DBlob* pPGS = nullptr;
	D3DCompileFromFile(L"PGeometryShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PGS_main", "gs_5_0", 0, NULL, &pPGS, nullptr);
	gDevice->CreateGeometryShader(pPGS->GetBufferPointer(), pPGS->GetBufferSize(), nullptr, &gPGeometryShader);
	pPGS->Release();

	//Particle pixel-shader
	ID3DBlob* PPS = nullptr;
	D3DCompileFromFile(L"PPixelShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PPS_main", "ps_5_0", 0, NULL, &PPS, nullptr);

	gDevice->CreatePixelShader(PPS->GetBufferPointer(), PPS->GetBufferSize(), nullptr, &gPPixelShader);
	PPS->Release();

	//Particle compute-shader
	ID3DBlob* PCS = nullptr;
	D3DCompileFromFile(L"PComputeShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PCS_main", "cs_5_0", 0, NULL, &PCS, nullptr);

	gDevice->CreateComputeShader(PCS->GetBufferPointer(), PCS->GetBufferSize(), nullptr, &gPComputeShader);
	PCS->Release();
}

void Particles::CreateParticleBuffer(){
	//
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.StructureByteStride = sizeof(vert);
	bufferDesc.ByteWidth = sizeof(vert) * vertexArray.size();
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = vertexArray.data();

	gDevice->CreateBuffer(&bufferDesc, &data, &gPVertexBuffer);

	bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	gDevice->CreateBuffer(&bufferDesc, &data, &gPUAVBuffer);
	
	//
	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVdescView;
	ZeroMemory(&UAVdescView, sizeof(UAVdescView));
	UAVdescView.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	UAVdescView.Buffer.FirstElement = 0;
	UAVdescView.Format = DXGI_FORMAT_UNKNOWN;
	UAVdescView.Buffer.NumElements = vertexArray.size();
	UAVdescView.Buffer.Flags = 0;

	gDevice->CreateUnorderedAccessView(gPUAVBuffer, &UAVdescView, &gPUAV);

	wstring texture = L"snow.jpg";
	CreateWICTextureFromFile(gDevice, texture.c_str(), nullptr, &particleTextureSRV);
}

void Particles::DrawParticles(){
	UINT32 vertSize = sizeof(vert);
	UINT32 offset = 0;

	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	gDeviceContext->IASetInputLayout(gPVertexLayout);

	gDeviceContext->CSSetUnorderedAccessViews(0, 1, &gPUAV, nullptr);
	gDeviceContext->CSSetShader(gPComputeShader, nullptr, 0);
	
	gDeviceContext->Dispatch(particleCount/20, 1, 1);

	gDeviceContext->CSSetShader(nullptr, nullptr, 0);
	
	gDeviceContext->CopyResource(gPVertexBuffer, gPUAVBuffer);

	gDeviceContext->VSSetShader(gPVertexShader, nullptr, 0);
	gDeviceContext->GSSetShader(gPGeometryShader, nullptr, 0);
	gDeviceContext->PSSetShader(gPPixelShader, nullptr, 0);

	gDeviceContext->IASetVertexBuffers(0, 1, &gPVertexBuffer, &vertSize, &offset);
	gDeviceContext->PSSetShaderResources(0, 1, &particleTextureSRV);

	gDeviceContext->Draw(vertexArray.size(), 0);
}

Particles::~Particles(){
	gPComputeShader->Release();
	gPVertexShader->Release();
	gPGeometryShader->Release();
	gPPixelShader->Release();
	gPVertexLayout->Release();

	gPVertexBuffer->Release();
	gPUAVBuffer->Release();
	gPUAV->Release();
	particleTextureSRV->Release();

}