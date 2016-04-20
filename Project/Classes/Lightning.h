#ifndef H_LIGHTNING
#define H_LIGHTNING



//#define manager GraphicsManager::getInstance()
//#define resources GraphicsManager::getInstance().thesisData
#define gdevice m_graphicsManager->getDevice()
#define gdeviceContext m_graphicsManager->getDeviceContext()

class GraphicsManager;
class Lightning
{
public:
	Lightning();
	~Lightning();
	void Render();
	void Initialize();
private:
	GraphicsManager* m_graphicsManager;
};

#endif