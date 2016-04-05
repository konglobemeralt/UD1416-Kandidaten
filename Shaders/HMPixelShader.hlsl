Texture2D myTexture : register(t0);
Texture2D blendTexture1 : register(t1);
Texture2D blendTexture2 : register(t2);
Texture2D blendMap : register(t3);
Texture2D g_lightDepth : register(t4);

SamplerState samplerWrap : register(s0);
SamplerState samplerClamp : register(s1);
SamplerState samplerTerrain : register(s2);

struct HMVS_OUT
{
	float4 Pos : SV_POSITION;
	float4 Normal : NORMAL;
	float2 Tex : TEXCOORD;
	float2 BlendTex : TEXCOORD1;
	float4 Wpos : POSITION;
	float4 LightViewPos : POSITION1;
};

struct HMPS_OUT
{
	float4 Color : SV_Target0;
	float4 Normal : SV_Target1;
	float4 Wpos : SV_Target2;
	float4 Spec : SV_Target3;
};

HMPS_OUT HMPS_main(HMVS_OUT input)
{
	HMPS_OUT output;

	if (input.Normal.x < 0 || input.Normal.y < 0 || input.Normal.z < 0)
	{
		output.Normal = normalize(input.Normal * -1.0f);
	}
	else
	{
		output.Normal = normalize(input.Normal);
	}
	
	float4 color;
	float3 tmpColor, textureSample, blendTextureSample1, blendTextureSample2, blendMapSample;
	float bias;
	float2 projectTexCoord;
	float depthValue, depthValueSelfShadow;
	float lightDepthValue, lightDepthValueSelfShadow;

	bias = 0.001f;

	//Calculate blend
	textureSample = myTexture.Sample(samplerWrap, input.Tex).rgb;
	blendTextureSample1 = blendTexture1.Sample(samplerWrap, input.Tex).rgb;
	blendTextureSample2 = blendTexture2.Sample(samplerWrap, input.Tex).rgb;
	blendMapSample = blendMap.Sample(samplerWrap, input.BlendTex).rgb;

	tmpColor = lerp(textureSample, blendTextureSample1, blendMapSample.g);
	tmpColor = lerp(tmpColor, blendTextureSample2, blendMapSample.b);

	color = float4(tmpColor.rgb, 1.0f);

	//Calculate light/shadow
	projectTexCoord.x = (input.LightViewPos.x / input.LightViewPos.w) / 2.0f + 0.5f;
	projectTexCoord.y = (-input.LightViewPos.y / input.LightViewPos.w) / 2.0f + 0.5f;

	[flatten]
	if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
	{
		depthValue = g_lightDepth.Sample(samplerClamp, projectTexCoord).x;
		depthValueSelfShadow = depthValue * dot(input.Normal, float4(15.0f, 30.0f, -50.0f, 1.0f));

		lightDepthValueSelfShadow = input.LightViewPos.z / input.LightViewPos.w;
		lightDepthValue = lightDepthValueSelfShadow - bias;

		[flatten]
		if (lightDepthValue > depthValue || lightDepthValueSelfShadow > depthValueSelfShadow)
		{
			color /= 2.0f;
		}
	}

	output.Wpos = input.Wpos;
	output.Color = color;
	output.Spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	return output;
}