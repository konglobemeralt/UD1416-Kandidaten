struct VS_IN
{
	float3 Pos : POSITION : SV_POSITION;
	float2 Tex : TEXCOORD;
};

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};

cbuffer Matrices : register(b0)
{
	matrix view;
	matrix projection;
}

VS_OUT VS_main(VS_IN input)
{
	VS_OUT output;
	//output.Pos = float4(input.Pos, 1.0f);
	//output.Pos = mul(float4(input.Pos, 1.0f), mul(view, projection));
	output.Pos = mul(float4(input.Pos, 1.0f), view);
	output.Tex = input.Tex;

	return output;
}