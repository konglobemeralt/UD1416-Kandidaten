struct Corners
{
	float4 leftup;
	float4 leftdown;
	float4 rightup;
	float4 rightdown;
};

// Write the new position of every particle
RWStructuredBuffer<Corners> output : register(u0);
Texture2D tex : register(t0);
 
[numthreads(1, 1, 1)]
void CS_main(uint3 id : SV_DispatchThreadID)
{
	output[id.x].leftup		=	float4(0.0f, 0.0f, 0.0f, 0.0f);
	output[id.x].leftdown	=	float4(0.0f, 1.0f, 0.0f, 0.0f);
	output[id.x].rightup	=	float4(1.0f, 0.0f, 0.0f, 0.0f);
	output[id.x].rightdown	=	float4(1.0f, 1.0f, 0.0f, 0.0f);

	//float2 uv = tex.Sample(Linear, float2());
	//output[id.x].leftup		=	
}