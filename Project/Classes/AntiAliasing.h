#ifndef H_ANTIALIASING
#define H_ANTIALIASING

#include "GraphicsManager.h"

#define manager GraphicsManager::getInstance()
#define resources GraphicsManager::getInstance().thesisData
#define device GraphicsManager::getInstance().getDevice()
#define deviceContext GraphicsManager::getInstance().getDeviceContext()

void renderAntiAliasing();
void initAntiAliasing();

void initExample();

#endif