#include "Text.h"
#include "GraphicsManager.h"
#include "..\ApplicationContext.h"
Text::Text()
{

}

Text::~Text()
{
	m_d2dFactory->Release();
	m_dxgiDevice->Release();
	m_d2dDev->Release();
	m_d2dDevcon->Release();
	m_idxgSurface->Release();
	m_d2dRenderTarget->Release();
	m_blackBrush->Release();
	m_orangeBrush->Release();

	m_writeFactory->Release();
	m_writeTextFormat->Release();
}

void Text::Render() {
	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	UINT vertexSize = sizeof(float) * 5;
	UINT offset = 0;

	gdeviceContext->OMSetRenderTargets(1, m_graphicsManager->getBackbuffer(), nullptr);
	gdeviceContext->ClearRenderTargetView(*m_graphicsManager->getBackbuffer(), clearColor);

	gdeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	gdeviceContext->IASetInputLayout(m_graphicsManager->thesisData.inputLayouts["FirstLayout"]);
	gdeviceContext->PSSetSamplers(0, 1, &m_graphicsManager->thesisData.samplerStates["CoolSampler"]);

	gdeviceContext->VSSetShader(m_graphicsManager->thesisData.vertexShaders["VertexShader"], nullptr, 0);
	gdeviceContext->PSSetShader(m_graphicsManager->thesisData.pixelShaders["PixelShader"], nullptr, 0);

	gdeviceContext->IASetVertexBuffers(0, 1, m_graphicsManager->getQuad(), &vertexSize, &offset);

	RenderText();

	gdeviceContext->PSSetShaderResources(0, 1, &m_graphicsManager->thesisData.shaderResourceViews["Text"]);

	gdeviceContext->Draw(4, 0);

	gdeviceContext->PSSetShaderResources(0, 1, &m_graphicsManager->thesisData.shaderResourceViews["NULL"]);
}

void Text::Initialize() {
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
		XMFLOAT4X4 matrix;
	}myMatrix;

	m_graphicsManager->createConstantBuffer("myMatrix", &myMatrix, sizeof(cBuffer));



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

	m_graphicsManager->createVertexShader("VertexShader", "FirstLayout", layoutDesc, ARRAYSIZE(layoutDesc));



	// ###########################################################
	// ######				Other Shaders					######
	// ###########################################################
	//	void createPixelShader(
	//		string name
	//			);

	m_graphicsManager->createPixelShader("PixelShader"); // Name has to match shader name without .hlsl



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
	//m_graphicsManager->attachImage("ToneMapping/Arches_E_PineTree_Preview.jpg", "mySRV");



	// ###########################################################
	// ######		Render target & shader resource			######
	// ###########################################################
	//	void createSamplerState(
	//		string name,
	//		D3D11_FILTER filter = D3D11_FILTER_MIN_MAG_MIP_POINT,
	//		D3D11_TEXTURE_ADDRESS_MODE mode = D3D11_TEXTURE_ADDRESS_CLAMP
	//	);

	m_graphicsManager->createSamplerState("CoolSampler", D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP);

	// Initialize Systems
	InitializeDirect2D();
	InitializeDirectWrite();
}

void Text::InitializeDirect2D()
{
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
	texDesc.Height = m_graphicsManager->getWindowHeight();
	texDesc.Width = m_graphicsManager->getWindowWidth();
	texDesc.MipLevels = 1;
	texDesc.MiscFlags = 0;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	ID3D11Texture2D* d2dTextureTarget;
	CheckStatus(gdevice->CreateTexture2D(&texDesc, NULL, &d2dTextureTarget), L"CreateTexture2D");

	d2dTextureTarget->QueryInterface(&m_idxgSurface);
	d2dTextureTarget->Release();
	D2D1_RENDER_TARGET_PROPERTIES props =
		D2D1::RenderTargetProperties(
			D2D1_RENDER_TARGET_TYPE_DEFAULT,
			D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
			96,
			96);
	CheckStatus(m_d2dFactory->CreateDxgiSurfaceRenderTarget(m_idxgSurface, &props, &m_d2dRenderTarget), L"CreateDxgiSurfaceRenderTarget");
	m_graphicsManager->thesisData.shaderResourceViews["NULL"] = nullptr;

	// Brushes
	CheckStatus(m_d2dRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &m_blackBrush), L"CreateSolidColorBrush");
	CheckStatus(m_d2dRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Orange), &m_orangeBrush), L"CreateSolidColorBrush");

	// Create the render target view.
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = texDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;
	m_graphicsManager->createTexture2D(
		"Text",
		texDesc.Format,
		m_graphicsManager->getWindowWidth(),
		m_graphicsManager->getWindowHeight(),
		true,
		false,
		d2dTextureTarget
	);

	// Setup the description of the shader resource view.
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = texDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;
	m_graphicsManager->createTexture2D(
		"Text",
		texDesc.Format,
		m_graphicsManager->getWindowWidth(),
		m_graphicsManager->getWindowHeight(),
		false,
		true,
		d2dTextureTarget
	);
}

void Text::InitializeDirectWrite()
{
	// Factory
	CheckStatus(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&m_writeFactory)),
		L"DWriteCreateFactory");

	// Font
	m_text = L"KHK";
	m_textLength = (UINT32)wcslen(m_text);

	CheckStatus(m_writeFactory->CreateTextFormat(
		L"Arial",
		NULL,
		DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		500.0f,
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
		static_cast<FLOAT>(m_graphicsManager->getWindowWidth()),
		static_cast<FLOAT>(m_graphicsManager->getWindowHeight())
	);
}

void Text::RenderText()
{
	m_d2dRenderTarget->BeginDraw();
	m_d2dRenderTarget->SetTransform(D2D1::IdentityMatrix());
	m_d2dRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
	// Call the DrawText method of this class.
	m_d2dRenderTarget->DrawText(
		m_text,				// The string to render.
		m_textLength,			// The string's length.
		m_writeTextFormat,    // The text format.
		m_layoutRect,			// The region of the window where the text will be rendered.
		m_orangeBrush			// The brush used to draw the text.
	);
	m_d2dRenderTarget->EndDraw();
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