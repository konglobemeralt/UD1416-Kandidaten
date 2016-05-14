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
	void SetText(ID3D11ShaderResourceView* text[3]);
private:
	GraphicsManager* m_graphicsManager;
	UINT m_startFrame = 80;
	UINT m_imageCount = 80;
	UINT m_imageSum = 80;
	string m_UVFrame;
	string m_UVReflectionFrame;
	string m_beautyFrame;
	string m_diffuseFrame;
	string m_specularFrame;
	string m_refDistortFrame;
	string m_shadowFrame;
	string m_reflectionFrame;

	string m_textFrame1;
	string m_textFrame2;
	string m_textFrame3;
	
	bool m_renderUV					= true;
	bool m_renderUVReflection		= true;
	bool m_renderBeauty				= true;
	bool m_renderDiffuse			= true;
	bool m_renderSpecular			= true;
	bool m_renderReflectionDistorion= true;
	bool m_renderShadow				= true;
	bool m_renderReflection			= true;

	bool m_renderText				= true;

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

		//int m_text;

	} m_textureConstantBuffer;
	bool m_shotTaken = false;
	// Eriks test
	ID3D11Texture2D* texture;
};

#endif