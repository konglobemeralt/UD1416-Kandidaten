sampler sampWrap : register(s0);
sampler  sampClamp: register(s1);

Texture2D txDiffuse : register(t0);

cbuffer FXAACBuffer : register(b0)
{
	float2 texelSizeXY;
	float FXAA_blur_Texels_Threshhold; //hur m�nga texlar som kommer blurras �t varje h�ll
	float minimumBlurThreshhold; //hur mycket som kr�vs f�r att den ens ska blurra
	float FXAA_reduce_MULTIPLIER;
	float FXAA_reduce_MIN; //s� dirOffset inte ska bli noll
	float2 pad;
};

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD0;
};


float4 PS_main(VS_OUT input) : SV_TARGET
{

	//return float4(txDiffuse.Sample(sampClamp, input.Tex).xyz, 1.0f);
	float3 luma = float3(0.299f, 0.587f, 0.114f); //hur ljust n�nting �r kinda

	float lumaTR = dot(luma, txDiffuse.Sample(sampClamp, input.Tex + float2(1.0f, -1.0f) * texelSizeXY).xyz);
	float lumaTL = dot(luma, txDiffuse.Sample(sampClamp, input.Tex + float2(-1.0f, -1.0f) * texelSizeXY).xyz);
	float lumaBR = dot(luma, txDiffuse.Sample(sampClamp, input.Tex + float2(1.0f, 1.0f) * texelSizeXY).xyz);
	float lumaBL = dot(luma, txDiffuse.Sample(sampClamp, input.Tex + float2(-1.0f, 1.0f) * texelSizeXY).xyz);
	float lumaM = dot(luma, txDiffuse.Sample(sampClamp, input.Tex.xy).xyz);

	float2 dir;

	/*float tempAverage = (lumaTR + lumaTL + lumaBR + lumaBL) * 0.2f;
	return float4(tempAverage, tempAverage, tempAverage, 1.0f);*/

	//inverta med - f�r att v �r sett fr�n motsatta h�llet
	//dir.x tex kommer antingen vara 0 = ingen edge, �ver 0 s� ska det blurras �t positivt u, under 0 s� ska det blurras �t negativt u 
	//eller s� s�ger den bara att det beh�ver blurras i u �t b�da h�lle
	dir.x = -(lumaTR + lumaTL) - (lumaBR + lumaBL); //ifall denna �r st�rre �n 0 s� betyder det att de beh�ver blurras f�r d� ser de olika ut //+????
	dir.y = (lumaTR + lumaBR) - (lumaTL + lumaBL);

	float dirOffset = (max((lumaTR + lumaTL + lumaBR + lumaBL) * 0.25f * FXAA_reduce_MULTIPLIER, FXAA_reduce_MIN)); //average v�rde av luminocityn och sedan offset, men den ska minst vara ett visst v�rde f�r att ej bli 0
	//return float4(-dir.x, dir.y, 0, 1);
																													//vi har riktningen, men hur m�nga texels ska blurras?
																													//g�r om s� att det minsta v�rdet �r 1 texel l�ng
	float temp = 1.0f / min(abs(dir.x), abs(dir.y) + dirOffset); //dirOffset s� att den inte ska dela p� 0, dvs n�r det inte beh�ver blurras. Men ox� f�r att ha offset v�rde
	dir = dir * temp;

	//begr�nsa antalet texlar den kommer blurra
	dir.x = clamp(dir.x, -FXAA_blur_Texels_Threshhold, FXAA_blur_Texels_Threshhold);
	dir.y = clamp(dir.y, -FXAA_blur_Texels_Threshhold, FXAA_blur_Texels_Threshhold);

	dir *= texelSizeXY;

	float maxLuma = max(lumaM, max(max(lumaBR, lumaBL), max(lumaTR, lumaTL)));
	float minLuma = min(lumaM, min(min(lumaBR, lumaBL), min(lumaTR, lumaTL)));
	//blurr!!

	float3 res1 = 0.5f *  (txDiffuse.Sample(sampClamp, input.Tex + (dir * float2(1.0f / 3.0f - 0.5f, 1.0f / 3.0f - 0.5f))) +
		txDiffuse.Sample(sampClamp, input.Tex + (dir * float2(2.0f / 3.0f - 0.5f, 2.0f / 3.0f - 0.5f))));

	float3 res2 = res1 * 0.5f + 0.25f * (txDiffuse.Sample(sampClamp, input.Tex + (dir * float2(0.0f / 3.0f - 0.5f, 0.0f / 3.0f - 0.5f))) +
		txDiffuse.Sample(sampClamp, input.Tex + (dir * float2(3.0f / 3.0f - 0.5f, 3.0f / 3.0f - 0.5f))));


	float lumaRes2 = dot(luma, res2);

	if (lumaRes2 < minLuma || lumaRes2 > maxLuma) //check if sampled to far
	{
		return float4(res1.xyz, 1.0f);
	}


	return float4(res2.xyz, 1.0f);


	
	//meck
	float3 finalColor = float3(0,0,0);
	float3 blurredColor = float3(0, 0, 0);
	bool modified = false;
	//if (dir.x < -0.1f) //gaussian blur!
	//{
	//	blurredColor += (txDiffuse.Sample(sampClamp, input.Tex + texelSizeXY.y * 2) + //texelSizeXY.y???
	//	txDiffuse.Sample(sampClamp, input.Tex - texelSizeXY.y * 2) +
	//	txDiffuse.Sample(sampClamp, input.Tex).xyz) * 0.333f;
	//	modified = true;
	//}
	//if (dir.y < 0.1f) 
	//{
	//	blurredColor += (txDiffuse.Sample(sampClamp, input.Tex + texelSizeXY.x * 2) +
	//		txDiffuse.Sample(sampClamp, input.Tex - texelSizeXY.x * 2) +
	//		txDiffuse.Sample(sampClamp, input.Tex).xyz) * 0.333f;
	//		modified = true;
	//}
	//
	//if (modified) 
	//{
	//	return float4(blurredColor, 1.0f);
	//}

	//finalColor = txDiffuse.Sample(sampClamp, input.Tex).xyz;
	//return float4(finalColor.xyz, 1.0f);
	//float3 sampledDirX = txDiffuse.Sample(sampClamp, input.Tex + float2(dir.x, 0.0f));
	//float3 sampledDirY = txDiffuse.Sample(sampClamp, input.Tex + float2(0.0f, dir.y));

	//if (dir.y < -0.1f || dir.y > 0.1f || dir.x < -0.1f || dir.x > 0.1f) {
	//	finalColor = (txDiffuse.Sample(sampClamp, input.Tex) +
	//		sampledDirX + sampledDirY) * 0.333f;
	//	return float4(finalColor, 1.0f);
	//}
	//else {
	//	return float4(txDiffuse.Sample(sampClamp, input.Tex).xyz, 1.0f);
	//}
	//meck

	//float3 res1 = 0.5f *  txDiffuse.Sample(sampClamp, input.Tex + (dir * float2(1.0f / 3.0f - 0.5f, 1.0f / 3.0f - 0.5f))) +
	//	txDiffuse.Sample(sampClamp, input.Tex + (dir * float2(2.0f / 3.0f - 0.5f, 2.0f / 3.0f - 0.5f)));

	//float3 res2 = res1 * 0.5f + 0.25f * txDiffuse.Sample(sampClamp, input.Tex + (dir * float2(0.0f / 3.0f - 0.5f, 0.0f / 3.0f - 0.5f))) +
	//	txDiffuse.Sample(sampClamp, input.Tex + (dir * float2(3.0f / 3.0f - 0.5f, 3.0f / 3.0f - 0.5f)));

	//float lumaRes2Max = max(lumaM, max(max(lumaBR, lumaBL), max(lumaTR, lumaTL)));
	//float lumaRes2Min = min(lumaM, min(min(lumaBR, lumaBL), min(lumaTR, lumaTL)));
	//float lumaRes2 = dot(luma, res2);

	//if (lumaRes2 < lumaRes2Min || lumaRes2 > lumaRes2Max) //check if sampled to far
	//{
	//	return float4(res1.xyz, 1.0f);
	//}

	//
	//return float4(res2.xyz, 1.0f);

}

