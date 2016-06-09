#ifndef H_TONEMAPPING
#define H_TONEMAPPING

#include "GraphicsManager.h"
#include "../DirectXTex/DirectXTex.h"
#include "../DirectXTex/DirectXTexP.h"

#define manager m_graphicsManager
#define resources m_graphicsManager->thesisData
#define device m_graphicsManager->getDevice()
#define deviceContext m_graphicsManager->getDeviceContext()

class GraphicsManager;

class ToneMapping
{
public:
	void Render();
	void Initialize();
	ToneMapping();
	~ToneMapping();
private:
	GraphicsManager* m_graphicsManager;

	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	UINT vertexSize = sizeof(float) * 5;
	UINT offset = 0;

	UINT technique;
	int32_t textureWidth = 1024;
	ID3D11Texture2D* mipTexture;
};

#endif