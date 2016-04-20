#include "Lightning.h"

Lightning::Lightning() {}

Lightning::~Lightning() {}

void Lightning::Render() {
	float clearColor[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
	UINT vertexSize = sizeof(float) * 5;
	UINT offset = 0;

	deviceContext->OMSetRenderTargets(1, manager.getBackbuffer(), nullptr);
	deviceContext->ClearRenderTargetView(*manager.getBackbuffer(), clearColor);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);		//LINESTRIP
																					//deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);		//TRIANGLESTRIP
	deviceContext->IASetInputLayout(resources.inputLayouts["FirstLayout"]);
	deviceContext->PSSetSamplers(0, 1, &resources.samplerStates["CoolSampler"]);

	deviceContext->VSSetShader(resources.vertexShaders["LightningVertexShader"], nullptr, 0);
	//deviceContext->HSSetShader(resources.hullShaders["LightningHullShader"], nullptr, 0);
	//deviceContext->DSSetShader(resources.domainShaders["LightningDomainShader"], nullptr, 0);
	deviceContext->GSSetShader(resources.geometryShaders["LightningGeometryShader"], nullptr, 0);
	deviceContext->PSSetShader(resources.pixelShaders["LightningPixelShader"], nullptr, 0);

	deviceContext->IASetVertexBuffers(0, 1, manager.getQuad(), &vertexSize, &offset);

	deviceContext->Draw(4, 0);
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

	struct cBuffer {
		XMMATRIX WVP;
		XMMATRIX World;
		XMFLOAT4X4 matrix;
	}constantBuffer;

	manager.createConstantBuffer("constantBuffer", &constantBuffer, sizeof(cBuffer));


	///////////___________NEW_________________/////////////
	XMVECTOR camPos, camLook, camUp;
	XMMATRIX WVP, World, ViewSpace, Projection;

	camPos = XMVectorSet(0.0f, 7.5f, -5.0f, 0.0f);	// Camera StartPos		//-2.0 Moves camera back 2 units along Z
	camLook = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	camUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	ViewSpace = XMMatrixLookAtLH(camPos, camLook, camUp);
	Projection = XMMatrixPerspectiveFovLH(3.14*0.45, manager.getWindowWidth() / manager.getWindowHeight(), 0.5f, 20.0f); //	FLOAT FovAngleY, FLOAT AspectRatio, FLOAT NearZ, FLOAT FarZ

	World = XMMatrixIdentity();
	WVP = World * ViewSpace * Projection;

	constantBuffer.World = XMMatrixTranspose(World);
	constantBuffer.WVP = XMMatrixTranspose(WVP);

	deviceContext->UpdateSubresource(resources.constantBuffers["constantBuffer"], 0, NULL, &constantBuffer, 0, 0);	//NOT WORKING
	deviceContext->VSSetConstantBuffers(0, 1, &resources.constantBuffers["constantBuffer"]);	//NOT WORKING
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

	manager.createVertexShader("LightningVertexShader", "FirstLayout", layoutDesc, ARRAYSIZE(layoutDesc));



	// ###########################################################
	// ######				Other Shaders					######
	// ###########################################################
	//	void createPixelShader(
	//		string name
	//			);

	manager.createHullShader("LightningHullShader");
	manager.createDomainShader("LightningDomainShader");
	manager.createGeometryShader("LightningGeometryShader");
	manager.createPixelShader("LightningPixelShader"); // Name has to match shader name without .hlsl



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