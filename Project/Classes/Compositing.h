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
	void Render(string shaderResource = "", string rendertarget = "");
	void Initialize();
	
private:
	GraphicsManager* m_graphicsManager;
	UINT m_startFrame = 150;
	UINT m_imageCount = 150;
	UINT m_imageSum = 0;
	string m_UVFrame;
	string m_UVReflectionFrame;
	string m_beautyFrame;
	string m_diffuseFrame;
	string m_specularFrame;
	string m_refDistortFrame;
	string m_irradianceFrame;
	string m_reflectionFrame;
	string m_indirectFrame;


	bool m_renderSequence = true;

	bool m_renderUV = true;
	bool m_renderUVReflection = true;
	bool m_renderBeauty = true;
	bool m_renderDiffuse = true;
	bool m_renderSpecular = true;
	bool m_renderReflectionDistorion = true;
	bool m_renderIrradiance = true;
	bool m_renderReflection = true;
	bool m_renderIndirect = true;
	

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

	//used to check what to do for current frames
	int m_compositingIndex = 0;

	//used to get current player
	int m_playerIndex = 0;

	//first array playerChangeFrame, second 1=ShowPlayer 0 = hide
	int m_playerChangeFrame[2][25] =
	{
		{
			170,
			180,
			190,
			200,
			210,
			220,
			230,
			240,
			250,
			260,
			270,
			280,
			130,
			140,
			150,
			160,
			170,
			180,
			190,
			200,
			210,
			220,
			230,
			240,
			100
		},

		{
			1,
			0,
			1,
			0,
			1,
			0,
			1,
			0,
			1,
			0,
			1,
			0,
			1,
			0,
			1,
			0,
			1,
			0,
			1,
			0,
			1,
			0,
			1,
			0,
			1
		}
	};
};

#endif