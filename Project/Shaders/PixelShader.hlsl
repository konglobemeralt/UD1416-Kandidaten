struct VS_OUT {
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};

Texture2D txDiffuse : register(t0);
SamplerState sampAni;

float4 PS_main(VS_OUT input) : SV_TARGET
{
	//return float4(input.Tex.x, input.Tex.y, 0.0f, 1.0f);
	return txDiffuse.Sample(sampAni, input.Tex);
}