//#ifndef H_TEXT
//#define H_TEXT
//
////#define _MSC_VER 1700
//
//#include <d2d1_1.h>
//#include <d2d1_1helper.h>
//#include <dwrite_1.h>
//#include <atlstr.h>
//#include <fstream>
//#include <vector>
//#pragma comment(lib, "d2d1.lib")
//#pragma comment(lib, "Dwrite")
//
//#include "GraphicsManager.h"
//#include "..\ApplicationContext.h"
//
//#define manager m_graphicsManager
//#define resources m_graphicsManager->thesisData
//#define gdevice m_graphicsManager->getDevice()
//#define gdeviceContext m_graphicsManager->getDeviceContext()
//class GraphicsManager;
//class Text
//{
//public:
//	Text();
//	~Text();
//	void Render();
//	void Initialize();
//	ID3D11ShaderResourceView** GetText();
//
//private:
//	GraphicsManager* m_graphicsManager;
//
//	void InitializeDirect2D();
//	void CheckStatus(HRESULT hr, LPCTSTR titel);
//
//	// Values
//	bool m_firstTime;
//	bool m_edgeRendering;
//	bool m_fxaa;
//	bool m_ssaa;
//	UINT m_height;
//	UINT m_width;
//	float m_textSize;
//	float m_edgeSize;
//	float m_padding;
//	float m_scale;
//
//	// Direct3D
//	float clearColor[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
//	UINT vertexSize = sizeof(float) * 5;
//	UINT offset = 0;
//	XMFLOAT4X4 m_matrix;
//	ID3D11Buffer* m_textPlaneBuffer[3];
//	ID3D11Buffer* m_cameraBuffer = nullptr;
//	ID3D11BlendState* m_blendState;
//
//	// Custom solution
//	struct QuadData
//	{
//		XMFLOAT3 pos;
//		XMFLOAT2 uv;
//	};
//	QuadData** m_quadData;
//	QuadData** m_quad;
//	struct CameraData
//	{
//		XMFLOAT3 position;
//		XMFLOAT3 focus;
//	}* m_camData;
//	float m_quadWidth[3];
//	float m_quadHeight[3];
//	unsigned int m_framesAmount[3];
//	unsigned int m_vertexAmount[3];
//	unsigned int m_frameIndex[3];
//	float m_timer[3];
//	ifstream m_infile;
//	struct Matrices
//	{
//		XMFLOAT4X4 view;
//		XMFLOAT4X4 projection;
//	}m_matrices;
//	void Update(unsigned int id);
//
//	// Direct2D
//	ID2D1Factory1* m_d2dFactory = nullptr;
//	IDXGIDevice* m_dxgiDevice = nullptr;
//	ID2D1Device* m_d2dDev = nullptr;
//	ID2D1DeviceContext* m_d2dDevcon = nullptr;
//	ID3D11Texture2D* d2dTextureTarget[3];
//	IDXGISurface* m_idxgSurface[3];
//	ID2D1RenderTarget* m_d2dRenderTarget[3];
//	ID2D1SolidColorBrush* m_blackBrush[3];
//	ID2D1SolidColorBrush* m_orangeBrush[3];
//	ID3D11ShaderResourceView* finalText[3];
//	D2D1_COLOR_F d2dClearColor;
//
//	// DirectWrite
//	IDWriteFactory1* m_writeFactory = nullptr;
//	const wchar_t* m_text[3];
//	CString m_font = "DINNextLTPro-Regular.otf";
//	UINT32 m_textLength[3];
//	D2D1_RECT_F m_layoutRect;
//	IDWriteTextFormat* m_writeTextFormat[3];
//	RECT rc;
//
//	// Testing
//	ID2D1PathGeometry* m_pathGeometry[3];
//	ID2D1TransformedGeometry* m_transformedPathGeometry[3];
//	ID2D1GeometrySink* m_geometrySink = nullptr;
//	IDWriteFontFace* m_fontFaceBeginning = nullptr;
//	IDWriteFontFace1* m_fontFace = nullptr;
//	IDWriteFontFile* m_fontFiles = nullptr;
//	ID2D1StrokeStyle* m_strokeStyle = nullptr;
//	ID3D11Texture2D* tempD2DTexture = nullptr;
//	UINT* m_codePoints = nullptr;
//	UINT16* m_glyphIndices = nullptr;
//	D2D1_RECT_F bound[3];
//	int* m_advances = nullptr;
//	DWRITE_GLYPH_RUN m_glyphRun;
//	void DirectWriteEdge();
//	void GetTextOutline(const wchar_t* text, int index);
//	void EdgeRender();
//};
//
//#endif