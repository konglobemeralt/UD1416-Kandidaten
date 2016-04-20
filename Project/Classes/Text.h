#ifndef H_TEXT
#define H_TEXT

//#include "GraphicsManager.h"

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


private:
	GraphicsManager* m_graphicsManager;
};

#endif