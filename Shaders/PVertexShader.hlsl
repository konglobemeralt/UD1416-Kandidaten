cbuffer cBuffer1 : register(b0)
{
	float4x4 worldMatrix;
	float4x4 viewMatrix;
	float4x4 projMatrix;
	float4x4 viewMatrixInverse;
	float4x4 lightViewMatrix;
	float4x4 lightProjMatrix;
}

struct PVS_IN
{
	float3 Pos : POSITION;
	float3 Normal : NORMAL;
	float2 Tex : TEXCOORD;
	float2 LifeVelo : TEXCOORD1;
};

struct PVS_OUT
{
	float4 Pos : SV_POSITION;
	float4 Normal : NORMAL;
	float2 Tex : TEXCOORD;
};

PVS_OUT PVS_main(PVS_IN input)
{
	PVS_OUT output = (PVS_OUT)0;

	output.Pos = float4(input.Pos, 1.0f);
	output.Normal = float4(input.Normal, 0.0f);
	output.Tex = input.Tex;

	return output;
}