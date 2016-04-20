Texture2D FirstSRV: register(t0);
Texture2D SecondSRV: register(t1);
sampler SamplerWrap;

struct VS_OUT {
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};

float4 PS_main(VS_OUT input) : SV_TARGET
{
	
	float4 tempUV = FirstSRV.Sample(SamplerWrap, input.Tex);

	if (!(tempUV.z > 0))
	{
		return SecondSRV.Sample(SamplerWrap, saturate(tempUV));
	}
	else
		return tempUV;
	
}