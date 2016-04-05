#ifndef H_CONTENTMANAGER
#define H_CONTENTMANAGER

#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>

#include "DataStructures.h"

using namespace DirectX;
using namespace std;

class ContentManager {
private:
	ID3D11Device* gDevice;
	ID3D11DeviceContext* gDeviceContext;

	vector<StaticObject> staticObjects;
	StaticObject CreateStaticObject();

public:
	ContentManager(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	~ContentManager();

	void LoadContent();
	vector<StaticObject>* getStaticObjects();

	IndexedVertexBuffer CreateIVBuffer(vector<Vertex> vertices, vector<UINT> indices);
};

#endif