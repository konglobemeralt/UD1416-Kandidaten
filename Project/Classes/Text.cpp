#include "Text.h"

Text::Text()
{

}

Text::~Text()
{
	// Direct2D
	m_d2dFactory ? m_d2dFactory->Release() : 0;
	m_dxgiDevice ? m_dxgiDevice->Release() : 0;
	m_d2dDev ? m_d2dDev->Release() : 0;
	m_d2dDevcon ? m_d2dDevcon->Release() : 0;
	//d2dTextureTarget ? d2dTextureTarget->Release() : 0;
	//m_idxgSurface ? m_idxgSurface->Release() : 0;
	//m_d2dRenderTarget ? m_d2dRenderTarget->Release() : 0;
	//m_blackBrush ? m_blackBrush->Release() : 0;
	//m_orangeBrush ? m_orangeBrush->Release() : 0;

	// DirectWrite
	m_writeFactory ? m_writeFactory->Release() : 0;
	m_writeTextFormat ? m_writeTextFormat->Release() : 0;

	// EdgeTesting
	//m_pathGeometry ? m_pathGeometry->Release() : 0;
	m_geometrySink ? m_geometrySink->Release() : 0;
	m_fontFaceBeginning ? m_fontFaceBeginning->Release() : 0;
	m_fontFace ? m_fontFace->Release() : 0;
	m_fontFiles ? m_fontFiles->Release() : 0;
	m_strokeStyle ? m_strokeStyle->Release() : 0;
	m_codePoints ? delete[] m_codePoints : 0;
	m_glyphIndices ? delete[] m_glyphIndices : 0;
	m_advances ? delete[] m_advances : 0;
}

void Text::Render() {
	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	UINT vertexSize = sizeof(float) * 5;
	UINT offset = 0;

	gdeviceContext->OMSetRenderTargets(1, manager->getBackbuffer(), nullptr);
	gdeviceContext->ClearRenderTargetView(*manager->getBackbuffer(), clearColor);

	gdeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	gdeviceContext->IASetInputLayout(resources.inputLayouts["FirstLayout"]);
	gdeviceContext->PSSetSamplers(0, 1, &resources.samplerStates["Linear"]);
	gdeviceContext->PSSetSamplers(1, 1, &resources.samplerStates["Point"]);
	gdeviceContext->VSSetConstantBuffers(0, 1, &resources.constantBuffers["Rotation"]);

	gdeviceContext->VSSetShader(resources.vertexShaders["Text_VS"], nullptr, 0);
	gdeviceContext->PSSetShader(resources.pixelShaders["Text_PS"], nullptr, 0);

	gdeviceContext->IASetVertexBuffers(0, 1, manager->getQuad(), &vertexSize, &offset);

	//RenderText();
	EdgeRender();

	gdeviceContext->PSSetShaderResources(0, 1, &resources.shaderResourceViews["UV"]);
	//gdeviceContext->PSSetShaderResources(1, 1, &resources.shaderResourceViews["Erik"]);
	gdeviceContext->PSSetShaderResources(1, 1, &finalText[0]);
	gdeviceContext->PSSetShaderResources(2, 1, &resources.shaderResourceViews["U"]);
	gdeviceContext->PSSetShaderResources(3, 1, &resources.shaderResourceViews["V"]);
	gdeviceContext->PSSetConstantBuffers(0, 1, &resources.constantBuffers["Rotation"]);

	gdeviceContext->Draw(4, 0);

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
		"myRTV",
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		manager->getWindowWidth(),
		manager->getWindowHeight(),
		true,
		false
	);

	// Only SRV
	manager->createTexture2D(
		"mySRV",
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		manager->getWindowWidth(),
		manager->getWindowHeight(),
		true,
		false
	);

	// Both
	manager->createTexture2D("myRTVandSRV");

	// Add image on an SRV (base filepath will be set to the assets folder automatically)
	//m_graphicsManager->attachImage("ToneMapping/Arches_E_PineTree_Preview.jpg", "mySRV");



	// ###########################################################
	// ######		Render target & shader resource			######
	// ###########################################################
	//	void createSamplerState(
	//		string name,
	//		D3D11_FILTER filter = D3D11_FILTER_MIN_MAG_MIP_POINT,
	//		D3D11_TEXTURE_ADDRESS_MODE mode = D3D11_TEXTURE_ADDRESS_CLAMP
	//	);

	manager->attachImage("Fonts/Images/Cheese.jpg", "Erik");
	manager->attachImage("Fonts/Images/uv.png", "UV");
	manager->attachImage("Fonts/UV/XUV.png", "U");
	manager->attachImage("Fonts/UV/VUV.png", "V");
	manager->createSamplerState("Linear", D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP);
	manager->createSamplerState("Point", D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP);

	// Initialize Systems
	InitializeDirect2D();
	//InitializeDirectWrite();
	DirectWriteEdge();
	RotatePlane();
}

void Text::InitializeDirect2D()
{
	// Get values
	m_height = manager->getWindowHeight();
	m_width = manager->getWindowWidth();
	float doubleHeight = manager->getWindowHeight()*4;
	float doubleWidth = manager->getWindowWidth()*4;

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
	texDesc.Height = doubleHeight;
	texDesc.Width = doubleWidth;
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
			doubleWidth,
			doubleHeight,
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
	m_textLength = (UINT32)wcslen(m_text[0]);
	CheckStatus(m_writeFactory->CreateTextFormat(
		L"Arial",
		NULL,
		DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		m_textSize,
		L"en-us",
		&m_writeTextFormat),
		L"CreateTextFormat");

	// Center align the text.
	CheckStatus(m_writeTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER), L"SetTextAlignment");
	CheckStatus(m_writeTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER), L"SetParagraphAlignment");

	// layout to draw on
	m_layoutRect = D2D1::RectF(
		static_cast<FLOAT>(0.0f),
		static_cast<FLOAT>(0.0f),
		static_cast<FLOAT>(manager->getWindowWidth()),
		static_cast<FLOAT>(manager->getWindowHeight())
	);
}

//void Text::RenderText()
//{
//	m_d2dRenderTarget->BeginDraw();
//	m_d2dRenderTarget->SetTransform(D2D1::IdentityMatrix());
//	//m_d2dRenderTarget->SetTransform(D2D1::Matrix3x2F::Scale(-1.0f, 1.0f) * D2D1::Matrix3x2F::Translation(1000, 0));	// Must flip texture before send to Compositing
//	m_d2dRenderTarget->Clear(NULL);
//	// Call the DrawText method of this class.
//	//m_d2dRenderTarget->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE::D2D1_TEXT_ANTIALIAS_MODE_ALIASED);
//	m_d2dRenderTarget->DrawText(
//		m_text,					// The string to render.
//		m_textLength,			// The string's length.
//		m_writeTextFormat,		// The text format.
//		m_layoutRect,			// The region of the window where the text will be rendered.
//		m_orangeBrush			// The brush used to draw the text.
//	);
//	m_d2dRenderTarget->EndDraw();
//}

void Text::RotatePlane()
{
	XMMATRIX view = XMMatrixLookAtLH(XMVectorSet(0.0f, 0.0f, -3.0f, 0.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	XMMATRIX projection = XMMatrixPerspectiveFovLH(XM_PI * 0.45f, m_width / m_height, 0.1f, 1000.0f);

	XMMATRIX finalmatrix = view;
	//finalmatrix *= XMMatrixRotationZ(XMConvertToRadians(45)) * XMMatrixScaling(1.0f, 1.0f, 1.0f);;
	XMStoreFloat4x4(&m_matrix, XMMatrixTranspose(finalmatrix));
	m_graphicsManager->createConstantBuffer("Rotation", &m_matrix, sizeof(XMFLOAT4X4));
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

	m_text[0] = L"Text";
	m_text[1] = L"Pommes";
	m_text[2] = L"68";
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
	for (int i = 0; i < 3; i++)
	{
		D2D1::Matrix3x2F const matrix = D2D1::Matrix3x2F(
			1.0f, 0.0f,
			0.0f, 1.0f,
			//(m_uvWidth / 2) - (bound[i].right / 2) * m_scale, m_uvHeight * m_scale
			0,1000
		);
		m_d2dFactory->CreateTransformedGeometry(
			m_pathGeometry[i],
			&matrix,
			&m_transformedPathGeometry[i]);
	}

	// Glyphrun
	m_glyphRun.fontFace = m_fontFace;
	m_glyphRun.fontEmSize = m_textSize;
	m_glyphRun.glyphCount = m_textLength;
	m_glyphRun.glyphIndices = m_glyphIndices;
	//m_glyphRun.glyphAdvances = ;
	//m_glyphRun.glyphOffsets = ;
	m_glyphRun.isSideways = false;
	//m_glyphRun.bidiLevel = ;

	m_edgeSize = 10.0f;
}

void Text::GetTextOutline(const wchar_t* text, int index)
{
	m_textLength = (UINT32)wcslen(text);
	m_codePoints = new UINT[m_textLength];
	m_glyphIndices = new UINT16[m_textLength];
	ZeroMemory(m_codePoints, sizeof(UINT) * m_textLength);
	ZeroMemory(m_glyphIndices, sizeof(UINT16) * m_textLength);
	for (int i = 0; i<m_textLength; ++i)
	{
		m_codePoints[i] = text[i];
	}
	CheckStatus(m_fontFace->GetGlyphIndices(m_codePoints, m_textLength, m_glyphIndices), L"GetGlyphIndices");

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
		m_textLength,
		false,
		false,
		m_geometrySink),
		L"GetGlyphRunOutline");
	// Getting advances
	m_advances = new int[m_textLength];
	m_fontFace->GetDesignGlyphAdvances(m_textLength, m_glyphIndices, m_advances);

	CheckStatus(m_geometrySink->Close(), L"Close");
	
	m_pathGeometry[index]->GetBounds(D2D1::IdentityMatrix(), &bound[index]);

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

		// // Draw text with outline
		m_d2dRenderTarget[i]->SetTransform(
			//D2D1::Matrix3x2F::Translation(0, -((m_textSize) / 2)) * 
			D2D1::Matrix3x2F::Scale(m_scale*6, m_scale*6));
		m_d2dRenderTarget[i]->DrawGeometry(m_transformedPathGeometry[i], m_blackBrush[i], m_edgeSize);
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