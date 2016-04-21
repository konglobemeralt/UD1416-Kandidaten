Texture2D UVSRV: register(t0);
Texture2D PlayerSRV: register(t1);
Texture2D BackgroundSRV: register(t2);
sampler SamplerWrap;

struct VS_OUT {
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};

float4 PS_main(VS_OUT input) : SV_TARGET
{
	
	float4 tempUV = UVSRV.Sample(SamplerWrap, input.Tex);
	float4 tempUV2 = PlayerSRV.Sample(SamplerWrap, saturate(tempUV));
	
	
	float4 tempUVBackground = BackgroundSRV.Sample(SamplerWrap, input.Tex);

	

	float4 playerAmbient = float4(1.1f, 1.1f, 0.6f, 1.0f);
	float4 funkyLight = float4(1.6f, 0.6f, 9.6f, 1.0f);


	if (!(tempUV2.w < 0.5) && !(tempUV.z > 0))
	{
		return (playerAmbient * PlayerSRV.Sample(SamplerWrap, saturate(tempUV)) + (funkyLight * (PlayerSRV.Sample(SamplerWrap, saturate(tempUV))) * tempUVBackground));
	}
	else
		return tempUVBackground;
	
}