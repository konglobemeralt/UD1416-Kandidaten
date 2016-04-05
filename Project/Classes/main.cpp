//#include <windowsx.h>
//#include <d3d11.h>
//#include <DirectXMath.h>
//#include <d3dcompiler.h>
//#include <vector>
//#include <DirectXCollision.h>
//#include <fstream>
//#include <mmsystem.h>
//
//#pragma comment (lib, "WINMM.LIB")
//#pragma comment (lib, "d3d11.lib")
//#pragma comment (lib, "d3dcompiler.lib")

#include "Particles.h"
#include "HeightMap.h"
#include "QuadTree.h"
#include "Camera.h"
#include "ObjImporter.h"
#include "WindowManager.h"
#include "GraphicsManager.h"
#include "ContentManager.h"
#include "..\DirectXToolkit\Inc\WICTextureLoader.h"

#define PI (3.141592653589793)

//###################
//##### HOTKEYS #####
// TAB - Enable directional light
// ESC - Exit app
// SPACE - Toggle fly mode
// NUM-+ - Increase/ecrease speed

using namespace DirectX;
using namespace std;
//
//vector<ID3D11Buffer*> gVertexBuffer; //Vector<Vertex buffer>
//vector<ID3D11Buffer*> boxBuffer;
//vector<ID3D11Buffer*> quadBuffers;
//
//ID3D11Buffer* vxBuffer = nullptr; //Vertex buffer
//ID3D11Buffer* gMatrixBuffer = nullptr;
//ID3D11Buffer* gDirLightBuffer = nullptr;
//ID3D11Buffer* gPointLightBuffer = nullptr;
//ID3D11Buffer* indexBuffer = nullptr;
//
//vector<ID3D11ShaderResourceView*> objImporterRSV; //Vector<Texture RSV>
//ID3D11ShaderResourceView* firstRSV = nullptr; //Texture RSV
//
////Re-map buffers
//D3D11_MAPPED_SUBRESOURCE mSubResource;
//D3D11_MAPPED_SUBRESOURCE bSubResource;
//
//Camera cam;
//POINT mLastMousePos;
//BoundingFrustum frustum;
//QuadTree* quad = nullptr;
//HeightMap* terrain = nullptr;
//Particles* particleBox = nullptr;
//
//
//vector<int> vertexDrawCount;
//vector<XMFLOAT3> objectMatrix;
//vector<ObjImporter*> objectList;
//XMMATRIX terrainWorldMatrix;
//
//int textureCount = 0;
//int objectCount = 0;
//float mouseSensitivity = 0.05f;
//float yaw = 0.0f;
//float mRotX = 0.0f;
//float mRotY = 0.0f;
//int direction = 1; //Moving box
//bool flight = false;
//float dt = 0.6f;
//#pragma endregion Globals
//
//struct wvpMatrices
//{
//	XMFLOAT4X4 worldMatrix;
//	XMFLOAT4X4 viewMatrix;
//	XMFLOAT4X4 projMatrix;
//	XMFLOAT4X4 viewMatrixInverse;
//	XMFLOAT4X4 lightViewMatrix;
//	XMFLOAT4X4 lightProjMatrix;
//	XMFLOAT4 camPos;
//	XMFLOAT4 specular;
//};
//
//struct DirectionalLight{
//	float lightIntensity;
//	float lightDirectionX, lightDirectionY, lightDirectionZ;
//	float lightColorR, lightColorG, lightColorB;
//	float padding;
//};
//
//struct PointLight{
//	XMFLOAT3 pLightPosition;
//	float pLightIntensity;
//	XMFLOAT3 pLightColor;
//	float pLightRange;
//	XMFLOAT3 pLightAtt;
//	float padding;
//};
//
//struct PointLightStruct{
//	PointLight pLightStruct[10];
//	XMFLOAT4 pNumberOfLights;
//};
//
//DirectionalLight light1;
//bool lightSwitch = false;
//
//void UpdateBuffers(XMFLOAT3 wMatrix, XMFLOAT4 specularColor_Power) //Update wvp buffer
//{
//	//Constant buffer for wvp-matrices
//	wvpMatrices* wvpData;
//	gDeviceContext->Map(gMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mSubResource);
//	wvpData = (wvpMatrices*)mSubResource.pData;
//
//	XMVECTOR eyePos = { 15.0f, 30.0f, -50.0f };
//	XMVECTOR focusPos = { 0.0f, 0.0f, 0.0f };
//	XMVECTOR upDir = { 0.0f, 1.0f, 0.0f };
//	
//	XMStoreFloat4x4(&wvpData->worldMatrix, XMMatrixTranspose(XMMatrixTranslation(wMatrix.x, wMatrix.y, wMatrix.z)));
//	XMStoreFloat4x4(&wvpData->viewMatrix, XMMatrixTranspose(cam.View()));
//	XMStoreFloat4x4(&wvpData->projMatrix, XMMatrixTranspose(cam.Proj()));
//	XMStoreFloat4x4(&wvpData->viewMatrixInverse, XMMatrixTranspose(XMMatrixInverse(NULL, cam.View())));
//	XMStoreFloat4x4(&wvpData->lightViewMatrix, XMMatrixTranspose(XMMatrixLookAtLH(eyePos, focusPos, upDir)));
//	XMStoreFloat4x4(&wvpData->lightProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(150, 150, 0.1f, 200.0f)));
//	XMStoreFloat4(&wvpData->camPos, cam.GetPositionXM());
//	wvpData->specular = specularColor_Power;
//	gDeviceContext->Unmap(gMatrixBuffer, 0);
//
//	gDeviceContext->PSSetConstantBuffers(0, 1, &gMatrixBuffer);
//	gDeviceContext->GSSetConstantBuffers(0, 1, &gMatrixBuffer);
//	gDeviceContext->VSSetConstantBuffers(0, 1, &gMatrixBuffer);
//	gDeviceContext->CSSetConstantBuffers(0, 1, &gMatrixBuffer);
//}
//
//void UpdateBuffers2(const XMMATRIX& wMatrix) //Update wvp buffer
//{
//	wvpMatrices* wvpData;
//	gDeviceContext->Map(gMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mSubResource);
//	wvpData = (wvpMatrices*)mSubResource.pData;
//
//	XMVECTOR eyePos = { 15.0f, 30.0f, -50.0f };
//	XMVECTOR focusPos = { 0.0f, 0.0f, 0.0f };
//	XMVECTOR upDir = { 0.0f, 1.0f, 0.0f };
//	
//
//	XMStoreFloat4x4(&wvpData->worldMatrix, XMMatrixTranspose(wMatrix));
//	XMStoreFloat4x4(&wvpData->viewMatrix, XMMatrixTranspose(cam.View()));
//	XMStoreFloat4x4(&wvpData->projMatrix, XMMatrixTranspose(cam.Proj()));
//	XMStoreFloat4x4(&wvpData->viewMatrixInverse, XMMatrixTranspose(XMMatrixInverse(NULL, cam.View())));
//	XMStoreFloat4x4(&wvpData->lightViewMatrix, XMMatrixTranspose(XMMatrixLookAtLH(eyePos, focusPos, upDir)));
//	XMStoreFloat4x4(&wvpData->lightProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(150, 150, 0.1f, 200.0f)));
//	XMStoreFloat4(&wvpData->camPos, cam.GetPositionXM());
//	gDeviceContext->Unmap(gMatrixBuffer, 0);
//
//	gDeviceContext->PSSetConstantBuffers(0, 1, &gMatrixBuffer);
//	gDeviceContext->GSSetConstantBuffers(0, 1, &gMatrixBuffer);
//	gDeviceContext->VSSetConstantBuffers(0, 1, &gMatrixBuffer);
//	gDeviceContext->CSSetConstantBuffers(0, 1, &gMatrixBuffer);
//}
//
//void UpdateDirectionalLight(DirectionalLight* dirLight, float intensity, XMFLOAT3 direction, XMFLOAT3 color){
//	gDeviceContext->Map(gDirLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mSubResource);
//	dirLight = (DirectionalLight*)mSubResource.pData;
//
//	dirLight->lightIntensity = intensity;
//	dirLight->lightDirectionX = direction.x;
//	dirLight->lightDirectionY = direction.y;
//	dirLight->lightDirectionZ = direction.z;
//	dirLight->lightColorR = color.x;
//	dirLight->lightColorG = color.y;
//	dirLight->lightColorB = color.z;
//	dirLight->padding = 0.0f;
//
//	gDeviceContext->Unmap(gDirLightBuffer, 0);
//
//	gDeviceContext->PSSetConstantBuffers(1, 1, &gDirLightBuffer);
//}
//
//void UpdatePointLight(PointLight* pLight, float pLightIntensity, float pLightRange, XMFLOAT3 pLightPosition, XMFLOAT3 pLightColor, XMFLOAT3 pLightAtt){
//	pLight->pLightIntensity = pLightIntensity;
//	pLight->pLightRange = pLightRange;
//	pLight->pLightPosition = pLightPosition;
//	pLight->pLightColor = pLightColor;
//	pLight->pLightAtt = pLightAtt;
//	pLight->padding = 0.0f;
//}
//
//void UpdateBoxBuffers(int i){
//	XMMATRIX worldMatrix = XMMatrixTranslation(objectMatrix[i].x, objectMatrix[i].y, objectMatrix[i].z);
//
//	gDeviceContext->Map(boxBuffer[i], 0, D3D11_MAP_WRITE_DISCARD, 0, &bSubResource);
//
//	//BoundingBox temp;
//	XMFLOAT3* boxCorners = (XMFLOAT3*)bSubResource.pData;
//
//	BoundingBox temp;
//	objectList[i]->box.bBox.Transform(temp, worldMatrix);
//
//	temp.GetCorners(boxCorners);
//	
//	gDeviceContext->Unmap(boxBuffer[i], 0);
//}
//
//void CreateObject(wstring objectFile, XMFLOAT3 wMatrix, bool blendState, wstring blendTexture, wstring blendMap)
//{
//	// Load
//	ObjImporter* object;
//	object = new ObjImporter(objectFile, blendState);
//
//	object->ObjLoad();
//	objectMatrix.push_back(wMatrix);
//	objectList.push_back(object);
//

//
//	vertexDrawCount.push_back(object->totalVertices);
//
//	//AABB buffer
//	
//	XMFLOAT3 box[8];
//	object->box.bBox.GetCorners(box);
//
//	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
//	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
//	bufferDesc.ByteWidth = sizeof(XMFLOAT3)*8;
//
//	D3D11_SUBRESOURCE_DATA boxData;
//	boxData.pSysMem = box;
//	boxData.SysMemPitch = 0;
//	boxData.SysMemSlicePitch = 0;
//
//	boxBuffer.push_back(vxBuffer);
//	HRESULT hr1 = gDevice->CreateBuffer(&bufferDesc, &boxData, &boxBuffer[boxBuffer.size() - 1]);
//

//	
//	//Texture
//	objImporterRSV.push_back(firstRSV);
//
//	CreateWICTextureFromFile(gDevice, object->textureFile.c_str(), nullptr, &objImporterRSV[objImporterRSV.size() - 1]);
//}
//
//void SetViewport()
//{
//	D3D11_VIEWPORT vp;
//	vp.Width = (float)WindowWidth;
//	vp.Height = (float)WindowHeight;
//	vp.MinDepth = 0.0f;
//	vp.MaxDepth = 1.0f;
//	vp.TopLeftX = 0;
//	vp.TopLeftY = 0;
//
//	gDeviceContext->RSSetViewports(1, &vp);
//}
//
//void SetViewport2()
//{
//	D3D11_VIEWPORT vp;
//	vp.Width = 2048.0f;
//	vp.Height = 2048.0f;
//	vp.MinDepth = 0.0f;
//	vp.MaxDepth = 1.0f;
//	vp.TopLeftX = 0;
//	vp.TopLeftY = 0;
//
//	gDeviceContext->RSSetViewports(1, &vp);
//}
//
//void OnMouseMove(int x, int y)
//{
//	if (x != mLastMousePos.x || y != mLastMousePos.y)
//	{
//		if (x != mLastMousePos.x)
//		{
//			mRotX += (mouseSensitivity * XMConvertToRadians(x - mLastMousePos.x));
//
//			if (mRotX > (2 * XM_PI))
//			{
//				mRotX = 0;
//			}
//			else if (mRotX < -(2 * XM_PI))
//			{
//				mRotX = 0;
//			}
//
//			cam.RotateY(mRotX);
//		}
//
//		if (y != mLastMousePos.y)
//		{
//			mRotY += (mouseSensitivity * XMConvertToRadians(y - mLastMousePos.y));
//
//			if (mRotY > (2 * XM_PI))
//			{
//				mRotY = 0;
//			}
//			else if (mRotY < -(2 * XM_PI))
//			{
//				mRotY = 0;
//			}
//
//			cam.Pitch(mRotY);	
//		}
//
//		SetCursorPos(WindowWidth/2, WindowHeight/2);
//
//		mLastMousePos.x = x;
//		mLastMousePos.y = y;
//	}
//}
//
//void Movement(float dt)
//{
//	if (GetAsyncKeyState('W') & 0x8000)
//		cam.Walk(dt);
//	if (GetAsyncKeyState('S') & 0x8000)
//		cam.Walk(-dt);
//	if (GetAsyncKeyState('A') & 0x8000)
//		cam.Strafe(-dt);
//	if (GetAsyncKeyState('D') & 0x8000)
//		cam.Strafe(dt);
//}
//
//void Update(float dt)
//{
//	//Set y-pos based on terrain
//	if (flight == false){
//		XMFLOAT3 camPos = cam.GetPosition();
//		cam.SetPosition(camPos.x, terrain->getTerrainHeight(camPos.x, camPos.z), camPos.z);
//	}
//
//	cam.UpdateViewMatrix();
//	Movement(dt);
//
//	mRotX = 0;
//	mRotY = 0;
//
//	float speed = 0.3f;
//
//	if (direction == 1){
//		objectMatrix[1].x += speed;
//		if (objectMatrix[1].x >= 150.0f){
//			direction = 2;
//		}
//	}
//	if (direction == 2){
//		objectMatrix[1].z += speed;
//		if (objectMatrix[1].z >= 150.0f){
//			direction = 3;
//		}
//	}
//	if (direction == 3){
//		objectMatrix[1].x -= speed;
//		if (objectMatrix[1].x <= 50.0f){
//			direction = 4;
//		}
//	}
//	if (direction == 4){
//		objectMatrix[1].z -= speed;
//		if (objectMatrix[1].z <= 50.0f){
//			direction = 1;
//		}
//	}
//
//	BoundingFrustum temp(cam.Proj());
//	temp.Transform(frustum, XMMatrixInverse(NULL, cam.View()));
//}
//
//void setRasterizerStateCullMode(D3D11_CULL_MODE cullMode){
//	D3D11_RASTERIZER_DESC rastDesc;
//	ID3D11RasterizerState* rastState;
//
//	rastDesc.FillMode = D3D11_FILL_SOLID;
//	rastDesc.CullMode = cullMode;
//	rastDesc.FrontCounterClockwise = FALSE;
//	rastDesc.DepthBias = 0;
//	rastDesc.SlopeScaledDepthBias = 0.0f;
//	rastDesc.DepthBiasClamp = 0.0f;
//	rastDesc.DepthClipEnable = TRUE;
//	rastDesc.ScissorEnable = FALSE;
//	rastDesc.MultisampleEnable = FALSE;
//	rastDesc.AntialiasedLineEnable = FALSE;
//
//	HRESULT hr1 = gDevice->CreateRasterizerState(&rastDesc, &rastState);
//	gDeviceContext->RSSetState(rastState);
//}
//
//void CullQuadTree(){
//	for (int i = 0; i < objectList.size(); i++){
//		objectList[i]->box.isRendered = false;
//	}
//
//	quad->ObjectRenderState(quad, &frustum);
//}
enum PipelineState {
	PRE_RENDER,
	POST_RENDER
};

PipelineState pipelineState;
WindowManager* gWindowManager;
GraphicsManager* gGraphicsManager;
ContentManager* gContentManager;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
			//case VK_SPACE:
			//	if (flight == false)
			//		flight = true;
			//	else
			//		flight = false;
			//	return 0;
			//case VK_ADD:
			//	dt += 0.05f;
			//	return 0;
			//case VK_SUBTRACT:
			//	if (dt > 0.05f)
			//		dt -= 0.05f;
			//	return 0;
		case VK_TAB:
			//if (lightSwitch == false) {
			//	UpdateDirectionalLight(&light1, 1.0f, XMFLOAT3(15.0f, 30.0f, -50.0f), XMFLOAT3(0.1f, 0.0f, 0.0f));
			//	lightSwitch = true;
			//}
			//else {
			//	UpdateDirectionalLight(&light1, 0.0f, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));
			//	lightSwitch = false;
			//}
			if(pipelineState == PRE_RENDER)
				gGraphicsManager->CreateShaders();
			return 0;
		default:
			return 0;
		}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
		//case WM_MOUSEMOVE:
		//	OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		//	return 0;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
	gWindowManager = new WindowManager(hInstance, WndProc);
	gGraphicsManager = new GraphicsManager(gWindowManager->getWindowHandle(), gWindowManager->getWindowWidth(), gWindowManager->getWindowHeight());

	ID3D11Device* gDevice = gGraphicsManager->getDevice();
	ID3D11DeviceContext* gDeviceContext = gGraphicsManager->getDeviceContext();

	gContentManager = new ContentManager(gDevice, gDeviceContext);
	gContentManager->LoadContent();
	gGraphicsManager->setContentManager(gContentManager);

	MSG msg = { 0 };

	CoInitialize(NULL);

	D3D11_VIEWPORT vp;
	vp.Width = (float)gWindowManager->getWindowWidth();
	vp.Height = (float)gWindowManager->getWindowHeight();
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	gDeviceContext->RSSetViewports(1, &vp);

	//##################
	SetCursorPos(gWindowManager->getWindowWidth() / 2, gWindowManager->getWindowHeight() / 2);
	//cam.SetLens(45.0f*float(PI / 180), float(WindowWidth) / float(WindowHeight), 0.1f, 500.0f);
	//##################

	ShowWindow(*gWindowManager->getWindowHandle(), nCmdShow);

	static int fps = 0;
	static int uptime = 1;
	DWORD currentFrame = timeGetTime();
	static DWORD lastFrame = currentFrame;

	//### TEMP HARD CODED ###

	//#######################

	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			currentFrame = timeGetTime();

			if (currentFrame - lastFrame >= 1000) {
				wstring headerMessage;
				headerMessage.append(L"Uptime: " + to_wstring(uptime) + L"   FPS: " + to_wstring(fps));
				//headerMessage.append(L" X: " + to_wstring(cam.GetPosition().x) + L" Y: " + to_wstring(cam.GetPosition().y) + L" Z: " + to_wstring(cam.GetPosition().z));
				SetWindowText(*gWindowManager->getWindowHandle(), headerMessage.c_str());

				lastFrame = currentFrame;
				uptime++;
				fps = 0;
			}
			else
				fps++;

			//Update(dt);
			pipelineState = PRE_RENDER;
			gGraphicsManager->Render();
			gGraphicsManager->getSwapChain()->Present(0, 0);
			pipelineState = POST_RENDER;
		}
	}

	
	gGraphicsManager->Release();
	DestroyWindow(*gWindowManager->getWindowHandle());
	delete gWindowManager;
	delete gGraphicsManager;

	return (int) msg.wParam;
}

/*

* Continue sorting rendering of a hardcoded object (in main atm).
* Enable camera when rendering an object is possible.
* Work out the structure of object load -> create -> store -> render.
* Create object classes.
* Combined v/i-buffers for objects that will always be rendered togeather.
* Rename stuff that is not conveniently named.
* Optimize render pipeline by minimizing state-changes.
* Add deferred rendering.

*/
