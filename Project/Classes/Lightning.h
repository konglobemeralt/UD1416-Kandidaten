#ifndef H_LIGHTNING
#define H_LIGHTNING

#include "GraphicsManager.h"

#define manager GraphicsManager::getInstance()
#define resources GraphicsManager::getInstance().thesisData
#define device GraphicsManager::getInstance().getDevice()
#define deviceContext GraphicsManager::getInstance().getDeviceContext()

void renderLightning();
void initLightning();

#endif