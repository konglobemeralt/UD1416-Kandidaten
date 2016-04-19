struct VS_IN
{
	float3 Pos : POSITION;
	float2 Tex : TEXCOORD;
};

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};

VS_OUT VS_main(VS_IN input)
{
	VS_OUT output;		//VS_OUT output = (VS_OUT)0; //Från 3D_Space_Project
	//output.Pos = float4(input.Pos, 1.0f);	//Input Quad
	output.Pos = float4((input.Pos.x * 0.8), (input.Pos.y * 0.8), (input.Pos.z * 0.8), 1.0f);
	output.Tex = input.Tex;

	return output;
}