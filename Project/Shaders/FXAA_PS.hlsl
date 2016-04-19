SamplerState sampWrap : register(s0);
SamplerState  sampClamp: register(s1);

Texture2D txDiffuse : register(t0);

cbuffer FXAACBuffer : register(b0)
{
	float2 texelSizeXY; //blir ju tv� stycken coz man dividerar med height o width
	float2 pad;
};

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD0;
};


float4 PS_main(VS_OUT input) : SV_TARGET
{


	float3 luma = float3(0.299f, 0.587f, 0.114f);

	float lumaTR = dot(luma, txDiffuse.Sample(sampClamp, input.Tex + float2(1.0f, -1.0f) * texelSizeXY).xyz);
	float lumaTL = dot(luma, txDiffuse.Sample(sampClamp, input.Tex + float2(-1.0f, -1.0f) * texelSizeXY).xyz);
	float lumaBR = dot(luma, txDiffuse.Sample(sampClamp, input.Tex + float2(1.0f, 1.0f) * texelSizeXY).xyz);
	float lumaBL = dot(luma, txDiffuse.Sample(sampClamp, input.Tex + float2(-1.0f, 1.0f) * texelSizeXY).xyz);
	float lumaM = dot(luma, txDiffuse.Sample(sampClamp, input.Tex.xy));

	float2 dir;

	//inverta med - coz v �r sett fr�n motsatta h�llet
	//dir.x tex kommer antingen vara 0 = ingen edge, �ver 0 s� ska det blurras �t positivt u, under 0 s� ska det blurras �t negativt u 
	//eller s� s�ger den bara att det beh�ver blurras i u �t b�da h�lle
	dir.x = -(lumaTR + lumaTL) - (lumaBR + lumaBL); //ifall denna �r st�rre �n 0 s� betyder det att de beh�ver blurras f�r d� ser de olika ut
	dir.y = (lumaTR + lumaBR) - (lumaTL + lumaBL);

	float FXAA_reduce_MULTIPLIER = 1.0f / 8.0f;
	float FXAA_reduce_MIN = 1.0f / 128.0f;
	float FXAA_blur_Texels_Threshhold = 8.0f;

	float dirOffset = (min((lumaTR + lumaTL + lumaBR + lumaBL) * 0.25f * FXAA_reduce_MULTIPLIER, FXAA_reduce_MIN)); //average v�rde av luminocityn och sedan offset, men den ska minst vara ett visst v�rde f�r att ej bli 0

																													//vi har riktningen, men hur m�nga texels ska blurras?
																													//g�r om s� att det minsta v�rdet �r 1 texel l�ng
	float temp = 1.0f / min(abs(dir.x), abs(dir.y) + dirOffset); //dirOffset s� att den inte ska dela p� 0, dvs n�r det inte beh�ver blurras. Men ox� f�r att ha offset v�rde
	dir = dir * temp;

	//begr�nsa antalet texlar den kommer blurra
	clamp(dir.x, -FXAA_blur_Texels_Threshhold, FXAA_blur_Texels_Threshhold);
	clamp(dir.y, -FXAA_blur_Texels_Threshhold, FXAA_blur_Texels_Threshhold);

	dir *= texelSizeXY;
	//blurr!!
	float3 res1 = 0.5f *  txDiffuse.Sample(sampClamp, input.Tex + (dir * float2(1.0f / 3.0f - 0.5f, 1.0f / 3.0f - 0.5f))) *
		txDiffuse.Sample(sampClamp, input.Tex + (dir * float2(2.0f / 3.0f - 0.5f, 1.0f / 3.0f - 0.5f)));

	float3 res2 = res1 * 0.5f + 0.25f * txDiffuse.Sample(sampClamp, input.Tex + (dir * float2(0.0f / 3.0f - 0.5f, 1.0f / 3.0f - 0.5f))) *
		txDiffuse.Sample(sampClamp, input.Tex + (dir * float2(3.0f / 3.0f - 0.5f, 1.0f / 3.0f - 0.5f)));

	float lumaRes2Max = max(lumaM, max(max(lumaBR, lumaBL), max(lumaTR, lumaTL)));
	float lumaRes2Min = min(lumaM, min(min(lumaBR, lumaBL), min(lumaTR, lumaTL)));
	float lumaRes2 = dot(luma, res2);

	if (lumaRes2 < lumaRes2Min || lumaRes2 > lumaRes2Max) //check if sampled to far
	{
		return float4(res1.xyz, 1.0f);
	}

	return float4(res2.xyz, 1.0f);

}

