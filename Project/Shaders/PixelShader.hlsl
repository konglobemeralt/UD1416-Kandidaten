struct VS_OUT {
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};

Texture2D tex : register(t0);
SamplerState sampAni : register(s0);

float4 PS_main(VS_OUT input) : SV_TARGET
{
	return tex.Sample(sampAni, input.Tex);
}