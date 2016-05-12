struct Corners
{
	float2 leftup;
	float2 leftdown;
	float2 rightup;
	float2 rightdown;
};

// Write the new position of every particle
RWStructuredBuffer<Corners> output : register(u0);
Texture2D tex : register(t0);
 
[numthreads(1, 1, 1)]
void CS_main(uint3 id : SV_DispatchThreadID)
{
	float texX = 1024.0f;
	float texY = 760.0f;

	output[0].leftup		=	float2(0.0f, 0.0f);
	output[0].leftdown	=	float2(0.5f, 1.0f);
	output[0].rightup	=	float2(1.0f, 0.0f);
	output[0].rightdown	=	float2(1.0f, 1.0f);

	for (unsigned int i = 0; i < texX; i++)
	{
		for (unsigned int j = 0; j < texY; j++)
		{
			float2 pixel = float2(saturate(id.x), saturate(id.y));
		}
	}
	//float2 uv = tex.Sample(Linear, float2());
	//output[id.x].leftup		=	
}