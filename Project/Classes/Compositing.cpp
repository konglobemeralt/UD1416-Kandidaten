#include "Compositing.h"
#include "..\ApplicationContext.h"
#include "GraphicsManager.h"

Compositing::Compositing()
{

}

Compositing::~Compositing()
{

}

void Compositing::Render(string shaderresource, string rendertarget) {

	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	UINT vertexSize = sizeof(float) * 5;
	UINT offset = 0;

	//gdeviceContext->OMSetRenderTargets(1, manager->getBackbuffer(), nullptr);
	//gdeviceContext->ClearRenderTargetView(*manager->getBackbufferRTV(), clearColor);


	gdeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	gdeviceContext->IASetInputLayout(resources.inputLayouts["CompositingLayout"]);

	gdeviceContext->VSSetShader(resources.vertexShaders["CompositingVertexShader"], nullptr, 0);
	gdeviceContext->PSSetShader(resources.pixelShaders["CompositingPixelShader"], nullptr, 0);

	string framePadding;

	//Find correct background and UV images to sample from
	if (m_imageCount <= 9)
	{
		framePadding = "000";
	}
	else if (m_imageCount > 9 && m_imageCount < 100)
	{
		framePadding = "00";
	}
	else if (m_imageCount > 100 && m_imageCount < 999)
	{
		framePadding = "0";
	}
	else
	{
		framePadding = "";
	}


	

	
	//alla spelare [1][compositingIndex] om aktuella intervallet ska visa en spelare eller inte. 
	if (m_playerChangeFrame[1][m_compositingIndex] != 0)
	{
				manager->attachImage(manager->getInfoRetriever()->playerInfo[m_playerIndex].fileName, "PlayerSRV"); //hur ofta ska denna bytas? vid vilka frames?
	}
	else
	{
				
				manager->attachImage("null.fakefile", "PlayerSRV");
	}
	
	
	ID3D11Debug* DebugDevice = nullptr;
	HRESULT hr = gdevice->QueryInterface(IID_PPV_ARGS(&DebugDevice));
	DebugDevice->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);

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
		uvString = m_UVFrame + framePadding + to_string(m_imageCount) + ".png";
		uvRefString = m_UVReflectionFrame + framePadding + to_string(m_imageCount) + ".tif";
		beautyString = m_beautyFrame + framePadding + to_string(m_imageCount) + ".png";
		diffuseString = m_diffuseFrame + framePadding + to_string(m_imageCount) + ".png";
		specularString = m_specularFrame + framePadding + to_string(m_imageCount) + ".png";
		refDistortString = m_refDistortFrame + framePadding + to_string(m_imageCount) + ".png";
		irradianceString = m_irradianceFrame + framePadding + to_string(m_imageCount) + ".png";
		reflectionString = m_reflectionFrame + framePadding + to_string(m_imageCount) + ".png";
		indirectString = m_indirectFrame + framePadding + to_string(m_imageCount) + ".png";
		
	}
	else
	{
		uvString = m_UVFrame + ".tif";
		uvRefString = m_UVReflectionFrame + ".png";
		beautyString = m_beautyFrame + ".png";
		diffuseString = m_diffuseFrame + ".png";
		specularString = m_specularFrame + ".png";
		refDistortString = m_refDistortFrame + ".png";
		irradianceString = m_irradianceFrame + ".png";
		reflectionString = m_reflectionFrame + ".png";
		indirectString = m_indirectFrame + ".png";
		
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
	




	gdeviceContext->PSSetSamplers(0, 1, &resources.samplerStates["SamplerWrap"]);


	gdeviceContext->IASetVertexBuffers(0, 1, manager->getQuad(), &vertexSize, &offset);
	gdeviceContext->OMSetRenderTargets(1, &resources.renderTargetViews["Final"], nullptr);
	gdeviceContext->Draw(4, 0);

	//gdeviceContext->OMSetRenderTargets(1, manager->getBackbuffer(), nullptr);

	if (rendertarget.empty())
		gdeviceContext->OMSetRenderTargets(1, manager->getBackbuffer(), nullptr);
	else
		gdeviceContext->OMSetRenderTargets(1, &resources.renderTargetViews[rendertarget], nullptr);

	gdeviceContext->PSSetShader(resources.pixelShaders["PixelShader"], nullptr, 0);
	gdeviceContext->PSSetShaderResources(0, 1, &resources.shaderResourceViews["Final"]);
	gdeviceContext->Draw(4, 0);


	//Byt spelarbild/"stäng av" compositing vid fördefineerade frameIndex
	if (m_imageCount == m_playerChangeFrame[0][m_compositingIndex])
	{
		//gå till nästa compositing index
		m_compositingIndex++;

		//Om spelare ska visas gå till nästa spelare. 
		if (m_playerChangeFrame[1][m_compositingIndex] != 0)
		{
			m_playerIndex++;
		}
		

	}
	m_imageCount++;

	

	//if (currPlayerIndex >  manager->getInfoRetriever()->playerInfo.size() -1)
	//{
	//	currPlayerIndex = 0;
	//}
	//Add 1 to image count and if 400 reset to 0 to create a loop.
	//Sleep(5);

	//	if (m_imageCount == m_imageSum+1)
	//		m_imageCount = m_startFrame;
	//	if (!m_shotTaken)
	//	{
	//manager->saveImage("PresCube/WithLogoIrradiance.png", manager->pBackBuffer);
	//		m_shotTaken = true;
	//	}

	//Haxxig exit
	if (m_imageCount == 2275)
	{
		exit(EXIT_FAILURE);
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
	//manager->attachImage("BlueCircles.png", "PlayerSRV");




	m_UVFrame = "KHK_Logo_Rink_Animation_Alpha_uvLayer/uv_uv";
	m_UVReflectionFrame = "";
	m_beautyFrame = "KHK_Logo_Rink_Animation_Alpha_mainLayer/btBeauty.";
	m_diffuseFrame = "KHK_Logo_Rink_Animation_Alpha_mainLayer/btBeauty.";
	m_specularFrame = "KHK_Logo_Rink_Animation_Alpha_mainLayer/btBeauty.";
	m_refDistortFrame = "";
	m_indirectFrame = "KHK_Logo_Rink_Animation_Alpha_mainLayer/btBeauty.";
	m_reflectionFrame = "KHK_Logo_Rink_Animation_Alpha_mainLayer/btBeauty.";
	m_irradianceFrame = "KHK_Logo_Rink_Animation_Alpha_mainLayer/btBeauty.";



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
