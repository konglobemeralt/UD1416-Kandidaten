struct VS_OUT
{
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
	unsigned int choice = 2;
	// 0 = one texture uv
	// 1 = two texture uv
	// 2 = quad uv
	// 3 = show one texture uv
	// 4 = show two texture uv
	// 5 = show quad uv
	
	float4 color = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 texUV = textUV.Sample(Linear, input.Tex);	// Sample uv coordinates from texture
	float2 u = U.Sample(Linear, input.Tex);				// Sample u coordinates from texture
	float2 v = V.Sample(Linear, input.Tex);				// Sample v coordinates from texture
	float4 uv = float4((u.r), (v.g), 0.0f, 1.0f);		// put u and v toghter to uv
	color = text.Sample(Linear, saturate(texUV));

	if (choice == 0 && texUV.w > 0.99f && color.w >= 0.01f)
	{
		return color;
	}
	else if (choice == 1)
	{
		return text.Sample(Linear, saturate(uv));
	}
	else if (choice == 2)
	{
		return text.Sample(Linear, input.Tex);
	}
	else if (choice == 3)
	{
		return textUV.Sample(Linear, input.Tex);
	}
	else if (choice == 4)
	{
		return texUV;
	}
	else if (choice == 5)
	{
		return uv;
	}
	else if (choice == 6)
	{
		return float4(input.Tex, 0.0f, 1.0f);
	}
	return float4(1.0f, 0.0f, 0.0f, 0.0f);

	//return float4(input.Tex.x, input.Tex.y, 0.0f, 1.0f);

	//if (input.Tex.y > 0.5f)
	//	return txDiffuse.Sample(sampAni, input.Tex);
	//else
	//	return txDiffuse.Sample(sampPoint, input.Tex);
}