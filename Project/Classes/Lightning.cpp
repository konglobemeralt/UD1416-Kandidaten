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
	
	LightningVertex lightningVertices[2] =
	{
		-4.0f, 20.0f, 1.0f,	//v0 pos
		1.0f, 1.0f,

		5.0f, -20.0f, 5.0f,	//v1
		1.0f, 1.0f, 

		//-1.0f, -4.0f, -1.0f,	//v1
		//1.0f, 1.0f, 

		//3.0f, -8.0f, 1.0f,	//v1
		//1.0f, 1.0f

		//ZIG-ZAG Verts
		//-1.0f, 1.0f, 0.0f,	//v0 pos
		//0.0f, 0.0f,	//v0 tex

		//1.0f, 0.0f, 0.0f,	//v1	
		//1.0f, 0.0f,	//v1 tex

		//1.0f, 0.0f, 0.0f,	//v1
		//1.0f, 0.0f,	//v1 tex

		//-1.0f, -1.0f, 0.0f, //v2
		//0.0f, 1.0f,	//v2 tex

		//-1.0f, -1.0f, 0.0f, //v2
		//0.0f, 1.0f,	//v2 tex

		//1.0f, -2.0f, 0.0f,   //v3
		//1.0f, 1.0f	//v3 color

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

void Lightning::createStreamVertexBuffer()
{
	D3D11_BUFFER_DESC SOVBD;
	memset(&SOVBD, 0, sizeof(SOVBD));
	SOVBD.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT;
	SOVBD.Usage = D3D11_USAGE_DEFAULT;
	SOVBD.ByteWidth = sizeof(LightningVertex) * ((baseLightningSegments * 2) * 5);
	SOVBD.CPUAccessFlags = 0;
	SOVBD.MiscFlags = 0;

	//D3D11_SUBRESOURCE_DATA data2;
	//data2.pSysMem = lightningVertices;
	gdevice->CreateBuffer(&SOVBD, 0,/*&data2,*/ &SOlightningBuffer);
}

void Lightning::Render() {
	float clearColor[4] = { 0.4f, 0.4f, 0.4f, 1.0f };
	//float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	UINT vertexSize = sizeof(float) * 5;
	UINT offset = 0;

	//float lineWidth;
	//lineWidth = 0.1f;

	detectInput();
	updateFreeLookCamera();

	XMMATRIX WVP, World, Projection, WV;

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
	WV = World * XMViewSpace;
	//WVP = Projection * XMViewSpace * World;
	//WV = XMViewSpace * World;
	//WVP = XMMatrixInverse(nullptr, WVP);
	//XMMatrixInverse;

	World = XMMatrixTranspose(World);
	WV = XMMatrixTranspose(WV);
	WVP = XMMatrixTranspose(WVP);
	XMViewSpace = XMMatrixTranspose(XMViewSpace);
	Projection = XMMatrixTranspose(Projection);

	XMStoreFloat4x4(&lightningCBuffer.World, World);
	XMStoreFloat4x4(&lightningCBuffer.WV, WV);
	XMStoreFloat4x4(&lightningCBuffer.WVP, WVP);
	XMStoreFloat4x4(&lightningCBuffer.projection, Projection);
	lightningCBuffer.camPos = camPos;
	lightningCBuffer.lineWidth = 0.1f;
	lightningCBuffer.baseLightningSegments = baseLightningSegments;
	lightningCBuffer.seed = randSeed;

	gdeviceContext->UpdateSubresource(m_graphicsManager->thesisData.constantBuffers["lightningCBuffer"], 0, nullptr, &lightningCBuffer, 0, 0);

	

	//gdeviceContext->DrawAuto();
	
	//gdeviceContext->OMSetRenderTargets(1, m_graphicsManager->getBackbuffer(), nullptr);
	//gdeviceContext->ClearRenderTargetView(*m_graphicsManager->getBackbuffer(), clearColor);


	gdeviceContext->IASetVertexBuffers(0, 1, &lightningBuffer, &vertexSize, &offset);
	gdeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);		//LINESTRIP

	gdeviceContext->SOSetTargets(1, &SOlightningBuffer, &offset);

	gdeviceContext->IASetInputLayout(m_graphicsManager->thesisData.inputLayouts["FirstLayout"]);
	gdeviceContext->PSSetSamplers(0, 1, &m_graphicsManager->thesisData.samplerStates["CoolSampler"]);
	gdeviceContext->VSSetShader(m_graphicsManager->thesisData.vertexShaders["LightningVertexShader"], nullptr, 0);
	gdeviceContext->GSSetShader(StreamOutGS, nullptr, 0);
	gdeviceContext->HSSetShader(NULL, nullptr, 0);
	gdeviceContext->DSSetShader(NULL, nullptr, 0);
	gdeviceContext->PSSetShader(NULL, nullptr, 0);
	//gdeviceContext->OMSetDepthStencilState(NULL, 0);

	gdeviceContext->Draw(2, 0);
	//gdeviceContext->DrawAuto();

	gdeviceContext->SOSetTargets(1, &nullBuffer[0], &offset);



	
	gdeviceContext->OMSetRenderTargets(1, m_graphicsManager->getBackbuffer(), nullptr);
	gdeviceContext->ClearRenderTargetView(*m_graphicsManager->getBackbuffer(), clearColor);
	
	gdeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST);	//PATCHLIST 2 POINTS
	//gdeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	gdeviceContext->IASetVertexBuffers(0, 1, &SOlightningBuffer, &vertexSize, &offset);

	gdeviceContext->IASetInputLayout(m_graphicsManager->thesisData.inputLayouts["FirstLayout"]);
	gdeviceContext->PSSetSamplers(0, 1, &m_graphicsManager->thesisData.samplerStates["CoolSampler"]);
	
	gdeviceContext->VSSetShader(m_graphicsManager->thesisData.vertexShaders["LightningVertexShader"], nullptr, 0);
	gdeviceContext->HSSetShader(m_graphicsManager->thesisData.hullShaders["LightningHullShader"], nullptr, 0);
	gdeviceContext->DSSetShader(m_graphicsManager->thesisData.domainShaders["LightningDomainShader"], nullptr, 0);
	if (GS == 1)
		gdeviceContext->GSSetShader(m_graphicsManager->thesisData.geometryShaders["LightningGeometryShader"], nullptr, 0);
	else
		gdeviceContext->GSSetShader(m_graphicsManager->thesisData.geometryShaders["CubeLightningGeometryShader"], nullptr, 0);
	gdeviceContext->PSSetShader(m_graphicsManager->thesisData.pixelShaders["LightningPixelShader"], nullptr, 0);
	
	
	//gdeviceContext->Draw(64, 0);
	gdeviceContext->DrawAuto();
}

float RandomNumber(float Min, float Max)
{
	srand(time(NULL));
	return ((float(rand()) / float(RAND_MAX)) * (Max - Min)) + Min;
}

void Lightning::Initialize() {
	m_graphicsManager = ApplicationContext::GetInstance().GetGraphicsManager();

	//	1 = Flat Billboard Geometry Shader		
	//	2 = CUBE Geometry Shader
	GS = 2;

	//WIREFRAME //WIREFRAME //WIREFRAME
	m_graphicsManager->setRasterstate(D3D11_CULL_NONE, D3D11_FILL_WIREFRAME);
	//SOLID //SOLID  //SOLID //SOLID 
	m_graphicsManager->setRasterstate(D3D11_CULL_NONE, D3D11_FILL_SOLID);

	createVertexBuffer();
	createStreamVertexBuffer();
	m_graphicsManager->createConstantBuffer("lightningCBuffer", &lightningCBuffer, sizeof(cBuffer));

	//mt19937 generator;
	//uniform_real_distribution<float> uniform_distribution(-10.0, 10.0);
	//auto my_rand = bind(uniform_distribution, generator);
	//randSeed = my_rand();
	//vector<float> random_numbers(1000);
	//generate(random_numbers.begin(), random_numbers.end(), my_rand);

	//randSeed = RandomNumber(-10.0f, 10.0f);

	srand(time(NULL));
	randSeed = rand() % 200 + (-100);	//Range 1 to X

	//randSeed = 1 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (100 - 1)));

	wndHandle = ApplicationContext::GetInstance().GetWindowManager()->getWindowHandle();
	InitDirectInput(*ApplicationContext::GetInstance().GetWindowManager()->getHinstance());			//Initiates the keyboard/mouse input function thingies

	lightningCBuffer.tessLevel = 1.0f;
	lightningCBuffer.density = 1.0f;

	gdeviceContext->VSSetConstantBuffers(0, 1, &m_graphicsManager->thesisData.constantBuffers["lightningCBuffer"]);
	gdeviceContext->HSSetConstantBuffers(0, 1, &m_graphicsManager->thesisData.constantBuffers["lightningCBuffer"]);
	gdeviceContext->GSSetConstantBuffers(0, 1, &m_graphicsManager->thesisData.constantBuffers["lightningCBuffer"]);

	D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	m_graphicsManager->createVertexShader("LightningVertexShader", "FirstLayout", layoutDesc, ARRAYSIZE(layoutDesc));

	m_graphicsManager->createHullShader("LightningHullShader");
	m_graphicsManager->createDomainShader("LightningDomainShader");
	
	if(GS == 1)
		m_graphicsManager->createGeometryShader("LightningGeometryShader");
	else
		m_graphicsManager->createGeometryShader("CubeLightningGeometryShader");

	m_graphicsManager->createPixelShader("LightningPixelShader"); // Name has to match shader name without .hlsl

	D3D11_SO_DECLARATION_ENTRY SODeclaration[] =
	{
		// semantic name, semantic index, start component, component count, output slot
		{ 0, "SV_POSITION", 0, 0, 3, 0 },   // output all components of position
		//{ 0, "NORMAL", 0, 0, 3, 0 },     // output the first 3 of the normal
		{ 0, "TEXCOORD", 0, 0, 2, 0 },     // output the first 2 texture coordinates
	};

	wstring shaderName = L"Shaders/LightningSOGeometryShader.hlsl";
	D3DCompileFromFile(shaderName.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "GS_main", "gs_5_0", D3DCOMPILE_DEBUG, NULL, &ppShader, nullptr);
	
	gdevice->CreateGeometryShaderWithStreamOutput(ppShader->GetBufferPointer(), ppShader->GetBufferSize(), SODeclaration, ARRAYSIZE(SODeclaration), NULL, 0, 0, NULL, &StreamOutGS);

	//gdevice->CreateGeometryShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &StreamOutGS);




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
	if (keyboardState[DIK_R])
	{
		lightningCBuffer.tessLevel += 0.005f;
		if (lightningCBuffer.tessLevel > 64.0f)
			lightningCBuffer.tessLevel = 64.0f;
	}
	if (keyboardState[DIK_F])
	{
		lightningCBuffer.tessLevel -= 0.005f;
		if (lightningCBuffer.tessLevel < 1.0f)
			lightningCBuffer.tessLevel = 1.0f;
	}
	if (keyboardState[DIK_T])
	{	
		dens += 0.005f;
		if (dens > 10.0f)
			dens = 10.0f;
		lightningCBuffer.density = dens;
	}
	if (keyboardState[DIK_G])
	{
		dens -= 0.005f;
		if (dens < 1.0f)
			dens = 1.0f;
		lightningCBuffer.density = dens;
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


//OLD PIPELINE
//
//gdeviceContext->UpdateSubresource(m_graphicsManager->thesisData.constantBuffers["lightningCBuffer"], 0, nullptr, &lightningCBuffer, 0, 0);
//
//gdeviceContext->OMSetRenderTargets(1, m_graphicsManager->getBackbuffer(), nullptr);
//gdeviceContext->ClearRenderTargetView(*m_graphicsManager->getBackbuffer(), clearColor);
//
//gdeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST);	//PATCHLIST 2 POINTS
//																							//gdeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_6_CONTROL_POINT_PATCHLIST);	//PATCHLIST 6 POINTS
//																							//gdeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);		//LINESTRIP
//																							//gdeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);		//TRIANGLESTRIP
//gdeviceContext->IASetInputLayout(m_graphicsManager->thesisData.inputLayouts["FirstLayout"]);
//gdeviceContext->PSSetSamplers(0, 1, &m_graphicsManager->thesisData.samplerStates["CoolSampler"]);
//
//gdeviceContext->VSSetShader(m_graphicsManager->thesisData.vertexShaders["LightningVertexShader"], nullptr, 0);
//gdeviceContext->HSSetShader(m_graphicsManager->thesisData.hullShaders["LightningHullShader"], nullptr, 0);
//gdeviceContext->DSSetShader(m_graphicsManager->thesisData.domainShaders["LightningDomainShader"], nullptr, 0);
//if (GS == 1)
//gdeviceContext->GSSetShader(m_graphicsManager->thesisData.geometryShaders["LightningGeometryShader"], nullptr, 0);
//else
//gdeviceContext->GSSetShader(m_graphicsManager->thesisData.geometryShaders["CubeLightningGeometryShader"], nullptr, 0);
//gdeviceContext->PSSetShader(m_graphicsManager->thesisData.pixelShaders["LightningPixelShader"], nullptr, 0);
//
//gdeviceContext->IASetVertexBuffers(0, 1, &lightningBuffer, &vertexSize, &offset);
//
//gdeviceContext->Draw(4, 0);