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
	UINT imageCount = 1;
	string imageWithZero;
};

#endif