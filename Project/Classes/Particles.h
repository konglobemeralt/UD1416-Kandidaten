#ifndef PARTICLES_H
#define PARTICLES_H

#include <ctime>
#include <cstdlib>
#include <vector>
#include <d3d11.h>
#include <Windows.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#include "..\DirectXToolkit\Inc\WICTextureLoader.h"

#pragma comment (lib, "d3dcompiler.lib")

using namespace std;
using namespace DirectX;

class Particles{
private:
	struct vert{
		float x, y, z;
		float nx, ny, nz;
		float u, v;
		float lifespan, velocity;
	};

	ID3D11Device* gDevice;
	ID3D11DeviceContext* gDeviceContext;

	ID3D11ComputeShader* gPComputeShader;
	ID3D11VertexShader* gPVertexShader;
	ID3D11GeometryShader* gPGeometryShader;
	ID3D11PixelShader* gPPixelShader;
	ID3D11InputLayout* gPVertexLayout;

	ID3D11Buffer* gPVertexBuffer;
	ID3D11Buffer* gPUAVBuffer;
	ID3D11UnorderedAccessView* gPUAV;
	ID3D11ShaderResourceView* particleTextureSRV;

	vert vertex;
	vector<vert> vertexArray;

public:
	Particles(ID3D11Device* gDevice, ID3D11DeviceContext* gDeviceContext, int particleCount, XMFLOAT2 particleSize, XMFLOAT3 particleBox, XMFLOAT3 boxPosition);
	~Particles();

	int particleCount;
	XMFLOAT2 particleSize;
	XMFLOAT3 particleBox;
	XMFLOAT3 boxPosition;

	void InitParticles();
	void CreateParticleShaders();
	void CreateParticleBuffer();
	void DrawParticles();
};

#endif