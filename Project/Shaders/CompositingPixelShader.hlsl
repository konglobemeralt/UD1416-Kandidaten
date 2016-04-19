Texture2D mySRV;
sampler SamplerWrap;

struct VS_OUT {
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};

float4 PS_main(VS_OUT input) : SV_TARGET
{
	//render UV coords of mesh
	//return float4(input.Tex.x, input.Tex.y, 0.0f, 1.0f);

	return mySRV.Sample(SamplerWrap, input.Tex);
}