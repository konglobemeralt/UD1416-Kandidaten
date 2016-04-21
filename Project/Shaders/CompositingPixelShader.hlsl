Texture2D FirstSRV: register(t0);
Texture2D SecondSRV: register(t1);
Texture2D BackgroundSRV: register(t2);
sampler SamplerWrap;

struct VS_OUT {
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};

float4 PS_main(VS_OUT input) : SV_TARGET
{
	
	float4 tempUV = FirstSRV.Sample(SamplerWrap, input.Tex);
	float4 tempUV2 = SecondSRV.Sample(SamplerWrap, saturate(tempUV));
	
	
	float4 tempUVBackground = BackgroundSRV.Sample(SamplerWrap, input.Tex);


	if (!(tempUV2.w == 0) && !(tempUV.z > 0))
	{

		return SecondSRV.Sample(SamplerWrap, saturate(tempUV));
	}
	else
		return tempUVBackground;
	
}