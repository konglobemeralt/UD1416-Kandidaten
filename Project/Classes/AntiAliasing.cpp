#include "AntiAliasing.h"

AntiAliasing::AntiAliasing() {}

AntiAliasing::~AntiAliasing() {}

void AntiAliasing::Render() {
	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	UINT vertexSize = sizeof(float) * 5;
	UINT offset = 0;

	deviceContext->OMSetRenderTargets(1, manager.getBackbuffer(), nullptr);
	deviceContext->ClearRenderTargetView(*manager.getBackbuffer(), clearColor);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	deviceContext->IASetInputLayout(resources.inputLayouts["AASimpleLayout"]);
	deviceContext->PSSetSamplers(0, 1, &resources.samplerStates["AAClampSampler"]);

	deviceContext->VSSetConstantBuffers(0, 1, &resources.constantBuffers["Simple_VS_cb"]);
	deviceContext->PSSetConstantBuffers(0, 1, &resources.constantBuffers["FXAA_PS_cb"]);

	deviceContext->PSSetShaderResources(0, 1, &resources.shaderResourceViews["FXAA_Test"]);

	deviceContext->VSSetShader(resources.vertexShaders["SimpleVertexShader"], nullptr, 0);
	deviceContext->PSSetShader(resources.pixelShaders["FXAA_PS"], nullptr, 0);

	deviceContext->IASetVertexBuffers(0, 1, manager.getQuad(), &vertexSize, &offset);

	deviceContext->Draw(4, 0);
}

void AntiAliasing::Initialize() {
	// ###########################################################
	// ######				Constant buffer					######
	// ###########################################################
	//	void createConstantBuffer(
	//		string name,
	//		D3D11_BUFFER_DESC desc,
	//		const void* data
	//	);

	struct Simple_VS_ConstantBuffer { //view space typ
		XMFLOAT4X4 matrix;
	}Simple_VS_cb;

	manager.createConstantBuffer("Simple_VS_cb", &Simple_VS_cb, sizeof(Simple_VS_ConstantBuffer));


	struct FXAA_PS_ConstantBuffer { //texelsize n shiet
		XMFLOAT2 texelSizeXY;
		float FXAA_blur_Texels_Threshhold = 8.0f; //hur många texlar som kommer blurras åt varje håll
	}FXAA_PS_cb;

	manager.createConstantBuffer("FXAA_PS_cb", &FXAA_PS_cb, sizeof(FXAA_PS_ConstantBuffer));



	// ###########################################################
	// ######				Vertex Shader					######
	// ###########################################################
	//	void createVertexShader(
	//		string shaderName,
	//		string layoutName,
	//		D3D11_INPUT_ELEMENT_DESC* desc,
	//		UINT size);

	D3D11_INPUT_ELEMENT_DESC AASimpleLayout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	manager.createVertexShader("SimpleVertexShader", "AASimpleLayout", AASimpleLayout, ARRAYSIZE(AASimpleLayout)); //vertexshader och och layout


	manager.createPixelShader("FXAA_PS"); // Name has to match shader name without .hlsl



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

	manager.createTexture2D( //shaderresource
		"FXAA_Test",
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		manager.getWindowWidth(),
		manager.getWindowHeight(),
		false,
		true
	);

	manager.attachImage("AntiAliasing/Images/AATest.png", "FXAA_Test"); //attachea till shaderresourcen
	
	// ###########################################################
	// ######		Render target & shader resource			######
	// ###########################################################
	//	void createSamplerState(
	//		string name,
	//		D3D11_FILTER filter = D3D11_FILTER_MIN_MAG_MIP_POINT,
	//		D3D11_TEXTURE_ADDRESS_MODE mode = D3D11_TEXTURE_ADDRESS_CLAMP
	//	);

	manager.createSamplerState("AAWrapSampler", D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP);
	manager.createSamplerState("AAClampSampler", D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_CLAMP);
}

void AntiAliasing::InitExample()
{
	// ###########################################################
	// ######				Constant buffer					######
	// ###########################################################
	//	void createConstantBuffer(
	//		string name,
	//		D3D11_BUFFER_DESC desc,
	//		const void* data
	//	);

	struct cBuffer {
		XMFLOAT4X4 matrix;
	}myMatrix;

	manager.createConstantBuffer("myMatrix", &myMatrix, sizeof(cBuffer));



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

	manager.createVertexShader("VertexShader", "FirstLayout", layoutDesc, ARRAYSIZE(layoutDesc));



	// ###########################################################
	// ######				Other Shaders					######
	// ###########################################################
	//	void createPixelShader(
	//		string name
	//			);

	manager.createPixelShader("PixelShader"); // Name has to match shader name without .hlsl



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
	manager.createTexture2D(
		"myRTV",
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		manager.getWindowWidth(),
		manager.getWindowHeight(),
		true,
		false
	);

	// Only SRV
	manager.createTexture2D(
		"mySRV",
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		manager.getWindowWidth(),
		manager.getWindowHeight(),
		true,
		false
	);

	// Both
	manager.createTexture2D("myRTVandSRV");

	// Add image on an SRV (base filepath will be set to the assets folder automatically)
	manager.attachImage("ToneMapping/Images/picture.jpg", "mySRV");



	// ###########################################################
	// ######		Render target & shader resource			######
	// ###########################################################
	//	void createSamplerState(
	//		string name,
	//		D3D11_FILTER filter = D3D11_FILTER_MIN_MAG_MIP_POINT,
	//		D3D11_TEXTURE_ADDRESS_MODE mode = D3D11_TEXTURE_ADDRESS_CLAMP
	//	);

	manager.createSamplerState("CoolSampler", D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP);
}