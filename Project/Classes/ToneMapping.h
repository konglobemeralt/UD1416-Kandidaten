#ifndef H_TONEMAPPING
#define H_TONEMAPPING

#include "GraphicsManager.h"

#define manager GraphicsManager::getInstance()
#define resources GraphicsManager::getInstance().thesisData
#define device GraphicsManager::getInstance().getDevice()
#define deviceContext GraphicsManager::getInstance().getDeviceContext()

void renderToneMapping();
void initToneMapping();

#endif