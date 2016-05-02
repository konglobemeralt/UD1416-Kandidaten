struct VS_OUT {
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};

Texture2D textUV : register(t0);
Texture2D text : register(t1);
SamplerState sampAni : register(s0);
SamplerState sampPoint : register(s1);

float4 PS_main(VS_OUT input) : SV_TARGET
{

	float4 uv = textUV.Sample(sampAni, input.Tex);
	return text.Sample(sampAni, saturate(uv));

	return float4(input.Tex.x, input.Tex.y, 0.0f, 1.0f);

	//if (input.Tex.y > 0.5f)
	//	return txDiffuse.Sample(sampAni, input.Tex);
	//else
	//	return txDiffuse.Sample(sampPoint, input.Tex);
}