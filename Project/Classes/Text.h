#ifndef H_TEXT
#define H_TEXT

#include <d2d1_1.h>
#include <d2d1_1helper.h>
#include <dwrite_1.h>
#include <atlstr.h>
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "Dwrite")

#include "GraphicsManager.h"
#include "..\ApplicationContext.h"
#include "WindowManager.h"

#pragma comment (lib, "dinput8.lib")	//Keyboard/mouse input
#pragma comment (lib, "dxguid.lib")		//Keyboard/mouse input
#include <dinput.h>						//Keyboard/mouse input

#define manager m_graphicsManager
#define resources m_graphicsManager->thesisData
#define gdevice m_graphicsManager->getDevice()
#define gdeviceContext m_graphicsManager->getDeviceContext()

using namespace DirectX;
class GraphicsManager;
class Text
{
public:
	Text();
	~Text();
	void Render();
	void Initialize();
	ID3D11ShaderResourceView** GetText();
	bool InitDirectInput(HINSTANCE hInstance);

private:
	GraphicsManager* m_graphicsManager;

	void InitializeDirect2D();
	void InitializeDirectWrite();
	void CheckStatus(HRESULT hr, LPCTSTR titel);
	void RenderText();
	void CalculateSize();

	// Values
	bool m_firstTime;
	bool m_edgeRendering;
	bool m_fxaa;
	bool m_ssaa;
	bool m_thesis;
	unsigned int m_ssaaSize;
	float m_sizeMultiplier;
	UINT m_height;
	UINT m_width;
	float m_uvWidth;
	float m_uvHeight;
	float m_textSize;
	float m_edgeSize;
	float m_padding;
	float m_scale;

	// Direct3D
	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	UINT vertexSize = sizeof(float) * 5;
	UINT offset = 0;
	ID3D11Query* m_query;

	// Direct2D
	ID2D1Factory1* m_d2dFactory = nullptr;
	IDXGIDevice* m_dxgiDevice = nullptr;
	ID2D1Device* m_d2dDev = nullptr;
	ID2D1DeviceContext* m_d2dDevcon = nullptr;
	ID3D11Texture2D* d2dTextureTarget[3];
	IDXGISurface* m_idxgSurface[3];
	ID2D1RenderTarget* m_d2dRenderTarget[3];
	ID2D1SolidColorBrush* m_blackBrush[3];
	ID2D1SolidColorBrush* m_orangeBrush[3];
	ID3D11ShaderResourceView* finalText[3];
	D2D1_COLOR_F d2dClearColor;

	// DirectWrite
	IDWriteFactory1* m_writeFactory = nullptr;
	const wchar_t* m_text[3];
	CString m_font = "arial.ttf";
	UINT32 m_textLength[3];
	D2D1_RECT_F m_layoutRect;
	IDWriteTextFormat* m_writeTextFormat[3];
	RECT rc;

	// Testing
	ID2D1PathGeometry* m_pathGeometry[3];
	ID2D1TransformedGeometry* m_transformedPathGeometry[3];
	ID2D1GeometrySink* m_geometrySink = nullptr;
	IDWriteFontFace* m_fontFaceBeginning = nullptr;
	IDWriteFontFace1* m_fontFace = nullptr;
	IDWriteFontFile* m_fontFiles = nullptr;
	ID2D1StrokeStyle* m_strokeStyle = nullptr;
	ID3D11Texture2D* tempD2DTexture = nullptr;
	UINT* m_codePoints = nullptr;
	UINT16* m_glyphIndices = nullptr;
	D2D1_RECT_F bound[3];
	int* m_advances = nullptr;
	DWRITE_GLYPH_RUN m_glyphRun;
	void DirectWriteEdge();
	void GetTextOutline(const wchar_t* text, int index);
	void EdgeRender();

	// Camera
	void DetectInput();
	void UpdateFreeLookCamera();
	struct Matrices
	{
		XMFLOAT4X4 world;
		XMFLOAT4X4 view;
		XMFLOAT4X4 projection;
		XMFLOAT4 useMatrices;
	}m_matrices;
	float speed = 0.01f;

	XMFLOAT4 worldForward, worldRight, camForward, camRight;
	XMFLOAT4X4 ViewSpace;
	//XMFLOAT4 camPos, camLook, camUp;
	XMFLOAT4 camPos;
	XMFLOAT4 camLook;
	XMFLOAT4 camUp;
	float moveLR = 0.0f;	//MOVE LEFT AND RIGHT
	float moveFB = 0.0f;	//MOVE FORWARD AND BACK
	float camYaw = 0.0f;	//CAMERA ROTATION
	float camPitch = 0.0f;	//CAMERA ROTATION

	IDirectInputDevice8* DIKeyboard;
	IDirectInputDevice8* DIMouse;
	DIMOUSESTATE mouseLastState;
	LPDIRECTINPUT8 DirectInput;

	HWND* wndHandle;
};

#endif