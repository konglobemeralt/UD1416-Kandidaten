cbuffer cBuffer1 : register(b0)
{
	float4x4 worldMatrix;
	float4x4 viewMatrix;
	float4x4 projMatrix;
	float4x4 viewMatrixInverse;
	float4x4 lightViewMatrix;
	float4x4 lightProjMatrix;
}

struct BVS_IN
{
	float3 Pos : POSITION;
};

struct BVS_OUT
{
	float4 Pos : SV_POSITION;
};

BVS_OUT BVS_main(BVS_IN input)
{
	BVS_OUT output;

	output.Pos = mul(float4(input.Pos, 1.0f), mul(viewMatrix, projMatrix));

	return output;
}