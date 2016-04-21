Texture2D FirstSRV;
sampler SamplerWrap;

struct VS_OUT {
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};

float4 PS_main(VS_OUT input) : SV_TARGET
{
	float3 inColor = FirstSRV.Sample(SamplerWrap, input.Tex).rgb;
	float L = max(inColor.r, inColor.g);
	L = max(L, inColor.b);
	//float red = inColor.r / L;
	//float green = inColor.g / L;
	//float blue = inColor.b / L;
	//L = log(L);

	return float4(L, 0.0f, 0.0f, 1.0f);
}