cbuffer cBuffer1 : register(b0)
{
	float4x4 worldMatrix;
	float4x4 viewMatrix;
	float4x4 projMatrix;
	float4x4 viewMatrixInverse;
	float4x4 lightViewMatrix;
	float4x4 lightProjMatrix;
}

struct SMVS_IN
{
	float3 Pos : POSITION;
	float3 Normal : NORMAL;
	float2 Tex : TEXCOORD;
};

struct SMVS_OUT
{
	float4 Pos : SV_POSITION;
	float3 Normal : NORMAL;
	float2 Tex : TEXCOORD;
};

SMVS_OUT SMVS_main(SMVS_IN input)
{
	SMVS_OUT output = (SMVS_OUT)0;

	output.Pos = mul(float4(input.Pos, 1.0f), mul(worldMatrix, mul(lightViewMatrix, lightProjMatrix)));
	output.Normal = input.Normal;
	output.Tex = input.Tex;

	return output;
}