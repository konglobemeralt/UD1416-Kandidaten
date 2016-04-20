#ifndef H_ANTIALIASING
#define H_ANTIALIASING

#include "GraphicsManager.h"
#include "../DirectXTex/DirectXTex.h"
#include "../DirectXTex/DirectXTexP.h"

#define manager GraphicsManager::getInstance()
#define resources GraphicsManager::getInstance().thesisData
#define device GraphicsManager::getInstance().getDevice()
#define deviceContext GraphicsManager::getInstance().getDeviceContext()

class AntiAliasing {
private:

public:
	AntiAliasing();
	~AntiAliasing();

	void Render();
	void Initialize();

	void InitExample();

	void RenderSSAA();
	void RenderFXAA();
};

#endif