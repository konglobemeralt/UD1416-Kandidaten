cbuffer cBuffer1 : register(b0)
{
	float4x4 worldMatrix;
	float4x4 viewMatrix;
	float4x4 projMatrix;
	float4x4 viewMatrixInverse;
	float4x4 lightViewMatrix;
	float4x4 lightProjMatrix;
}

struct BufType{
	float x, y, z;
	float nx, ny, nz;
	float u, v;
	float lifespan, velocity;
};

RWStructuredBuffer<BufType> output : register(u0);

[numthreads(20, 1, 1)]
void PCS_main(uint3 DTid : SV_DispatchThreadID)
{
	output[DTid.x].y = output[DTid.x].y - output[DTid.x].velocity / 5 + (output[DTid.x].lifespan * step(output[DTid.x].y, 1.0f));
}