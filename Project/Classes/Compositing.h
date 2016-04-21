#ifndef H_COMPOSITING
#define H_COMPOSITING

#include "GraphicsManager.h"
#include "../DirectXTex/DirectXTex.h"
#include "../DirectXTex/DirectXTexP.h"

#define manager m_graphicsManager
#define resources m_graphicsManager->thesisData
#define device m_graphicsManager->getDevice()
#define deviceContext m_graphicsManager->getDeviceContext()

class GraphicsManager;
class Compositing
{
public:
	Compositing();
	~Compositing();
	void Render();
	void Initialize();
private:
	GraphicsManager* m_graphicsManager;
	UINT imageCount = 1;
	string imageWithZero;
	string imageWithZeroBGR;
};

#endif