#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H

#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>
#include <Windows.h>

#include "..\DirectXToolkit\Inc\WICTextureLoader.h"

using namespace DirectX;
using namespace std;

class HeightMap
{
private:
	struct vert{
		float x, y, z;
		float nx, ny, nz;
		float u, v;
		float blendU, blendV;
	};

	ID3D11Buffer* hmVertexBuffer;
	ID3D11Buffer* hmIndexBuffer;
	vector<ID3D11ShaderResourceView*> textureSRV;

	vector<vert> v;
	vector<DWORD> indices;

	wstring textureFile[4];

	int NumFaces;
	int NumVertices;

	int cols;
	int rows;

public:
	struct HeightMapInfo{		// Heightmap structure
		int terrainWidth;		// Width of heightmap
		int terrainHeight;		// Height (Length) of heightmap
		XMFLOAT3 *heightMap;	// Array to store terrain's vertex positions
	};
	HeightMapInfo hminfo;

	HeightMap(wstring textureFile1, wstring textureFile2, wstring textureFile3, wstring blendTexture, float terrainScale, float heightFactor);
	~HeightMap();

	int terrainScale;
	float heightFactor;

	void HeightMapLoad(char* filename);
	void CreateGrid();
	void CreateIndexList();
	void CalculateNormals();
	void SetupBuffers(ID3D11Device* gDevice);
	void DrawHM(ID3D11DeviceContext* gDeviceContext, ID3D11VertexShader* HMVertexShader, ID3D11PixelShader* HMPixelShader, ID3D11ShaderResourceView* lightDepthMap);
	float getTerrainHeight(float camX, float camZ);
};

#endif