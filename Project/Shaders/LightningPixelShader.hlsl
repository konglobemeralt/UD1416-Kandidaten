

struct VS_OUT {
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};

float4 PS_main(VS_OUT input) : SV_TARGET
{
	return float4(1.0f, 0.0f, 1.0f, 1.0f);
}