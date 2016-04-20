#ifndef H_TONEMAPPING
#define H_TONEMAPPING

#include "GraphicsManager.h"
#include "../DirectXTex/DirectXTex.h"
#include "../DirectXTex/DirectXTexP.h"

#define manager GraphicsManager::getInstance()
#define resources GraphicsManager::getInstance().thesisData
#define device GraphicsManager::getInstance().getDevice()
#define deviceContext GraphicsManager::getInstance().getDeviceContext()

class ToneMapping {
private:

public:
	ToneMapping();
	~ToneMapping();

	void Render();
	void Initialize();
};

#endif