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

		m_timer[i] = 0.0f;
		m_frameIndex[i] = 0;
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
		// Initialize Systems
		m_text[0] = L"Text";
		m_text[1] = L"Pommes";
		m_text[2] = L"68";
		InitializeDirect2D();
		DirectWriteEdge();
		m_firstTime = false;
	}

	gdeviceContext->OMSetRenderTargets(1, manager->getBackbuffer(), nullptr);
	gdeviceContext->ClearRenderTargetView(*manager->getBackbuffer(), clearColor);

	gdeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	gdeviceContext->IASetInputLayout(resources.inputLayouts["FirstLayout"]);
	gdeviceContext->PSSetSamplers(0, 1, &resources.samplerStates["Linear"]);

	gdeviceContext->VSSetShader(resources.vertexShaders["Text_VS"], nullptr, 0);
	gdeviceContext->PSSetShader(resources.pixelShaders["Text_PS"], nullptr, 0);
	gdeviceContext->VSSetConstantBuffers(0, 1, &m_cameraBuffer);

	for (size_t i = 0; i < 3; i++)
	{
			Update(i);
		gdeviceContext->IASetVertexBuffers(0, 1, &m_textPlaneBuffer[i], &vertexSize, &offset);
		EdgeRender();

		// FXAA
		if (m_fxaa)
		{
			gdeviceContext->OMSetRenderTargets(1, &resources.renderTargetViews["Final"], nullptr);
		}

		// Set textures
		gdeviceContext->PSSetShaderResources(0, 1, &finalText[i]);
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
	}
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

	// Read files
	D3D11_BUFFER_DESC bufferDesc;
	D3D11_SUBRESOURCE_DATA data;
	m_quadData = new QuadData*[3];
	m_quad = new QuadData*[3];
	for (size_t i = 0; i < 3; i++)
	{
		if(i == 0)
			m_infile.open("Assets/Fonts/TextPlane1.bin", std::ofstream::binary);
		else if (i == 1)
			m_infile.open("Assets/Fonts/TextPlane2.bin", std::ofstream::binary);
		else if (i == 2)
			m_infile.open("Assets/Fonts/TextPlane3.bin", std::ofstream::binary);
		if (m_infile)
		{
			m_infile.read((char*)&m_framesAmount[i], sizeof(int));
			m_infile.read((char*)&m_vertexAmount[i], sizeof(int));
			m_quadData[i] = new QuadData[m_framesAmount[i] * m_vertexAmount[i]];
			m_camData = new CameraData[m_framesAmount[i]];
			m_infile.read((char*)m_quadData[i], sizeof(QuadData) * m_framesAmount[i] * m_vertexAmount[i]);
			m_infile.read((char*)m_camData, sizeof(CameraData) * m_framesAmount[i]);
		}
		m_infile.close();

		m_quad[i] = new QuadData[m_vertexAmount[i]];
		float maxX = -D3D11_FLOAT32_MAX;
		float minX = D3D11_FLOAT32_MAX;
		float maxY = -D3D11_FLOAT32_MAX;
		float minY = D3D11_FLOAT32_MAX;
		for (size_t j = 0; j < m_vertexAmount[i]; j++)
		{
			m_quad[i][j] = m_quadData[i][j];
			if (m_quad[i][j].pos.x > maxX)
			{
				maxX = m_quad[i][j].pos.x;
			}
			if (m_quad[i][j].pos.x < minX)
			{
				minX = m_quad[i][j].pos.x;
			}

			if (m_quad[i][j].pos.y > maxY)
			{
				maxY = m_quad[i][j].pos.y;
			}
			if (m_quad[i][j].pos.y < minY)
			{
				minY = m_quad[i][j].pos.y;
			}
		}
		m_quadWidth[i] = (maxX - minX) * 0.5f * m_width;
		m_quadHeight[i] = (maxY - minY) * 0.5f * m_height;

		memset(&bufferDesc, 0, sizeof(bufferDesc));
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.ByteWidth = sizeof(QuadData) * m_vertexAmount[i];

		data.pSysMem = m_quad[i];
		gdevice->CreateBuffer(&bufferDesc, &data, &m_textPlaneBuffer[i]);
	}

	// Camera
	XMMATRIX view = XMMatrixLookAtLH(
		XMVectorSet(m_camData[0].position.x, m_camData[0].position.y, m_camData[0].position.z, 0.0f),
		XMVectorSet(m_camData[0].focus.x, m_camData[0].focus.y, m_camData[0].focus.z, 0.0f),
		XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	XMStoreFloat4x4(&m_matrix, XMMatrixTranspose(view));
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	data.pSysMem = &m_matrix;
	gdevice->CreateBuffer(&bufferDesc, &data, &m_cameraBuffer);

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
	// Factory
	CheckStatus(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_d2dFactory), L"D2D1CreateFactory");

	// Device & DeviceContext
	CheckStatus(gdevice->QueryInterface(__uuidof(IDXGIDevice), (void **)&m_dxgiDevice), L"QueryInterface");
	CheckStatus(m_d2dFactory->CreateDevice(m_dxgiDevice, &m_d2dDev), L"CreateDevice");
	CheckStatus(m_d2dDev->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &m_d2dDevcon), L"CreateDeviceContext");

	for (int i = 0; i < 3; i++)
	{
		if (m_ssaa)
		{
			m_quadHeight[i] *= 4;
			m_quadWidth[i] *= 4;
		}
		// Rendertarget
		D3D11_TEXTURE2D_DESC texDesc;
		texDesc.ArraySize = 1;
		texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0;
		texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		texDesc.Height = m_quadHeight[i];
		texDesc.Width = m_quadWidth[i];
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
			m_quadWidth[i],
			m_quadHeight[i],
			true,
			true,
			d2dTextureTarget[i]
		);
		finalText[i] = resources.shaderResourceViews["Text" + to_string(i)];
	}
}

void Text::Update(unsigned int id)
{
	m_timer[id] += 0.06f;
	if (m_timer[id] >= 1.0f)
	{
		D3D11_MAPPED_SUBRESOURCE mapsub;
		gdeviceContext->Map(m_textPlaneBuffer[id], 0, D3D11_MAP_WRITE_DISCARD, 0, &mapsub);
		memcpy(mapsub.pData, (char*)m_quadData[id] + (sizeof(QuadData) * m_frameIndex[id] * m_vertexAmount[id]), sizeof(QuadData) * m_vertexAmount[id]);
		gdeviceContext->Unmap(m_textPlaneBuffer[id], 0);

		if (id == 0)
		{
			//XMMATRIX view = XMMatrixLookAtLH(XMVectorSet(0.0f, 0.0f, -3.0f, 0.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
			XMMATRIX view = XMMatrixLookAtLH(
				XMVectorSet(m_camData[m_frameIndex[id]].position.x, m_camData[m_frameIndex[id]].position.y, m_camData[m_frameIndex[id]].position.z, 0.0f),
				XMVectorSet(m_camData[m_frameIndex[id]].focus.x, m_camData[m_frameIndex[id]].focus.y, m_camData[m_frameIndex[id]].focus.z, 0.0f),
				XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
			XMStoreFloat4x4(&m_matrix, XMMatrixTranspose(view));

			gdeviceContext->Map(m_cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapsub);
			memcpy(mapsub.pData, (char*)&m_matrix, sizeof(XMFLOAT4X4));
			gdeviceContext->Unmap(m_cameraBuffer, 0);
		}

		if (m_frameIndex[id] >= m_framesAmount[id] -1)
		{
			m_frameIndex[id] = 0;
		}
		else
		{
			m_frameIndex[id]++;
		}
		m_timer[id] = 0.0f;
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
	m_scale = (m_quadWidth[0]) / ((maxSize) + m_padding);

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
		m_d2dRenderTarget[i]->Clear(NULL);

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