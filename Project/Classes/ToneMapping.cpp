#include "ToneMapping.h"
#include "..\ApplicationContext.h"
#include "GraphicsManager.h"

ToneMapping::ToneMapping()
{

}

ToneMapping::~ToneMapping()
{

}

void ToneMapping::Render() {
	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	UINT vertexSize = sizeof(float) * 5;
	UINT offset = 0;
	Sleep(1000);
	deviceContext->OMSetRenderTargets(1, manager->getBackbuffer(), nullptr);
	deviceContext->ClearRenderTargetView(*manager->getBackbuffer(), clearColor);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	deviceContext->IASetInputLayout(resources.inputLayouts["TM_Layout"]);

	deviceContext->VSSetShader(resources.vertexShaders["TM_VertexShader"], nullptr, 0);
	deviceContext->PSSetShader(resources.pixelShaders["TM_PixelShader"], nullptr, 0);

	if (imageCount < 9)
		imageWithZero = "ToneMapping/KHK/SceneWithLightning.00";
	else if (imageCount > 9 && imageCount < 100)
		imageWithZero = "ToneMapping/KHK/SceneWithLightning.0";
	else
		imageWithZero = "ToneMapping/KHK/SceneWithLightning.";

	string cat = imageWithZero + to_string(imageCount) + ".png";

	manager->attachImage(cat, "FirstSRV");
	deviceContext->PSSetShaderResources(0, 1, &resources.shaderResourceViews["FirstSRV"]);

	deviceContext->PSSetSamplers(0, 1, &resources.samplerStates["SamplerWrap"]);

	deviceContext->IASetVertexBuffers(0, 1, manager->getQuad(), &vertexSize, &offset);

	deviceContext->Draw(4, 0);

	imageCount++;
	if (imageCount == 56)
		imageCount = 0;
	//manager->saveImage("ToneMapping/OutputImages/image.png", manager->pBackBuffer);
}

void ToneMapping::Initialize() {
	// ###########################################################
	// ######				Constant buffer					######
	// ###########################################################
	//	void createConstantBuffer(
	//		string name,
	//		D3D11_BUFFER_DESC desc,
	//		const void* data
	//	);
	manager = ApplicationContext::GetInstance().GetGraphicsManager();
	struct cBuffer {
		XMFLOAT4X4 matrix;
	}myMatrix;

	manager->createConstantBuffer("myMatrix", &myMatrix, sizeof(cBuffer));



	// ###########################################################
	// ######				Vertex Shader					######
	// ###########################################################
	//	void createVertexShader(
	//		string shaderName,
	//		string layoutName,
	//		D3D11_INPUT_ELEMENT_DESC* desc,
	//		UINT size);

	D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	manager->createVertexShader("TM_VertexShader", "TM_Layout", layoutDesc, ARRAYSIZE(layoutDesc));



	// ###########################################################
	// ######				Other Shaders					######
	// ###########################################################
	//	void createPixelShader(
	//		string name
	//			);

	manager->createPixelShader("TM_PixelShader"); // Name has to match shader name without .hlsl



												  // ###########################################################
												  // ######		Render target & shader resource			######
												  // ###########################################################
												  //	void createTexture2D(
												  //		string name,
												  //		DXGI_FORMAT format = DXGI_FORMAT_R32G32B32A32_FLOAT,
												  //		UINT width = GraphicsManager::getInstance().getWindowWidth(),
												  //		UINT height = GraphicsManager::getInstance().getWindowHeight(),
												  //		bool renderTarget = true,
												  //		bool shaderResource = true
												  //	);

												  // Only RTV
	manager->createTexture2D(
		"FirstRTV",
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		manager->getWindowWidth(),
		manager->getWindowHeight(),
		true,
		false
	);

	// Only SRV
	manager->createTexture2D(
		"FirstSRV",
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		manager->getWindowWidth(),
		manager->getWindowHeight(),
		false,
		true
	);

	// Both
	manager->createTexture2D("FirstSRVRTV");

	// Add image on an SRV (base filepath will be set to the assets folder automatically)
	manager->attachImage("ToneMapping/picture.png", "FirstSRV");



	// ###########################################################
	// ######		Render target & shader resource			######
	// ###########################################################
	//	void createSamplerState(
	//		string name,
	//		D3D11_FILTER filter = D3D11_FILTER_MIN_MAG_MIP_POINT,
	//		D3D11_TEXTURE_ADDRESS_MODE mode = D3D11_TEXTURE_ADDRESS_CLAMP
	//	);

	manager->createSamplerState("SamplerWrap", D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP);
}