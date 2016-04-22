#ifndef H_LIGHTNING
#define H_LIGHTNING

#include <d3d11.h>
#pragma comment (lib, "dinput8.lib")	//Keyboard/mouse input
#pragma comment (lib, "dxguid.lib")		//Keyboard/mouse input
#include <dinput.h>						//Keyboard/mouse input
#include <DirectXMath.h>

//#define manager GraphicsManager::getInstance()
//#define resources GraphicsManager::getInstance().thesisData
#define gdevice m_graphicsManager->getDevice()
#define gdeviceContext m_graphicsManager->getDeviceContext()

using namespace DirectX;
class WindowManager;
class GraphicsManager;
class Lightning
{
public:
	Lightning();
	~Lightning();
	void detectInput();
	bool InitDirectInput(HINSTANCE hInstance);
	void updateFreeLookCamera();
	void Render();
	void Initialize();
private:
	GraphicsManager* m_graphicsManager;
	ID3D11Buffer* lightningBuffer;
	void createVertexBuffer();

	float speed = 0.001;

	XMFLOAT4 worldForward, worldRight, camForward, camRight;
	XMFLOAT4X4 ViewSpace;
	//XMFLOAT4 camPos, camLook, camUp;
	XMFLOAT4 camPos = XMFLOAT4(0.0f, 0.0f, -1.0f, 1.0f);	// Camera StartPos		//-2.0 Moves camera back 2 units along Z
	XMFLOAT4 camLook = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 camUp = XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f);
	float moveLR = 0.0f;	//MOVE LEFT AND RIGHT
	float moveFB = 0.0f;	//MOVE FORWARD AND BACK
	float camYaw = 0.0f;	//CAMERA ROTATION
	float camPitch = 0.0f;	//CAMERA ROTATION

	IDirectInputDevice8* DIKeyboard;
	IDirectInputDevice8* DIMouse;
	DIMOUSESTATE mouseLastState;
	LPDIRECTINPUT8 DirectInput;

	HWND* wndHandle;

	struct cBuffer 
	{
		XMFLOAT4X4 WVP;
		XMFLOAT4X4 World;
		XMFLOAT4 camPos;
		float lineWidth;
		float tessLevel;
		XMFLOAT2 padding;
	};
	cBuffer lightningCBuffer;
};

#endif