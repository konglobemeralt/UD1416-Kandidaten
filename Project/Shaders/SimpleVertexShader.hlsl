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
	output.Pos = float4(input.Pos.xyz, 1.0f);
	output.Tex = input.Tex;

	return output;
}