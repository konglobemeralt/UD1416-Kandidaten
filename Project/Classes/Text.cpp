#include "Text.h"

Text::Text()
{
	for (int i = 0; i < 3; i++)
	{
		d2dTextureTarget[i] = nullptr;
		m_idxgSurface[i] = nullptr;
		m_d2dRenderTarget[i] = nullptr;
		m_blackBrush[i] = nullptr;
		m_orangeBrush[i] = nullptr;

		m_writeTextFormat[i] = nullptr;

		m_pathGeometry[i] = nullptr;
	}

	m_firstTime = true;
	m_edgeRendering = true;
	m_fxaa = false;
	m_ssaa = false;
	m_height = 0;
	m_width = 0;
	m_uvWidth = 0.0f;
	m_uvHeight = 0.0f;
	m_textSize = 100.0f;
	m_edgeSize = 3.0f;
	m_padding = 50.0f;
	m_scale = 1.0f;

	d2dClearColor.r = 1.0f;
	d2dClearColor.g = 0.0f;
	d2dClearColor.b = 0.0f;
	d2dClearColor.a = 1.0f;
}

Text::~Text()
{
	// Direct2D
	m_d2dFactory ? m_d2dFactory->Release() : 0;
	m_dxgiDevice ? m_dxgiDevice->Release() : 0;
	m_d2dDev ? m_d2dDev->Release() : 0;
	m_d2dDevcon ? m_d2dDevcon->Release() : 0;

	// DirectWrite
	m_writeFactory ? m_writeFactory->Release() : 0;

	// EdgeTesting
	m_geometrySink ? m_geometrySink->Release() : 0;
	m_fontFaceBeginning ? m_fontFaceBeginning->Release() : 0;
	m_fontFace ? m_fontFace->Release() : 0;
	m_fontFiles ? m_fontFiles->Release() : 0;
	m_strokeStyle ? m_strokeStyle->Release() : 0;
	m_codePoints ? delete[] m_codePoints : 0;
	m_glyphIndices ? delete[] m_glyphIndices : 0;
	m_advances ? delete[] m_advances : 0;

	for (int i = 0; i < 3; i++)
	{
		d2dTextureTarget[i] ? d2dTextureTarget[i]->Release() : 0;
		m_idxgSurface[i] ? m_idxgSurface[i]->Release() : 0;
		m_d2dRenderTarget[i] ? m_d2dRenderTarget[i]->Release() : 0;
		m_blackBrush[i] ? m_blackBrush[i]->Release() : 0;
		m_orangeBrush[i] ? m_orangeBrush[i]->Release() : 0;

		m_writeTextFormat[i] ? m_writeTextFormat[i]->Release() : 0;

		m_pathGeometry[i] ? m_pathGeometry[i]->Release() : 0;
	}
}

void Text::Render() 
{
	if (m_firstTime)
	{
		CalculateSize();
		// Initialize Systems
		m_text[0] = L"Text";
		m_text[1] = L"Pommes";
		m_text[2] = L"68";
		InitializeDirect2D();
		if(m_edgeRendering)
			DirectWriteEdge();
		else
			InitializeDirectWrite();
		RotatePlane();
		m_firstTime = false;
	}

	UpdateTextQuad();
	gdeviceContext->OMSetRenderTargets(1, manager->getBackbuffer(), nullptr);
	gdeviceContext->ClearRenderTargetView(*manager->getBackbuffer(), clearColor);

	gdeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	gdeviceContext->IASetInputLayout(resources.inputLayouts["FirstLayout"]);
	gdeviceContext->PSSetSamplers(0, 1, &resources.samplerStates["Linear"]);
	gdeviceContext->PSSetSamplers(1, 1, &resources.samplerStates["Point"]);
	//gdeviceContext->VSSetConstantBuffers(0, 1, &resources.constantBuffers["Rotation"]);

	gdeviceContext->VSSetShader(resources.vertexShaders["Text_VS"], nullptr, 0);
	gdeviceContext->PSSetShader(resources.pixelShaders["Text_PS"], nullptr, 0);

	gdeviceContext->IASetVertexBuffers(0, 1, &m_textPlaneBuffer, &vertexSize, &offset);
	gdeviceContext->PSSetShaderResources(0, 1, &resources.shaderResourceViews["UV"]);
	gdeviceContext->VSSetConstantBuffers(0, 1, &resources.constantBuffers["Matrices"]);
	gdeviceContext->PSSetConstantBuffers(0, 1, &m_buffer2);

	if (m_edgeRendering)
		EdgeRender();
	else
		RenderText();

	// FXAA
	if (m_fxaa)
	{
		gdeviceContext->PSSetShaderResources(0, 1, &resources.shaderResourceViews["NULL"]);
		gdeviceContext->OMSetRenderTargets(1, &resources.renderTargetViews["Final"], nullptr);
	}

	// Set textures
	gdeviceContext->PSSetShaderResources(0, 1, &resources.shaderResourceViews["UV"]);
	//gdeviceContext->PSSetShaderResources(1, 1, &resources.shaderResourceViews["Checker"]);
	gdeviceContext->PSSetShaderResources(1, 1, &finalText[0]);
	gdeviceContext->PSSetShaderResources(2, 1, &resources.shaderResourceViews["U"]);
	gdeviceContext->PSSetShaderResources(3, 1, &resources.shaderResourceViews["V"]);
	gdeviceContext->Draw(4, 0);

	//manager->saveImage("Fonts/UV/saved.png", manager->pBackBuffer);

	// Render FXAA
	if (m_fxaa)
	{
		gdeviceContext->OMSetRenderTargets(1, manager->getBackbuffer(), nullptr);
		gdeviceContext->PSSetShaderResources(0, 1, &resources.shaderResourceViews["Final"]);
		gdeviceContext->PSSetShader(resources.pixelShaders["FXAA_PS"], nullptr, 0);
		gdeviceContext->PSSetConstantBuffers(0, 1, &resources.constantBuffers["FXAA_PS_cb"]);
		gdeviceContext->Draw(4, 0);
	}

	//// Genereate mip maps
	//gdeviceContext->PSSetShaderResources(0, 1, &resources.shaderResourceViews["NULL"]);
	//gdeviceContext->CopyResource(tempD2DTexture, d2dTextureTarget);
	//gdeviceContext->GenerateMips(resources.shaderResourceViews["Text"]);
}

void Text::Initialize() {
	manager = ApplicationContext::GetInstance().GetGraphicsManager();
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
	manager->createConstantBuffer("myMatrix", &myMatrix, sizeof(cBuffer));
	m_height = manager->getWindowHeight();
	m_width = manager->getWindowWidth();

	struct FXAA_PS_ConstantBuffer { //texelsize n shiet
		XMFLOAT2 texelSizeXY;
		float FXAA_blur_Texels_Threshhold; //hur m?nga texlar som kommer blurras ?t varje h?ll
		float minimumBlurThreshhold; //hur mycket som kr?vs f?r att den ens ska blurra
		float FXAA_reduce_MULTIPLIER;
		float FXAA_reduce_MIN; //s? dirOffset inte ska bli noll
		XMFLOAT2 pad;
	}FXAA_PS_cb;

	FXAA_PS_cb.texelSizeXY.x = 1.0f / m_width;
	FXAA_PS_cb.texelSizeXY.y = 1.0f / m_height;
	FXAA_PS_cb.FXAA_blur_Texels_Threshhold = 5.0f;
	//FXAA_PS_cb.minimumBlurThreshhold = 0.0001f;
	FXAA_PS_cb.FXAA_reduce_MULTIPLIER = 1.0f / 3.0f;
	FXAA_PS_cb.FXAA_reduce_MIN = 1.0f / 32.0f;
	manager->createConstantBuffer("FXAA_PS_cb", &FXAA_PS_cb, sizeof(FXAA_PS_ConstantBuffer));

	// To compute shader
	struct Corners
	{
		XMFLOAT2 leftup;
		XMFLOAT2 leftdown;
		XMFLOAT2 rightup;
		XMFLOAT2 rightdown;
	}corners;
	corners.leftup = XMFLOAT2(0.0f, 0.0f);
	corners.leftdown = XMFLOAT2(0.0f, 1.0f);
	corners.rightup = XMFLOAT2(1.0f, 0.0f);
	corners.rightdown = XMFLOAT2(1.0f, 1.0f);

	//D3D11_BUFFER_DESC bufferDesc;
	//memset(&bufferDesc, 0, sizeof(bufferDesc));
	//bufferDesc.ByteWidth = sizeof(XMFLOAT2) * 4;
	//bufferDesc.StructureByteStride = sizeof(XMFLOAT2) * 4;
	//bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	//bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	//bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	//bufferDesc.CPUAccessFlags = 0;
	//D3D11_SUBRESOURCE_DATA subData;
	//subData.pSysMem = &corners;
	//gdevice->CreateBuffer(&bufferDesc, &subData, &m_buffer);
	//
	//D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	//uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	//uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	//uavDesc.Buffer.FirstElement = 0;
	//uavDesc.Buffer.NumElements = 1;
	//uavDesc.Buffer.Flags = 0;
	//gdevice->CreateUnorderedAccessView(m_buffer, &uavDesc, &m_uav);

	//D3D11_BUFFER_DESC bufferDesc;
	//memset(&bufferDesc, 0, sizeof(bufferDesc));
	//bufferDesc.ByteWidth = sizeof(XMFLOAT2) * 4;
	//bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	//bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	//bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//D3D11_SUBRESOURCE_DATA subData;
	//subData.pSysMem = &corners;
	//gdevice->CreateBuffer(&bufferDesc, nullptr, &m_buffer);
	//
	//memset(&bufferDesc, 0, sizeof(bufferDesc));
	//bufferDesc.ByteWidth = sizeof(XMFLOAT2) * 4;
	//bufferDesc.StructureByteStride = sizeof(XMFLOAT2) * 4;
	//bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	//bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	//bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	//bufferDesc.CPUAccessFlags = 0;
	//gdevice->CreateBuffer(&bufferDesc, nullptr, &m_buffer2);
	//
	//D3D11_UNORDERED_ACCESS_VIEW_DESC particleUAVDesc;
	//particleUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	//particleUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	//particleUAVDesc.Buffer.FirstElement = 0;
	//particleUAVDesc.Buffer.NumElements = 1;
	//particleUAVDesc.Buffer.Flags = 0;
	//gdevice->CreateUnorderedAccessView(m_buffer2, &particleUAVDesc, &m_uav);

	D3D11_SUBRESOURCE_DATA subData;
	subData.pSysMem = &corners;

	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.ByteWidth = sizeof(XMFLOAT2) * 4;
	bufferDesc.StructureByteStride = 0;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	gdevice->CreateBuffer(&bufferDesc, &subData, &m_buffer2);

	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.ByteWidth = sizeof(XMFLOAT2) * 4;
	bufferDesc.StructureByteStride = sizeof(XMFLOAT2) * 4;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.CPUAccessFlags = 0;
	gdevice->CreateBuffer(&bufferDesc, &subData, &m_buffer);

	D3D11_UNORDERED_ACCESS_VIEW_DESC particleUAVDesc;
	particleUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	particleUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	particleUAVDesc.Buffer.FirstElement = 0;
	particleUAVDesc.Buffer.NumElements = 1;
	//particleUAVDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;
	particleUAVDesc.Buffer.Flags = 0;
	gdevice->CreateUnorderedAccessView(m_buffer, &particleUAVDesc, &m_uav);

	// ###########################################################
	// ######				Vertex Shader					######
	// ###########################################################
	//	void createVertexShader(
	//		string shaderName,
	//		string layoutName,
	//		D3D11_INPUT_ELEMENT_DESC* desc,
	//		UINT size);

	D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
		{ "SV_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	manager->createVertexShader("Text_VS", "FirstLayout", layoutDesc, ARRAYSIZE(layoutDesc));

	// ###########################################################
	// ######				Other Shaders					######
	// ###########################################################
	//	void createPixelShader(
	//		string name
	//			);

	manager->createPixelShader("Text_PS"); // Name has to match shader name without .hlsl
	manager->createPixelShader("Text_Size_PS");
	manager->createPixelShader("FXAA_PS");
	manager->createComputeShader("Text_CS");

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

	// Add image on an SRV (base filepath will be set to the assets folder automatically)
	//m_graphicsManager->attachImage("ToneMapping/Arches_E_PineTree_Preview.jpg", "mySRV");

	manager->attachImage("Fonts/Images/Checker3.png", "Checker");
	manager->attachImage("Fonts/UV/CorrectRotationHalf.png", "UV");
	manager->attachImage("Fonts/UV/XUV.png", "U");
	manager->attachImage("Fonts/UV/VUV.png", "V");

	//ID3D11Texture2D* texture;
	//ID3D11Resource* resource;
	//resources.shaderResourceViews["UV"]->GetResource(&resource);
	//resource->QueryInterface<ID3D11Texture2D>(&texture);
	//D3D11_TEXTURE2D_DESC texDesc;
	//texture->GetDesc(&texDesc);

	//m_uvHeight = texDesc.Height;
	//m_uvWidth = texDesc.Width;

	//manager->createTexture2D("Final",
	//	DXGI_FORMAT_R32G32B32A32_FLOAT,
	//	texDesc.Width,
	//	texDesc.Height);

	// ###########################################################
	// ######		Render target & shader resource			######
	// ###########################################################
	//	void createSamplerState(
	//		string name,
	//		D3D11_FILTER filter = D3D11_FILTER_MIN_MAG_MIP_POINT,
	//		D3D11_TEXTURE_ADDRESS_MODE mode = D3D11_TEXTURE_ADDRESS_CLAMP
	//	);

	manager->createSamplerState("Linear", D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP);
	manager->createSamplerState("Point", D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP);

	// Check support
	D3D11_FEATURE_DATA_D3D11_OPTIONS1 formatSupport;
	gdevice->CheckFeatureSupport(D3D11_FEATURE_D3D11_OPTIONS1, &formatSupport, sizeof(D3D11_FEATURE_DATA_D3D11_OPTIONS1));
	// TiledResourceTier D3D11_TILED_RESOURCES_TIER_1	=	(1)
	// MinMaxFiltering									=	0
	// ClearViewAlsoSupportsDepthOnlyFormats			=	1
	// MapOnDefaultBuffers								=	1

	// Query
	D3D11_QUERY_DESC qDesc;
	qDesc.MiscFlags = 0;
	qDesc.Query = D3D11_QUERY_OCCLUSION;
	CheckStatus(gdevice->CreateQuery(&qDesc, &m_query), L"CreateQuery");

	// Read files
	m_infile.open("Assets/Fonts/Test.bin", std::ofstream::binary);
	if (m_infile)
	{
		m_infile.read((char*)&m_framesAmount, sizeof(int));
		m_infile.read((char*)&m_vertexAmount, sizeof(int));
		m_quadData = new QuadData[m_framesAmount * m_vertexAmount];
		m_infile.read((char*)m_quadData, sizeof(QuadData) * m_framesAmount * m_vertexAmount);
	}
	m_infile.close();

	m_quad = new QuadData[m_vertexAmount];
	for (size_t i = 0; i < m_vertexAmount; i++)
	{
		m_quad[i] = m_quadData[i];
	}

	m_quadWidth = (m_quad[3].pos.x - m_quad[0].pos.x) * 0.5f * m_width;
	m_quadHeight = (m_quad[3].pos.y - m_quad[0].pos.y) * 0.5f * m_height;

	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.ByteWidth = sizeof(QuadData) * m_vertexAmount;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = m_quad;
	gdevice->CreateBuffer(&bufferDesc, &data, &m_textPlaneBuffer);

	XMStoreFloat4x4(&m_matrices.view, XMMatrixLookAtLH(XMVectorSet(0.0f, 0.0f, -1.0f, 1.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)));
	XMStoreFloat4x4(&m_matrices.projection, XMMatrixPerspectiveFovLH(XM_PI * 0.45f, float(m_width) / float(m_height), 0.1f, 1000.0f));
	manager->createConstantBuffer("Matrices", &m_matrices, sizeof(Matrices));
}

void Text::InitializeDirect2D()
{
	// Get values
	if (m_ssaa)
	{
		m_quadHeight *= 4;
		m_quadWidth *= 4;
	}

	// Factory
	CheckStatus(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_d2dFactory), L"D2D1CreateFactory");

	// Device & DeviceContext
	CheckStatus(gdevice->QueryInterface(__uuidof(IDXGIDevice), (void **)&m_dxgiDevice), L"QueryInterface");
	CheckStatus(m_d2dFactory->CreateDevice(m_dxgiDevice, &m_d2dDev), L"CreateDevice");
	CheckStatus(m_d2dDev->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &m_d2dDevcon), L"CreateDeviceContext");

	// Rendertarget
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.ArraySize = 1;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.Height = m_quadHeight;
	texDesc.Width = m_quadWidth;
	texDesc.MipLevels = 1;
	texDesc.MiscFlags = 0;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	D2D1_RENDER_TARGET_PROPERTIES props =
		D2D1::RenderTargetProperties(
			D2D1_RENDER_TARGET_TYPE_DEFAULT,
			D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
			96,
			96);
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = texDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;
	resources.shaderResourceViews["NULL"] = nullptr;

	for (int i = 0; i < 3; i++)
	{
		CheckStatus(gdevice->CreateTexture2D(&texDesc, NULL, &d2dTextureTarget[i]), L"CreateTexture2D");
		//gdevice->CreateTexture2D(&texDesc, NULL, &d2dTextureTarget[i]);

		d2dTextureTarget[i]->QueryInterface(&m_idxgSurface[i]);
		d2dTextureTarget[i]->Release();

		// Brushes
		CheckStatus(m_d2dFactory->CreateDxgiSurfaceRenderTarget(m_idxgSurface[i], &props, &m_d2dRenderTarget[i]), L"CreateDxgiSurfaceRenderTarget");
		CheckStatus(m_d2dRenderTarget[i]->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_blackBrush[i]), L"CreateSolidColorBrush");
		CheckStatus(m_d2dRenderTarget[i]->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Orange), &m_orangeBrush[i]), L"CreateSolidColorBrush");

		// Create the render target view.
		manager->createTexture2D(
			"Text" + to_string(i),
			texDesc.Format,
			m_quadWidth,
			m_quadHeight,
			true,
			true,
			d2dTextureTarget[i]
		);
		finalText[i] = resources.shaderResourceViews["Text" + to_string(i)];
	}

	//// With mipmaps
	//texDesc.ArraySize = 1;
	//texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	//texDesc.CPUAccessFlags = 0;
	//texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	//texDesc.Height = doubleHeight;
	//texDesc.Width = doubleWidth;
	//texDesc.MipLevels = 1;
	//texDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
	////texDesc.SampleDesc.Count = 1;
	////texDesc.SampleDesc.Quality = 0;
	//texDesc.Usage = D3D11_USAGE_DEFAULT;
	//CheckStatus(gdevice->CreateTexture2D(&texDesc, NULL, &tempD2DTexture), L"CreateTexture2D");
	//
	//ID3D11ShaderResourceView* srv;
	//D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	//srvDesc.Format = texDesc.Format;
	//srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	//srvDesc.Texture2D.MostDetailedMip = 0;
	//srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
	//CheckStatus(gdevice->CreateShaderResourceView(d2dTextureTarget, &srvDesc, &srv), L"CreateShaderResourceView");
	//for (int i = 0; i < 3; i++)
	//{
	//	finalText[i] = srv;
	//}
	//resources.shaderResourceViews["Text"] = srv;

	//manager->createTexture2D(
	//	"Text",
	//	texDesc.Format,
	//	manager->getWindowWidth(),
	//	manager->getWindowHeight(),
	//	false,
	//	true,
	//	tempD2DTexture
	//);
}

void Text::InitializeDirectWrite()
{
	// Factory
	CheckStatus(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&m_writeFactory)),
		L"DWriteCreateFactory");

	// Font
	for (int i = 0; i < 3; i++)
	{
		m_textLength[i] = (UINT32)wcslen(m_text[i]);
		CheckStatus(m_writeFactory->CreateTextFormat(
			L"Arial",
			NULL,
			DWRITE_FONT_WEIGHT_REGULAR,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			m_textSize,
			L"en-us",
			&m_writeTextFormat[i]),
			L"CreateTextFormat");

		// Center align the text.
		CheckStatus(m_writeTextFormat[i]->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER), L"SetTextAlignment");
		CheckStatus(m_writeTextFormat[i]->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER), L"SetParagraphAlignment");

		// layout to draw on
		m_layoutRect = D2D1::RectF(
			static_cast<FLOAT>(0.0f),
			static_cast<FLOAT>(0.0f),
			static_cast<FLOAT>(manager->getWindowWidth()),
			static_cast<FLOAT>(manager->getWindowHeight())
			);
	}
}

void Text::RenderText()
{
	for (int i = 0; i < 3; i++)
	{
		m_d2dRenderTarget[i]->BeginDraw();
		m_d2dRenderTarget[i]->SetTransform(D2D1::IdentityMatrix());
		//m_d2dRenderTarget->SetTransform(D2D1::Matrix3x2F::Scale(-1.0f, 1.0f) * D2D1::Matrix3x2F::Translation(1000, 0));	// Must flip texture before send to Compositing
		m_d2dRenderTarget[i]->Clear(NULL);
		// Call the DrawText method of this class.
		//m_d2dRenderTarget->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE::D2D1_TEXT_ANTIALIAS_MODE_ALIASED);
		m_d2dRenderTarget[i]->DrawText(
			m_text[i],					// The string to render.
			m_textLength[i],			// The string's length.
			m_writeTextFormat[i],		// The text format.
			m_layoutRect,			// The region of the window where the text will be rendered.
			m_orangeBrush[i]			// The brush used to draw the text.
			);
		m_d2dRenderTarget[i]->EndDraw();
	}
}

void Text::RotatePlane()
{
	XMMATRIX view = XMMatrixLookAtLH(XMVectorSet(0.0f, 0.0f, -3.0f, 0.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	XMMATRIX projection = XMMatrixPerspectiveFovLH(XM_PI * 0.45f, (float)m_width / (float)m_height, 0.1f, 1000.0f);

	XMMATRIX finalmatrix = view;
	//finalmatrix *= XMMatrixRotationZ(XMConvertToRadians(45)) * XMMatrixScaling(1.0f, 1.0f, 1.0f);;
	XMStoreFloat4x4(&m_matrix, XMMatrixTranspose(finalmatrix));
	m_graphicsManager->createConstantBuffer("Rotation", &m_matrix, sizeof(XMFLOAT4X4));
}

void Text::UpdateTextQuad()
{
	m_timer += 0.06f;
	if (m_timer >= 1.0f)
	{
		D3D11_MAPPED_SUBRESOURCE mapsub;
		gdeviceContext->Map(m_textPlaneBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapsub);
		memcpy(mapsub.pData, (char*)m_quadData + (sizeof(QuadData) * m_frameIndex * m_vertexAmount), sizeof(QuadData) * m_vertexAmount);
		gdeviceContext->Unmap(m_textPlaneBuffer, 0);

		if (m_frameIndex >= m_framesAmount-1)
		{
			m_frameIndex = 0;
		}
		else
		{
			m_frameIndex++;
		}
		m_timer = 0.0f;
	}
}

void Text::DirectWriteEdge()
{
	// Factory
	CheckStatus(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&m_writeFactory)),
		L"DWriteCreateFactory");

	CString strPath;
	TCHAR* pstrExePath = strPath.GetBuffer(MAX_PATH);

	::GetModuleFileName(0, pstrExePath, MAX_PATH);
	strPath.ReleaseBuffer();

	strPath = strPath.Left(strPath.ReverseFind(L'\\') + 1);
	strPath = strPath.Left(strPath.ReverseFind(L'\\'));
	strPath = strPath.Left(strPath.ReverseFind(L'\\') + 1);
	strPath += L"Assets\\Fonts\\" + m_font;

	CheckStatus(m_writeFactory->CreateFontFileReference(strPath, NULL, &m_fontFiles), L"CreateFontFileReference");

	CheckStatus(m_writeFactory->CreateFontFace(
		DWRITE_FONT_FACE_TYPE_TRUETYPE,
		1,
		&m_fontFiles,
		0,
		DWRITE_FONT_SIMULATIONS_NONE,
		&m_fontFaceBeginning),
		L"CreateFontFace");
	m_fontFaceBeginning->QueryInterface<IDWriteFontFace1>(&m_fontFace);

	float maxSize = 0.0f;
	for (int i = 0; i < 3; i++)
	{
		GetTextOutline(m_text[i], i);
		if (bound[i].right > maxSize)
			maxSize = bound[i].right + bound[i].left;
	}
	// Scale text
	m_scale = (m_quadWidth) / ((maxSize) + m_padding);

	// Center text
	for (int i = 0; i < 3; i++)
	{
		//D2D1::Matrix3x2F const matrix = D2D1::Matrix3x2F(
		//	1.0f, 0.0f,
		//	0.0f, 1.0f,
		//	0,-0
		//);
		D2D1::Matrix3x2F const matrix = D2D1::Matrix3x2F(
			m_scale, 0.0f,
			0.0f, -m_scale,
			//(m_quadWidth * 2) - (bound[i].right / 2), m_quadHeight * 2 + bound[i].bottom
			//m_padding / 2.0f, (m_quadHeight / m_scale) + (bound[0].top + bound[0].bottom)
			//m_padding / 2.0f, (m_quadHeight / 2.0f) - ((abs(bound[0].top) + bound[0].bottom))
			(m_padding * m_scale) / 2.0f, (m_padding * m_scale) / 2.0f
			);
		m_d2dFactory->CreateTransformedGeometry(
			m_pathGeometry[i],
			&matrix,
			&m_transformedPathGeometry[i]);
	}
}

void Text::GetTextOutline(const wchar_t* text, int index)
{
	m_textLength[index] = (UINT32)wcslen(text);
	m_codePoints = new UINT[m_textLength[index]];
	m_glyphIndices = new UINT16[m_textLength[index]];
	ZeroMemory(m_codePoints, sizeof(UINT) * m_textLength[index]);
	ZeroMemory(m_glyphIndices, sizeof(UINT16) * m_textLength[index]);
	for (unsigned int i = 0; i < m_textLength[index]; ++i)
	{
		m_codePoints[i] = text[i];
	}
	CheckStatus(m_fontFace->GetGlyphIndices(m_codePoints, m_textLength[index], m_glyphIndices), L"GetGlyphIndices");

	//Create the path geometry
	CheckStatus(m_d2dFactory->CreatePathGeometry(&m_pathGeometry[index]), L"CreatePathGeometry");
	CheckStatus(m_pathGeometry[index]->Open((ID2D1GeometrySink**)&m_geometrySink), L"Open");
	
	DWRITE_GLYPH_OFFSET glyphOffset = { 100.0f, 1.0f };
	FLOAT glyphAdvances = 250.0f;
	// (48.0f / 72.0f)*96.0f
	CheckStatus(m_fontFace->GetGlyphRunOutline(
		m_textSize,
		m_glyphIndices,
		NULL,
		NULL,
		m_textLength[index],
		false,
		false,
		m_geometrySink),
		L"GetGlyphRunOutline");
	// Getting advances
	m_advances = new int[m_textLength[index]];
	m_fontFace->GetDesignGlyphAdvances(m_textLength[index], m_glyphIndices, m_advances);

	CheckStatus(m_geometrySink->Close(), L"Close");
	
	m_pathGeometry[index]->GetBounds(D2D1::IdentityMatrix(), &bound[index]);
	float hej;
	m_pathGeometry[0]->ComputeLength(D2D1::IdentityMatrix(), &hej);

	// Stroke style
	float dashes[] = { 0.0f };
	CheckStatus(m_d2dFactory->CreateStrokeStyle(
		D2D1::StrokeStyleProperties(
			D2D1_CAP_STYLE_FLAT,
			D2D1_CAP_STYLE_FLAT,
			D2D1_CAP_STYLE_FLAT,
			D2D1_LINE_JOIN_MITER,
			10.0f,
			D2D1_DASH_STYLE_SOLID,
			0.0f),
		NULL,
		NULL,
		&m_strokeStyle),
		L"CreateStrokeStyle");
}

void Text::EdgeRender()
{
	for (int i = 0; i < 3; i++)
	{
		// Clear
		m_d2dRenderTarget[i]->BeginDraw();
		m_d2dRenderTarget[i]->SetTransform(D2D1::IdentityMatrix());
		m_d2dRenderTarget[i]->Clear(d2dClearColor);

		//// // Draw text with outline
		//m_d2dRenderTarget[i]->SetTransform(
		//	//D2D1::Matrix3x2F::Translation(-2000, 2000) * 
		//	D2D1::Matrix3x2F::Translation(800, -800) *
		//	D2D1::Matrix3x2F::Scale(m_scale, -m_scale)
		//	//D2D1::Matrix3x2F::Rotation(270.0f)
		//);
		m_d2dRenderTarget[i]->DrawGeometry(m_transformedPathGeometry[i], m_blackBrush[i], m_edgeSize * m_scale);
		m_d2dRenderTarget[i]->FillGeometry(m_transformedPathGeometry[i], m_orangeBrush[i]);

		m_d2dRenderTarget[i]->EndDraw();
	}
}

void Text::CalculateSize()
{
	// Standard
	//gdeviceContext->OMSetRenderTargets(1, &resources.renderTargetViews["Final"], nullptr);
	//gdeviceContext->ClearRenderTargetView(resources.renderTargetViews["Final"], clearColor);
	gdeviceContext->OMSetRenderTargets(1, manager->getBackbuffer(), nullptr);
	gdeviceContext->ClearRenderTargetView(*manager->getBackbuffer(), clearColor);

	gdeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	gdeviceContext->IASetInputLayout(resources.inputLayouts["FirstLayout"]);
	gdeviceContext->PSSetSamplers(0, 1, &resources.samplerStates["Linear"]);
	//gdeviceContext->VSSetConstantBuffers(0, 1, &resources.constantBuffers["Rotation"]);

	gdeviceContext->VSSetShader(resources.vertexShaders["Text_VS"], nullptr, 0);
	gdeviceContext->PSSetShader(resources.pixelShaders["Text_Size_PS"], nullptr, 0);

	gdeviceContext->IASetVertexBuffers(0, 1, manager->getQuad(), &vertexSize, &offset);
	gdeviceContext->PSSetShaderResources(0, 1, &resources.shaderResourceViews["UV"]);
	gdeviceContext->Begin(m_query);
	gdeviceContext->Draw(4, 0);

	// Query
	gdeviceContext->End(m_query);
	UINT64 queryData;
	while (S_OK != gdeviceContext->GetData(m_query, &queryData, m_query->GetDataSize(), 0)){}
	
	// Calculate texture size
	if (queryData > 0)
	{
		m_height = queryData / (m_uvWidth / m_uvHeight);
		m_height = sqrt(m_height);
		m_width = m_height * (m_uvWidth / m_uvHeight);
	}

	// ComputeShader
	gdeviceContext->CSSetUnorderedAccessViews(0, 1, &m_uav, nullptr);
	gdeviceContext->CSSetShaderResources(0, 1, &resources.shaderResourceViews["UV"]);
	gdeviceContext->CSSetShader(resources.computeShaders["Text_CS"], nullptr, 0);
	gdeviceContext->CSSetSamplers(0, 1, &resources.samplerStates["Linear"]);
	//gdeviceContext->Dispatch(32, 40, 1);
	gdeviceContext->Dispatch(1, 1, 1);
	gdeviceContext->CSSetUnorderedAccessViews(0, 1, &m_nullUAV, nullptr);
	gdeviceContext->CSSetShader(nullptr, nullptr, 0);
	gdeviceContext->CopyResource(m_buffer2, m_buffer);

	//// Pixel shader corner location
	//gdeviceContext->PSSetShader(resources.pixelShaders["TextCorners_PS"], nullptr, 0);
	//gdeviceContext->PSSetConstantBuffers(0, 1, &m_buffer);
	//gdeviceContext->Draw(4, 0);
}

ID3D11ShaderResourceView** Text::GetText()
{
	return finalText;
}

void Text::CheckStatus(HRESULT hr, LPCTSTR titel)
{
	if (hr == E_ABORT)
		MessageBox(NULL, L"HREULT = E_ABORT", titel, NULL);
	else if(hr == E_ACCESSDENIED)
		MessageBox(NULL, L"HREULT = E_ACCESSDENIED", titel, NULL);
	else if (hr == E_FAIL)
		MessageBox(NULL, L"HREULT = E_FAIL", titel, NULL);
	else if (hr == E_HANDLE)
		MessageBox(NULL, L"HREULT = E_HANDLE", titel, NULL);
	else if (hr == E_INVALIDARG)
		MessageBox(NULL, L"HREULT = E_INVALIDARG", titel, NULL);
	else if (hr == E_NOINTERFACE)
		MessageBox(NULL, L"HREULT = E_NOINTERFACE", titel, NULL);
	else if (hr == E_NOTIMPL)
		MessageBox(NULL, L"HREULT = E_NOTIMPL", titel, NULL);
	else if (hr == E_OUTOFMEMORY)
		MessageBox(NULL, L"HREULT = E_OUTOFMEMORY", titel, NULL);
	else if (hr == E_POINTER)
		MessageBox(NULL, L"HREULT = E_POINTER", titel, NULL);
	else if (hr == E_UNEXPECTED)
		MessageBox(NULL, L"HREULT = E_UNEXPECTED", titel, NULL);
}