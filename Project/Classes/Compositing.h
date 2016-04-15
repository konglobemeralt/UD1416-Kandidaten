#ifndef H_COMPOSITING
#define H_COMPOSITING

#include "GraphicsManager.h"

#define manager GraphicsManager::getInstance()
#define resources GraphicsManager::getInstance().thesisData
#define device GraphicsManager::getInstance().getDevice()
#define deviceContext GraphicsManager::getInstance().getDeviceContext()

void renderCompositing();
void initCompositing();

#endif