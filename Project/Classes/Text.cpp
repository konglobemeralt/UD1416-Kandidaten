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

	m_imageFXAA = false;
	m_imageSSAA = false;
	m_firstTime = true;
	m_edgeRendering = true;
	m_fxaa = false;
	m_ssaa = false;
	m_thesis = true;
	m_ssaaSize = 1; // 1 when no ssaa
	m_sizeMultiplier = 1.0f;
	m_height = 0;
	m_width = 0;
	m_uvWidth = 0.0f;
	m_uvHeight = 0.0f;
	m_textSize = 75.0f * m_ssaaSize * m_sizeMultiplier;
	m_edgeSize = 4.0f * m_ssaaSize * m_sizeMultiplier;
	m_padding = 50.0f;
	m_scale = 1.0f;
	m_simonImage = "PowerLine";
	m_render = NONE;

	if (m_render == FXAA_SSAA)
	{
		m_imageFXAA = false;
		m_imageSSAA = true;
		m_fxaa = true;
		m_ssaa = false;
		m_ssaaSize = 1;
	}
	else if (m_render == FXAA_SSAA_FXAA)
	{
		m_imageFXAA = true;
		m_imageSSAA = true;
		m_fxaa = true;
		m_ssaa = false;
		m_ssaaSize = 1;
	}
	else if (m_render == SSAA_SSAA)
	{
		m_imageFXAA = false;
		m_imageSSAA = true;
		m_fxaa = false;
		m_ssaa = true;
		m_ssaaSize = 2;
		m_textSize *= m_ssaaSize;
		m_edgeSize *= m_ssaaSize;
	}
	else if (m_render == NONE)
	{
		m_imageFXAA = false;
		m_imageSSAA = false;
		m_fxaa = false;
		m_ssaa = false;
		m_ssaaSize = 1;
	}

	d2dClearColor.r = 0.0f;
	d2dClearColor.g = 0.0f;
	d2dClearColor.b = 0.0f;
	d2dClearColor.a = 0.0f;

	// Camera
	camPos = XMFLOAT4(-0.2f, 0.0f, -2.0f, 1.0f);
	camLook = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	camUp = XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);
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
		m_text[0] = L"";
		m_text[1] = L"This is";
		m_text[2] = L"a test!";
		InitializeDirect2D();
		if(m_edgeRendering)
			DirectWriteEdge();
		else
			InitializeDirectWrite();
		m_firstTime = false;
	}

	// Camera
	DetectInput();
	UpdateFreeLookCamera();

	gdeviceContext->OMSetRenderTargets(1, manager->getBackbuffer(), nullptr);
	gdeviceContext->ClearRenderTargetView(*manager->getBackbuffer(), clearColor);

	gdeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	gdeviceContext->IASetInputLayout(resources.inputLayouts["FirstLayout"]);
	gdeviceContext->PSSetSamplers(0, 1, &resources.samplerStates["Linear"]);
	gdeviceContext->PSSetSamplers(1, 1, &resources.samplerStates["Point"]);
	gdeviceContext->VSSetConstantBuffers(0, 1, &resources.constantBuffers["Matrices"]);
	gdeviceContext->VSSetShader(resources.vertexShaders["Text_VS"], nullptr, 0);
	gdeviceContext->PSSetShader(resources.pixelShaders["Text_PS"], nullptr, 0);
	gdeviceContext->IASetVertexBuffers(0, 1, manager->getQuad(), &vertexSize, &offset);

	// Text render
	if (m_edgeRendering)
		EdgeRender();
	else
		RenderText();

	// FXAA_SSAA
	if (m_fxaa && !m_ssaa && !m_imageFXAA && m_imageSSAA)
	{
		gdeviceContext->PSSetShaderResources(1, 1, &resources.shaderResourceViews["Simon"]);
		gdeviceContext->Draw(4, 0);
		gdeviceContext->PSSetShaderResources(0, 1, &resources.shaderResourceViews["Text0"]);
		gdeviceContext->PSSetShader(resources.pixelShaders["FXAA_PS"], nullptr, 0);
		gdeviceContext->PSSetConstantBuffers(0, 1, &resources.constantBuffers["FXAA_PS_cb"]);
		gdeviceContext->Draw(4, 0);
	}

	// FXAA_SSAA_FXAA
	else if (m_fxaa && !m_ssaa && m_imageFXAA && m_imageSSAA)
	{
		// Set render target view
		gdeviceContext->OMSetRenderTargets(1, &resources.renderTargetViews["Final"], nullptr);
		gdeviceContext->ClearRenderTargetView(resources.renderTargetViews["Final"], clearColor);

		// Draw textures
		gdeviceContext->PSSetShaderResources(1, 1, &resources.shaderResourceViews["Simon"]);
		gdeviceContext->Draw(4, 0);
		gdeviceContext->PSSetShaderResources(1, 1, &resources.shaderResourceViews["Text0"]);
		gdeviceContext->Draw(4, 0);

		// Do FXAA on both at same time
		gdeviceContext->OMSetRenderTargets(1, manager->getBackbuffer(), nullptr);
		gdeviceContext->ClearRenderTargetView(*manager->getBackbuffer(), clearColor);
		gdeviceContext->PSSetShaderResources(0, 1, &resources.shaderResourceViews["Final"]);
		gdeviceContext->PSSetShader(resources.pixelShaders["FXAA_PS"], nullptr, 0);
		gdeviceContext->PSSetConstantBuffers(0, 1, &resources.constantBuffers["FXAA_PS_cb"]);
		gdeviceContext->Draw(4, 0);
		gdeviceContext->PSSetShaderResources(0, 1, &resources.shaderResourceViews["NULL"]);
	}

	// SSAA_SSAA
	else if (!m_fxaa && m_ssaa && !m_imageFXAA && m_imageSSAA)
	{
		// Set textures
		gdeviceContext->PSSetShaderResources(1, 1, &resources.shaderResourceViews["Simon"]);
		gdeviceContext->Draw(4, 0);
		gdeviceContext->PSSetShaderResources(1, 1, &resources.shaderResourceViews["Text0"]);
		gdeviceContext->Draw(4, 0);
	}

	// Without
	else if (!m_fxaa && !m_ssaa && !m_imageFXAA && !m_imageSSAA)
	{
		// Set textures
		gdeviceContext->PSSetShaderResources(1, 1, &resources.shaderResourceViews["Simon"]);
		gdeviceContext->Draw(4, 0);
		gdeviceContext->PSSetShaderResources(1, 1, &resources.shaderResourceViews["Text0"]);
		gdeviceContext->Draw(4, 0);
	}

	//// Save image
	//if (m_fxaa && m_imageSSAA)
	//	manager->saveImage("Fonts/Saved/Combined/" + m_simonImage + "_FXAA_SSAA.png", manager->pBackBuffer);
	//else if (m_fxaa && m_imageSSAA && m_imageFXAA)
	//	manager->saveImage("Fonts/Saved/Combined/" + m_simonImage + "_FXAA_SSAA_FXAA.png", manager->pBackBuffer);
	//else if (m_ssaa && m_imageSSAA)
	//	manager->saveImage("Fonts/Saved/Combined/" + m_simonImage + "_SAAA_SSAA.png", manager->pBackBuffer);
	//else if(!m_fxaa && !m_ssaa && !m_imageFXAA && !m_imageSSAA)
	//	manager->saveImage("Fonts/Saved/Combined/" + m_simonImage + "_None.png", manager->pBackBuffer);

	//// Genereate mip maps
	//gdeviceContext->PSSetShaderResources(0, 1, &resources.shaderResourceViews["NULL"]);
	//gdeviceContext->CopyResource(tempD2DTexture, d2dTextureTarget);
	//gdeviceContext->GenerateMips(resources.shaderResourceViews["Text"]);
}

void Text::Initialize() {
	manager = ApplicationContext::GetInstance().GetGraphicsManager();
	wndHandle = ApplicationContext::GetInstance().GetWindowManager()->getWindowHandle();
	InitDirectInput(*ApplicationContext::GetInstance().GetWindowManager()->getHinstance());
	// ###########################################################
	// ######				Constant buffer					######
	// ###########################################################
	//	void createConstantBuffer(
	//		string name,
	//		D3D11_BUFFER_DESC desc,
	//		const void* data
	//	);

	// Matrices
	XMMATRIX view = XMMatrixLookAtLH(XMVectorSet(0.0f, 0.0f, -3.0f, 0.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	XMMATRIX projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), (float)manager->getWindowWidth() / (float)manager->getWindowHeight(), 0.1f, 1000.0f);
	//projection = XMMatrixOrthographicOffCenterLH(0.0f, (float)manager->getWindowWidth(), (float)manager->getWindowHeight(), 0.0f, 0.1f, 1000.0f);

	XMMATRIX transform = XMMatrixRotationY(XMConvertToRadians(-65)) * XMMatrixRotationZ(XMConvertToRadians(45));
	//XMMATRIX transform = XMMatrixIdentity();
	//transform = XMMatrixIdentity();
	transform *= XMMatrixScaling(1.0f, 1.0f, 1.0f);

	XMStoreFloat4x4(&m_matrices.world, XMMatrixTranspose(transform));
	XMStoreFloat4x4(&m_matrices.view, XMMatrixTranspose(view));
	XMStoreFloat4x4(&m_matrices.projection, XMMatrixTranspose(projection));
	m_matrices.useMatrices.x = 0;
	manager->createConstantBuffer("Matrices", &m_matrices, sizeof(Matrices));

	// FXAA
	struct FXAA_PS_ConstantBuffer { //texelsize n shiet
		XMFLOAT2 texelSizeXY;
		float FXAA_blur_Texels_Threshhold; //hur många texlar som kommer blurras åt varje håll
		float minimumBlurThreshhold; //hur mycket som krävs för att den ens ska blurra
		float FXAA_reduce_MULTIPLIER;
		float FXAA_reduce_MIN; //så dirOffset inte ska bli noll
		XMFLOAT2 pad;
	}FXAA_PS_cb;

	FXAA_PS_cb.texelSizeXY.x = 1.0f / manager->getWindowWidth();
	FXAA_PS_cb.texelSizeXY.y = 1.0f / manager->getWindowHeight();
	FXAA_PS_cb.FXAA_blur_Texels_Threshhold = 2.0f;
	//FXAA_PS_cb.minimumBlurThreshhold = 0.0001f;
	FXAA_PS_cb.FXAA_reduce_MULTIPLIER = 1.0f / 2.0f;
	FXAA_PS_cb.FXAA_reduce_MIN = 1.0f / 64.0f;
	manager->createConstantBuffer("FXAA_PS_cb", &FXAA_PS_cb, sizeof(FXAA_PS_ConstantBuffer));

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

	manager->attachImage("Fonts/Images/Checker.png", "Erik");
	manager->attachImage("Fonts/UV/uv7.png", "UV");
	manager->attachImage("Fonts/UV/XUV.png", "U");
	manager->attachImage("Fonts/UV/VUV.png", "V");

	ID3D11Texture2D* texture;
	ID3D11Resource* resource;
	resources.shaderResourceViews["UV"]->GetResource(&resource);
	resource->QueryInterface<ID3D11Texture2D>(&texture);
	D3D11_TEXTURE2D_DESC texDesc;
	texture->GetDesc(&texDesc);

	m_uvHeight = texDesc.Height;
	m_uvWidth = texDesc.Width;

	manager->createTexture2D("Final",
		DXGI_FORMAT_R32G32B32A32_FLOAT);
	
	string name = "Fonts/Images/Simon/" + m_simonImage;
	if (m_imageSSAA)
		manager->attachImage(name + "Med" + ".png", "Simon");
	else
		manager->attachImage(name + "Low" + ".png", "Simon");

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

	// Query
	D3D11_QUERY_DESC qDesc;
	qDesc.MiscFlags = 0;
	qDesc.Query = D3D11_QUERY_OCCLUSION;
	CheckStatus(gdevice->CreateQuery(&qDesc, &m_query), L"CreateQuery");

	// Set transparency to work
	D3D11_BLEND_DESC blendDesc;
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_INV_DEST_ALPHA;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	gdevice->CreateBlendState(&blendDesc, &m_blendState);
	float blendfactor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	gdeviceContext->OMSetBlendState(m_blendState, blendfactor, 0xffffffff);
}

void Text::InitializeDirect2D()
{
	// Get values
	//m_height = manager->getWindowHeight();
	//m_width = manager->getWindowWidth();
	if (m_thesis)
	{
		m_height = manager->getWindowHeight() * m_sizeMultiplier;
		m_width = manager->getWindowWidth() * m_sizeMultiplier;
	}
	if (m_ssaa)
	{
		m_height *= m_ssaaSize;
		m_width *= m_ssaaSize;
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
	texDesc.Height = m_height;
	texDesc.Width = m_width;
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
			m_width,
			m_height,
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
			maxSize = bound[i].right;
	}
	// Scale text
	m_scale = (m_width - m_padding) / (maxSize - m_padding);

	// Center text
	D2D1::Matrix3x2F matrix;
	for (int i = 0; i < 3; i++)
	{
		if (i == 0)
		{
			matrix = D2D1::Matrix3x2F(
				1.0f, 0.0f,
				0.0f, 1.0f,
				//(m_uvWidth * 2) - (bound[i].right / 2), m_uvHeight*2 + bound[i].bottom
				0, 0
				);
		}
		if (i == 1)
		{
			matrix = D2D1::Matrix3x2F(
				1.0f, 0.0f,
				0.0f, 1.0f,
				0, 0 * m_ssaaSize
				);
		}
		if (i == 2)
		{
			matrix = D2D1::Matrix3x2F(
				1.0f, 0.0f,
				0.0f, 1.0f,
				0, 75 * m_ssaaSize
				);
		}
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

	// Stroke style
	float dashes[] = { 0.0f };
	CheckStatus(m_d2dFactory->CreateStrokeStyle(
		D2D1::StrokeStyleProperties(
			D2D1_CAP_STYLE_FLAT,
			D2D1_CAP_STYLE_FLAT,
			D2D1_CAP_STYLE_FLAT,
			D2D1_LINE_JOIN_MITER_OR_BEVEL,
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
	m_d2dRenderTarget[0]->Clear(d2dClearColor);
	for (int i = 0; i < 3; i++)
	{
		// Clear
		m_d2dRenderTarget[0]->BeginDraw();
		m_d2dRenderTarget[0]->SetTransform(D2D1::IdentityMatrix());

		// // Draw text with outline
		m_d2dRenderTarget[0]->SetTransform(
			D2D1::Matrix3x2F::Translation(280 * m_ssaaSize * m_sizeMultiplier, 65 * m_ssaaSize * m_sizeMultiplier)
			//D2D1::Matrix3x2F::Scale(m_scale, m_scale)
			//D2D1::Matrix3x2F::Rotation(270.0f)
		);
		m_d2dRenderTarget[0]->DrawGeometry(m_transformedPathGeometry[i], m_blackBrush[i], m_edgeSize, m_strokeStyle);
		m_d2dRenderTarget[0]->FillGeometry(m_transformedPathGeometry[i], m_orangeBrush[i]);

		m_d2dRenderTarget[0]->EndDraw();
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
	gdeviceContext->VSSetConstantBuffers(0, 1, &resources.constantBuffers["Matrices"]);

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

void Text::UpdateFreeLookCamera()
{
	XMVECTOR worldForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	XMVECTOR worldRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR camForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	XMVECTOR camRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	//Convert to XM!
	XMMATRIX camRotation;
	XMMATRIX XMViewSpace = XMLoadFloat4x4(&ViewSpace);
	XMVECTOR XMCamLook = XMLoadFloat4(&camLook);
	XMVECTOR XMCamUp = XMLoadFloat4(&camUp);
	XMVECTOR XMCamPos = XMLoadFloat4(&camPos);

	camRotation = XMMatrixRotationRollPitchYaw(camPitch, camYaw, 0);		//Function parameters: (Pitch in radians to rotate, Yaw -||-, Roll -||-);
	XMCamLook = XMVector3TransformCoord(worldForward, camRotation);			//SETS CAMERA TARGET VECTOR
	XMCamLook = XMVector3Normalize(XMCamLook);								//NORMALIZES CAMERA TARGET VECTOR

	camRight = XMVector3TransformCoord(worldRight, camRotation);			//Update Cam Right Vector relative to rotation
	camForward = XMVector3TransformCoord(worldForward, camRotation);		//Update Cam Forward Vector relative to rotation
	XMCamUp = XMVector3Cross(camForward, camRight);							//Set Cam Up Vector with cross product

	XMCamPos += moveLR * camRight;											//Set the left and right position of the camera
	XMCamPos += moveFB * camForward;										//Set the forward and back position of the camera		//Move Along Cams Forward vector

	XMCamLook += XMCamPos;													//Add the camera position to the target (look) vector
	XMViewSpace = XMMatrixLookAtLH(XMCamPos, XMCamLook, XMCamUp);			//SET THE NEW CAMERA POSITION PROPERTIES(Updates the view matrix)
																			//Convert Back to floats!
	//XMCamLook = XMVectorSet(0, 0, 0, 0);
	XMStoreFloat4(&camLook, XMCamLook);
	XMStoreFloat4(&camUp, XMCamUp);
	XMStoreFloat4(&camPos, XMCamPos);
	XMViewSpace = XMMatrixLookAtLH(XMCamPos, XMCamLook, XMCamUp);
	XMStoreFloat4x4(&m_matrices.view, XMMatrixTranspose(XMViewSpace));
	gdeviceContext->UpdateSubresource(resources.constantBuffers["Matrices"], 0, nullptr, &m_matrices, 0, 0);
}

void Text::DetectInput()
{
	DIMOUSESTATE mouseCurrentState;		//Mouse input
	BYTE keyboardState[256];			//Keyboard input

	DIKeyboard->Acquire();
	DIMouse->Acquire();

	DIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrentState);
	DIKeyboard->GetDeviceState(sizeof(keyboardState), (LPVOID)&keyboardState);

	moveLR = 0.0f;
	moveFB = 0.0f;

	//float speed = 0.01;

	if (keyboardState[DIK_ESCAPE] & 0x80)						//Shuts the window when the ESCAPE key is pressed
	{
		PostMessage(*wndHandle, WM_DESTROY, 0, 0);
	}

	if (keyboardState[DIK_LEFT] || keyboardState[DIK_A])		//Moves left when the A key or the LEFT arrow is pressed
	{
		moveLR = -speed;
	}
	if (keyboardState[DIK_RIGHT] || keyboardState[DIK_D])		//Moves right when the D key or the RIGHT arrow is pressed
	{
		moveLR = speed;
	}
	if (keyboardState[DIK_UP] || keyboardState[DIK_W])			//Moves forward when the W key or the UP arrow is pressed
	{
		moveFB = speed;
	}
	if (keyboardState[DIK_DOWN] || keyboardState[DIK_S])		//Moves backwards when the S key or the DOWN arrow is pressed
	{
		moveFB = -speed;
	}
	if ((mouseCurrentState.lX != mouseLastState.lX) || (mouseCurrentState.lY != mouseLastState.lY))		//Rotates the camera with the mouse input!
	{
		camYaw += mouseCurrentState.lX * 0.001f;				//Rotates camera to the right and left
		camPitch += mouseCurrentState.lY * 0.001f;				//Rotates camera up and down
		mouseLastState = mouseCurrentState;
	}
}

bool Text::InitDirectInput(HINSTANCE hInstance)
{
	HRESULT WINAPI DirectInput8Create(
		HINSTANCE hinst,
		DWORD dwVersion,
		REFIID riidltf,
		LPVOID *ppvOut,
		LPUNKNOWN punkOuter
	);

	HRESULT hr = DirectInput8Create(hInstance,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&DirectInput,
		NULL);

	hr = DirectInput->CreateDevice(GUID_SysKeyboard, &DIKeyboard, NULL);

	hr = DirectInput->CreateDevice(GUID_SysMouse, &DIMouse, NULL);

	hr = DIKeyboard->SetDataFormat(&c_dfDIKeyboard);
	hr = DIKeyboard->SetCooperativeLevel(*wndHandle, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	hr = DIMouse->SetDataFormat(&c_dfDIMouse);
	hr = DIMouse->SetCooperativeLevel(*wndHandle, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);

	return true;
}