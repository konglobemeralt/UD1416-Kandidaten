struct VS_IN
{
	float3 Pos : POSITION;
	float2 Tex : TEXCOORD;
};

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};

cbuffer Matrix : register(b0)
{
	matrix rotationM;
}

VS_OUT VS_main(VS_IN input)
{
	VS_OUT output;
	output.Pos = mul(float4(input.Pos, 1.0f), rotationM);
	output.Tex = input.Tex;

	return output;
}