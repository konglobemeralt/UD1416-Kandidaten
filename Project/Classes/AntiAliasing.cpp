#include "AntiAliasing.h"
#include "GraphicsManager.h"
#include "..\ApplicationContext.h"
#include "WindowManager.h"

AntiAliasing::AntiAliasing()
{

}
AntiAliasing::~AntiAliasing()
{

}

void AntiAliasing::Render(string shaderresource, string rendertarget) {

	manager->ResetViews();

	if (rendertarget.empty()) 
		deviceContext->OMSetRenderTargets(1, manager->getBackbuffer(), nullptr);
	else 
		deviceContext->OMSetRenderTargets(1, &resources.renderTargetViews[rendertarget], nullptr);

	float clearColor[4] = { 0.0f, 0.1f, 0.0f, 1.0f };
	UINT vertexSize = sizeof(float) * 5;
	UINT offset = 0;

	//ResetFreeLookCamera();
	//gdeviceContext->OMSetRenderTargets(1, &m_graphicsManager->thesisData.renderTargetViews["HighResRTV"], nullptr); //sätt in rendertarget här om man nu vill skriva till texture!
	//gdeviceContext->ClearRenderTargetView(m_graphicsManager->thesisData.renderTargetViews["HighResRTV"], clearColor);
	//gdeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	//gdeviceContext->IASetInputLayout(m_graphicsManager->thesisData.inputLayouts["AASimpleLayout"]);
	//gdeviceContext->PSSetConstantBuffers(0, 1, &m_graphicsManager->thesisData.constantBuffers["FXAA_PS_cb"]);
	//gdeviceContext->PSSetShaderResources(0, 1, &m_graphicsManager->thesisData.shaderResourceViews["SSAA_Test"]);
	//gdeviceContext->VSSetShader(m_graphicsManager->thesisData.vertexShaders["SimpleVertexShader"], nullptr, 0);
	//gdeviceContext->PSSetShader(m_graphicsManager->thesisData.pixelShaders["SSAA_PS"], nullptr, 0);
	//gdeviceContext->IASetVertexBuffers(0, 1, m_graphicsManager->getQuad(), &vertexSize, &offset);
	//gdeviceContext->Draw(4, 0);
	//går ej!!!!!!!
	
	detectInput();
	updateFreeLookCamera();

	float clearColor2[4] = { 0.2f, 0.0f, 0.0f, 1.0f };

	//gdeviceContext->OMSetRenderTargets(1, m_graphicsManager->getBackbuffer(), nullptr); //sätt in rendertarget här om man nu vill skriva till texture!
	//deviceContext->OMSetRenderTargets(1, &resources.renderTargetViews["pipeline_SRV_RTV"], nullptr);
	//gdeviceContext->ClearRenderTargetView(*m_graphicsManager->getBackbuffer(), clearColor2);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	deviceContext->IASetInputLayout(m_graphicsManager->thesisData.inputLayouts["AASimpleLayout"]);

	deviceContext->PSSetConstantBuffers(0, 1, &m_graphicsManager->thesisData.constantBuffers["FXAA_PS_cb"]);

	//deviceContext->PSSetShaderResources(0, 1, &resources.shaderResourceViews["pipeline_SRV_RTV"]);
	//gdeviceContext->PSSetShaderResources(0, 1, &m_graphicsManager->thesisData.shaderResourceViews["ImageHigh"]);
	//gdeviceContext->PSSetShaderResources(1, 1, &m_graphicsManager->thesisData.shaderResourceViews["ImageLow"]);

	ResetFreeLookCamera();
	deviceContext->VSSetShader(m_graphicsManager->thesisData.vertexShaders["SimpleVertexShader"], nullptr, 0);

	deviceContext->IASetVertexBuffers(0, 1, m_graphicsManager->getQuad(), &vertexSize, &offset);

	//deviceContext->OMSetRenderTargets(1, m_graphicsManager->getBackbuffer(), nullptr);

	if (shaderresource.empty())
		return;
	else
		deviceContext->PSSetShaderResources(0, 1, &resources.shaderResourceViews[shaderresource]);
//	deviceContext->PSSetShaderResources(0, 1, m_graphicsManager->getBackbufferSRV());

	renderType = (Rendering)0;
	string type = "";

	if (renderType == 0)
	{
		//gdeviceContext->PSSetShaderResources(0, 1, &m_graphicsManager->thesisData.shaderResourceViews["ImageLow"]);
		deviceContext->PSSetShader(m_graphicsManager->thesisData.pixelShaders["FXAA_PS"], nullptr, 0);
		type = "FXAA";
	}
	else if (renderType == 1)
	{
		deviceContext->PSSetShader(m_graphicsManager->thesisData.pixelShaders["SSAA_PS"], nullptr, 0);
		type = "SSAA";
	}
	else if(renderType == 2)//both
	{
		deviceContext->PSSetShader(m_graphicsManager->thesisData.pixelShaders["FXAA_PS"], nullptr, 0);
		type = "Both";
	}
	else 
	{
		//gdeviceContext->PSSetShaderResources(0, 1, &m_graphicsManager->thesisData.shaderResourceViews["ImageLow"]);
		deviceContext->PSSetShader(m_graphicsManager->thesisData.pixelShaders["SSAA_PS"], nullptr, 0);
		type = "Aliased";
	}

	deviceContext->Draw(4, 0);

	//if (hasStored)
	//{
	//	m_graphicsManager->saveImage("AntiAliasing/AAImages/" + type + AATex1, m_graphicsManager->pBackBuffer);
	//	if (renderType >= 3)
	//	{
	//		hasStored = false;
	//	}
	//}
	//renderType = static_cast<Rendering>(static_cast<int>(renderType) + 1);
}

void AntiAliasing::Initialize() {

	m_graphicsManager = ApplicationContext::GetInstance().GetGraphicsManager();
	
	InitConstantBuffers();
	wndHandle = ApplicationContext::GetInstance().GetWindowManager()->getWindowHandle();
	InitDirectInput(*ApplicationContext::GetInstance().GetWindowManager()->getHinstance());

	// ###########################################################
	// ######				Vertex Shader					######
	// ###########################################################

	D3D11_INPUT_ELEMENT_DESC AASimpleLayout[] = {
		{ "SV_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	m_graphicsManager->createVertexShader("SimpleVertexShader", "AASimpleLayout", AASimpleLayout, ARRAYSIZE(AASimpleLayout)); //vertexshader och och layout
	m_graphicsManager->createPixelShader("FXAA_PS"); // Name has to match shader name without .hlsl
	m_graphicsManager->createPixelShader("SSAA_PS");

	InitTextures();

	m_graphicsManager->createTexture2D("FirstSRVRTV");

	m_graphicsManager->createSamplerState("AAWrapSampler", D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP); //dessa ska kanske vara point eller nått när jag kör FXAA?
	m_graphicsManager->createSamplerState("AAClampSampler", D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP);
	deviceContext->PSSetSamplers(0, 1, &m_graphicsManager->thesisData.samplerStates["AAWrapSampler"]);
	deviceContext->PSSetSamplers(1, 1, &m_graphicsManager->thesisData.samplerStates["AAClampSampler"]);
}

void AntiAliasing::InitConstantBuffers()
{
	m_graphicsManager->createConstantBuffer("FXAA_PS_cb", &FXAA_PS_cb, sizeof(FXAA_PS_ConstantBuffer));
	m_graphicsManager->createConstantBuffer("Matrix_VS_cb", &Matrix_VS_cb, sizeof(Matrix_VS_ConstantBuffer));


	FXAA_PS_cb.texelSizeXY.x = 1.0f / m_graphicsManager->getWindowWidth();
	FXAA_PS_cb.texelSizeXY.y = 1.0f / m_graphicsManager->getWindowHeight();
	FXAA_PS_cb.FXAA_blur_Texels_Threshhold = 2.0f;
	//FXAA_PS_cb.minimumBlurThreshhold = 0.0001f;
	FXAA_PS_cb.edgeDetectionTexelMULTIPLIER = 2.0f;
	FXAA_PS_cb.FXAA_reduce_MULTIPLIER = 1.0f / 2.0f;
	FXAA_PS_cb.FXAA_reduce_MIN = 1.0f / 64.0f;


	deviceContext->UpdateSubresource(m_graphicsManager->thesisData.constantBuffers["FXAA_PS_cb"], 0, NULL, &FXAA_PS_cb, 0, 0);
}

void AntiAliasing::InitTextures()
{
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
	//m_graphicsManager->createTexture2D( //shaderresource
	//	"FXAA_Test",
	//	DXGI_FORMAT_R32G32B32A32_FLOAT,
	//	m_graphicsManager->getWindowWidth(), //spelar ingen roll
	//	m_graphicsManager->getWindowHeight(),
	//	false,
	//	true
	//);
	//m_graphicsManager->createTexture2D( //shaderresource
	//	"SSAA_Test",
	//	DXGI_FORMAT_R32G32B32A32_FLOAT,
	//	m_graphicsManager->getWindowWidth(),
	//	m_graphicsManager->getWindowHeight(),
	//	false,
	//	true
	//);

	m_graphicsManager->createTexture2D( //shaderresource
		"HighResRTV",
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		m_graphicsManager->getWindowWidth(),
		m_graphicsManager->getWindowHeight(),
		true,
		true
	);

	//m_graphicsManager->attachImage("AntiAliasing/Images/AATest.png", "FXAA_Test"); //attachea till shaderresourcen
	//m_graphicsManager->attachImage("AntiAliasing/Images/SceneWithLightning.030.png", "SSAA_Test");
	AATex1 = "BushLow.png"; //low
	AATex2 = "BushMed.png"; //high
	m_graphicsManager->attachImage("AntiAliasing/Images/" + AATex1, "ImageLow");
	m_graphicsManager->attachImage("AntiAliasing/Images/" + AATex2, "ImageHigh");
}

void AntiAliasing::ResetFreeLookCamera()
{

	XMStoreFloat4x4(&Matrix_VS_cb.world, XMMatrixIdentity());
	XMStoreFloat4x4(&Matrix_VS_cb.projection, XMMatrixIdentity());
	XMStoreFloat4x4(&Matrix_VS_cb.view, XMMatrixIdentity());
	XMStoreFloat4x4(&Matrix_VS_cb.WVP, XMMatrixIdentity());

	deviceContext->UpdateSubresource(m_graphicsManager->thesisData.constantBuffers["Matrix_VS_cb"], 0, nullptr, &Matrix_VS_cb, 0, 0);
	deviceContext->VSSetConstantBuffers(1, 1, &m_graphicsManager->thesisData.constantBuffers["Matrix_VS_cb"]);
}

void AntiAliasing::updateFreeLookCamera()
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

	camRotation = XMMatrixRotationRollPitchYaw(camPitch, camYaw, 0);	//Function parameters: (Pitch in radians to rotate, Yaw -||-, Roll -||-);
	XMCamLook = XMVector3TransformCoord(worldForward, camRotation);		//SETS CAMERA TARGET VECTOR
	XMCamLook = XMVector3Normalize(XMCamLook);								//NORMALIZES CAMERA TARGET VECTOR

	camRight = XMVector3TransformCoord(worldRight, camRotation);		//Update Cam Right Vector relative to rotation
	camForward = XMVector3TransformCoord(worldForward, camRotation);	//Update Cam Forward Vector relative to rotation
	XMCamUp = XMVector3Cross(camForward, camRight);						//Set Cam Up Vector with cross product

	XMCamPos += moveLR * camRight;										//Set the left and right position of the camera
	XMCamPos += moveFB * camForward;										//Set the forward and back position of the camera		//Move Along Cams Forward vector

	XMCamLook += XMCamPos;													//Add the camera position to the target (look) vector
	XMViewSpace = XMMatrixLookAtLH(XMCamPos, XMCamLook, XMCamUp);				//SET THE NEW CAMERA POSITION PROPERTIES(Updates the view matrix)

																				//Convert Back to floats!
	XMStoreFloat4(&camLook, XMCamLook);
	XMStoreFloat4(&camUp, XMCamUp);
	XMStoreFloat4(&camPos, XMCamPos);
	XMStoreFloat4x4(&ViewSpace, XMViewSpace);


	XMMATRIX WVP, World, Projection;

	//XMMATRIX XMViewSpace = XMLoadFloat4x4(&ViewSpace);
	//XMVECTOR XMCamPos, XMCamLook, XMCamUp;
	XMCamPos = XMLoadFloat4(&camPos);
	XMCamLook = XMLoadFloat4(&camLook);
	XMCamUp = XMLoadFloat4(&camUp);

	XMViewSpace = XMMatrixLookAtLH(XMCamPos, XMCamLook, XMCamUp);
	Projection = XMMatrixPerspectiveFovLH(3.14f*0.45f, (float)(m_graphicsManager->getWindowWidth() / m_graphicsManager->getWindowHeight()), 0.01f, 20.0f); //	FLOAT FovAngleY, FLOAT AspectRatio, FLOAT NearZ, FLOAT FarZ
																																				//Projection = XMMatrixOrthographicLH(20, 20, 0.5f, 20.0f);
	World = XMMatrixIdentity();
	WVP = World * XMViewSpace * Projection;

	//constantBuffer.WVP = XMMatrixTranspose(WVP);	//OLD Version
	XMStoreFloat4x4(&Matrix_VS_cb.world, XMMatrixTranspose(World));
	XMStoreFloat4x4(&Matrix_VS_cb.projection, XMMatrixTranspose(Projection));
	XMStoreFloat4x4(&Matrix_VS_cb.view, XMMatrixTranspose(XMViewSpace));
	XMStoreFloat4x4(&Matrix_VS_cb.WVP, XMMatrixTranspose(WVP));
	//Matrix_VS_cb.view = XMViewSpace;
	//gdeviceContext->VSSetConstantBuffers(0, 1, &m_graphicsManager->thesisData.constantBuffers["lightningCBuffer"]);
	deviceContext->UpdateSubresource(m_graphicsManager->thesisData.constantBuffers["Matrix_VS_cb"], 0, nullptr, &Matrix_VS_cb, 0, 0);
	deviceContext->VSSetConstantBuffers(1, 1, &m_graphicsManager->thesisData.constantBuffers["Matrix_VS_cb"]);
}
void AntiAliasing::detectInput()
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
		camYaw += mouseCurrentState.lX * 0.001f;					//Rotates camera to the right and left

		camPitch += mouseCurrentState.lY * 0.001f;				//Rotates camera up and down

		mouseLastState = mouseCurrentState;
	}
}

bool AntiAliasing::InitDirectInput(HINSTANCE hInstance)
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


	hr = DirectInput->CreateDevice(GUID_SysKeyboard,
		&DIKeyboard,
		NULL);

	hr = DirectInput->CreateDevice(GUID_SysMouse,
		&DIMouse,
		NULL);


	hr = DIKeyboard->SetDataFormat(&c_dfDIKeyboard);
	hr = DIKeyboard->SetCooperativeLevel(*wndHandle, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	hr = DIMouse->SetDataFormat(&c_dfDIMouse);
	hr = DIMouse->SetCooperativeLevel(*wndHandle, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);

	return true;
}

void AntiAliasing::InitExample()
{
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
	m_graphicsManager->attachImage("ToneMapping/Images/picture.jpg", "mySRV");



	// ###########################################################
	// ######		Render target & shader resource			######
	// ###########################################################
	//	void createSamplerState(
	//		string name,
	//		D3D11_FILTER filter = D3D11_FILTER_MIN_MAG_MIP_POINT,
	//		D3D11_TEXTURE_ADDRESS_MODE mode = D3D11_TEXTURE_ADDRESS_CLAMP
	//	);

	m_graphicsManager->createSamplerState("CoolSampler", D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP);
}