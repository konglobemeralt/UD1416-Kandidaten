Texture2D UVSRV: register(t0);
Texture2D PlayerSRV: register(t1);
Texture2D BackgroundSRV: register(t2);
Texture2D UVReflectionSRV: register(t3);
Texture2D DiffuseSRV: register(t4);
Texture2D SpecularSRV: register(t5);
Texture2D IrradianceSRV: register(t6);
Texture2D ShadowSRV: register(t7);
Texture2D ReflectionSRV: register(t8);


sampler SamplerWrap;

struct VS_OUT {
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};

float4 PS_main(VS_OUT input) : SV_TARGET
{
	//float4 outColor;

	float4 tempBeauty = BackgroundSRV.Sample(SamplerWrap, input.Tex);
	float4 tempUV = UVSRV.Sample(SamplerWrap, input.Tex);
	float4 tempUVRef = UVReflectionSRV.Sample(SamplerWrap, input.Tex);
	float4 tempDiffuse = DiffuseSRV.Sample(SamplerWrap, input.Tex);
	float4 tempSpecular = SpecularSRV.Sample(SamplerWrap, input.Tex);
	float4 tempIrradiance = IrradianceSRV.Sample(SamplerWrap, input.Tex);
	float4 tempShadow = ShadowSRV.Sample(SamplerWrap, input.Tex);
	float4 tempReflection = ReflectionSRV.Sample(SamplerWrap, input.Tex);

	float4 tempPlayer = PlayerSRV.Sample(SamplerWrap, saturate(tempUV));
	float4 tempPlayerReflection = PlayerSRV.Sample(SamplerWrap, saturate(tempUVRef));

	
	float4 playerColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 playerReflectionColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	if (!(tempPlayer.w < 0.5) && !(tempUV.z > 0))
	{
		playerColor = tempPlayer;
		float4 outColor;
		outColor = float4(pow(saturate(tempDiffuse + /*tempIrradiance +*/ tempSpecular + tempReflection + playerColor)*playerColor, 1.0 / 2.2));
		return outColor;
	}
	if (!(tempPlayerReflection.w < 0.5) && !(tempUVRef.z > 0))
	{
		playerReflectionColor = tempPlayerReflection;
		float4 outColor;
		outColor = float4(pow(saturate(tempDiffuse + /*tempIrradiance +*/ tempSpecular + tempReflection + playerColor)* tempPlayerReflection, 1.0 / 2.2));
		
		return outColor;
	}

	//return float4(0.0f, 0.0f, 0.0f, 0.0f);
	//Background
	float4 outColor = float4(pow(saturate(tempDiffuse + /*tempIrradiance +*/ tempSpecular + tempReflection), 1.0 /2.2));
	return outColor;
	/*float4 outColor;
	outColor = tempPlayerReflection + playerColor;
	return outColor;*/
	/*float4 tempUV = UVSRV.Sample(SamplerWrap, input.Tex);
	float4 tempUV2 = PlayerSRV.Sample(SamplerWrap, saturate(tempUV));
	
	
	float4 tempUVBackground = BackgroundSRV.Sample(SamplerWrap, input.Tex);

	

	float4 playerAmbient = float4(1.1f, 1.1f, 0.7f, 1.0f);
	float4 funkyLight = float4(1.6f, 1.6f, 0.6f, 1.0f);


	if (!(tempUV2.w < 0.5) && !(tempUV.z > 0))
	{
		return (playerAmbient * PlayerSRV.Sample(SamplerWrap, saturate(tempUV)) + (funkyLight * (PlayerSRV.Sample(SamplerWrap, saturate(tempUV))) * tempUVBackground));
	}
	else
		return tempUVBackground;*/
	
}