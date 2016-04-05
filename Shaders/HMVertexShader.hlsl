cbuffer cBuffer1 : register(b0)
{
	float4x4 worldMatrix;
	float4x4 viewMatrix;
	float4x4 projMatrix;
	float4x4 viewMatrixInverse;
	float4x4 lightViewMatrix;
	float4x4 lightProjMatrix;
}

struct HMVS_IN
{
	float3 Pos : POSITION;
	float3 Normal : NORMAL;
	float2 Tex : TEXCOORD;
	float2 BlendTex : TEXCOORD1;
};

struct HMVS_OUT
{
	float4 Pos : SV_POSITION;
	float4 Normal : NORMAL;
	float2 Tex : TEXCOORD;
	float2 BlendTex : TEXCOORD1;
	float4 Wpos : POSITION;
	float4 LightViewPos : POSITION1;
};

HMVS_OUT HMVS_main(HMVS_IN input)
{
	HMVS_OUT output = (HMVS_OUT)0;

	output.Pos = mul(float4(input.Pos, 1.0f), mul(worldMatrix, mul(viewMatrix, projMatrix)));
	output.Normal = mul(float4(input.Normal, 1.0f), worldMatrix);
	output.Tex = input.Tex;
	output.BlendTex = input.BlendTex;
	output.Wpos = mul(float4(input.Pos, 1.0f), worldMatrix);
	output.LightViewPos = mul(float4(input.Pos, 1.0f), mul(worldMatrix, mul(lightViewMatrix, lightProjMatrix)));

	return output;
}