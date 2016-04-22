Texture2D FirstSRV;
sampler SamplerWrap;

struct VS_OUT {
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};

float4 PS_main(VS_OUT input) : SV_TARGET
{
	float3 inColor = FirstSRV.Sample(SamplerWrap, input.Tex).rgb;

	float avgL = (0.2126 * inColor.r) + (0.7152 * inColor.g) + (0.0722 * inColor.b);
	float maxL = max(inColor.r, inColor.g);
	maxL = max(maxL, inColor.b);
	float minL = min(inColor.r, inColor.g);
	minL = min(minL, inColor.b);
	
	//float red = inColor.r / L;
	//float green = inColor.g / L;
	//float blue = inColor.b / L;
	//L = log(L);

	//store avg in R and max in G

	return float4(avgL, maxL, minL, 1.0f);
}