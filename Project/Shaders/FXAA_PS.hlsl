SamplerState sampWrap : register(s0);
SamplerState  sampClamp: register(s1);

Texture2D txDiffuse : register(t0);

cbuffer FXAACBuffer : register(b0)
{
	float2 texelSizeXY; //blir ju två stycken coz man dividerar med height o width
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

	//inverta med - coz v är sett från motsatta hållet
	//dir.x tex kommer antingen vara 0 = ingen edge, över 0 så ska det blurras åt positivt u, under 0 så ska det blurras åt negativt u 
	//eller så säger den bara att det behöver blurras i u åt båda hålle
	dir.x = -(lumaTR + lumaTL) - (lumaBR + lumaBL); //ifall denna är större än 0 så betyder det att de behöver blurras för då ser de olika ut
	dir.y = (lumaTR + lumaBR) - (lumaTL + lumaBL);

	float FXAA_reduce_MULTIPLIER = 1.0f / 8.0f;
	float FXAA_reduce_MIN = 1.0f / 128.0f;
	float FXAA_blur_Texels_Threshhold = 8.0f;

	float dirOffset = (min((lumaTR + lumaTL + lumaBR + lumaBL) * 0.25f * FXAA_reduce_MULTIPLIER, FXAA_reduce_MIN)); //average värde av luminocityn och sedan offset, men den ska minst vara ett visst värde för att ej bli 0

																													//vi har riktningen, men hur många texels ska blurras?
																													//gör om så att det minsta värdet är 1 texel lång
	float temp = 1.0f / min(abs(dir.x), abs(dir.y) + dirOffset); //dirOffset så att den inte ska dela på 0, dvs när det inte behöver blurras. Men oxå för att ha offset värde
	dir = dir * temp;

	//begränsa antalet texlar den kommer blurra
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

