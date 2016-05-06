#ifndef H_ANTIALIASING
#define H_ANTIALIASING

#include "GraphicsManager.h"
#pragma comment (lib, "dinput8.lib")	//Keyboard/mouse input
#pragma comment (lib, "dxguid.lib")		//Keyboard/mouse input
#include <dinput.h>						//Keyboard/mouse input

//#define manager GraphicsManager::getInstance()
//#define resources GraphicsManager::getInstance().thesisData
#define gdevice m_graphicsManager->getDevice()
#define gdeviceContext m_graphicsManager->getDeviceContext()
class WindowManager;
class GraphicsManager;
class AntiAliasing
{
public:
	AntiAliasing();
	~AntiAliasing();
	void Render();
	void Initialize();

	void InitExample();

	void RenderSSAA();
	void RenderFXAA();
private:
	GraphicsManager* m_graphicsManager;

	//movement*****************************
	void detectInput();
	bool InitDirectInput(HINSTANCE hInstance);
	void updateFreeLookCamera();
	void ResetFreeLookCamera();

	float speed = 0.001;

	XMFLOAT4 worldForward, worldRight, camForward, camRight;
	XMFLOAT4X4 ViewSpace;
	//XMFLOAT4 camPos, camLook, camUp;
	XMFLOAT4 camPos = XMFLOAT4(0.0f, 0.0f, -5.0f, 1.0f);	// Camera StartPos		//-2.0 Moves camera back 2 units along Z
	XMFLOAT4 camLook = XMFLOAT4(-5.0f, -5.0f, -5.0f, 1.0f);
	XMFLOAT4 camUp = XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);
	float moveLR = 0.0f;	//MOVE LEFT AND RIGHT
	float moveFB = 0.0f;	//MOVE FORWARD AND BACK
	float camYaw = 0.0f;	//CAMERA ROTATION
	float camPitch = 0.0f;	//CAMERA ROTATION

	IDirectInputDevice8* DIKeyboard;
	IDirectInputDevice8* DIMouse;
	DIMOUSESTATE mouseLastState;
	LPDIRECTINPUT8 DirectInput;
	HWND* wndHandle;
	//movement*****************************

	//cbuffers
	void InitConstantBuffers();
	void InitTextures();

	struct Matrix_VS_ConstantBuffer
	{
		XMFLOAT4X4 world;
		XMFLOAT4X4 view;
		XMFLOAT4X4 projection;
		XMFLOAT4X4 WVP;
	}Matrix_VS_cb;

	struct FXAA_PS_ConstantBuffer { //texelsize n shiet
		XMFLOAT2 texelSizeXY;
		float FXAA_blur_Texels_Threshhold; //hur många texlar som kommer blurras åt varje håll
		float edgeDetectionTexelMULTIPLIER;
		float FXAA_reduce_MULTIPLIER;
		float FXAA_reduce_MIN; //så dirOffset inte ska bli noll
		XMFLOAT2 pad;
	}FXAA_PS_cb;

	bool hasStored = true;

	string AATex1;
	string AATex2;

	enum Rendering {FXAA, SSAA, Both, Aliased};
	Rendering renderType = (Rendering)0;
};


#endif