#ifndef H_COMPOSITING
#define H_COMPOSITING

#include "GraphicsManager.h"
#include "../DirectXTex/DirectXTex.h"
#include "../DirectXTex/DirectXTexP.h"

#define manager m_graphicsManager
#define resources m_graphicsManager->thesisData
#define gdevice m_graphicsManager->getDevice()
#define gdeviceContext m_graphicsManager->getDeviceContext()

class GraphicsManager;
class Compositing
{
public:
	Compositing();
	~Compositing();
	void Render();
	void Initialize();
private:
	GraphicsManager* m_graphicsManager;
	UINT m_startFrame = 2;
	UINT m_imageCount = 2;
	UINT m_imageSum = 120;
	string m_UVFrame;
	string m_UVReflectionFrame;
	string m_beautyFrame;
	string m_diffuseFrame;
	string m_specularFrame;
	string m_irradianceFrame;
	string m_shadowFrame;
	string m_reflectionFrame;
	
	bool m_renderUV					= true;
	bool m_renderUVReflection		= true;
	bool m_renderBeauty				= true;
	bool m_renderDiffuse			= true;
	bool m_renderSpecular			= true;
	bool m_renderIrradiance			= true;
	bool m_renderShadow				= true;
	bool m_renderReflection			= true;


	struct EnabledTextures
	{
		int m_UV;
		int m_UVRef;
		int m_beauty;
		int m_diffuse;
		int m_specular;
		int m_irradiance;
		int m_shadow;
		int m_reflection;
	} m_textureConstantBuffer;
};

#endif