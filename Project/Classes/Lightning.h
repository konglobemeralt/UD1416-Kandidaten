#ifndef H_LIGHTNING
#define H_LIGHTNING

#include "GraphicsManager.h"
#include "../DirectXTex/DirectXTex.h"
#include "../DirectXTex/DirectXTexP.h"

#define manager GraphicsManager::getInstance()
#define resources GraphicsManager::getInstance().thesisData
#define device GraphicsManager::getInstance().getDevice()
#define deviceContext GraphicsManager::getInstance().getDeviceContext()

class Lightning {
private:

public:
	Lightning();
	~Lightning();

	void Render();
	void Initialize();
};

#endif