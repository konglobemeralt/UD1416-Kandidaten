//cbuffer cBuffer1 : register(b0)
//{
//	float4x4 worldMatrix;
//	float4x4 viewMatrix;
//	float4x4 projMatrix;
//	float4x4 viewMatrixInverse;
//	float4x4 lightViewMatrix;
//	float4x4 lightProjMatrix;
//}


struct VS_IN
{
	float3 Pos : POSITION;
	//float3 Normal : NORMAL;
	float2 Tex : TEXCOORD;
};

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	//float4 Normal : NORMAL;
	float2 Tex : TEXCOORD;
	//float4 Wpos : POSITION;
};

VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	output.Pos = float4(input.Pos, 1.0f);
	//output.Normal = mul(float4(input.Normal, 0.0f), worldMatrix);
	output.Tex = input.Tex;
	//output.Wpos = mul(output.Pos, worldMatrix);

	return output;
}