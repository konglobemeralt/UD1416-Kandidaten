struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};

Texture2D text : register(t0);
SamplerState Linear : register(s0);

float4 PS_main(VS_OUT input) : SV_TARGET
{
	return text.Sample(Linear, input.Tex);
}