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
	unsigned int choice = 0;
	// 0 = one texture uv
	// 1 = two texture uv
	// 2 = quad uv
	// 3 = show one texture uv
	// 4 = show two texture uv
	// 5 = show quad uv

	float4 texUV = textUV.Sample(Linear, input.Tex);	// Sample uv coordinates from texture
	float2 u = U.Sample(Linear, input.Tex);				// Sample u coordinates from texture
	float2 v = V.Sample(Linear, input.Tex);				// Sample v coordinates from texture
	float4 uv = float4((u.r), (v.g), 0.0f, 1.0f);		// put u and v toghter to uv
	if (choice == 0)
	{
		return text.Sample(Linear, saturate(texUV));
	}
	if (choice == 1)
	{
		return text.Sample(Linear, saturate(uv));
	}
	if (choice == 2)
	{
		return text.Sample(Linear, input.Tex);
	}
	if (choice == 3)
	{
		return textUV.Sample(Linear, input.Tex);
	}
	if (choice == 4)
	{
		return texUV;
	}
	if (choice == 5)
	{
		return uv;
	}
	if (choice == 6)
	{
		return float4(input.Tex, 0.0f, 1.0f);
	}

	//return float4(input.Tex.x, input.Tex.y, 0.0f, 1.0f);

	//if (input.Tex.y > 0.5f)
	//	return txDiffuse.Sample(sampAni, input.Tex);
	//else
	//	return txDiffuse.Sample(sampPoint, input.Tex);
}