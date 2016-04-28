#include "Compositing.h"
#include "..\ApplicationContext.h"
#include "GraphicsManager.h"

Compositing::Compositing()
{

}

Compositing::~Compositing()
{

}

void Compositing::Render() {
	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	UINT vertexSize = sizeof(float) * 5;
	UINT offset = 0;

	// Compositing
	gdeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	gdeviceContext->IASetInputLayout(resources.inputLayouts["CompositingLayout"]);
	gdeviceContext->VSSetShader(resources.vertexShaders["CompositingVertexShader"], nullptr, 0);
	gdeviceContext->OMSetRenderTargets(1, manager->getBackbuffer(), nullptr);
	gdeviceContext->ClearRenderTargetView(*manager->getBackbuffer(), clearColor);
	gdeviceContext->PSSetShader(resources.pixelShaders["CompositingPixelShader"], nullptr, 0);

	//Find correct background and UV images to sample from
	if (imageCount < 9)
	{
		UVFrame = "UVTEST/UVTEST_00";
		BgrFrame = "background/Backgrnd_00";
	}
	else if (imageCount > 9 && imageCount < 100)
		{
			UVFrame = "UVTEST/UVTEST_0";
			BgrFrame = "background/Backgrnd_0";
		}
	else
	{
		UVFrame = "UVTEST/UVTEST_";
		BgrFrame = "background/Backgrnd_";
	}
		

	//Switch image to composit depending on frame
	if (imageCount == 0)
	{
		manager->attachImage("dickbutt.png", "PlayerSRV");
	}

	if (imageCount == 79)
	{
		manager->attachImage("Pepe.png", "PlayerSRV");
	}

	if (imageCount == 120)
	{
		manager->attachImage("bert.png", "PlayerSRV");
	}


	if (imageCount == 175)
	{
		manager->attachImage("trump.png", "PlayerSRV");
	}

	if (imageCount == 242)
	{
		manager->attachImage("putin.png", "PlayerSRV");
		
	}

	if (imageCount == 284)
	{
		manager->attachImage("putin2.png", "PlayerSRV");

	}

	if (imageCount == 329)
	{
		manager->attachImage("putin.png", "PlayerSRV");
	}
	//manager->attachImage("putin.png", "PlayerSRV");

	//// AA
	//gdeviceContext->OMSetRenderTargets(1, &resources.renderTargetViews["PlayerSRV"], nullptr);
	//gdeviceContext->PSSetShaderResources(0, 1, &resources.shaderResourceViews["PlayerSRV"]);
	//gdeviceContext->PSSetShader(resources.pixelShaders["FXAA_PS"], nullptr, 0);
	//gdeviceContext->PSSetConstantBuffers(0, 1, &resources.constantBuffers["FXAA_PS_cb"]);
	//gdeviceContext->PSSetSamplers(1, 1, &resources.samplerStates["SamplerWrap"]);
	//gdeviceContext->Draw(4, 0);

	//Concatenate to filename to find correct UV and Backgroudn image.
	string cat = UVFrame + to_string(imageCount) + ".png";
	string cat2 = BgrFrame + to_string(imageCount) + ".png";
	
	//manager->attachImage("Fonts/UV/UV1.png", "UVSRV");
	manager->attachImage(cat, "UVSRV");
	manager->attachImage(cat2, "BackgroundSRV");
	gdeviceContext->PSSetShaderResources(2, 1, &resources.shaderResourceViews["BackgroundSRV"]);
	
	//Attach shader resources
	gdeviceContext->PSSetShaderResources(0, 1, &resources.shaderResourceViews["UVSRV"]);
	gdeviceContext->PSSetShaderResources(1, 1, &resources.shaderResourceViews["PlayerSRV"]);
	gdeviceContext->PSSetShaderResources(1, 1, &text);
	//gdeviceContext->PSSetShaderResources(2, 1, &resources.shaderResourceViews["BackgroundSRV"]);

	gdeviceContext->PSSetSamplers(0, 1, &resources.samplerStates["SamplerWrap"]);

	gdeviceContext->IASetVertexBuffers(0, 1, manager->getQuad(), &vertexSize, &offset);
	gdeviceContext->PSSetSamplers(0, 1, &resources.samplerStates["SamplerWrap"]);
	gdeviceContext->PSSetSamplers(1, 1, &resources.samplerStates["Point"]);
	//gdeviceContext->OMSetRenderTargets(1, &resources.renderTargetViews["Final"], nullptr);

	gdeviceContext->Draw(4, 0);

	// Text

	//// FXAA
	//gdeviceContext->OMSetRenderTargets(1, manager->getBackbuffer(), nullptr);
	//gdeviceContext->PSSetShaderResources(0, 1, &resources.shaderResourceViews["Final"]);
	//gdeviceContext->PSSetShader(resources.pixelShaders["FXAA_PS"], nullptr, 0);
	//gdeviceContext->PSSetConstantBuffers(0, 1, &resources.constantBuffers["FXAA_PS_cb"]);
	//gdeviceContext->PSSetSamplers(1, 1, &resources.samplerStates["SamplerWrap"]);
	//gdeviceContext->Draw(4, 0);

	//// SSAA
	//gdeviceContext->OMSetRenderTargets(1, manager->getBackbuffer(), nullptr);
	//gdeviceContext->PSSetShaderResources(0, 1, &resources.shaderResourceViews["Final"]);
	//gdeviceContext->PSSetShader(resources.pixelShaders["PixelShader"], nullptr, 0);
	//gdeviceContext->PSSetSamplers(0, 1, &resources.samplerStates["SamplerWrap"]);
	//gdeviceContext->PSSetSamplers(1, 1, &resources.samplerStates["Point"]);
	//gdeviceContext->Draw(4, 0);

	//// Final
	//gdeviceContext->PSSetShaderResources(0, 1, &resources.shaderResourceViews["Final"]);
	//gdeviceContext->VSSetShader(resources.vertexShaders["VertexShader"], nullptr, 0);
	//gdeviceContext->PSSetShader(resources.pixelShaders["PixelShader"], nullptr, 0);
	//gdeviceContext->Draw(4, 0);

	//Add 1 to image count and if 400 reset to 0 to create a loop.
	Sleep(50);
	imageCount++;
	if (imageCount == 400)
		imageCount = 0;
	//manager->saveImage("ToneMapping/OutputImages/image.png", manager->pBackBuffer);
	Sleep(0);
}

void Compositing::Initialize() {
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

	manager->createVertexShader("CompositingVertexShader", "CompositingLayout", layoutDesc, ARRAYSIZE(layoutDesc));
	manager->createVertexShader("VertexShader", "FirstLayout", layoutDesc, ARRAYSIZE(layoutDesc));



	// ###########################################################
	// ######				Other Shaders					######
	// ###########################################################
	//	void createPixelShader(
	//		string name
	//			);

	manager->createPixelShader("CompositingPixelShader"); // Name has to match shader name without .hlsl



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
		true,
		false
	);

	// Both
	manager->createTexture2D("BackgroundSRV");

	// Add image on an SRV (base filepath will be set to the assets folder automatically)
	manager->attachImage("Ring.png", "PlayerSRV");



	// ###########################################################
	// ######		Render target & shader resource			######
	// ###########################################################
	//	void createSamplerState(
	//		string name,
	//		D3D11_FILTER filter = D3D11_FILTER_MIN_MAG_MIP_POINT,
	//		D3D11_TEXTURE_ADDRESS_MODE mode = D3D11_TEXTURE_ADDRESS_CLAMP
	//	);

	manager->createSamplerState("SamplerWrap", D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP);
	manager->createSamplerState("Point", D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP);

	AA();
}

void Compositing::SetText(ID3D11ShaderResourceView * text)
{
	this->text = text;
}

void Compositing::AA()
{
	struct FXAA_PS_ConstantBuffer { //texelsize n shiet
		XMFLOAT2 texelSizeXY;
		float FXAA_blur_Texels_Threshhold; //hur många texlar som kommer blurras åt varje håll
		float minimumBlurThreshhold; //hur mycket som krävs för att den ens ska blurra
		float FXAA_reduce_MULTIPLIER;
		float FXAA_reduce_MIN; //så dirOffset inte ska bli noll
		XMFLOAT2 pad;
	}FXAA_PS_cb;

	FXAA_PS_cb.texelSizeXY.x = 1.0f / m_graphicsManager->getWindowWidth();
	FXAA_PS_cb.texelSizeXY.y = 1.0f / m_graphicsManager->getWindowHeight();
	FXAA_PS_cb.FXAA_blur_Texels_Threshhold = 20.0f;
	//FXAA_PS_cb.minimumBlurThreshhold = 0.0001f;
	FXAA_PS_cb.FXAA_reduce_MULTIPLIER = 1.0f / 3.0f;
	FXAA_PS_cb.FXAA_reduce_MIN = 1.0f / 32.0f;

	m_graphicsManager->createConstantBuffer("FXAA_PS_cb", &FXAA_PS_cb, sizeof(FXAA_PS_ConstantBuffer));

	manager->createPixelShader("FXAA_PS");
	manager->createPixelShader("PixelShader");

	manager->createTexture2D("Final", 
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		manager->getWindowWidth(),
		manager->getWindowHeight());
}

void Compositing::SetTextImages()
{
	resources.shaderResourceViews["AliasedText"];
	resources.shaderResourceViews["FXAAText"];
	resources.shaderResourceViews["SSAAText"];
}