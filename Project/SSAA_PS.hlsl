sampler sampWrap : register(s0);
sampler  sampClamp: register(s1);

Texture2D txDiffuse : register(t0);

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD0;
};


float4 PS_main(VS_OUT input) : SV_TARGET
{
	return float4(txDiffuse.Sample(sampClamp, input.Tex).xyz, 1.0f);
}
