#ifndef H_GRAPHICSMANAGER
#define H_GRAPHICSMANAGER

#include <windowsx.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <mmsystem.h>
#include <DirectXMath.h>

#pragma comment (lib, "WINMM.LIB")
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

#include "ContentManager.h"

using namespace DirectX;

class GraphicsManager {
private:
	ContentManager* gContentManager;

	IDXGISwapChain* gSwapChain = nullptr;
	ID3D11Device* gDevice = nullptr;
	ID3D11DeviceContext* gDeviceContext = nullptr;
	ID3D11RenderTargetView* gBackbufferRTV = nullptr;

	HWND* windowHandle;
	UINT windowWidth;
	UINT windowHeight;

	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	UINT32 vertexSize;
	UINT32 quadSize;
	UINT32 boxSize;
	UINT32 offset;

	void CreateGbuffers();
	void CreateSamplers();
	void setRasterstate(D3D11_CULL_MODE cullmode);
	HRESULT CreateDirect3DContext();
	void ResetViews();

	// Layouts
	ID3D11InputLayout* gVertexLayout = nullptr;
	ID3D11InputLayout* gDeferredVertexLayout = nullptr;
	ID3D11InputLayout* gShadowMapLayout = nullptr;
	ID3D11InputLayout* gBoxLayout = nullptr;
	ID3D11InputLayout* gHMLayout = nullptr;

	// Shaders
	ID3D11VertexShader* gVertexShader = nullptr;
	ID3D11VertexShader* gDeferredVertexShader = nullptr;
	ID3D11VertexShader* gShadowMapVertexShader = nullptr;
	ID3D11VertexShader* gBoxVertexShader = nullptr;
	ID3D11VertexShader* gHMVertexShader = nullptr;
	ID3D11PixelShader* gPixelShader = nullptr;
	ID3D11PixelShader* gDeferredPixelShader = nullptr;
	ID3D11PixelShader* gShadowMapPixelShader = nullptr;
	ID3D11PixelShader* gBoxPixelShader = nullptr;
	ID3D11PixelShader* gHMPixelShader = nullptr;
	ID3D11GeometryShader* gGeometryShader = nullptr;

	// Quad
	ID3D11Buffer* gQuadBuffer = nullptr;

	// G-BUFFERS
	ID3D11RenderTargetView* g_ColorRTV = nullptr;
	ID3D11RenderTargetView* g_NormalRTV = nullptr;
	ID3D11RenderTargetView* g_WposRTV = nullptr;
	ID3D11RenderTargetView* g_DiffuseRTV = nullptr;
	ID3D11RenderTargetView* g_RTV[4] = { g_ColorRTV, g_NormalRTV , g_WposRTV, g_DiffuseRTV};
	
	ID3D11ShaderResourceView* g_ColorSRV = nullptr;
	ID3D11ShaderResourceView* g_NormalSRV = nullptr;
	ID3D11ShaderResourceView* g_LightSRV = nullptr;
	ID3D11ShaderResourceView* g_WposSRV = nullptr;
	ID3D11ShaderResourceView* g_DiffuseSRV = nullptr;
	ID3D11ShaderResourceView* g_SRV[5] = { g_ColorSRV, g_NormalSRV, g_LightSRV, g_WposSRV, g_DiffuseSRV };
	
	ID3D11Texture2D* g_Color = nullptr;
	ID3D11Texture2D* g_Normal = nullptr;
	ID3D11Texture2D* g_cameraDepth = nullptr;
	ID3D11Texture2D* g_lightDepth = nullptr;
	ID3D11Texture2D* g_Wpos = nullptr;
	ID3D11Texture2D* g_Diffuse = nullptr;
	
	ID3D11DepthStencilView* g_cameraDSV = nullptr;
	ID3D11DepthStencilView* g_lightDSV = nullptr;

	// Samplers
	ID3D11SamplerState* samplerStateWrap;
	ID3D11SamplerState* samplerStateClamp;
	ID3D11SamplerState* samplerStateTerrain;

	// Rasterstate
	ID3D11RasterizerState* rasterState;

	//Empty views to clear depth, RTV's and SRV's
	ID3D11DepthStencilView* emptyDSV;
	ID3D11RenderTargetView* emptyRTV[4];
	ID3D11ShaderResourceView* emptySRV[8];
	
public:
	GraphicsManager(HWND* hwnd, UINT winWidth, UINT winHeight);
	~GraphicsManager();

	IDXGISwapChain* getSwapChain();
	ID3D11Device* getDevice();
	ID3D11DeviceContext* getDeviceContext();
	ID3D11RenderTargetView* getBackbufferRTV();

	void setContentManager(ContentManager* manager);

	void Render();
	void Release();
	void CreateShaders();
};

#endif