struct Corners
{
	float2 leftup;
	float2 leftdown;
	float2 rightup;
	float2 rightdown;
};

// Write the new position of every particle
RWStructuredBuffer<Corners> output : register(u0);
//AppendStructuredBuffer<Corners> output : register(u0);
Texture2D tex : register(t0);
SamplerState Linear : register(s0);
 
//[numthreads(32, 19, 1)]
[numthreads(1, 1, 1)]
void CS_main(uint3 id : SV_DispatchThreadID)
{
	float texX = 1024.0f;
	float texY = 760.0f;

	//Corners corners;
	//corners.leftup		=	float2(0.0f, 0.0f);
	//corners.leftdown	=	float2(0.5f, 1.0f);
	//corners.rightup		=	float2(1.0f, 0.0f);
	//corners.rightdown	=	float2(1.0f, 1.0f);
	//output.Append(corners);

	output[0].leftup	=	float2(0.0f, 0.0f);
	output[0].leftdown	=	float2(0.0f, 1.0f);
	output[0].rightup	=	float2(1.0f, 0.0f);
	output[0].rightdown	=	float2(1.0f, 1.0f);

	//float2 pixel = float2(0.0f, 0.0f);

	//for (unsigned int i = 0; i < texX; i++)
	//{
	//	for (unsigned int j = 0; j < texY; j++)
	//	{
	//		pixel = float2(saturate(id.x), saturate(id.y));
	//	}
	//}
	//float4 uv = tex.SampleLevel(Linear, pixel, 0);
	//output[id.x].leftup = uv;
}