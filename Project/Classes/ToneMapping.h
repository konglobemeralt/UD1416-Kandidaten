#ifndef H_TONEMAPPING
#define H_TONEMAPPING

#include "GraphicsManager.h"
#include "../DirectXTex/DirectXTex.h"
#include "../DirectXTex/DirectXTexP.h"
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>

//#define manager GraphicsManager::getInstance()
//#define resources GraphicsManager::getInstance().thesisData
#define gdevice m_graphicsManager->getDevice()
#define gdeviceContext m_graphicsManager->getDeviceContext()
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
int a;
};

#endif