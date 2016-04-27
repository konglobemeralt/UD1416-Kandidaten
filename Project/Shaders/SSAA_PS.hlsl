sampler sampWrap : register(s0);
sampler  sampClamp: register(s1);

Texture2D txDiffuse : register(t0);

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD0;
};

cbuffer FXAACBuffer : register(b0)
{
	float2 texelSizeXY;
	float FXAA_blur_Texels_Threshhold; //hur många texlar som kommer blurras åt varje håll
	float minimumBlurThreshhold; //hur mycket som krävs för att den ens ska blurra
	float FXAA_reduce_MULTIPLIER;
	float FXAA_reduce_MIN; //så dirOffset inte ska bli noll
	float2 pad;
};

float4 PS_main(VS_OUT input) : SV_TARGET
{
	float4 final = float4(0,0,0,0);

	//float2 offset[4];
	//offset[0] = float2(texelSizeXY.x, 0.0f);
	//offset[1] = float2(-texelSizeXY.x, 0.0f);
	//offset[2] = float2(0.0f, texelSizeXY.y);
	//offset[3] = float2(0.0f, -texelSizeXY.y);

	//final += float4(txDiffuse.Sample(sampClamp, input.Tex).xyz, 1.0f);
	//final += float4(txDiffuse.Sample(sampClamp, input.Tex + offset[0]).xyz, 1.0f);
	//final += float4(txDiffuse.Sample(sampClamp, input.Tex + offset[1]).xyz, 1.0f);
	//final += float4(txDiffuse.Sample(sampClamp, input.Tex + offset[2]).xyz, 1.0f);
	//final += float4(txDiffuse.Sample(sampClamp, input.Tex + offset[3]).xyz, 1.0f);

	//final *= 0.2f;

	return float4(txDiffuse.Sample(sampClamp, input.Tex).xyz, 1.0f);
}
