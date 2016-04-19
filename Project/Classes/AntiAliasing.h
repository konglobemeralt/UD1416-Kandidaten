#ifndef H_ANTIALIASING
#define H_ANTIALIASING

//#include "GraphicsManager.h"

//#define manager GraphicsManager::getInstance()
//#define resources GraphicsManager::getInstance().thesisData
#define gdevice m_graphicsManager->getDevice()
#define gdeviceContext m_graphicsManager->getDeviceContext()
class GraphicsManager;
class AntiAliasing
{
public:
	AntiAliasing();
	~AntiAliasing();
	void Render();
	void Initialize();

	void InitExample();

	void RenderSSAA();
	void RenderFXAA();
private:
	GraphicsManager* m_graphicsManager;
};


#endif