struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};

struct Corners
{
	float2 leftup;
	float2 leftdown;
	float2 rightup;
	float2 rightdown;
};

RWStructuredBuffer<Corners> output;
SamplerState Linear : register(s0);
SamplerState Point : register(s1);

float4 PS_main(VS_OUT input) : SV_TARGET
{
	//return float4(output.leftup.x, output.leftup.x, output.leftup.x, output.leftup.x);
	return float4(0.0f, 0.0f, 0.0f, 0.0f);
}