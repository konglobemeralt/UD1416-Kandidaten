Texture2D UVSRV: register(t0);
Texture2D PlayerSRV: register(t1);
Texture2D BackgroundSRV: register(t2);
Texture2D UVReflectionSRV: register(t3);
Texture2D DiffuseSRV: register(t4);
Texture2D SpecularSRV: register(t5);
Texture2D ReflectiveDistortSRV: register(t6);
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

	float2x2 rotationMatrix = float2x2(0, 1, -1, 0);

	float4 tempBeauty = BackgroundSRV.Sample(SamplerWrap, input.Tex);
	float4 tempUV = UVSRV.Sample(SamplerWrap, input.Tex);
	float4 tempUVRef = UVReflectionSRV.Sample(SamplerWrap, input.Tex);
	float4 tempDiffuse = DiffuseSRV.Sample(SamplerWrap, input.Tex);
	float4 tempSpecular = SpecularSRV.Sample(SamplerWrap, input.Tex);
	float4 tempRefDistort = ReflectiveDistortSRV.Sample(SamplerWrap, input.Tex);
	float4 tempShadow = ShadowSRV.Sample(SamplerWrap, input.Tex);
	float4 tempReflection = ReflectionSRV.Sample(SamplerWrap, input.Tex);

	float4 tempPlayer = PlayerSRV.Sample(SamplerWrap, tempUV);
	float4 tempPlayerReflection = PlayerSRV.Sample(SamplerWrap, tempUVRef);
	//return tempPlayer;

	//Text planes
	float4 tempText1 = Text1SRV.Sample(SamplerWrap, input.Tex);
	float4 textyTest1 = FirstNameSRV.Sample(SamplerWrap, saturate(tempText1));

	float4 tempText2 = Text2SRV.Sample(SamplerWrap, input.Tex);
	float4 textyTest2 = LastNameSRV.Sample(SamplerWrap, saturate(tempText2));

	float4 tempText3 = Text3SRV.Sample(SamplerWrap, input.Tex);
	float4 textyTest3 = NumberSRV.Sample(SamplerWrap, saturate(tempText3));



	float4 playerColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 playerReflectionColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	
	bool singleImage = false;
	//Text name plane1
	if (!singleImage)
	{
		if ((tempText1.w > 0.99) && textyTest1.w != 0)
		{
			playerColor = textyTest1;
			float4 outColor;
			outColor = float4(pow(saturate(tempDiffuse + tempSpecular + tempReflection + playerColor)*playerColor, 1.0 / 2.2));
			return outColor;
		}


		//Text name plane2
		if ((tempText2.w > 0.99) && textyTest2.w != 0)
		{
			playerColor = textyTest2;
			float4 outColor;
			outColor = float4(pow(saturate(tempDiffuse + tempSpecular + tempReflection + playerColor)*playerColor, 1.0 / 2.2));
			return outColor;
		}

		//Text name plane3
		if ((tempText3.w > 0.99) && textyTest3.w != 0)
		{
			playerColor = textyTest3;
			float4 outColor;
			outColor = float4(pow(saturate(tempDiffuse + tempSpecular + tempReflection + playerColor)*playerColor, 1.0 / 2.2));
			return outColor;
		}
		bool additive = true;
		//Player on puck
		if ((tempUV.w > 0.999) && tempPlayer.w != 0)
		{
			float multiplier = 10.0f;
			if (additive)
			{
				playerColor = tempPlayer;
				float4 outColor;
				outColor = float4(pow(saturate((tempDiffuse*tempDiffuse.w) + (tempSpecular*tempSpecular.w)  + tempReflection + (playerColor)), 1.0 / 2.2));
				return outColor;
			}

			else
			{
				playerColor = tempPlayer;
				float4 outColor;
				outColor = float4(pow(saturate(tempDiffuse + tempSpecular * (playerColor * multiplier)), 1.0 / 2.2));
				return outColor;
			}
		}

		//Reflection on the is
		if (!(tempPlayerReflection.w < 0.9) && tempUVRef.w > 0.999f)
		{
			playerReflectionColor = tempPlayerReflection;
			playerReflectionColor = (tempDiffuse + playerReflectionColor * (tempRefDistort.x));
			float4 outColor;
			outColor = float4(pow(saturate(tempDiffuse + tempSpecular + tempReflection + playerReflectionColor), 1.0 / 2.2));
			//outColor = float4(pow(saturate(tempDiffuse + tempSpecular + tempReflection * (tempDiffuse * (playerReflectionColor * (tempIrradiance*-1.0f)))), 1.0 / 2.2));
			return outColor;
		}
		float4 outColor = float4(pow(saturate((tempDiffuse*tempDiffuse.w) + (tempSpecular*tempSpecular.w) + (tempReflection*tempReflection.w) /*tempIrradiance*/), 1.0 / 2.2));
		//float4 outColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
		return outColor;
	}
	
	else
	{
		return float4(pow((tempBeauty), 1.0/2.2));
	}





	//return float4(0.0f, 0.0f, 0.0f, 0.0f);
	//Background
	
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