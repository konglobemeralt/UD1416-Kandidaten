struct VS_OUT {
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};

Texture2D textUV : register(t0);
Texture2D text : register(t1);
Texture2D U : register(t2);
Texture2D V : register(t3);
SamplerState Linear : register(s0);
SamplerState Point : register(s1);

float4 PS_main(VS_OUT input) : SV_TARGET
{
	float4 uv = textUV.Sample(Linear, input.Tex);
	float4 u = U.Sample(Linear, input.Tex);
	float4 v = V.Sample(Linear, input.Tex);
	uv = float4(u.r, v.g, 0.0f, 1.0f);
	return text.Sample(Linear, uv);
	return text.Sample(Linear, input.Tex);
	return textUV.Sample(Linear, input.Tex);

	//return float4(input.Tex.x, input.Tex.y, 0.0f, 1.0f);

	//if (input.Tex.y > 0.5f)
	//	return txDiffuse.Sample(sampAni, input.Tex);
	//else
	//	return txDiffuse.Sample(sampPoint, input.Tex);
}