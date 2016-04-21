#include "ToneMapping.h"
#include "..\ApplicationContext.h"
#include "GraphicsManager.h"

ToneMapping::ToneMapping()
{
	technique = REINHARD;
}

ToneMapping::~ToneMapping()
{
	
}

void ToneMapping::renderGao() {
	deviceContext->OMSetRenderTargets(1, manager->getBackbuffer(), nullptr);
	deviceContext->ClearRenderTargetView(*manager->getBackbuffer(), clearColor);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	deviceContext->IASetInputLayout(resources.inputLayouts["GAO_Layout"]);

	deviceContext->VSSetShader(resources.vertexShaders["GAO_VertexShader"], nullptr, 0);
	deviceContext->PSSetShader(resources.pixelShaders["GAO_PixelShader"], nullptr, 0);

	manager->attachImage("ToneMapping/Gao/inputImage.tif", "GAO_SRV");
	deviceContext->PSSetShaderResources(0, 1, &resources.shaderResourceViews["GAO_SRV"]);
	deviceContext->PSSetSamplers(0, 1, &resources.samplerStates["GAO_SamplerWrap"]);
	deviceContext->IASetVertexBuffers(0, 1, manager->getQuad(), &vertexSize, &offset);

	deviceContext->Draw(4, 0);

	manager->saveImage("ToneMapping/Gao/outputImage.png", manager->pBackBuffer);
}

void ToneMapping::initGao() {
	// #### CONSTANT BUFFER
	manager = ApplicationContext::GetInstance().GetGraphicsManager();
	struct cBuffer {
		XMFLOAT4X4 matrix;
	}myMatrix;

	manager->createConstantBuffer("GAO_constantBuffer", &myMatrix, sizeof(cBuffer));

	// #### VERTEX SHADER
	D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	manager->createVertexShader("GAO_VertexShader", "GAO_Layout", layoutDesc, ARRAYSIZE(layoutDesc));

	// #### PIXEL SHADER
	manager->createPixelShader("GAO_PixelShader"); // Name has to match shader name without .hlsl

												   // #### SRV
	manager->createTexture2D(
		"GAO_SRV",
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		manager->getWindowWidth(),
		manager->getWindowHeight(),
		false,
		true
	);

	// #### SAMPLER
	manager->createSamplerState("GAO_SamplerWrap", D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP);
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

	manager->attachImage("ToneMapping/Reinhard/inputImage.tif", "REINHARD_SRV");
	deviceContext->PSSetShaderResources(0, 1, &resources.shaderResourceViews["REINHARD_SRV"]);
	deviceContext->PSSetSamplers(0, 1, &resources.samplerStates["REINHARD_SamplerWrap"]);
	deviceContext->IASetVertexBuffers(0, 1, manager->getQuad(), &vertexSize, &offset);

	deviceContext->Draw(4, 0);

	manager->saveImage("ToneMapping/Reinhard/outputImage.png", manager->pBackBuffer);
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

													  // #### SRV
	manager->createTexture2D(
		"REINHARD_SRV",
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		manager->getWindowWidth(),
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