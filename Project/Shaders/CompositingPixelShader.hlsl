Texture2D UVSRV: register(t0);
Texture2D PlayerSRV: register(t1);
Texture2D BackgroundSRV: register(t2);
Texture2D UVReflectionSRV: register(t3);
Texture2D DiffuseSRV: register(t4);
Texture2D SpecularSRV: register(t5);
Texture2D ReflectiveDistortSRV: register(t6);
Texture2D IrradianceSRV: register(t7);
Texture2D ReflectionSRV: register(t8);
Texture2D IndirectSRV: register(t9);
//Text planes UV
Texture2D Text1SRV: register(t10);
Texture2D Text2SRV: register(t11);
Texture2D Text3SRV: register(t12);
//Text plane Text
Texture2D FirstNameSRV: register(t13);
Texture2D LastNameSRV: register(t14);
Texture2D NumberSRV: register(t15);

sampler SamplerWrap;

//Blending functions
//Additive
float4 Additive(float4 p_base, float4 p_blend)
{
	return p_base + p_blend;
}
//Subtract
float4 Subtractive(float4 p_base, float4 p_blend)
{
	return p_base - p_blend;
}

//Multiplicative
float4 Multiply(float4 p_base, float4 p_blend, float p_multiplier = 1.0f)
{
	return (p_base * p_blend * p_multiplier);
}
//Screen
float4 Screen(float4 p_base, float4 p_blend)
{
	return(1 - (1 - p_base) * (1 - p_blend));
}
//Lighten
float4 Lighten(float4 p_base, float4 p_blend)
{
	float4 outputColor;
	outputColor.rgb = max(p_base.rgb, p_blend.rgb);
}
//Overlay
float4 Overlay(float4 p_base, float4 p_blend)
{
	float4 result;
	result = step(0.5, p_base);
	result = lerp((p_base*p_blend * 2), (1.0 - (2.0*(1.0 - p_base)*(1.0 - p_blend))), result);
	result.a = 1.0f;
	return result;
}
//Color Dodge
float4 ColorDodge(float4 p_base, float4 p_blend)
{
	return (p_base / (1 - p_blend));
}
//Color Burn
float4 ColorBurn(float4 p_base, float4 p_blend)
{
	return (1 - (1 - p_base) / p_blend);
}
//Vivid Light
float4 VividLight(float4 p_base, float4 p_blend)
{
	float4 result;
	if (p_blend.r > .5f)
		result.r = (1 - (1 - p_base.r) / (2 * (p_blend.r - .5f)));
	else
		result.r = p_base.r / (1 - 2 * p_blend.r);

	if (p_blend.g > .5f)
		result.g = (1 - (1 - p_base.g) / (2 * (p_blend.g - .5f)));
	else
		result.g = p_base.g / (1 - 2 * p_blend.g);

	if (p_blend.b > .5f)
		result.b = (1 - (1 - p_base.b) / (2 * (p_blend.b - .5f)));
	else
		result.b = p_base.b / (1 - 2 * p_blend.b);
	result.a = 1.0f;
	return result;
}



struct VS_OUT {
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};


SamplerState sampler_linear_clamp
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Clamp;
	AddressV = Clamp;
};




float4 PS_main(VS_OUT input) : SV_TARGET
{
	//float4 outColor;



	//float2x2 rotationMatrix = float2x2(0, 1, -1, 0);
	//SamplerWrap.SampleLevel(PlayerSRV, 0, 3);
	//UVSRV.SampleLevel(sampler_linear_clamp, 0, 0);
	//PlayerSRV.SampleLevel(sampler_linear_clamp, 0, 0);

	float4 tempBeauty = BackgroundSRV.Sample(SamplerWrap, input.Tex);
	float4 tempUV = UVSRV.Sample(SamplerWrap, input.Tex);
	float4 tempUVRef = UVReflectionSRV.Sample(SamplerWrap, input.Tex);
	float4 tempDiffuse = DiffuseSRV.Sample(SamplerWrap, input.Tex);
	float4 tempSpecular = SpecularSRV.Sample(SamplerWrap, input.Tex);
	float4 tempRefDistort = ReflectiveDistortSRV.Sample(SamplerWrap, input.Tex);
	float4 tempIrradiance = IrradianceSRV.Sample(SamplerWrap, input.Tex);
	float4 tempReflection = ReflectionSRV.Sample(SamplerWrap, input.Tex);
	float4 tempIndirect = IndirectSRV.Sample(SamplerWrap, input.Tex);
	float4 tempPlayer = PlayerSRV.Sample(SamplerWrap, tempUV);
	float4 tempPlayerReflection = PlayerSRV.Sample(SamplerWrap, tempUVRef);

	//return tempPlayer;
	//return tempUV;

	//Text planes
	float4 tempText1 = Text1SRV.Sample(SamplerWrap, input.Tex);
	float4 textyTest1 = FirstNameSRV.Sample(SamplerWrap, saturate(tempText1));


	float4 tempText2 = Text2SRV.Sample(SamplerWrap, input.Tex);
	float4 textyTest2 = LastNameSRV.Sample(SamplerWrap, saturate(tempText2));

	float4 tempText3 = Text3SRV.Sample(SamplerWrap, input.Tex);
	float4 textyTest3 = NumberSRV.Sample(SamplerWrap, saturate(tempText3));

	float4 playerColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 playerReflectionColor = float4(0.0f, 0.0f, 0.0f, 0.0f);



	//float4 backgroundColor = float4(tempDiffuse + tempSpecular + tempReflection * tempIrradiance + tempIndirect);
	//if (tempSpecular.b < .99)
	//	tempSpecular.b *= 0;
	//if (tempSpecular.g < .99)
	//	tempSpecular.g *= 0;

	float4 backgroundColor = tempIrradiance + tempSpecular;
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
		bool additive = false;
		//Blend modes
		//0. Replace
		//1. Additive
		//2. Multiplicative
		//3. Screen
		int blendMode = 1;
		float multiplier = 1.0f;
		//Player on puck
		if ((tempUV.w > 0.999) && tempPlayer.w != 0)
		{
			playerColor = tempPlayer;
			if (blendMode == 0)
			{
				return float4(pow(PlayerSRV.Sample(SamplerWrap, UVSRV.Sample(SamplerWrap, input.Tex)), 1 / 1.4));
			}
			if (blendMode == 1)
			{

				return float4(pow(saturate(Additive(backgroundColor, playerColor)), 1 / 1.5));

			}
			else if (blendMode == 2)
			{
				playerColor = tempPlayer;
				return float4(pow(saturate(Multiply(backgroundColor, playerColor, multiplier)), 1 / 2.2));
			}
			else if (blendMode == 3)
			{
				playerColor = tempPlayer;
				return float4(pow(saturate(Screen(backgroundColor, playerColor)), 1 / 2.2));
			}
			else if (blendMode == 4)
			{
				playerColor = tempPlayer;
				return float4(pow(saturate(Subtractive(backgroundColor, playerColor)), 1 / 2.2));
			}
			else if (blendMode == 5)
			{
				return float4(pow(saturate(Overlay(backgroundColor, playerColor)), 1 / 2.2));
			}
			else if (blendMode == 6)
			{
				return float4(pow(saturate(ColorDodge(backgroundColor, playerColor)), 1 / 2.2));
			}
			else if (blendMode == 7)
			{
				return float4(pow(saturate(ColorBurn(backgroundColor, playerColor)), 1 / 2.2));
			}
			else if (blendMode == 8)
			{
				return float4(pow(saturate(VividLight(backgroundColor, playerColor)), 1 / 2.2));
			}
			else if (blendMode == -1)
			{

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
		//float4 outColor = float4(pow(saturate((tempDiffuse*tempDiffuse.w) + (tempSpecular*tempSpecular.w) + (tempReflection*tempReflection.w) /*tempIrradiance*/), 1.0 / 2.2));
		//float4 outColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 outColor = float4(pow(saturate(backgroundColor), 1 / 2.2));
		return outColor;
	}

	else
	{
		return float4(pow((tempBeauty), 1.0 / 2.2));
	}



}