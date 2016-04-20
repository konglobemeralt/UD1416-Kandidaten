Texture2D mySRV : register(t0);
Texture2D mySRV2 : register(t1);
sampler SamplerWrap;

struct VS_OUT {
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};


float4 PS_main(VS_OUT input) : SV_TARGET
{
	//render UV coords of mesh
	//return float4(input.Tex.x, input.Tex.y, 0.0f, 1.0f);


	float4 tempUV = mySRV.Sample(SamplerWrap, input.Tex);
	
	//float2 tex = (tempUV.x, tempUV.y);
	
	//float3 tex;
	//tex.y = (0.148 * tempUV.x) - (0.291 * tempUV.y) + (0.439 *  tempUV.z);
	//tex.x = (0.439 * tempUV.x) - (0.368 *  tempUV.y) - (0.071 * tempUV.z);
	//
	//tex.y =  0.299 * tempUV.x + 0.587 * tempUV.y + 0.114 * tempUV.z;
	//tex.z = -0.147 * tempUV.x - 0.289 * tempUV.y + 0.436 * tempUV.z;
	//tex.x =  0.615 * tempUV.x - 0.515 * tempUV.y - 0.100 * tempUV.z;
	if (tempUV.w > 0)
	{
		return mySRV2.Sample(SamplerWrap, saturate(tempUV));
	}

	else
	{
		return float4(0.0f, 1.0f, 0.0f, 0.0f);
	}


}