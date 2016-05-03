struct VS_OUT {
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};

Texture2D textUV : register(t0);
SamplerState Linear : register(s0);

float4 PS_main(VS_OUT input) : SV_TARGET
{
	float4 texUV = textUV.Sample(Linear, input.Tex);

	if (texUV.w > 0.5f)
	{
		return texUV;
	}
	//discard;
	clip(texUV.w < 0.5f ? -1 : 1);
	return texUV;
}