#include "Lightning.h"
#include "GraphicsManager.h"
#include "..\ApplicationContext.h"
Lightning::Lightning()
{

}
Lightning::~Lightning()
{

}


void Lightning::Render() {
	float clearColor[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
	UINT vertexSize = sizeof(float) * 5;
	UINT offset = 0;

	gdeviceContext->OMSetRenderTargets(1, m_graphicsManager->getBackbuffer(), nullptr);
	gdeviceContext->ClearRenderTargetView(*m_graphicsManager->getBackbuffer(), clearColor);

	gdeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);		//LINESTRIP
	//deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);		//TRIANGLESTRIP
	gdeviceContext->IASetInputLayout(m_graphicsManager->thesisData.inputLayouts["FirstLayout"]);
	gdeviceContext->PSSetSamplers(0, 1, &m_graphicsManager->thesisData.samplerStates["CoolSampler"]);

	gdeviceContext->VSSetShader(m_graphicsManager->thesisData.vertexShaders["LightningVertexShader"], nullptr, 0);
	//deviceContext->HSSetShader(resources.hullShaders["LightningHullShader"], nullptr, 0);
	//deviceContext->DSSetShader(resources.domainShaders["LightningDomainShader"], nullptr, 0);
	gdeviceContext->GSSetShader(m_graphicsManager->thesisData.geometryShaders["LightningGeometryShader"], nullptr, 0);
	gdeviceContext->PSSetShader(m_graphicsManager->thesisData.pixelShaders["LightningPixelShader"], nullptr, 0);

	gdeviceContext->IASetVertexBuffers(0, 1, m_graphicsManager->getQuad(), &vertexSize, &offset);

	gdeviceContext->Draw(4, 0);
}

void Lightning::Initialize() {
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
		XMMATRIX WVP;
		XMMATRIX World;
		XMFLOAT4X4 matrix;
	}constantBuffer;

	m_graphicsManager->createConstantBuffer("constantBuffer", &constantBuffer, sizeof(cBuffer));


	///////////___________NEW_________________/////////////
	XMVECTOR camPos, camLook, camUp;
	XMMATRIX WVP, World, ViewSpace, Projection;

	camPos = XMVectorSet(0.0f, 7.5f, -5.0f, 0.0f);	// Camera StartPos		//-2.0 Moves camera back 2 units along Z
	camLook = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	camUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	ViewSpace = XMMatrixLookAtLH(camPos, camLook, camUp);
	Projection = XMMatrixPerspectiveFovLH(3.14*0.45, m_graphicsManager->getWindowWidth() / m_graphicsManager->getWindowHeight(), 0.5f, 20.0f); //	FLOAT FovAngleY, FLOAT AspectRatio, FLOAT NearZ, FLOAT FarZ

	World = XMMatrixIdentity();
	WVP = World * ViewSpace * Projection;

	constantBuffer.World = XMMatrixTranspose(World);
	constantBuffer.WVP = XMMatrixTranspose(WVP);

	gdeviceContext->UpdateSubresource(m_graphicsManager->thesisData.constantBuffers["constantBuffer"], 0, NULL, &constantBuffer, 0, 0);	//NOT WORKING
	gdeviceContext->VSSetConstantBuffers(0, 1, &m_graphicsManager->thesisData.constantBuffers["constantBuffer"]);	//NOT WORKING
	///////////___________NEW_________________/////////////

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

	m_graphicsManager->createVertexShader("LightningVertexShader", "FirstLayout", layoutDesc, ARRAYSIZE(layoutDesc));



	// ###########################################################
	// ######				Other Shaders					######
	// ###########################################################
	//	void createPixelShader(
	//		string name
	//			);

	m_graphicsManager->createHullShader("LightningHullShader");
	m_graphicsManager->createDomainShader("LightningDomainShader");
	m_graphicsManager->createGeometryShader("LightningGeometryShader");
	m_graphicsManager->createPixelShader("LightningPixelShader"); // Name has to match shader name without .hlsl



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