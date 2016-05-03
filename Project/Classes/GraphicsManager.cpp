#include "GraphicsManager.h"
#include "..\ApplicationContext.h"
#include "AntiAliasing.h"
#include "Compositing.h"
#include "Lightning.h"
#include "Text.h"
#include "ToneMapping.h"

using namespace std;
void GraphicsManager::initGraphics(HWND* hwnd) {
	windowHandle = hwnd;
	CreateDirect3DContext();
	setRasterstate(D3D11_CULL_NONE);

	struct TriangleVertex
	{
		float x, y, z;
		float u, v;
	}
	triangleVertices[4] =
	{
		-1.0f, 1.0f, 0.0f,	//v0 pos
		0.0f, 0.0f,	//v0 tex

		1.0f, 1.0f, 0.0f,	//v1
		1.0f, 0.0f,	//v1 tex

		-1.0f, -1.0f, 0.0f, //v2
		0.0f, 1.0f,	//v2 tex

		1.0f, -1.0f, 0.0f,   //v3
		1.0f, 1.0f	//v3 color
	};

	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(triangleVertices);

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = triangleVertices;
	gDevice->CreateBuffer(&bufferDesc, &data, &gQuadBuffer);
}

void GraphicsManager::Render() {
	switch (ApplicationContext::GetInstance().GetUser())
	{
	case TEXT:
		//ApplicationContext::GetInstance().GetTextObject()->Render();
		break;
	case COMPOSITING:
		ApplicationContext::GetInstance().GetCompositingObject()->Render();
		break;
	case TONEMAPPING:
		ApplicationContext::GetInstance().GetToneMappingObject()->Render();
		break;
	case ANTIALIASING:
		ApplicationContext::GetInstance().GetAntiAliasingObject()->Render();
		break;
	case LIGHTNING:
		ApplicationContext::GetInstance().GetLightningObject()->Render();
		break;
	case ALL:
		ApplicationContext::GetInstance().GetTextObject()->Render();
		ApplicationContext::GetInstance().GetCompositingObject()->Render();
		//ApplicationContext::GetInstance().GetToneMappingObject()->Render();
		//ApplicationContext::GetInstance().GetAntiAliasingObject()->Render();
		//ApplicationContext::GetInstance().GetLightningObject()->Render();
		break;
	default:
		break;
	}
	
}

IDXGISwapChain* GraphicsManager::getSwapChain() {
	return gSwapChain;
}

ID3D11Device* GraphicsManager::getDevice() {
	return gDevice;
}

ID3D11DeviceContext* GraphicsManager::getDeviceContext() {
	return gDeviceContext;
}

void GraphicsManager::setRasterstate(D3D11_CULL_MODE cullmode) {
	D3D11_RASTERIZER_DESC rastDesc;

	rastDesc.FillMode = D3D11_FILL_SOLID;
	rastDesc.CullMode = cullmode;
	rastDesc.FrontCounterClockwise = FALSE;
	rastDesc.DepthBias = 0;
	rastDesc.SlopeScaledDepthBias = 0.0f;
	rastDesc.DepthBiasClamp = 0.0f;
	rastDesc.DepthClipEnable = FALSE;
	rastDesc.ScissorEnable = FALSE;
	rastDesc.MultisampleEnable = FALSE;
	rastDesc.AntialiasedLineEnable = FALSE;

	gDevice->CreateRasterizerState(&rastDesc, &rasterState);
	gDeviceContext->RSSetState(rasterState);
}

HRESULT GraphicsManager::CreateDirect3DContext() {
	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	scd.BufferCount = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_UNORDERED_ACCESS;
	scd.OutputWindow = *windowHandle;
	scd.SampleDesc.Count = 1;
	scd.Windowed = TRUE;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_SINGLETHREADED,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scd,
		&gSwapChain,
		&gDevice,
		NULL,
		&gDeviceContext
	);

	if (SUCCEEDED(hr)) {
		//ID3D11Texture2D* pBackBuffer = nullptr;
		gSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

		gDevice->CreateRenderTargetView(pBackBuffer, NULL, &gBackbufferRTV);
		//pBackBuffer->Release();
	};

	return hr;
}

void GraphicsManager::ResetViews(){
	emptyDSV = NULL;
	int x = 0;
	
	for (int i = 0; i < 4; i++){
		emptyRTV[i] = NULL;
		for (int y= 0; y < 2; y++){
			emptySRV[y + x] = NULL;
		}
		x += 2;
	}
	
	gDeviceContext->OMSetRenderTargets(4, emptyRTV, emptyDSV);
	gDeviceContext->PSSetShaderResources(0, 8, emptySRV);
}

UINT GraphicsManager::getWindowWidth() {
	return windowWidth;
}

UINT GraphicsManager::getWindowHeight() {
	return windowHeight;
}

ID3D11RenderTargetView** GraphicsManager::getBackbuffer() {
	return &gBackbufferRTV;
}

void GraphicsManager::setWindowSize(UINT width = 1280, UINT height = 1024) {
	windowWidth = width;
	windowHeight = height;
}

ID3D11Buffer** GraphicsManager::getQuad() {
	return &gQuadBuffer;
}

void GraphicsManager::createConstantBuffer(string name, const void* data, UINT size) {
	ID3D11Buffer* constantBuffer;

	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = size;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA cbInitData;
	cbInitData.pSysMem = data;
	cbInitData.SysMemPitch = 0;
	cbInitData.SysMemSlicePitch = 0;

	gDevice->CreateBuffer(&desc, &cbInitData, &constantBuffer);

	thesisData.constantBuffers[name] = constantBuffer;
}

void GraphicsManager::createVertexShader(string shaderName, string layoutName, D3D11_INPUT_ELEMENT_DESC* desc, UINT size) {
	// Shader
	ID3DBlob* blob = nullptr;
	ID3D11VertexShader* shader;
	wstring catName = L"Shaders/" + wstring(shaderName.begin(), shaderName.end()) + L".hlsl";

	HRESULT HR = D3DCompileFromFile(catName.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS_main", "vs_5_0", 0, NULL, &blob, nullptr);
	HR = gDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &shader);

	thesisData.vertexShaders[shaderName] = shader;

	// Layout
	ID3D11InputLayout* layout;

	HR = gDevice->CreateInputLayout(desc, size, blob->GetBufferPointer(), blob->GetBufferSize(), &layout);
	blob->Release();

	thesisData.inputLayouts[layoutName] = layout;
}

void GraphicsManager::createDomainShader(string name) {
	ID3DBlob* blob = nullptr;
	ID3D11DomainShader* shader;
	wstring shaderName = L"Shaders/" + wstring(name.begin(), name.end()) + L".hlsl";

	D3DCompileFromFile(shaderName.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "DS_main", "ds_5_0", 0, NULL, &blob, nullptr);
	gDevice->CreateDomainShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &shader);
	blob->Release();

	thesisData.domainShaders[name] = shader;
}

void GraphicsManager::createHullShader(string name) {
	ID3DBlob* blob = nullptr;
	ID3D11HullShader* shader;
	wstring shaderName = L"Shaders/" + wstring(name.begin(), name.end()) + L".hlsl";

	D3DCompileFromFile(shaderName.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "HS_main", "hs_5_0", 0, NULL, &blob, nullptr);
	gDevice->CreateHullShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &shader);
	blob->Release();

	thesisData.hullShaders[name] = shader;
}

void GraphicsManager::createGeometryShader(string name) {
	ID3DBlob* blob = nullptr;
	ID3D11GeometryShader* shader;
	wstring shaderName = L"Shaders/" + wstring(name.begin(), name.end()) + L".hlsl";

	D3DCompileFromFile(shaderName.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "GS_main", "gs_5_0", 0, NULL, &blob, nullptr);
	gDevice->CreateGeometryShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &shader);
	blob->Release();

	thesisData.geometryShaders[name] = shader;
}

void GraphicsManager::createPixelShader(string name) {
	ID3DBlob* blob = nullptr;
	ID3D11PixelShader* shader;
	wstring shaderName = L"Shaders/" + wstring(name.begin(), name.end()) + L".hlsl";

	HRESULT HR = D3DCompileFromFile(shaderName.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS_main", "ps_5_0", 0, NULL, &blob, nullptr);
	HR = gDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &shader);
	blob->Release();

	thesisData.pixelShaders[name] = shader;
}

void GraphicsManager::createComputeShader(string name) {
	ID3DBlob* blob = nullptr;
	ID3D11ComputeShader* shader;
	wstring shaderName = L"Shaders/" + wstring(name.begin(), name.end()) + L".hlsl";

	D3DCompileFromFile(shaderName.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "CS_main", "cs_5_0", 0, NULL, &blob, nullptr);
	gDevice->CreateComputeShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &shader);
	blob->Release();

	thesisData.computeShaders[name] = shader;
}

void GraphicsManager::createTexture2D(
	string name, 
	DXGI_FORMAT format,
	UINT width,
	UINT height,
	bool renderTarget,
	bool shaderResource,
	ID3D11Texture2D* texture)
{

	if (renderTarget == true || shaderResource == true) {
		if (texture == nullptr) {
			D3D11_TEXTURE2D_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Width = width;
			desc.Height = height;
			desc.MipLevels = 0;
			desc.ArraySize = 1;
			desc.Format = format;
			desc.SampleDesc.Count = 1;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = 0;
			desc.MiscFlags = 0;

			if (renderTarget == false)
				desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			if (shaderResource == false)
				desc.BindFlags = D3D11_BIND_RENDER_TARGET;

			HRESULT iasdf = gDevice->CreateTexture2D(&desc, nullptr, &texture); // add subresource
		}

		if (renderTarget == true) {
			ID3D11RenderTargetView* rtv;

			D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
			rtvDesc.Format = format;
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			rtvDesc.Texture2D.MipSlice = 0;

			HRESULT hrr = gDevice->CreateRenderTargetView(texture, &rtvDesc, &rtv);

			thesisData.renderTargetViews[name] = rtv;
		}

		if (shaderResource == true) {
			ID3D11ShaderResourceView* srv;

			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
			srvDesc.Format = format;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MostDetailedMip = 0;
			srvDesc.Texture2D.MipLevels = 1;

			HRESULT qweq = gDevice->CreateShaderResourceView(texture, &srvDesc, &srv);

			thesisData.shaderResourceViews[name] = srv;
		}

		thesisData.textures[name] = texture;
	}
}

void GraphicsManager::createSamplerState(
	string name, 
	D3D11_FILTER filter,
	D3D11_TEXTURE_ADDRESS_MODE mode)
{
	ID3D11SamplerState* sampler;

	D3D11_SAMPLER_DESC desc;
	desc.Filter = filter;
	desc.AddressU = mode;
	desc.AddressV = mode;
	desc.AddressW = mode;
	desc.MipLODBias = 0.0f;
	desc.MaxAnisotropy = 1;
	desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	desc.BorderColor[0] = 1.0f;
	desc.BorderColor[1] = 1.0f;
	desc.BorderColor[2] = 1.0f;
	desc.BorderColor[3] = 1.0f;
	desc.MinLOD = -D3D11_FLOAT32_MAX;
	desc.MaxLOD = D3D11_FLOAT32_MAX;

	gDevice->CreateSamplerState(&desc, &sampler);

	thesisData.samplerStates[name] = sampler;
}

void GraphicsManager::attachImage(string textureName, string srvName) {
	string cat = "Assets/" + textureName;
	if (thesisData.shaderResourceViews[srvName] != nullptr)
		thesisData.shaderResourceViews[srvName]->Release();
	HRESULT HR = CreateWICTextureFromFile(gDevice, wstring(cat.begin(), cat.end()).c_str(), nullptr, &thesisData.shaderResourceViews[srvName]);
}

void GraphicsManager::saveImage(string fileName, ID3D11Texture2D* texture2d, const GUID &fileType) {
	string cat = "Assets/" + fileName;
	ScratchImage image;
	CaptureTexture(gDevice, gDeviceContext, texture2d, image);
	const Image* img = image.GetImage(0, 0, 0);
	SaveToWICFile(*img, WIC_FLAGS_NONE, fileType, wstring(cat.begin(), cat.end()).c_str());
	
}

void GraphicsManager::generateMips(string inputTexture, string outputSRV) {
	ScratchImage rtvScratch;
	HRESULT aaaa = CaptureTexture(gDevice, gDeviceContext, thesisData.textures[inputTexture], rtvScratch);

	ScratchImage mipScratch;
	HRESULT qwerty = GenerateMipMaps(rtvScratch.GetImages(), rtvScratch.GetImageCount(), rtvScratch.GetMetadata(), TEX_FILTER_FANT, 0, mipScratch);

	if (thesisData.shaderResourceViews[outputSRV])
		thesisData.shaderResourceViews[outputSRV]->Release();

	HRESULT asdfg = CreateShaderResourceView(gDevice, mipScratch.GetImages(), mipScratch.GetImageCount(), mipScratch.GetMetadata(), &thesisData.shaderResourceViews[outputSRV]);
}