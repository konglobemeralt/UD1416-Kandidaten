#ifndef H_GRAPHICSMANAGER
#define H_GRAPHICSMANAGER

#include <windowsx.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <mmsystem.h>
#include <DirectXMath.h>
#include <string>
#include <d3d11sdklayers.h>
#include "DataStructures.h"

#include "../DirectXToolkit/Inc/WICTextureLoader.h"
#include "../DirectXTex/DirectXTexP.h"

#pragma comment (lib, "WINMM.LIB")
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")


using namespace DirectX;

class GraphicsManager {
private:
	IDXGISwapChain* gSwapChain = nullptr;
	ID3D11Device* gDevice = nullptr;
	ID3D11DeviceContext* gDeviceContext = nullptr;
	ID3D11RenderTargetView* gBackbufferRTV = nullptr;

	HWND* windowHandle;


	void setRasterstate(D3D11_CULL_MODE cullmode);
	HRESULT CreateDirect3DContext();
	

	// Quad
	ID3D11Buffer* gQuadBuffer = nullptr;

	// Rasterstate
	ID3D11RasterizerState* rasterState;

	//Empty views to clear depth, RTV's and SRV's
	ID3D11DepthStencilView* emptyDSV;
	ID3D11RenderTargetView* emptyRTV[4];
	ID3D11ShaderResourceView* emptySRV[8];
	
public:
	void ResetViews();
	/*static GraphicsManager& getInstance(){
		static GraphicsManager instance;
		return instance;
	}*/
	GraphicsManager() {};
	void Render();
	
	UINT windowWidth = 512; //1280
	UINT windowHeight = 512; //1024

	ID3D11Texture2D* pBackBuffer = nullptr;
	ThesisData thesisData;

	//GraphicsManager(GraphicsManager const&) = delete;
	//void operator=(GraphicsManager const&) = delete;

	void initGraphics(HWND* hwnd);

	IDXGISwapChain* getSwapChain();
	ID3D11Device* getDevice();
	ID3D11DeviceContext* getDeviceContext();
	ID3D11Buffer** getQuad();
	UINT getWindowWidth();
	UINT getWindowHeight();
	ID3D11RenderTargetView** getBackbuffer();

	void setWindowSize(UINT width, UINT height);

	void createConstantBuffer(std::string name, const void* data, UINT size);
	void createVertexShader(std::string shaderName, std::string layoutName, D3D11_INPUT_ELEMENT_DESC* desc, UINT size);
	void createDomainShader(std::string name);
	void createHullShader(std::string name);
	void createGeometryShader(std::string name);
	void createPixelShader(std::string name);
	void createComputeShader(std::string name);
	void createTexture2D(
		std::string name,
		DXGI_FORMAT format = DXGI_FORMAT_R32G32B32A32_FLOAT, 
		UINT width = 1280,
		UINT height =1024,
		bool renderTarget = true,
		bool shaderResource = true
	);
	void createSamplerState(
		std::string name, 
		D3D11_FILTER filter = D3D11_FILTER_MIN_MAG_MIP_POINT,
		D3D11_TEXTURE_ADDRESS_MODE mode = D3D11_TEXTURE_ADDRESS_CLAMP
	);
	void attachImage(string textureName, string srvName);
	void saveImage(string fileName, ID3D11Texture2D* texture2d, const GUID &fileType = GUID_ContainerFormatPng);
};

#endif