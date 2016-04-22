Texture2D FirstSRV;
sampler SamplerWrap;

cbuffer mipLevel {
	int4 levels;
};

struct VS_OUT {
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};

float4 PS_main(VS_OUT input) : SV_TARGET
{
	return FirstSRV.SampleLevel(SamplerWrap, input.Tex, levels.x);
}