//Texture2D g_lightDepth : register(t0);
//Texture2D myTexture : register(t1);
//
//SamplerState samplerWrap : register(s0);
//SamplerState samplerClamp : register(s1);
//
//cbuffer cBuffer1 : register(b0)
//{
//	float4x4 worldMatrix;
//	float4x4 viewMatrix;
//	float4x4 projMatrix;
//	float4x4 viewMatrixInverse;
//	float4x4 lightViewMatrix;
//	float4x4 lightProjMatrix;
//	float4 camPos;
//	float4 specularColor_Power;
//}
//
//cbuffer DirectionalLight : register(b1)
//{
//	float lightIntensity;
//	float3 lightDirection;
//	float3 lightColor;
//	float padding;
//}

struct GS_OUT
{
	float4 Pos : SV_POSITION;
	//float4 Normal : NORMAL;
	float2 Tex : TEXCOORD;
	//float4 Wpos : POSITION;
	//float4 LightViewPos : POSITION1;
};

struct PS_OUT
{
	float4 Color : SV_Target0;
	//float4 Normal : SV_Target1;
	//float4 Wpos : SV_Target2;
	//float4 Diffuse : SV_Target3;
};

PS_OUT PS_main(GS_OUT input) : SV_TARGET
{
	PS_OUT output;

	////if (input.Normal.x < 0 || input.Normal.y < 0 || input.Normal.z < 0)
	////{
	////	output.Normal = input.Normal * -1.0f;
	////}
	////else
	////{
	//	output.Normal = input.Normal;
	////}

	//float4 color;
	//float bias;
	//float2 projectTexCoord;
	//float depthValue, depthValueSelfShadow;
	//float lightDepthValue, lightDepthValueSelfShadow;

	//bias = 0.001f;
	//color = myTexture.Sample(samplerWrap, input.Tex);

	////From WVP to NDC(-1 - 1) to uv(0 - 1)
	//projectTexCoord.x = (input.LightViewPos.x / input.LightViewPos.w) / 2.0f + 0.5f;
	//projectTexCoord.y = (-input.LightViewPos.y / input.LightViewPos.w) / 2.0f + 0.5f;

	//[flatten]
	//if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
	//{
	//	//depthValue = g_lightDepth2.Sample(samplerClamp2, projectTexCoord).x;
	//	depthValue = g_lightDepth.Sample(samplerClamp, projectTexCoord).x;
	//	depthValueSelfShadow = depthValue * dot(input.Normal, float4(lightDirection.x, lightDirection.y, lightDirection.z, 1.0f));

	//	lightDepthValueSelfShadow = input.LightViewPos.z / input.LightViewPos.w;
	//	lightDepthValue = lightDepthValueSelfShadow - bias;

	//	[flatten]
	//	if (lightDepthValue > depthValue || lightDepthValueSelfShadow > depthValueSelfShadow)
	//	{
	//		color /= 2.0f;
	//	}
	//}

	//output.Wpos = input.Wpos;
	//output.Diffuse = float4(0.0f, 0.0f, 1.0f, 1.0f);
	output.Color = float4(0.0f, 0.0f, 1.0f, 1.0f);
	
	return output;
}