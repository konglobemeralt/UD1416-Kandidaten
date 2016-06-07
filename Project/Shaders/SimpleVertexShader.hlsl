cbuffer FXAACBuffer : register(b1)
{
	float4x4 world;
	float4x4 view;
	float4x4 projection;
	float4x4 WVP;
};

struct VS_IN {
	float3 Pos : SV_POSITION;
	float2 Tex : TEXCOORD0;
};

struct VS_OUT {
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD0;
};

VS_OUT VS_main(VS_IN input)
{
	VS_OUT output;
	output.Pos = mul(mul(mul(float4(input.Pos.xyz, 1.0f), world), view), projection);
	output.Tex = input.Tex;

	return output;	
}