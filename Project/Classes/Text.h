#ifndef H_TEXT
#define H_TEXT

#include <d2d1_1.h>
#include <d2d1_1helper.h>
#include <dwrite_1.h>
#include <atlstr.h>
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "Dwrite")

#include "GraphicsManager.h"
//
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
	ID3D11ShaderResourceView* GetText();

private:
	GraphicsManager* m_graphicsManager;

	void InitializeDirect2D();
	void InitializeDirectWrite();
	void CheckStatus(HRESULT hr, LPCTSTR titel);
	void RenderText();

	// Values
	unsigned int m_height;
	unsigned int m_width;
	float m_textSize = 500.0f;
	float m_edgeSize = 100.0f;

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
	IDWriteFactory1* m_writeFactory = nullptr;
	const wchar_t* m_text = L"Erik";
	CString m_font = "geo_1.ttf";
	UINT32 m_textLength;
	D2D1_RECT_F m_layoutRect;
	IDWriteTextFormat* m_writeTextFormat = nullptr;
	RECT rc;

	// Testing
	ID2D1PathGeometry* m_pathGeometry = nullptr;
	ID2D1GeometrySink* m_geometrySink = nullptr;
	IDWriteFontFace* m_fontFaceBeginning = nullptr;
	IDWriteFontFace1* m_fontFace = nullptr;
	IDWriteFontFile* m_fontFiles = nullptr;
	ID2D1StrokeStyle* m_strokeStyle = nullptr;
	UINT* m_codePoints = nullptr;
	UINT16* m_glyphIndices = nullptr;
	int* m_advances = nullptr;
	DWRITE_GLYPH_RUN m_glyphRun;
	void DirectWriteEdge();
	void EdgeRender();

	//// Helper functions
	//template<typename T> 
	//void SafeDelete(T*& a);
};

#endif