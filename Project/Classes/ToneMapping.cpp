#include "ToneMapping.h"
#include "..\ApplicationContext.h"
#include "GraphicsManager.h"

ToneMapping::ToneMapping()
{
	// ########################################################
	// ## Change width/height on SRV/RTV to match image size ##
	// ########################################################
}

ToneMapping::~ToneMapping()
{

}

void ToneMapping::Render() {
	// STANDARD SETTINGS
	struct cBuffer {
		XMINT4 mipLevel;
	}mipBuffer;

	deviceContext->IASetInputLayout(resources.inputLayouts["REINHARD_Layout"]);
	deviceContext->IASetVertexBuffers(0, 1, manager->getQuad(), &vertexSize, &offset);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	deviceContext->VSSetShader(resources.vertexShaders["REINHARD_VertexShader"], nullptr, 0);
	deviceContext->PSSetShader(resources.pixelShaders["REINHARD_PixelShader"], nullptr, 0);
	deviceContext->PSSetSamplers(0, 1, &resources.samplerStates["REINHARD_SamplerWrap"]);
	deviceContext->PSSetConstantBuffers(0, 1, &resources.constantBuffers["REINHARD_ConstantBuffer"]);

	manager->attachImage("ToneMapping/Reinhard/texttest3.tif", "REINHARD_SRV");
	//manager->attachImage("ToneMapping/Reinhard/The Marble Hall merged HDR.tif", "REINHARD_SRV"); // loads image every frame = superawesomeoptimization
	deviceContext->PSSetShaderResources(0, 1, &resources.shaderResourceViews["REINHARD_SRV"]);


	//// AVG LUMINANCE PASS
	mipBuffer.mipLevel = { textureWidth,	// miplevel
		1,				// Max(0) or Avg(1) Luminance, or final render(2)
		0,				// Unassigned
		0				// Unassigned
	};
	deviceContext->UpdateSubresource(resources.constantBuffers["REINHARD_ConstantBuffer"], 0, nullptr, &mipBuffer, 0, 0);
	deviceContext->OMSetRenderTargets(1, &resources.renderTargetViews["REINHARD_AvgLuminance_SRV_and_RTV"], nullptr);
	deviceContext->Draw(4, 0);
	manager->generateMips("REINHARD_AvgLuminance_SRV_and_RTV", "REINHARD_AvgLuminance_SRV_and_RTV");

	//// FINAL RENDER
	UINT SHIFT = 0x10;
	UINT CTRL = 0x11;
	UINT G = 0x47;
	UINT L = 0x4C;
	UINT ADD = 0x6B;
	UINT SUB = 0x6D;

	//// TOGGLE GLOBAL
	if (GetAsyncKeyState(G)) {
		mipBuffer.mipLevel = { textureWidth,	// miplevel
			2,				// Max(0) or Avg(1) Luminance, or final render(2)
			0,				// Global(0) or Local(1) TMO, or no TMO(2)
			reinhardKey		// Key
		};
	}

	//// TOGGLE LOCAL
	else /*if (GetAsyncKeyState(L)) */{
		mipBuffer.mipLevel = { textureWidth,	// miplevel
			2,				// Max(0) or Avg(1) Luminance, or final render(2)
			1,				// Global(0) or Local(1) TMO, or no TMO(2)
			reinhardKey		// Key
		};
	}

	//// NO TMO
	//else {
	//	mipBuffer.mipLevel = { textureWidth,	// miplevel
	//		2,				// Max(0) or Avg(1) Luminance, or final render(2)
	//		2,				// Global(0) or Local(1) TMO, or no TMO(2)
	//		reinhardKey		// Key
	//	};
	//}

	//// TOGGLE LUMINANCE
	//if (GetAsyncKeyState(SHIFT)) {
	//	mipBuffer.mipLevel.z += 3;
	//}

	//// ADJUST KEY
	//if (GetAsyncKeyState(ADD)) {
	//	if (GetAsyncKeyState(CTRL))
	//		reinhardKey += 10;
	//	else
	//		reinhardKey += 1;
	//}
	//if (GetAsyncKeyState(SUB)) {
	//	if (GetAsyncKeyState(CTRL))
	//		reinhardKey -= 10;
	//	else
	//		reinhardKey -= 1;
	//}

	deviceContext->UpdateSubresource(resources.constantBuffers["REINHARD_ConstantBuffer"], 0, nullptr, &mipBuffer, 0, 0);
	deviceContext->OMSetRenderTargets(1, manager->getBackbuffer(), nullptr);
	deviceContext->PSSetShaderResources(1, 1, &resources.shaderResourceViews["REINHARD_AvgLuminance_SRV_and_RTV"]);

	deviceContext->Draw(4, 0);
	manager->saveImage("ToneMapping/Reinhard/Local.png", manager->pBackBuffer);

	//if (manager->frameCount < 100) {
	//	manager->saveImage("../Results/image" + to_string(manager->frameCount) + ".png", manager->pBackBuffer);
	//	manager->frameCount++;
	//}
	//else {
	//	PostQuitMessage(0);
	//}
}

void ToneMapping::Initialize() {
	manager = ApplicationContext::GetInstance().GetGraphicsManager();
	textureWidth = int32_t(log(manager->windowWidth) / log(2));

	// #### CONSTANT BUFFER
	struct cBuffer {
		XMINT4 mipLevel = { 0, 0, 0, 0 };
	}mipBuffer;

	mipBuffer.mipLevel = { textureWidth, 1, 0, 0 };
	manager->createConstantBuffer("REINHARD_ConstantBuffer", &mipBuffer, sizeof(cBuffer));

	// #### VERTEX SHADER
	D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	manager->createVertexShader("REINHARD_VertexShader", "REINHARD_Layout", layoutDesc, ARRAYSIZE(layoutDesc));

	// #### PIXEL SHADER
	manager->createPixelShader("REINHARD_PixelShader"); // Name has to match shader name without .hlsl

														// #### SRV
	manager->createTexture2D("REINHARD_SRV", DXGI_FORMAT_R32G32B32A32_FLOAT, manager->getWindowWidth(), manager->getWindowHeight(), false, true);
	manager->createTexture2D("REINHARD_AvgLuminance_SRV_and_RTV", DXGI_FORMAT_R32G32B32A32_FLOAT, manager->getWindowWidth(), manager->getWindowHeight(), true, true);

	// #### SAMPLER
	manager->createSamplerState("REINHARD_SamplerWrap", D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP);
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