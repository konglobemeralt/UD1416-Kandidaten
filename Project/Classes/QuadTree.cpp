#include "QuadTree.h"

QuadTree::QuadTree(ID3D11Device* device, XMFLOAT3 center, XMFLOAT3 extents, int levels, vector<ID3D11Buffer*>& buffers, vector<ObjImporter*> objectList, vector<XMFLOAT3> objectMatrix){
	gDevice = device;
	quadCenter = center;
	quadSize.x = extents.x; // Width
	quadSize.y = extents.y; // Height
	quadSize.z = extents.z; // Depth

	quadBox.quadLevels = levels;
	quadBox.box.Center = quadCenter;
	quadBox.box.Extents = quadSize;

	CreateBuffers();
	buffers.push_back(boxBuffer);
	
	if (quadBox.quadLevels > 0)
		CreateChildren(buffers, objectList, objectMatrix);
	else
		getContainedObjects(objectList, objectMatrix);
}

void QuadTree::CreateChildren(vector<ID3D11Buffer*>& buffers, vector<ObjImporter*> objectList, vector<XMFLOAT3> objectMatrix){
	quadBox.children[0] = new QuadTree(gDevice, XMFLOAT3(quadCenter.x - (quadSize.x / 2.0f), quadCenter.y, quadCenter.z + (quadSize.z / 2.0f)), XMFLOAT3(quadSize.x / 2.0f, quadSize.y, quadSize.z / 2.0f), quadBox.quadLevels - 1, buffers, objectList, objectMatrix);
	quadBox.children[1] = new QuadTree(gDevice, XMFLOAT3(quadCenter.x + (quadSize.x / 2.0f), quadCenter.y, quadCenter.z + (quadSize.z / 2.0f)), XMFLOAT3(quadSize.x / 2.0f, quadSize.y, quadSize.z / 2.0f), quadBox.quadLevels - 1, buffers, objectList, objectMatrix);
	quadBox.children[2] = new QuadTree(gDevice, XMFLOAT3(quadCenter.x - (quadSize.x / 2.0f), quadCenter.y, quadCenter.z - (quadSize.z / 2.0f)), XMFLOAT3(quadSize.x / 2.0f, quadSize.y, quadSize.z / 2.0f), quadBox.quadLevels - 1, buffers, objectList, objectMatrix);
	quadBox.children[3] = new QuadTree(gDevice, XMFLOAT3(quadCenter.x + (quadSize.x / 2.0f), quadCenter.y, quadCenter.z - (quadSize.z / 2.0f)), XMFLOAT3(quadSize.x / 2.0f, quadSize.y, quadSize.z / 2.0f), quadBox.quadLevels - 1, buffers, objectList, objectMatrix);
}

void QuadTree::ObjectRenderState(QuadTree* quad, BoundingFrustum* frustum){
	if (quad->quadBox.quadLevels == 0){
		if (frustum->Intersects(quad->quadBox.box) || frustum->Contains(quad->quadBox.box)){
			for (int i = 0; i < quad->quadBox.quadObjects.size(); i++){
				quad->quadBox.quadObjects[i]->box.isRendered = true;
			}
		}
	}
	else{
		for (int i = 0; i < 4; i++){
			quad->quadBox.children[i]->ObjectRenderState(quad->quadBox.children[i], frustum);
		}
	}
}

void QuadTree::getContainedObjects(vector<ObjImporter*> objectList, vector<XMFLOAT3> objectMatrix){
	for (int i = 0; i < objectList.size(); i++){
		BoundingBox temp(objectList[i]->box.bBox);
		temp.Transform(temp, XMMatrixTranslation(objectMatrix[i].x, objectMatrix[i].y, objectMatrix[i].z));

		if (quadBox.box.Intersects(temp) || quadBox.box.Contains(temp)){
			quadBox.quadObjects.push_back(objectList[i]);
		}
	}
}

void QuadTree::CreateBuffers(){
	XMFLOAT3 box[8];
	quadBox.box.GetCorners(box);

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(XMFLOAT3)* 8;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA boxData;
	boxData.pSysMem = box;
	boxData.SysMemPitch = 0;
	boxData.SysMemSlicePitch = 0;

	gDevice->CreateBuffer(&bufferDesc, &boxData, &boxBuffer);
}

void QuadTree::Clear(QuadTree* quad){
	for (unsigned int i = 0; i < 4; i++){
		if (quad->quadBox.quadLevels > 1)
			quad->quadBox.children[i]->Clear(quad->quadBox.children[i]);
		delete quad->quadBox.children[i];
	}
}
QuadTree::~QuadTree(){

}