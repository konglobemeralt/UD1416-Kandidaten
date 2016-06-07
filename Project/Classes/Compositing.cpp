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

	string framePadding;

	//Find correct background and UV images to sample from
	if (m_imageCount <= 9)
	{
		framePadding = "00";
	}
	else if (m_imageCount > 9 && m_imageCount < 100)
	{
		framePadding = "0";
	}
	else
	{
		framePadding = "";
	}
		

	//Switch image to composit depending on frame
	/*if (imageCount == 0)
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
	}*/

	if (m_imageCount == 2)
	{
		manager->attachImage("dickbutt3.png", "PlayerSRV");
	}
	if (m_imageCount == 30)
	{
		manager->attachImage("putin.png", "PlayerSRV");
	}
	if (m_imageCount == 90)
	{
		manager->attachImage("dickbutt3.png", "PlayerSRV");
	}

	string uvString;
	string uvRefString;
	string beautyString;
	string diffuseString;
	string specularString;
	string refDistortString;
	string irradianceString;
	string reflectionString;
	string indirectString;
	string textPlane1String;
	string textPlane2String;
	string textPlane3String;
	//Concatenate to filename to find correct UV and Backgroudn image.
	if (m_renderSequence)
	{
		uvString			= m_UVFrame + framePadding + to_string(m_imageCount) + ".png";
		uvRefString		= m_UVReflectionFrame + framePadding + to_string(m_imageCount) + ".png";
		beautyString		= m_beautyFrame + framePadding + to_string(m_imageCount) + ".png";
		diffuseString	= m_diffuseFrame + framePadding + to_string(m_imageCount) + ".png";
		specularString	= m_specularFrame + framePadding + to_string(m_imageCount) + ".png";
		refDistortString = m_refDistortFrame + framePadding + to_string(m_imageCount) + ".png";
		irradianceString		= m_irradianceFrame + framePadding + to_string(m_imageCount) + ".png";
		reflectionString = m_reflectionFrame + framePadding + to_string(m_imageCount) + ".png";
		indirectString = m_indirectFrame + framePadding + to_string(m_imageCount) + ".png";
		textPlane1String = m_textFrame1 + framePadding + to_string(m_imageCount) + ".png";
		textPlane2String = m_textFrame2 + framePadding + to_string(m_imageCount) + ".png";
		textPlane3String = m_textFrame3 + framePadding + to_string(m_imageCount) + ".png";
	}
	else
	{
		uvString = m_UVFrame + ".png";
		uvRefString = m_UVReflectionFrame + ".png";
		beautyString = m_beautyFrame + ".png";
		diffuseString = m_diffuseFrame  + ".png";
		specularString = m_specularFrame + ".png";
		refDistortString = m_refDistortFrame + ".png";
		irradianceString = m_irradianceFrame + ".png";
		reflectionString = m_reflectionFrame + ".png";
		indirectString = m_indirectFrame + ".png";
		textPlane1String = m_textFrame1 + ".png";
		textPlane2String = m_textFrame2 + ".png";
		textPlane3String = m_textFrame3 + ".png";
	}

	//Create and attach shader resources
	if (m_renderUV)
	{
		manager->attachImage(uvString, "UVSRV");
		gdeviceContext->PSSetShaderResources(0, 1, &resources.shaderResourceViews["UVSRV"]);
	}

	gdeviceContext->PSSetShaderResources(1, 1, &resources.shaderResourceViews["PlayerSRV"]);

	if (m_renderUVReflection)
	{
		manager->attachImage(uvRefString, "UVrefSRV");
		gdeviceContext->PSSetShaderResources(3, 1, &resources.shaderResourceViews["UVrefSRV"]);
	}
	if (m_renderBeauty)
	{
		manager->attachImage(beautyString, "BeautySRV");
		gdeviceContext->PSSetShaderResources(2, 1, &resources.shaderResourceViews["BeautySRV"]);
	}
	if (m_renderDiffuse)
	{
		manager->attachImage(diffuseString, "DiffuseSRV");
		gdeviceContext->PSSetShaderResources(4, 1, &resources.shaderResourceViews["DiffuseSRV"]);
	}
	if (m_renderSpecular)
	{
		manager->attachImage(specularString, "SpecularSRV");
		gdeviceContext->PSSetShaderResources(5, 1, &resources.shaderResourceViews["SpecularSRV"]);
	}
	if (m_renderReflectionDistorion)
	{
		manager->attachImage(refDistortString, "ReflectiveDistortSRV");
		gdeviceContext->PSSetShaderResources(6, 1, &resources.shaderResourceViews["ReflectiveDistortSRV"]);
	}
	if (m_renderIrradiance)
	{
		manager->attachImage(irradianceString, "IrradianceSRV");
		gdeviceContext->PSSetShaderResources(7, 1, &resources.shaderResourceViews["IrradianceSRV"]);
	}
	if (m_renderReflection)
	{
		manager->attachImage(reflectionString, "ReflectionSRV");
		gdeviceContext->PSSetShaderResources(8, 1, &resources.shaderResourceViews["ReflectionSRV"]);
	}
	if (m_renderIndirect)
	{
		manager->attachImage(indirectString, "IndirectSRV");
		gdeviceContext->PSSetShaderResources(9, 1, &resources.shaderResourceViews["IndirectSRV"]);
	}
	if (m_renderText)
	{
		manager->attachImage("textPlane1/untitled.046.png", "Text1SRV");
		gdeviceContext->PSSetShaderResources(10, 1, &resources.shaderResourceViews["Text1SRV"]);

		manager->attachImage(textPlane2String, "Text2SRV");
		gdeviceContext->PSSetShaderResources(11, 1, &resources.shaderResourceViews["Text2SRV"]);

		manager->attachImage(textPlane3String, "Text3SRV");
		gdeviceContext->PSSetShaderResources(12, 1, &resources.shaderResourceViews["Text3SRV"]);

		//manager->attachImage("firstName.png", "FirstNameSRV");
		gdeviceContext->PSSetShaderResources(13, 1, &resources.shaderResourceViews["FirstNameSRV"]);

		//manager->attachImage("lastName.png", "LastNameSRV");
		gdeviceContext->PSSetShaderResources(14, 1, &resources.shaderResourceViews["LastNameSRV"]);

		//manager->attachImage("number.png", "NumberSRV");
		gdeviceContext->PSSetShaderResources(15, 1, &resources.shaderResourceViews["NumberSRV"]);


	}

	//manager->attachImage(cat, "UVSRV");
	//manager->attachImage(cat2, "BackgroundSRV");
	
	
	/*gdeviceContext->PSSetShaderResources(0, 1, &resources.shaderResourceViews["UVSRV"]);
	gdeviceContext->PSSetShaderResources(1, 1, &resources.shaderResourceViews["PlayerSRV"]);
	gdeviceContext->PSSetShaderResources(2, 1, &resources.shaderResourceViews["BackgroundSRV"]);*/

	gdeviceContext->PSSetSamplers(0, 1, &resources.samplerStates["SamplerWrap"]);

	gdeviceContext->IASetVertexBuffers(0, 1, manager->getQuad(), &vertexSize, &offset);
	//gdeviceContext->OMSetRenderTargets(1, &resources.renderTargetViews["Final"], nullptr);
	gdeviceContext->Draw(4, 0);

	//gdeviceContext->OMSetRenderTargets(1, manager->getBackbuffer(), nullptr);
	//gdeviceContext->PSSetShader(resources.pixelShaders["PixelShader"], nullptr, 0);
	//gdeviceContext->PSSetShaderResources(0, 1, &resources.shaderResourceViews["Final"]);
	//gdeviceContext->Draw(4, 0);

	//Add 1 to image count and if 400 reset to 0 to create a loop.
	//Sleep(5);
	m_imageCount++;
	if (m_imageCount == m_imageSum+1)
		m_imageCount = m_startFrame;
	if (!m_shotTaken)
	{
		manager->saveImage("PresCube/WithLogoIrradiance.png", manager->pBackBuffer);
		m_shotTaken = true;
	}
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
	manager->createPixelShader("PixelShader");



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

	// Boath
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = manager->getWindowWidth();
	desc.Height = manager->getWindowHeight();
	desc.MipLevels = 0;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	gdevice->CreateTexture2D(&desc, nullptr, &texture); // add subresource
	manager->createTexture2D(
		"Final",
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		manager->getWindowWidth(),
		manager->getWindowHeight(),
		true,
		true,
		texture
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
	manager->attachImage("BlueCircles.png", "PlayerSRV");




	m_UVFrame =					"RenderCube/UV/MasterBeauty_1080AA";
	m_UVReflectionFrame =		"";
	m_beautyFrame =				"RenderCube/Floor/f_DirrectIrradiance";
	m_diffuseFrame =			"RenderCube/Floor/f_DiffuseMaterialColor";
	m_specularFrame =			"RenderCube/Floor/f_Specular";
	m_refDistortFrame =			"";
	m_indirectFrame =			"RenderCube/Floor/f_Indirect";
	//m_reflectionFrame =			"RenderCube/Floor/f_Reflection";
	m_irradianceFrame =			"RenderCube/Floor/f_DirectIrradiance";
	m_textFrame1 = "textPlane1/untitled.";
	m_textFrame2 = "textPlane2/untitled.";
	m_textFrame3 = "textPlane3/untitled.";
	
	
	m_textureConstantBuffer.m_UV = m_renderUV;
	m_textureConstantBuffer.m_UVRef = m_renderUVReflection;
	m_textureConstantBuffer.m_beauty = m_renderBeauty;
	m_textureConstantBuffer.m_diffuse = m_renderDiffuse;
	m_textureConstantBuffer.m_specular = m_renderSpecular;
	m_textureConstantBuffer.m_irradiance = m_renderReflectionDistorion;
	m_textureConstantBuffer.m_shadow = m_renderIrradiance;
	m_textureConstantBuffer.m_reflection = m_renderReflection;

	//m_textureConstantBuffer.m_text = m_renderText;


	manager->createConstantBuffer("textureBuffer", &m_textureConstantBuffer, sizeof(EnabledTextures));

	// ###########################################################
	// ######		Render target & shader resource			######
	// ###########################################################
	//	void createSamplerState(
	//		string name,
	//		D3D11_FILTER filter = D3D11_FILTER_MIN_MAG_MIP_POINT,
	//		D3D11_TEXTURE_ADDRESS_MODE mode = D3D11_TEXTURE_ADDRESS_CLAMP
	//	);

	manager->createSamplerState("SamplerWrap", D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP);
}

void Compositing::SetText(ID3D11ShaderResourceView* text[3])
{
	resources.shaderResourceViews["FirstNameSRV"] = text[0];
	resources.shaderResourceViews["LastNameSRV"] = text[1];
	resources.shaderResourceViews["NumberSRV"] = text[2];
}