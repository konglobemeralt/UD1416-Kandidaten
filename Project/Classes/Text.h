#ifndef H_TEXT
#define H_TEXT

#include <d2d1_1.h>
#include <d2d1_1helper.h>
#include <dwrite.h>
#include <atlstr.h>
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "Dwrite")

//#include "GraphicsManager.h"

//#define manager GraphicsManager::getInstance()
//#define resources GraphicsManager::getInstance().thesisData
#define gdevice m_graphicsManager->getDevice()
#define gdeviceContext m_graphicsManager->getDeviceContext()
class GraphicsManager;
class Text
{
public:
	Text();
	~Text();
	void Render();
	void Initialize();


private:
	GraphicsManager* m_graphicsManager;

	void InitializeDirect2D();
	void InitializeDirectWrite();
	void CheckStatus(HRESULT hr, LPCTSTR titel);
	void RenderText();

	// Direct2D
	ID2D1Factory1* m_d2dFactory = nullptr;
	IDXGIDevice* m_dxgiDevice = nullptr;
	ID2D1Device* m_d2dDev = nullptr;
	ID2D1DeviceContext* m_d2dDevcon = nullptr;
	IDXGISurface* m_idxgSurface = nullptr;
	ID2D1RenderTarget* m_d2dRenderTarget = nullptr;
	ID2D1SolidColorBrush* m_blackBrush = nullptr;
	ID2D1SolidColorBrush* m_orangeBrush = nullptr;

	// DirectWrite
	IDWriteFactory* m_writeFactory = nullptr;
	const wchar_t* m_text = nullptr;
	UINT32 m_textLength;
	D2D1_RECT_F m_layoutRect;
	IDWriteTextFormat* m_writeTextFormat = nullptr;
	RECT rc;

	//// Helper functions
	//template<typename T> 
	//void SafeDelete(T*& a);
};

#endif