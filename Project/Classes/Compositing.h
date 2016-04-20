#ifndef H_COMPOSITING
#define H_COMPOSITING

//#include "GraphicsManager.h"

//#define manager GraphicsManager::getInstance()
//#define resources GraphicsManager::getInstance().thesisData
#define gdevice m_graphicsManager->getDevice()
#define gdeviceContext m_graphicsManager->getDeviceContext()
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

};

#endif