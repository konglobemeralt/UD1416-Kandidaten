Texture2D myTexture : register(t0);

SamplerState samplerWrap : register(s0);

struct PVS_OUT
{
	float4 Pos : SV_POSITION;
	float4 Normal : NORMAL;
	float2 Tex : TEXCOORD;
};

float4 PPS_main(PVS_OUT input) : SV_TARGET
{

	float3 color = myTexture.Sample(samplerWrap, input.Tex).rgb;

	return float4(color, 1.0f);
}