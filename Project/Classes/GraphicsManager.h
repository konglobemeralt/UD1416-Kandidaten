#ifndef H_GRAPHICSMANAGER
#define H_GRAPHICSMANAGER

#include <windowsx.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <mmsystem.h>
#include <DirectXMath.h>
#include <string>

#include "base.h"
#include "DataStructures.h"
//#include "ToneMapping.h"
//#include "Text.h"
//#include "AntiAliasing.h"
//#include "Compositing.h"
//#include "Lightning.h"

#pragma comment (lib, "WINMM.LIB")
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

using namespace DirectX;

class Compositing;
class AntiAliasing;
class Text;
class ToneMapping;
class Lightning;

class GraphicsManager {
private:
	GraphicsManager();
	~GraphicsManager();
	GraphicsManager(GraphicsManager const&) = delete;
	void operator=(GraphicsManager const&) = delete;
	static GraphicsManager* graphicsManager;



	IDXGISwapChain*				gSwapChain = nullptr;
	ID3D11Device*				gDevice = nullptr;
	ID3D11DeviceContext*		gDeviceContext = nullptr;
	ID3D11RenderTargetView*		gBackbufferRTV = nullptr;
	ID3D11Debug*				debug;

	HWND* windowHandle;
	UINT windowWidth	=		1280;
	UINT windowHeight	=		1024;

	void setRasterstate(D3D11_CULL_MODE cullmode);
	HRESULT CreateDirect3DContext();
	void ResetViews();

	// Quad
	ID3D11Buffer*				gQuadBuffer = nullptr;

	// Rasterstate
	ID3D11RasterizerState*		rasterState;

	//Empty views to clear depth, RTV's and SRV's
	ID3D11DepthStencilView*		emptyDSV;
	ID3D11RenderTargetView*		emptyRTV[4];
	ID3D11ShaderResourceView*	emptySRV[8];

	AntiAliasing* m_antiAliasing = nullptr;
	Compositing* m_compositing = nullptr;
	Lightning* m_lightning = nullptr;
	Text* m_text = nullptr;
	ToneMapping* m_toneMapping = nullptr;
	
public:
	static GraphicsManager& getInstance(){
		return *graphicsManager;
	}

	AntiAliasing* GetAntiAliasing() const { return m_antiAliasing; }

	
	static void Startup();
	static void Shutdown();
	void Render();

	int user = TEXT;

	ThesisData thesisData;

	/*GraphicsManager(GraphicsManager const&) = delete;
	void operator=(GraphicsManager const&) = delete;*/

	void initGraphics(HWND* hwnd);

	IDXGISwapChain* getSwapChain();
	ID3D11Device* getDevice();
	ID3D11DeviceContext* getDeviceContext();
	ID3D11Buffer** getQuad();
	UINT getWindowWidth();
	UINT getWindowHeight();
	ID3D11RenderTargetView** getBackbuffer();

	void setWindowSize(UINT width, UINT height);

	void createConstantBuffer(string name, const void* data, UINT size);
	void createVertexShader(string shaderName, string layoutName, D3D11_INPUT_ELEMENT_DESC* desc, UINT size);
	void createDomainShader(string name);
	void createHullShader(string name);
	void createGeometryShader(string name);
	void createPixelShader(string name);
	void createComputeShader(string name);
	void createTexture2D(
		string name, 
		DXGI_FORMAT format = DXGI_FORMAT_R32G32B32A32_FLOAT, 
		UINT width = GraphicsManager::getInstance().getWindowWidth(),
		UINT height = GraphicsManager::getInstance().getWindowHeight(),
		bool renderTarget = true,
		bool shaderResource = true
	);
	void createSamplerState(
		string name, 
		D3D11_FILTER filter = D3D11_FILTER_MIN_MAG_MIP_POINT,
		D3D11_TEXTURE_ADDRESS_MODE mode = D3D11_TEXTURE_ADDRESS_CLAMP
	);


};

#endif