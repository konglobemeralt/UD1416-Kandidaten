#include "ToneMapping.h"
#include "..\ApplicationContext.h"
#include "GraphicsManager.h"

ToneMapping::ToneMapping()
{
	// ########################################################
	// ## Change width/height on SRV/RTV to match image size ##
	// ########################################################
	technique = GAO;
}

ToneMapping::~ToneMapping()
{

}

void ToneMapping::renderGao() {
	deviceContext->OMSetRenderTargets(1, &resources.renderTargetViews["GAO_Luminance_RTV"], nullptr);
	//deviceContext->ClearRenderTargetView(*manager->getBackbuffer(), clearColor);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	deviceContext->IASetInputLayout(resources.inputLayouts["GAO_Layout"]);
	deviceContext->PSSetSamplers(0, 1, &resources.samplerStates["GAO_SamplerWrap"]);

	deviceContext->VSSetShader(resources.vertexShaders["GAO_VertexShader"], nullptr, 0);
	deviceContext->PSSetShader(resources.pixelShaders["GAO_PixelShader"], nullptr, 0);

	manager->attachImage("ToneMapping/Gao/inputImage.tif", "GAO_SRV");
	deviceContext->PSSetShaderResources(0, 1, &resources.shaderResourceViews["GAO_SRV"]);
	deviceContext->IASetVertexBuffers(0, 1, manager->getQuad(), &vertexSize, &offset);

	deviceContext->Draw(4, 0);

	// MIPS
	deviceContext->OMSetRenderTargets(1, manager->getBackbuffer(), nullptr);

	manager->generateMips("GAO_Luminance_RTV", "GAO_Luminance_SRV");

	XMINT4 newMip = { textureWidth, 0, 0, 0 };
	deviceContext->UpdateSubresource(resources.constantBuffers["GAO_ConstantBuffer"], 0, nullptr, &newMip, 0, 0);

	deviceContext->PSSetShader(resources.pixelShaders["GAO_FinalPixelShader"], nullptr, 0);
	deviceContext->PSSetShaderResources(0, 1, &resources.shaderResourceViews["GAO_Luminance_SRV"]);
	deviceContext->PSSetConstantBuffers(0, 1, &resources.constantBuffers["GAO_ConstantBuffer"]);

	deviceContext->Draw(4, 0);

	//manager->saveImage("ToneMapping/Gao/outputImage.png", manager->pBackBuffer);
}

void ToneMapping::initGao() {
	manager = ApplicationContext::GetInstance().GetGraphicsManager();
	textureWidth = int32_t(log(manager->windowWidth) / log(2));

	// #### CONSTANT BUFFER
	struct cBuffer {
		XMINT4 mipLevel;
	}mipBuffer;

	manager->createConstantBuffer("GAO_ConstantBuffer", &mipBuffer, sizeof(cBuffer));

	// #### VERTEX SHADER
	D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	manager->createVertexShader("GAO_VertexShader", "GAO_Layout", layoutDesc, ARRAYSIZE(layoutDesc));

	// #### PIXEL SHADER
	manager->createPixelShader("GAO_PixelShader"); // Name has to match shader name without .hlsl
	manager->createPixelShader("GAO_FinalPixelShader"); // Name has to match shader name without .hlsl

	 // #### SRV
	manager->createTexture2D("GAO_SRV", DXGI_FORMAT_R32G32B32A32_FLOAT, manager->getWindowWidth(), manager->getWindowHeight(), false, true);
	manager->createTexture2D("GAO_Luminance_SRV", DXGI_FORMAT_R32G32B32A32_FLOAT, manager->getWindowWidth(), manager->getWindowHeight(), false, true);
	manager->createTexture2D("GAO_Luminance_RTV", DXGI_FORMAT_R32G32B32A32_FLOAT, manager->getWindowWidth(), manager->getWindowHeight(), true, true);

	// #### SAMPLER
	manager->createSamplerState("GAO_SamplerWrap", D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP);

	// #### OTHER
	//deviceContext->GenerateMips(resources.shaderResourceViews["GAO_Luminance_RTVSRVMIP"]);

	//D3D11_MAPPED_SUBRESOURCE mipSubresource;
	//deviceContext->Map(resources.textures["GAO_Luminance_RTVSRVMIP"], log(manager->windowWidth) / log(2), D3D11_MAP_READ, 0, &mipSubresource);

}

void ToneMapping::renderMeylan() {
	deviceContext->OMSetRenderTargets(1, manager->getBackbuffer(), nullptr);
	deviceContext->ClearRenderTargetView(*manager->getBackbuffer(), clearColor);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	deviceContext->IASetInputLayout(resources.inputLayouts["MEYLAN_Layout"]);

	deviceContext->VSSetShader(resources.vertexShaders["MEYLAN_VertexShader"], nullptr, 0);
	deviceContext->PSSetShader(resources.pixelShaders["MEYLAN_PixelShader"], nullptr, 0);

	manager->attachImage("ToneMapping/Meylan/inputImage.tif", "MEYLAN_SRV");
	deviceContext->PSSetShaderResources(0, 1, &resources.shaderResourceViews["MEYLAN_SRV"]);
	deviceContext->PSSetSamplers(0, 1, &resources.samplerStates["MEYLAN_SamplerWrap"]);
	deviceContext->IASetVertexBuffers(0, 1, manager->getQuad(), &vertexSize, &offset);

	deviceContext->Draw(4, 0);

	manager->saveImage("ToneMapping/Meylan/outputImage.png", manager->pBackBuffer);
}

void ToneMapping::initMeylan() {
	// #### CONSTANT BUFFER
	manager = ApplicationContext::GetInstance().GetGraphicsManager();
	struct cBuffer {
		XMFLOAT4X4 matrix;
	}myMatrix;

	manager->createConstantBuffer("MEYLAN_constantBuffer", &myMatrix, sizeof(cBuffer));

	// #### VERTEX SHADER
	D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	manager->createVertexShader("MEYLAN_VertexShader", "MEYLAN_Layout", layoutDesc, ARRAYSIZE(layoutDesc));

	// #### PIXEL SHADER
	manager->createPixelShader("MEYLAN_PixelShader"); // Name has to match shader name without .hlsl

													  // #### SRV
	manager->createTexture2D(
		"MEYLAN_SRV",
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		manager->getWindowWidth(),
		manager->getWindowHeight(),
		false,
		true
	);

	// #### SAMPLER
	manager->createSamplerState("MEYLAN_SamplerWrap", D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP);
}

void ToneMapping::renderReinhard() {
	deviceContext->OMSetRenderTargets(1, manager->getBackbuffer(), nullptr);
	deviceContext->ClearRenderTargetView(*manager->getBackbuffer(), clearColor);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	deviceContext->IASetInputLayout(resources.inputLayouts["REINHARD_Layout"]);

	deviceContext->VSSetShader(resources.vertexShaders["REINHARD_VertexShader"], nullptr, 0);
	deviceContext->PSSetShader(resources.pixelShaders["REINHARD_PixelShader"], nullptr, 0);

	manager->attachImage("ToneMapping/Reinhard/texttest3.tif", "REINHARD_SRV");
	//manager->attachImage("ToneMapping/Reinhard/The Marble Hall merged HDR.tif", "REINHARD_SRV"); // loads image every frame = superawesomeoptimization
	deviceContext->PSSetShaderResources(0, 1, &resources.shaderResourceViews["REINHARD_SRV"]);
	deviceContext->PSSetSamplers(0, 1, &resources.samplerStates["REINHARD_SamplerWrap"]);
	deviceContext->IASetVertexBuffers(0, 1, manager->getQuad(), &vertexSize, &offset);

	//// AVG LUMINANCE PASS
	deviceContext->Draw(4, 0);

	//ID3D11Resource* cpuResource;
	//ID3D11Resource* avgResource;
	//resources.shaderResourceViews["REINHARD_CPU_handle"]->GetResource(&cpuResource);
	//resources.shaderResourceViews["REINHARD_AvgLuminance_SRV_and_RTV"]->GetResource(&avgResource);
	//
	//deviceContext->CopyResource(cpuResource, avgResource);
	//char* pData = nullptr;
	//REFGUID ref = GUID();
	//HRESULT hr = cpuResource->GetPrivateData(ref, 0, pData);

	//// FINAL RENDER

	UINT SHIFT	= 0x10;
	UINT CTRL	= 0x11;
	UINT G		= 0x47;
	UINT L		= 0x4C;
	UINT ADD	= 0x6B;
	UINT SUB	= 0x6D;

	// TOGGLE GLOBAL
	if (GetAsyncKeyState(G)) {
		mipBuffer.mipLevel = {	textureWidth,	// miplevel
								2,				// Max(0) or Avg(1) Luminance, or final render(2)
								0,				// Global(0) or Local(1) TMO, or no TMO(2)
								reinhardKey		// Key
		};
	}
	// TOGGLE LOCAL
	else if (GetAsyncKeyState(L)) {
		mipBuffer.mipLevel = {	textureWidth,	// miplevel
								2,				// Max(0) or Avg(1) Luminance, or final render(2)
								1,				// Global(0) or Local(1) TMO, or no TMO(2)
								reinhardKey		// Key
		};
	}
	// NO TMO
	else {
		mipBuffer.mipLevel = {	textureWidth,	// miplevel
								2,				// Global(0) or Local(1) TMO, or no TMO(2)
								reinhardKey		// Key
		};
	}

	// TOGGLE LUMINANCE
	if (GetAsyncKeyState(SHIFT)) {
		mipBuffer.mipLevel.z += 3;
	}

	// ADJUST KEY
	if (GetAsyncKeyState(ADD)) {
		if (GetAsyncKeyState(CTRL))
			reinhardKey += 10;
		else
			reinhardKey += 1;
	}
	if (GetAsyncKeyState(SUB)) {
		if (GetAsyncKeyState(CTRL))
			reinhardKey -= 10;
		else
			reinhardKey -= 1;
	}

	manager->saveImage("ToneMapping/Reinhard/Local.png", manager->pBackBuffer);
}

void ToneMapping::initReinhard() {
	// #### CONSTANT BUFFER
	manager = ApplicationContext::GetInstance().GetGraphicsManager();
	struct cBuffer {
		XMFLOAT4X4 matrix;
	}myMatrix;

	manager->createConstantBuffer("REINHARD_constantBuffer", &myMatrix, sizeof(cBuffer));

	// #### VERTEX SHADER
	D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	manager->createVertexShader("REINHARD_VertexShader", "REINHARD_Layout", layoutDesc, ARRAYSIZE(layoutDesc));

	// #### PIXEL SHADER
	manager->createPixelShader("REINHARD_PixelShader"); // Name has to match shader name without .hlsl

	//D3D11_TEXTURE2D_DESC desc;
	//ZeroMemory(&desc, sizeof(desc));
	//desc.Width = 1;
	//desc.Height = 1;
	//desc.MipLevels = 0;
	//desc.ArraySize = 1;
	//desc.Format = DXGI_FORMAT_R32_FLOAT;
	//desc.SampleDesc.Count = 1;
	//desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	//desc.Usage = D3D11_USAGE_STAGING;
	//desc.CPUAccessFlags = 0;
	//desc.MiscFlags = 0;

	//HRESULT iasdf = device->CreateTexture2D(&desc, nullptr, &reinhardCPUtext); // add subresource

													  // #### SRV
	manager->createTexture2D("REINHARD_SRV",						DXGI_FORMAT_R32G32B32A32_FLOAT, manager->getWindowWidth(),	manager->getWindowHeight(), false,	true);
	manager->createTexture2D("REINHARD_AvgLuminance_SRV_and_RTV",	DXGI_FORMAT_R32G32B32A32_FLOAT, manager->getWindowWidth(),	manager->getWindowHeight(), true,	true);
	//manager->createTexture2D("REINHARD_CPU_handle",					DXGI_FORMAT_R32G32B32A32_FLOAT, manager->getWindowWidth(), manager->getWindowHeight(),	false,	true);
	//manager->createTexture2D("REINHARD_MaxLuminance_SRV",			DXGI_FORMAT_R32_FLOAT,			1,							1,							false,	true, reinhardCPUtext);
		manager->getWindowHeight(),
		false,
		true
	);

	// #### SAMPLER
	manager->createSamplerState("REINHARD_SamplerWrap", D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP);
}

void ToneMapping::Initialize() {
	switch (technique)
	{
	case GAO:
		initGao();
		break;
	case MEYLAN:
		initMeylan();
		break;
	case REINHARD:
		initReinhard();
		break;
	default:
		break;
	}
}

void ToneMapping::Render() {
	switch (technique)
	{
	case GAO:
		renderGao();
		break;
	case MEYLAN:
		renderMeylan();
		break;
	case REINHARD:
		renderReinhard();
		break;
	default:
		break;
	}
}

//void ToneMapping::Initialize() {
//	// ###########################################################
//	// ######				Constant buffer					######
//	// ###########################################################
//	//	void createConstantBuffer(
//	//		string name,
//	//		D3D11_BUFFER_DESC desc,
//	//		const void* data
//	//	);
//	manager = ApplicationContext::GetInstance().GetGraphicsManager();
//	struct cBuffer {
//		XMFLOAT4X4 matrix;
//	}myMatrix;
//
//	manager->createConstantBuffer("myMatrix", &myMatrix, sizeof(cBuffer));
//
//
//
//	// ###########################################################
//	// ######				Vertex Shader					######
//	// ###########################################################
//	//	void createVertexShader(
//	//		string shaderName,
//	//		string layoutName,
//	//		D3D11_INPUT_ELEMENT_DESC* desc,
//	//		UINT size);
//
//	D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
//		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//	};
//
//	manager->createVertexShader("TM_VertexShader", "TM_Layout", layoutDesc, ARRAYSIZE(layoutDesc));
//
//
//
//	// ###########################################################
//	// ######				Other Shaders					######
//	// ###########################################################
//	//	void createPixelShader(
//	//		string name
//	//			);
//
//	manager->createPixelShader("TM_PixelShader"); // Name has to match shader name without .hlsl
//
//
//
//												  // ###########################################################
//												  // ######		Render target & shader resource			######
//												  // ###########################################################
//												  //	void createTexture2D(
//												  //		string name,
//												  //		DXGI_FORMAT format = DXGI_FORMAT_R32G32B32A32_FLOAT,
//												  //		UINT width = GraphicsManager::getInstance().getWindowWidth(),
//												  //		UINT height = GraphicsManager::getInstance().getWindowHeight(),
//												  //		bool renderTarget = true,
//												  //		bool shaderResource = true
//												  //	);
//
//												  // Only RTV
//	manager->createTexture2D(
//		"FirstRTV",
//		DXGI_FORMAT_R32G32B32A32_FLOAT,
//		manager->getWindowWidth(),
//		manager->getWindowHeight(),
//		true,
//		false
//	);
//
//	// Only SRV
//	manager->createTexture2D(
//		"FirstSRV",
//		DXGI_FORMAT_R32G32B32A32_FLOAT,
//		manager->getWindowWidth(),
//		manager->getWindowHeight(),
//		false,
//		true
//	);
//
//	// Both
//	manager->createTexture2D("FirstSRVRTV");
//
//	// Add image on an SRV (base filepath will be set to the assets folder automatically)
//	//manager->attachImage("ToneMapping/test8k.jpg", "FirstSRV");
//
//
//
//	// ###########################################################
//	// ######		Render target & shader resource			######
//	// ###########################################################
//	//	void createSamplerState(
//	//		string name,
//	//		D3D11_FILTER filter = D3D11_FILTER_MIN_MAG_MIP_POINT,
//	//		D3D11_TEXTURE_ADDRESS_MODE mode = D3D11_TEXTURE_ADDRESS_CLAMP
//	//	);
//
//	manager->createSamplerState("SamplerWrap", D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP);
//}