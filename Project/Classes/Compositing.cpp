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

	gdeviceContext->OMSetRenderTargets(1, manager->getBackbuffer(), nullptr);
	gdeviceContext->ClearRenderTargetView(*manager->getBackbuffer(), clearColor);

	gdeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	gdeviceContext->IASetInputLayout(resources.inputLayouts["CompositingLayout"]);

	gdeviceContext->VSSetShader(resources.vertexShaders["CompositingVertexShader"], nullptr, 0);
	gdeviceContext->PSSetShader(resources.pixelShaders["CompositingPixelShader"], nullptr, 0);


	if (imageCount == 0)
	{
		manager->attachImage("dickbutt.png", "SecondSRV");
	}


	if (imageCount < 9)
	{
		imageWithZero = "UVTEST/UVTEST_00";
		imageWithZeroBGR = "background/Backgrnd_00";
	}
	else if (imageCount > 9 && imageCount < 100)
		{
			imageWithZero = "UVTEST/UVTEST_0";
			imageWithZeroBGR = "background/Backgrnd_0";
		}
	
	else
	{
		imageWithZero = "UVTEST/UVTEST_";
		imageWithZeroBGR = "background/Backgrnd_";
	}
		

	if (imageCount == 79)
	{
		manager->attachImage("Pepe.png", "SecondSRV");
	}

	if (imageCount == 120)
	{
		manager->attachImage("bert.png", "SecondSRV");
	}


	if (imageCount == 175)
	{
		manager->attachImage("trump.png", "SecondSRV");
	}

	if (imageCount == 242)
	{
		manager->attachImage("putin.png", "SecondSRV");
		
	}

	if (imageCount == 284)
	{
		manager->attachImage("putin2.png", "SecondSRV");

	}

	if (imageCount == 329)
	{
		manager->attachImage("putin.png", "SecondSRV");

	}

	string cat = imageWithZero + to_string(imageCount) + ".png";
	string cat2 = imageWithZeroBGR + to_string(imageCount) + ".png";
	manager->attachImage(cat, "FirstSRV");
	manager->attachImage(cat2, "BackgroundSRV");
	gdeviceContext->PSSetShaderResources(0, 1, &resources.shaderResourceViews["FirstSRV"]);
	gdeviceContext->PSSetShaderResources(1, 1, &resources.shaderResourceViews["SecondSRV"]);
	gdeviceContext->PSSetShaderResources(2, 1, &resources.shaderResourceViews["BackgroundSRV"]);

	gdeviceContext->PSSetSamplers(0, 1, &resources.samplerStates["SamplerWrap"]);

	gdeviceContext->IASetVertexBuffers(0, 1, manager->getQuad(), &vertexSize, &offset);

	gdeviceContext->Draw(4, 0);

	imageCount++;
	if (imageCount == 400)
		imageCount = 0;
	//manager->saveImage("ToneMapping/OutputImages/image.png", manager->pBackBuffer);

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
	manager->createTexture2D("FirstSRVRTV");

	// Add image on an SRV (base filepath will be set to the assets folder automatically)
	manager->attachImage("dickbutt.png", "SecondSRV");



	// ###########################################################
	// ######		Render target & shader resource			######
	// ###########################################################
	//	void createSamplerState(
	//		string name,
	//		D3D11_FILTER filter = D3D11_FILTER_MIN_MAG_MIP_POINT,
	//		D3D11_TEXTURE_ADDRESS_MODE mode = D3D11_TEXTURE_ADDRESS_CLAMP
	//	);

	manager->createSamplerState("SamplerWrap", D3D11_FILTER_MAXIMUM_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_WRAP);
}