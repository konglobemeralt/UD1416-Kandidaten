#include "GraphicsManager.h"

GraphicsManager::GraphicsManager(HWND* hwnd, UINT winWidth, UINT winHeight) {
	windowHandle = hwnd;
	windowWidth = winWidth;
	windowHeight = winHeight;

	vertexSize = sizeof(float) * 8;
	quadSize = sizeof(float) * 5;
	boxSize = sizeof(XMFLOAT3);
	offset = 0;

	CreateDirect3DContext();
	CreateShaders();
	CreateGbuffers();
	CreateSamplers();
	setRasterstate(D3D11_CULL_NONE);

	struct TriangleVertex
	{
		float x, y, z;
		float u, v;
	}
	triangleVertices[4] =
	{
		-1.0f, 1.0f, 0.0f,	//v0 pos
		0.0f, 0.0f,	//v0 tex

		1.0f, 1.0f, 0.0f,	//v1
		1.0f, 0.0f,	//v1 tex

		-1.0f, -1.0f, 0.0f, //v2
		0.0f, 1.0f,	//v2 tex

		1.0f, -1.0f, 0.0f,   //v3
		1.0f, 1.0f	//v3 color
	};

	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(triangleVertices);

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = triangleVertices;
	gDevice->CreateBuffer(&bufferDesc, &data, &gQuadBuffer);

	gContentManager->LoadContent();
}

GraphicsManager::~GraphicsManager() {
	delete gContentManager;
}

IDXGISwapChain* GraphicsManager::getSwapChain() {
	return gSwapChain;
}

ID3D11Device* GraphicsManager::getDevice() {
	return gDevice;
}

ID3D11DeviceContext* GraphicsManager::getDeviceContext() {
	return gDeviceContext;
}

ID3D11RenderTargetView* GraphicsManager::getBackbufferRTV() {
	return gBackbufferRTV;
}

void GraphicsManager::setContentManager(ContentManager* manager) {
	gContentManager = manager;
}

void GraphicsManager::Render() {
	ResetViews();

	gDeviceContext->VSSetShader(nullptr, nullptr, 0);
	gDeviceContext->HSSetShader(nullptr, nullptr, 0);
	gDeviceContext->DSSetShader(nullptr, nullptr, 0);
	gDeviceContext->GSSetShader(nullptr, nullptr, 0);
	gDeviceContext->PSSetShader(nullptr, nullptr, 0);
	gDeviceContext->CSSetShader(nullptr, nullptr, 0);
	
	gDeviceContext->PSSetShaderResources(0, 0, NULL);
	
	gDeviceContext->PSSetSamplers(0, 0, nullptr);
	//gDeviceContext->PSSetSamplers(1, 1, &samplerStateClamp);
	//gDeviceContext->PSSetSamplers(2, 1, &samplerStateTerrain);
	
	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gDeviceContext->IASetInputLayout(gDeferredVertexLayout);
	//
	//
	////CullQuadTree();
	//
	////First Pass ############################################################################
	////SetViewport2(); // Re-size vp to fit light depth (2048x2048)
	//
	//gDeviceContext->OMSetRenderTargets(0, nullptr, g_lightDSV);
	//gDeviceContext->ClearDepthStencilView(g_lightDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
	//
	//gDeviceContext->VSSetShader(gShadowMapVertexShader, nullptr, 0);
	//gDeviceContext->PSSetShader(gShadowMapPixelShader, nullptr, 0);
	//	
	////for (unsigned int i = 0; i < gVertexBuffer.size(); i++){
	////	if (objectList[i]->box.isRendered == true){
	////		UpdateBuffers(objectMatrix[i], objectList[i]->specularColor_Power);
	////
	////		gDeviceContext->IASetVertexBuffers(0, 1, &gVertexBuffer[i], &vertexSize, &offset);
	////
	////		gDeviceContext->Draw(vertexDrawCount[i], 0);
	////	}
	////}
	////
	////SetViewport(); // Change vp size back to normal
	//
	//
	////Second Pass - Objects ############################################################################
	gDeviceContext->OMSetRenderTargets(1, &g_ColorRTV, g_cameraDSV);
	gDeviceContext->ClearDepthStencilView(g_cameraDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
	gDeviceContext->ClearRenderTargetView(g_RTV[0], clearColor);
	//gDeviceContext->ClearRenderTargetView(g_RTV[1], clearColor);
	//gDeviceContext->ClearRenderTargetView(g_RTV[2], clearColor);
	//gDeviceContext->ClearRenderTargetView(g_RTV[3], clearColor);
	
	gDeviceContext->VSSetShader(gVertexShader, nullptr, 0);
	gDeviceContext->GSSetShader(gGeometryShader, nullptr, 0);
	gDeviceContext->PSSetShader(gPixelShader, nullptr, 0);

	////vector<ID3D11ShaderResourceView*> tempSRV2;
	////tempSRV2.push_back(g_SRV[2]);
	////
	for (unsigned int i = 0; i < gContentManager->getStaticObjects()->size(); i++){
		//if (objectList[i]->box.isRendered == true){
		//	UpdateBuffers(objectMatrix[i], objectList[i]->specularColor_Power);
		//	tempSRV2.push_back(objImporterRSV[i]);
	
			
			//gDeviceContext->PSSetShaderResources(0, 2, tempSRV2.data());
			gDeviceContext->IASetVertexBuffers(0, 1, &gContentManager->getStaticObjects()->at(i).buffer.vertexBuffer, &vertexSize, &offset);
			gDeviceContext->DrawIndexed(gContentManager->getStaticObjects()->at(i).indexCount, 0, 0);
	
		//	tempSRV2.pop_back();
		//}
	}
	//
	////Second Pass - Height Map
	//gDeviceContext->IASetInputLayout(gHMLayout);
	////UpdateBuffers2(terrainWorldMatrix);
	////terrain->DrawHM(gDeviceContext, gHMVertexShader, gHMPixelShader, g_SRV[2]);
	//gDeviceContext->IASetInputLayout(gVertexLayout);
	//
	////Second pass - Object Bounding Box
	//gDeviceContext->OMSetRenderTargets(1, g_RTV, g_cameraDSV);
	//gDeviceContext->VSSetShader(gBoxVertexShader, nullptr, 0);
	//gDeviceContext->GSSetShader(nullptr, nullptr, 0);
	//gDeviceContext->PSSetShader(gBoxPixelShader, nullptr, 0);
	//gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	////gDeviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	////for (unsigned int i = 0; i < boxBuffer.size(); i++){
	////	if (objectList[i]->box.isRendered == true){
	////		UpdateBoxBuffers(i);
	////		UpdateBuffers(objectMatrix[i], objectList[i]->specularColor_Power);
	////
	////		gDeviceContext->IASetVertexBuffers(0, 1, &boxBuffer[i], &boxSize, &offset);
	////		gDeviceContext->DrawIndexed(16, 0, 0);
	////	}
	////}
	////
	//////Second pass - QuadTree
	////for (unsigned int i = 0; i < quadBuffers.size(); i++){
	////	gDeviceContext->IASetVertexBuffers(0, 1, &quadBuffers[i], &boxSize, &offset);
	////	gDeviceContext->DrawIndexed(16, 0, 0);
	////}
	//
	//gDeviceContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);
	//
	
	//Third Pass ############################################################################
	gDeviceContext->PSSetSamplers(0, 1, &samplerStateWrap);
	gDeviceContext->OMSetRenderTargets(1, &gBackbufferRTV, nullptr);
	gDeviceContext->ClearRenderTargetView(gBackbufferRTV, clearColor);
	
	gDeviceContext->VSSetShader(gDeferredVertexShader, nullptr, 0);
	//gDeviceContext->GSSetShader(nullptr, nullptr, 0);
	gDeviceContext->PSSetShader(gDeferredPixelShader, nullptr, 0);
			
	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	gDeviceContext->IASetInputLayout(gDeferredVertexLayout);
	
	gDeviceContext->IASetVertexBuffers(0, 1, &gQuadBuffer, &quadSize, &offset);
	gDeviceContext->PSSetShaderResources(0, 1, &g_ColorSRV);

	//TEMPTEST:
	//gDeviceContext->PSSetShaderResources(0, 0, &emptySRV[0]);
			
	gDeviceContext->Draw(4, 0);
	
	//Third Pass - Particles
	//gDeviceContext->OMSetRenderTargets(1, &gBackbufferRTV, g_cameraDSV);
	//UpdateBuffers2(XMMatrixTranslation(particleBox->boxPosition.x, particleBox->boxPosition.y, particleBox->boxPosition.z));
	//particleBox->DrawParticles();
}

void GraphicsManager::CreateShaders(){
	if (gVertexLayout)			gVertexLayout->Release();
	if (gDeferredVertexLayout)	gDeferredVertexLayout->Release();
	if (gShadowMapLayout)		gShadowMapLayout->Release();
	if (gBoxLayout)				gBoxLayout->Release();
	if (gHMLayout)				gHMLayout->Release();
	if (gVertexShader)			gVertexShader->Release();
	if (gDeferredVertexShader)	gDeferredVertexShader->Release();
	if (gShadowMapVertexShader)	gShadowMapVertexShader->Release();
	if (gBoxVertexShader)		gBoxVertexShader->Release();
	if (gHMVertexShader)		gHMVertexShader->Release();
	if (gDeferredPixelShader)	gDeferredPixelShader->Release();
	if (gShadowMapPixelShader)	gShadowMapPixelShader->Release();
	if (gBoxPixelShader)		gBoxPixelShader->Release();
	if (gHMPixelShader)			gHMPixelShader->Release();
	if (gGeometryShader)		gGeometryShader->Release();

	ID3DBlob* pVS = nullptr;
	D3DCompileFromFile(L"../Shaders/VertexShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS_main", "vs_5_0", 0, NULL, &pVS, nullptr);
	gDevice->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &gVertexShader);
	
	//Vertex-shader layout
	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	gDevice->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), pVS->GetBufferPointer(), pVS->GetBufferSize(), &gVertexLayout);
	pVS->Release();
	
	//Geometry-shader
	ID3DBlob* pGS = nullptr;
	D3DCompileFromFile(L"../Shaders/GeometryShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "GS_main", "gs_5_0", 0, NULL, &pGS, nullptr);
	gDevice->CreateGeometryShader(pGS->GetBufferPointer(), pGS->GetBufferSize(), nullptr, &gGeometryShader);
	pGS->Release();
	
	//Pixel-shader
	ID3DBlob* pPS = nullptr;
	D3DCompileFromFile(L"../Shaders/PixelShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS_main", "ps_5_0", 0, NULL, &pPS, nullptr);
	
	gDevice->CreatePixelShader(pPS->GetBufferPointer(), pPS->GetBufferSize(), nullptr, &gPixelShader);
	pPS->Release();
	
	//#############################
	//Deferred vertex-shader
	ID3DBlob* pDVS = nullptr;
	D3DCompileFromFile(L"../Shaders/DeferredVertexShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "DVS_main", "vs_5_0", 0, NULL, &pDVS, nullptr);
	gDevice->CreateVertexShader(pDVS->GetBufferPointer(), pDVS->GetBufferSize(), nullptr, &gDeferredVertexShader);
	
	//Deferred vertex-shader layout
	D3D11_INPUT_ELEMENT_DESC inputDeferredDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	gDevice->CreateInputLayout(inputDeferredDesc, ARRAYSIZE(inputDeferredDesc), pDVS->GetBufferPointer(), pDVS->GetBufferSize(), &gDeferredVertexLayout);
	pDVS->Release();
	
	//Deferred pixel-shader
	if (gDeferredPixelShader)
		gDeferredPixelShader->Release();

	ID3DBlob* pDPS = nullptr;
	D3DCompileFromFile(L"../Shaders/DeferredPixelShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "DPS_main", "ps_5_0", 0, NULL, &pDPS, nullptr);
	
	gDevice->CreatePixelShader(pDPS->GetBufferPointer(), pDPS->GetBufferSize(), nullptr, &gDeferredPixelShader);
	pDPS->Release();
	
	//#############################
	//Shadow map vertex-shader
	ID3DBlob* pSMVS = nullptr;
	D3DCompileFromFile(L"../Shaders/ShadowMapVertexShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "SMVS_main", "vs_5_0", 0, NULL, &pSMVS, nullptr);
	gDevice->CreateVertexShader(pSMVS->GetBufferPointer(), pSMVS->GetBufferSize(), nullptr, &gShadowMapVertexShader);
	
	//Shadow map vertex-shader layout
	D3D11_INPUT_ELEMENT_DESC inputShadowMapDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	gDevice->CreateInputLayout(inputShadowMapDesc, ARRAYSIZE(inputShadowMapDesc), pSMVS->GetBufferPointer(), pSMVS->GetBufferSize(), &gShadowMapLayout);
	pSMVS->Release();
	
	//Shadow map pixel-shader (void) Needed?
	ID3DBlob* pSMPS = nullptr;
	D3DCompileFromFile(L"../Shaders/ShadowMapPixelShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "SMPS_main", "ps_5_0", 0, NULL, &pSMPS, nullptr);
	
	gDevice->CreatePixelShader(pSMPS->GetBufferPointer(), pSMPS->GetBufferSize(), nullptr, &gShadowMapPixelShader);
	pSMPS->Release();
	
	//#############################
	//Box vertex-shader
	ID3DBlob* pBVS = nullptr;
	D3DCompileFromFile(L"../Shaders/BoxVertexShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "BVS_main", "vs_5_0", 0, NULL, &pBVS, nullptr);
	gDevice->CreateVertexShader(pBVS->GetBufferPointer(), pBVS->GetBufferSize(), nullptr, &gBoxVertexShader);
	
	D3D11_INPUT_ELEMENT_DESC inputBoxDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	gDevice->CreateInputLayout(inputDeferredDesc, ARRAYSIZE(inputBoxDesc), pBVS->GetBufferPointer(), pBVS->GetBufferSize(), &gBoxLayout);
	pBVS->Release();
	
	//Box pixel-shader
	ID3DBlob* pBPS = nullptr;
	D3DCompileFromFile(L"../Shaders/BoxPixelShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "BPS_main", "ps_5_0", 0, NULL, &pBPS, nullptr);
	
	gDevice->CreatePixelShader(pBPS->GetBufferPointer(), pBPS->GetBufferSize(), nullptr, &gBoxPixelShader);
	pBPS->Release();
	
	//#############################
	//Height-map vertex-shader
	ID3DBlob* HMVS = nullptr;
	D3DCompileFromFile(L"../Shaders/HMVertexShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "HMVS_main", "vs_5_0", 0, NULL, &HMVS, nullptr);
	gDevice->CreateVertexShader(HMVS->GetBufferPointer(), HMVS->GetBufferSize(), nullptr, &gHMVertexShader);
	
	//Height-map vertex-shader layout
	D3D11_INPUT_ELEMENT_DESC inputHMDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	gDevice->CreateInputLayout(inputHMDesc, ARRAYSIZE(inputHMDesc), HMVS->GetBufferPointer(), HMVS->GetBufferSize(), &gHMLayout);
	HMVS->Release();
	
	//Height-map pixel-shader
	ID3DBlob* HMPS = nullptr;
	D3DCompileFromFile(L"../Shaders/HMPixelShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "HMPS_main", "ps_5_0", 0, NULL, &HMPS, nullptr);
	
	gDevice->CreatePixelShader(HMPS->GetBufferPointer(), HMPS->GetBufferSize(), nullptr, &gHMPixelShader);
	HMPS->Release();
}

void GraphicsManager::CreateGbuffers() {
	//G-Buffers
	D3D11_TEXTURE2D_DESC colorDesc;
	ZeroMemory(&colorDesc, sizeof(colorDesc));
	colorDesc.Width = windowWidth;
	colorDesc.Height = windowHeight;
	colorDesc.MipLevels = 0;
	colorDesc.ArraySize = 1;
	colorDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	colorDesc.SampleDesc.Count = 1;
	colorDesc.Usage = D3D11_USAGE_DEFAULT;
	colorDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	colorDesc.CPUAccessFlags = 0;
	colorDesc.MiscFlags = 0;

	gDevice->CreateTexture2D(&colorDesc, nullptr, &g_Color);
	gDevice->CreateTexture2D(&colorDesc, nullptr, &g_Normal);
	gDevice->CreateTexture2D(&colorDesc, nullptr, &g_Wpos);
	gDevice->CreateTexture2D(&colorDesc, nullptr, &g_Diffuse);

	D3D11_RENDER_TARGET_VIEW_DESC colorTextureRTVdesc;
	colorTextureRTVdesc.Format = colorDesc.Format;
	colorTextureRTVdesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	colorTextureRTVdesc.Texture2D.MipSlice = 0;

	gDevice->CreateRenderTargetView(g_Color, &colorTextureRTVdesc, &g_RTV[0]);
	gDevice->CreateRenderTargetView(g_Normal, &colorTextureRTVdesc, &g_RTV[1]);
	gDevice->CreateRenderTargetView(g_Wpos, &colorTextureRTVdesc, &g_RTV[2]);
	gDevice->CreateRenderTargetView(g_Diffuse, &colorTextureRTVdesc, &g_RTV[3]);

	D3D11_SHADER_RESOURCE_VIEW_DESC colorTextureSRVdesc;
	colorTextureSRVdesc.Format = colorDesc.Format;
	colorTextureSRVdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	colorTextureSRVdesc.Texture2D.MostDetailedMip = 0;
	colorTextureSRVdesc.Texture2D.MipLevels = 1;

	gDevice->CreateShaderResourceView(g_Color, &colorTextureSRVdesc, &g_SRV[0]);
	gDevice->CreateShaderResourceView(g_Normal, &colorTextureSRVdesc, &g_SRV[1]);
	gDevice->CreateShaderResourceView(g_Wpos, &colorTextureSRVdesc, &g_SRV[3]);
	gDevice->CreateShaderResourceView(g_Diffuse, &colorTextureSRVdesc, &g_SRV[4]);

	// Depth
	D3D11_TEXTURE2D_DESC depthMapDesc;
	
	ZeroMemory(&depthMapDesc, sizeof(depthMapDesc));
	
	depthMapDesc.Width = windowWidth;
	depthMapDesc.Height = windowHeight;
	depthMapDesc.MipLevels = 1;
	depthMapDesc.ArraySize = 1;
	depthMapDesc.Format = DXGI_FORMAT_R32_TYPELESS;;
	depthMapDesc.SampleDesc.Count = 1;
	depthMapDesc.SampleDesc.Quality = 0;
	depthMapDesc.Usage = D3D11_USAGE_DEFAULT;
	depthMapDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	depthMapDesc.CPUAccessFlags = 0;
	depthMapDesc.MiscFlags = 0;
	
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	descDSV.Flags = 0;
	
	D3D11_SHADER_RESOURCE_VIEW_DESC lightDepthSRVdesc;
	lightDepthSRVdesc.Format = DXGI_FORMAT_R32_FLOAT;
	lightDepthSRVdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	lightDepthSRVdesc.Texture2D.MostDetailedMip = 0;
	lightDepthSRVdesc.Texture2D.MipLevels = 1;
	
	gDevice->CreateTexture2D(&depthMapDesc, nullptr, &g_cameraDepth);
	gDevice->CreateDepthStencilView(g_cameraDepth, &descDSV, &g_cameraDSV);
	
	depthMapDesc.Width = 2048;
	depthMapDesc.Height = 2048;
	
	gDevice->CreateTexture2D(&depthMapDesc, nullptr, &g_lightDepth);
	gDevice->CreateDepthStencilView(g_lightDepth, &descDSV, &g_lightDSV);
	gDevice->CreateShaderResourceView(g_lightDepth, &lightDepthSRVdesc, &g_SRV[2]);

	////Constant buffer for wvp-matrices
	//D3D11_BUFFER_DESC MatrixBufferDesc;
	//memset(&MatrixBufferDesc, 0, sizeof(MatrixBufferDesc));
	//MatrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	//MatrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	//MatrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//MatrixBufferDesc.ByteWidth = sizeof(wvpMatrices);
	//gDevice->CreateBuffer(&MatrixBufferDesc, nullptr, &gMatrixBuffer);

	//MatrixBufferDesc.ByteWidth = sizeof(DirectionalLight);
	//gDevice->CreateBuffer(&MatrixBufferDesc, nullptr, &gDirLightBuffer);

	//MatrixBufferDesc.ByteWidth = sizeof(PointLightStruct);
	//gDevice->CreateBuffer(&MatrixBufferDesc, nullptr, &gPointLightBuffer);
}

void GraphicsManager::CreateSamplers() {
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 0;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	gDevice->CreateSamplerState(&samplerDesc, &samplerStateWrap);

	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	gDevice->CreateSamplerState(&samplerDesc, &samplerStateTerrain);

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	gDevice->CreateSamplerState(&samplerDesc, &samplerStateClamp);
}

void GraphicsManager::Release(){
	for (unsigned int i = 0; i < 4; i++){
		g_RTV[i]->Release();
	}

	for (unsigned int i = 0; i < 5; i++){
		g_SRV[i]->Release();
	}

	gBackbufferRTV->Release();
	gSwapChain->Release();
	gDevice->Release();
	gDeviceContext->Release();
	g_Color->Release();
	g_Normal->Release();
	g_cameraDepth->Release();
	g_lightDepth->Release();
	g_Wpos->Release();
	g_Diffuse->Release();
	g_cameraDSV->Release();
	g_lightDSV->Release();
	gQuadBuffer->Release();

	gVertexLayout->Release();
	gDeferredVertexLayout->Release();
	gShadowMapLayout->Release();
	gBoxLayout->Release();
	gHMLayout->Release();
	gVertexShader->Release();
	gDeferredVertexShader->Release();
	gShadowMapVertexShader->Release();
	gBoxVertexShader->Release();
	gHMVertexShader->Release();
	gPixelShader->Release();
	gDeferredPixelShader->Release();
	gShadowMapPixelShader->Release();
	gBoxPixelShader->Release();
	gHMPixelShader->Release();
	gGeometryShader->Release();

	samplerStateWrap->Release();
	samplerStateClamp->Release();
	samplerStateTerrain->Release();

	rasterState->Release();
}

void GraphicsManager::setRasterstate(D3D11_CULL_MODE cullmode) {
	D3D11_RASTERIZER_DESC rastDesc;

	rastDesc.FillMode = D3D11_FILL_SOLID;
	rastDesc.CullMode = cullmode;
	rastDesc.FrontCounterClockwise = FALSE;
	rastDesc.DepthBias = 0;
	rastDesc.SlopeScaledDepthBias = 0.0f;
	rastDesc.DepthBiasClamp = 0.0f;
	rastDesc.DepthClipEnable = TRUE;
	rastDesc.ScissorEnable = FALSE;
	rastDesc.MultisampleEnable = FALSE;
	rastDesc.AntialiasedLineEnable = FALSE;

	gDevice->CreateRasterizerState(&rastDesc, &rasterState);
	gDeviceContext->RSSetState(rasterState);
}

HRESULT GraphicsManager::CreateDirect3DContext() {
	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	scd.BufferCount = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_UNORDERED_ACCESS;
	scd.OutputWindow = *windowHandle;
	scd.SampleDesc.Count = 1;
	scd.Windowed = TRUE;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		D3D11_CREATE_DEVICE_DEBUG,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scd,
		&gSwapChain,
		&gDevice,
		NULL,
		&gDeviceContext
	);

	if (SUCCEEDED(hr)) {
		ID3D11Texture2D* pBackBuffer = nullptr;
		gSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

		gDevice->CreateRenderTargetView(pBackBuffer, NULL, &gBackbufferRTV);
		pBackBuffer->Release();
	};

	return hr;
}

void GraphicsManager::ResetViews(){
	emptyDSV = NULL;
	int x = 0;
	
	for (int i = 0; i < 4; i++){
		emptyRTV[i] = NULL;
		for (int y= 0; y < 2; y++){
			emptySRV[y + x] = NULL;
		}
		x += 2;
	}
	
	gDeviceContext->OMSetRenderTargets(4, emptyRTV, emptyDSV);
	gDeviceContext->PSSetShaderResources(0, 8, emptySRV);
}