#ifndef H_TEXT
#define H_TEXT

#include "GraphicsManager.h"

#define manager GraphicsManager::getInstance()
#define resources GraphicsManager::getInstance().thesisData
#define device GraphicsManager::getInstance().getDevice()
#define deviceContext GraphicsManager::getInstance().getDeviceContext()

void renderText();
void initText();

#endif