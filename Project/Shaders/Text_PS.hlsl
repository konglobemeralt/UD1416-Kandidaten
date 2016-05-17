struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};

cbuffer Corners : register(b0)
{
	float2 leftup;
	float2 leftdown;
	float2 rightup;
	float2 rightdown;
};

//struct Test
//{
//	float2 leftup;
//	float2 leftdown;
//	float2 rightup;
//	float2 rightdown;
//};
//StructuredBuffer<Test> test;

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
	// 3 = lerp rendering
	// 4 = show one texture uv
	// 5 = show two texture uv
	// 6 = show quad uv
	// 7 = show lerp uv
	
	float small = 0.001f;
	float4 color = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 texUV = textUV.Sample(Linear, input.Tex);	// Sample uv coordinates from texture
	float2 u = U.Sample(Linear, input.Tex);				// Sample u coordinates from texture
	float2 v = V.Sample(Linear, input.Tex);				// Sample v coordinates from texture
	float4 uv = float4((u.r), (v.g), 0.0f, 1.0f);		// put u and v toghter to uv
	color = text.Sample(Linear, saturate(texUV));

	float x = lerp(0.0f, 1.0f, saturate(input.Tex.x));
	float y = lerp(0.0f, 1.0f, saturate(input.Tex.y));
	x = lerp(leftdown.x, rightup.x, saturate(input.Tex.x));
	y = lerp(leftup.y, rightdown.y, saturate(input.Tex.y));
	float2 xy = float2(x, y);
	float4 lerpColor = text.Sample(Linear, xy);

	if (choice == 0 && texUV.w > 0.99f && color.w >= 0.01f)
	{
		//float2 test = float2(texUV.x, 1.0f - texUV.y);
		//return text.Sample(Linear, test);
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
		return lerpColor;
	}
	else if (choice == 4)
	{
		return textUV.Sample(Linear, input.Tex);
	}
	else if (choice == 5)
	{
		return uv;
	}
	else if (choice == 6)
	{
		return float4(input.Tex, 0.0f, 1.0f);
	}
	else if (choice == 7)
	{
		return float4(xy, 0.0f, 1.0f);
	}
	return float4(0.0f, 1.0f, 0.0f, 0.0f);
}