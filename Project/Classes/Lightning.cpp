#include "Lightning.h"
#include "GraphicsManager.h"
#include "..\ApplicationContext.h"
#include "WindowManager.h"
Lightning::Lightning()
{

}
Lightning::~Lightning()
{

}

void Lightning::createVertexBuffer()
{
	struct LightningVertex
	{
		float x, y, z;
		float u, v;
	}
	lightningVertices[4] =
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

	D3D11_BUFFER_DESC bufferDesc2;
	memset(&bufferDesc2, 0, sizeof(bufferDesc2));
	bufferDesc2.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc2.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc2.ByteWidth = sizeof(lightningVertices);

	D3D11_SUBRESOURCE_DATA data2;
	data2.pSysMem = lightningVertices;
	gdevice->CreateBuffer(&bufferDesc2, &data2, &lightningBuffer);


}

void Lightning::Render() {
	float clearColor[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
	UINT vertexSize = sizeof(float) * 5;
	UINT offset = 0;

	//float lineWidth;
	//lineWidth = 0.1f;

	detectInput();
	updateFreeLookCamera();

	XMMATRIX WVP, World, Projection;

	XMMATRIX XMViewSpace = XMLoadFloat4x4(&ViewSpace);
	XMVECTOR XMCamPos, XMCamLook, XMCamUp;
	XMCamPos = XMLoadFloat4(&camPos);
	XMCamLook = XMLoadFloat4(&camLook);
	XMCamUp = XMLoadFloat4(&camUp);

	//XMVECTOR camPos2 = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);	// Camera StartPos		//-2.0 Moves camera back 2 units along Z
	//XMVECTOR camLook2 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	//XMVECTOR camUp2 = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	//XMViewSpace = XMMatrixLookAtLH(camPos2, camLook2, camUp2);
	XMViewSpace = XMMatrixLookAtLH(XMCamPos, XMCamLook, XMCamUp);
	Projection = XMMatrixPerspectiveFovLH(3.14*0.45, m_graphicsManager->getWindowWidth() / m_graphicsManager->getWindowHeight(), 0.01f, 20.0f); //	FLOAT FovAngleY, FLOAT AspectRatio, FLOAT NearZ, FLOAT FarZ
	//Projection = XMMatrixOrthographicLH(20, 20, 0.5f, 20.0f);
	World = XMMatrixIdentity();
	WVP = World * XMViewSpace * Projection;

	//constantBuffer.WVP = XMMatrixTranspose(WVP);	//OLD Version
	XMStoreFloat4x4(&lightningCBuffer.World, XMMatrixTranspose(World));
	XMStoreFloat4x4(&lightningCBuffer.WVP, XMMatrixTranspose(WVP));
	lightningCBuffer.camPos = camPos;
	lightningCBuffer.lineWidth = 0.1f;

	gdeviceContext->UpdateSubresource(m_graphicsManager->thesisData.constantBuffers["lightningCBuffer"], 0, nullptr, &lightningCBuffer, 0, 0);
	
	gdeviceContext->OMSetRenderTargets(1, m_graphicsManager->getBackbuffer(), nullptr);
	gdeviceContext->ClearRenderTargetView(*m_graphicsManager->getBackbuffer(), clearColor);

	gdeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);		//LINESTRIP
	//gdeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);		//TRIANGLESTRIP
	gdeviceContext->IASetInputLayout(m_graphicsManager->thesisData.inputLayouts["FirstLayout"]);
	gdeviceContext->PSSetSamplers(0, 1, &m_graphicsManager->thesisData.samplerStates["CoolSampler"]);

	gdeviceContext->VSSetShader(m_graphicsManager->thesisData.vertexShaders["LightningVertexShader"], nullptr, 0);
	gdeviceContext->HSSetShader(m_graphicsManager->thesisData.hullShaders["LightningHullShader"], nullptr, 0);
	gdeviceContext->DSSetShader(m_graphicsManager->thesisData.domainShaders["LightningDomainShader"], nullptr, 0);
	gdeviceContext->GSSetShader(m_graphicsManager->thesisData.geometryShaders["LightningGeometryShader"], nullptr, 0);
	gdeviceContext->PSSetShader(m_graphicsManager->thesisData.pixelShaders["LightningPixelShader"], nullptr, 0);

	gdeviceContext->IASetVertexBuffers(0, 1, &lightningBuffer, &vertexSize, &offset);

	gdeviceContext->Draw(4, 0);
}

void Lightning::Initialize() {
	m_graphicsManager = ApplicationContext::GetInstance().GetGraphicsManager();

	//WIREFRAME //WIREFRAME //WIREFRAME
	m_graphicsManager->setRasterstate(D3D11_CULL_NONE, D3D11_FILL_WIREFRAME);

	//SOLID //SOLID  //SOLID //SOLID 
	//m_graphicsManager->setRasterstate(D3D11_CULL_NONE, D3D11_FILL_SOLID);

	createVertexBuffer();
	wndHandle = ApplicationContext::GetInstance().GetWindowManager()->getWindowHandle();
	InitDirectInput(*ApplicationContext::GetInstance().GetWindowManager()->getHinstance());			//Initiates the keyboard/mouse input function thingies

	

	// ###########################################################
	// ######				Constant buffer					######
	// ###########################################################
	//	void createConstantBuffer(
	//		string name,
	//		D3D11_BUFFER_DESC desc,
	//		const void* data
	//	);

	///////////___________NEW_________________/////////////

	m_graphicsManager->createConstantBuffer("lightningCBuffer", &lightningCBuffer, sizeof(cBuffer));


	gdeviceContext->VSSetConstantBuffers(0, 1, &m_graphicsManager->thesisData.constantBuffers["lightningCBuffer"]);
	//gdeviceContext->GSGetConstantBuffers(0, 1, &m_graphicsManager->thesisData.constantBuffers["lightningCBuffer"]);

	///////////___________NEW_________________/////////////

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

	m_graphicsManager->createVertexShader("LightningVertexShader", "FirstLayout", layoutDesc, ARRAYSIZE(layoutDesc));



	// ###########################################################
	// ######				Other Shaders					######
	// ###########################################################
	//	void createPixelShader(
	//		string name
	//			);

	m_graphicsManager->createHullShader("LightningHullShader");
	m_graphicsManager->createDomainShader("LightningDomainShader");
	m_graphicsManager->createGeometryShader("LightningGeometryShader");
	m_graphicsManager->createPixelShader("LightningPixelShader"); // Name has to match shader name without .hlsl



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

void Lightning::updateFreeLookCamera() 
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
}

void Lightning::detectInput() 
{
	DIMOUSESTATE mouseCurrentState;		//Mouse input
	BYTE keyboardState[256];			//Keyboard input

	DIKeyboard->Acquire();
	DIMouse->Acquire();

	DIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrentState);
	DIKeyboard->GetDeviceState(sizeof(keyboardState), (LPVOID)&keyboardState);

	moveLR = 0.0f;
	moveFB = 0.0f;

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

bool Lightning::InitDirectInput(HINSTANCE hInstance)
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