#ifndef OBJIMPORTER_H
#define OBJIMPORTER_H

#include <fstream>
#include <istream>
#include <sstream>
#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>

using namespace std;
using namespace DirectX;

struct vertexData{
	float x, y, z;
	float nx, ny, nz;
	float u, v;
};

struct objectBB{
	bool isRendered;
	BoundingBox bBox;
};

class ObjImporter
{
private:
	wstring objFile;
	wstring blendTexture;

	int meshTriangles;
	vector<XMFLOAT3> vertexPos; // Vertex coordinates
	vector<XMFLOAT3> vertexNormal; // Vertex normal vector
	vector<XMFLOAT2> textureCoord; // Vertex texture coordinates
	XMFLOAT3 bboxPointsMax;
	XMFLOAT3 bboxPointsMin;

	vector<vertexData> vertex;
	vertexData tempVertex;
	
public:
	bool blend;
	wstring textureFile;
	XMFLOAT4 specularColor_Power; //Ks (Specular Color)

	int totalVertices = 0;
	objectBB box;

	ObjImporter(wstring oFile, bool blendState);
	~ObjImporter();

	void ObjLoad();
	wstring ObjGetFileName();
	vector<vertexData>* GetData();
	size_t GetSize();
	void CleanUp();
};


#endif