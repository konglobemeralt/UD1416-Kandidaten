#include "AntiAliasing.h"
#include "GraphicsManager.h"
#include "..\ApplicationContext.h"

AntiAliasing::AntiAliasing()
{

}
AntiAliasing::~AntiAliasing()
{

}



void AntiAliasing::Render() {
	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	UINT vertexSize = sizeof(float) * 5;
	UINT offset = 0;

	gdeviceContext->OMSetRenderTargets(1, m_graphicsManager->getBackbuffer(), nullptr); //sätt in rendertarget här om man nu vill skriva till texture!
	gdeviceContext->ClearRenderTargetView(*m_graphicsManager->getBackbuffer(), clearColor);

	gdeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	gdeviceContext->IASetInputLayout(m_graphicsManager->thesisData.inputLayouts["AASimpleLayout"]);

	gdeviceContext->PSSetSamplers(0, 1, &m_graphicsManager->thesisData.samplerStates["AAWrapSampler"]);
	gdeviceContext->PSSetSamplers(1, 1, &m_graphicsManager->thesisData.samplerStates["AAClampSampler"]);

	gdeviceContext->VSSetConstantBuffers(0, 1, &m_graphicsManager->thesisData.constantBuffers["Simple_VS_cb"]);
	gdeviceContext->PSSetConstantBuffers(0, 1, &m_graphicsManager->thesisData.constantBuffers["FXAA_PS_cb"]);

	gdeviceContext->PSSetShaderResources(0, 1, &m_graphicsManager->thesisData.shaderResourceViews["SSAA_Test"]);

	gdeviceContext->VSSetShader(m_graphicsManager->thesisData.vertexShaders["SimpleVertexShader"], nullptr, 0);
	gdeviceContext->PSSetShader(m_graphicsManager->thesisData.pixelShaders["SSAA_PS"], nullptr, 0);

	gdeviceContext->IASetVertexBuffers(0, 1, m_graphicsManager->getQuad(), &vertexSize, &offset);

	gdeviceContext->Draw(4, 0);
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
	m_graphicsManager = ApplicationContext::GetInstance().GetGraphicsManager();
	struct Simple_VS_ConstantBuffer { //view space typ
		XMFLOAT4X4 matrix;
	}Simple_VS_cb;

	m_graphicsManager->createConstantBuffer("Simple_VS_cb", &Simple_VS_cb, sizeof(Simple_VS_ConstantBuffer));


	struct FXAA_PS_ConstantBuffer { //texelsize n shiet
		XMFLOAT2 texelSizeXY;
		float FXAA_blur_Texels_Threshhold; //hur många texlar som kommer blurras åt varje håll
		float minimumBlurThreshhold; //hur mycket som krävs för att den ens ska blurra
		float FXAA_reduce_MULTIPLIER;
		float FXAA_reduce_MIN; //så dirOffset inte ska bli noll
		XMFLOAT2 pad;
	}FXAA_PS_cb;	

	m_graphicsManager->createConstantBuffer("FXAA_PS_cb", &FXAA_PS_cb, sizeof(FXAA_PS_ConstantBuffer));

	FXAA_PS_cb.texelSizeXY.x = 1.0f / m_graphicsManager->getWindowWidth();
	FXAA_PS_cb.texelSizeXY.y = 1.0f / m_graphicsManager->getWindowHeight();
	FXAA_PS_cb.FXAA_blur_Texels_Threshhold = 2.0f;
	FXAA_PS_cb.minimumBlurThreshhold = 0.0001f;
	FXAA_PS_cb.FXAA_reduce_MULTIPLIER = 1.0f / 2.0f;
	FXAA_PS_cb.FXAA_reduce_MIN = 1.0f / 32.0f;
	

	gdeviceContext->UpdateSubresource(m_graphicsManager->thesisData.constantBuffers["FXAA_PS_cb"], 0, NULL, &FXAA_PS_cb, 0, 0);

	// ###########################################################
	// ######				Vertex Shader					######
	// ###########################################################
	//	void createVertexShader(
	//		string shaderName,
	//		string layoutName,
	//		D3D11_INPUT_ELEMENT_DESC* desc,
	//		UINT size);

	D3D11_INPUT_ELEMENT_DESC AASimpleLayout[] = {
		{ "SV_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	m_graphicsManager->createVertexShader("SimpleVertexShader", "AASimpleLayout", AASimpleLayout, ARRAYSIZE(AASimpleLayout)); //vertexshader och och layout


	m_graphicsManager->createPixelShader("FXAA_PS"); // Name has to match shader name without .hlsl

	m_graphicsManager->createPixelShader("SSAA_PS");

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

	m_graphicsManager->createTexture2D( //shaderresource
		"FXAA_Test",
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		m_graphicsManager->getWindowWidth(),
		m_graphicsManager->getWindowHeight(),
		false,
		true
	);

	m_graphicsManager->createTexture2D( //shaderresource
		"SSAA_Test",
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		1600,
		1066,
		false,
		true
	);

	m_graphicsManager->attachImage("AntiAliasing/Images/AATest.png", "FXAA_Test"); //attachea till shaderresourcen
	m_graphicsManager->attachImage("AntiAliasing/Images/SSAATest.jpg", "SSAA_Test");
	// ###########################################################
	// ######		Render target & shader resource			######
	// ###########################################################
	//	void createSamplerState(
	//		string name,
	//		D3D11_FILTER filter = D3D11_FILTER_MIN_MAG_MIP_POINT,
	//		D3D11_TEXTURE_ADDRESS_MODE mode = D3D11_TEXTURE_ADDRESS_CLAMP
	//	);

	m_graphicsManager->createSamplerState("AAWrapSampler", D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP);
	m_graphicsManager->createSamplerState("AAClampSampler", D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_CLAMP);
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
	m_graphicsManager = ApplicationContext::GetInstance().GetGraphicsManager();
	struct cBuffer {
		XMFLOAT4X4 matrix;
	}myMatrix;

	m_graphicsManager->createConstantBuffer("myMatrix", &myMatrix, sizeof(cBuffer));



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

	m_graphicsManager->createVertexShader("VertexShader", "FirstLayout", layoutDesc, ARRAYSIZE(layoutDesc));



	// ###########################################################
	// ######				Other Shaders					######
	// ###########################################################
	//	void createPixelShader(
	//		string name
	//			);

	m_graphicsManager->createPixelShader("PixelShader"); // Name has to match shader name without .hlsl



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
	m_graphicsManager->createTexture2D(
		"myRTV",
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		m_graphicsManager->getWindowWidth(),
		m_graphicsManager->getWindowHeight(),
		true,
		false
	);

	// Only SRV
	m_graphicsManager->createTexture2D(
		"mySRV",
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		m_graphicsManager->getWindowWidth(),
		m_graphicsManager->getWindowHeight(),
		true,
		false
	);

	// Both
	m_graphicsManager->createTexture2D("myRTVandSRV");

	// Add image on an SRV (base filepath will be set to the assets folder automatically)
	m_graphicsManager->attachImage("ToneMapping/Images/picture.jpg", "mySRV");



	// ###########################################################
	// ######		Render target & shader resource			######
	// ###########################################################
	//	void createSamplerState(
	//		string name,
	//		D3D11_FILTER filter = D3D11_FILTER_MIN_MAG_MIP_POINT,
	//		D3D11_TEXTURE_ADDRESS_MODE mode = D3D11_TEXTURE_ADDRESS_CLAMP
	//	);

	m_graphicsManager->createSamplerState("CoolSampler", D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP);
}