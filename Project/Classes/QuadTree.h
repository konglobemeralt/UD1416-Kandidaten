#ifndef QUADTREE_H
#define QUADTREE_H

#include "ObjImporter.h"
#include <DirectXMath.h>

using namespace DirectX;
using namespace std;

class QuadTree
{
private:
	struct boxStruct{
		int quadLevels;
		BoundingBox box;
		vector<ObjImporter*> quadObjects;
		QuadTree* children[4];
	};
	boxStruct quadBox;

	ID3D11Buffer* boxBuffer;
	
	XMFLOAT3 quadCenter;
	XMFLOAT3 quadSize;
	ID3D11Device* gDevice;

public:
	QuadTree(ID3D11Device* device, XMFLOAT3 center, XMFLOAT3 quadSize, int levels, vector<ID3D11Buffer*>& buffers, vector<ObjImporter*> objectList, vector<XMFLOAT3> objectMatrix);
	~QuadTree();

	void getContainedObjects(vector<ObjImporter*> objectList, vector<XMFLOAT3> objectMatrix);
	void ObjectRenderState(QuadTree* quad, BoundingFrustum* frustum);
	void CreateChildren(vector<ID3D11Buffer*>& buffers, vector<ObjImporter*> objectList, vector<XMFLOAT3> objectMatrix);
	void Clear(QuadTree* quad);
	void CreateBuffers();
};

#endif