Texture2D UVSRV: register(t0);
Texture2D PlayerSRV: register(t1);
Texture2D BackgroundSRV: register(t2);
Texture2D UVReflectionSRV: register(t3);
Texture2D DiffuseSRV: register(t4);
Texture2D SpecularSRV: register(t5);
Texture2D IrradianceSRV: register(t6);
Texture2D ShadowSRV: register(t7);
Texture2D ReflectionSRV: register(t8);

//Text planes UV
Texture2D Text1SRV: register(t9);
Texture2D Text2SRV: register(t10);
Texture2D Text3SRV: register(t11);
//Text plane Text
Texture2D FirstNameSRV: register(t12);
Texture2D LastNameSRV: register(t13);
Texture2D NumberSRV: register(t14);

sampler SamplerWrap;

struct VS_OUT {
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};

float4 PS_main(VS_OUT input) : SV_TARGET
{
	//float4 outColor;
	//return float4(input.Tex, 0.0f, 1.0f);

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


	//Text planes
	float4 tempText1 = Text1SRV.Sample(SamplerWrap, input.Tex);
	float4 textyTest1 = FirstNameSRV.Sample(SamplerWrap, saturate(tempText1));

	float4 tempText2 = Text2SRV.Sample(SamplerWrap, input.Tex);
	float4 textyTest2 = LastNameSRV.Sample(SamplerWrap, saturate(tempText2));

	float4 tempText3 = Text3SRV.Sample(SamplerWrap, input.Tex);
	float4 textyTest3 = NumberSRV.Sample(SamplerWrap, saturate(tempText3));



	float4 playerColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 playerReflectionColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	
	//Text name plane1
	if (!(textyTest1.w < 0.5))
	{
		playerColor = textyTest1;
		float4 outColor;
		outColor = float4(pow(saturate(tempDiffuse + /*tempIrradiance +*/ tempSpecular + tempReflection + playerColor)*playerColor, 1.0 / 2.2));
		return outColor;
	}


	//Text name plane2
	if (!(textyTest2.w < 0.5))
	{
		playerColor = textyTest2;
		float4 outColor;
		outColor = float4(pow(saturate(tempDiffuse + /*tempIrradiance +*/ tempSpecular + tempReflection + playerColor)*playerColor, 1.0 / 2.2));
		return outColor;
	}

	//Text name plane3
	if (!(textyTest3.w < 0.5) && !(tempText3.z > 0))
	{
		playerColor = textyTest3;
		float4 outColor;
		outColor = float4(pow(saturate(tempDiffuse + /*tempIrradiance +*/ tempSpecular + tempReflection + playerColor)*playerColor, 1.0 / 2.2));
		return outColor;
	}

	//Player on puck
	if (!(tempPlayer.w < 0.5) && !(tempUV.z > 0))
	{
		playerColor = tempPlayer;
		float4 outColor;
		outColor = float4(pow((tempDiffuse + /*tempIrradiance +*/ tempSpecular * (tempDiffuse * (playerColor * 150.0f))), 1.0 / 2.2)) * 2;
		return outColor;
	}

	//Reflection on the is
	if (!(tempPlayerReflection.w < 0.5) && !(tempUVRef.z > 0))
	{
		playerReflectionColor = tempPlayerReflection;
		float4 outColor;
		outColor = float4(pow(saturate(tempDiffuse + /*tempIrradiance +*/ tempSpecular + (tempDiffuse * (playerReflectionColor * 1.0f))), 1.0 / 2.2));
		



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