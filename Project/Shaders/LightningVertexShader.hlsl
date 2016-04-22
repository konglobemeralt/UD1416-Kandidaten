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

cbuffer cBuffer : register(b0)
{
	float4x4 WVP;
	float4x4 World;
	float4 camPos;
	float lineWidth;
	float tessLevel;
	float2 padding;
};

VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;		//VS_OUT output = (VS_OUT)0; //Från 3D_Space_Project
	output.Pos = float4(input.Pos, 1.0f);	//Input Quad
	//output.Pos = float4((input.Pos.x * 0.8), (input.Pos.y * 0.8), (input.Pos.z * 0.8), 1.0f);
	//output.Pos = float4((input.Pos.x * lineWidth), (input.Pos.y * lineWidth), (input.Pos.z * lineWidth), 1.0f);
	//output.Pos = mul(float4(input.Pos, 1.0), WVP);	//Calculate Vertex WVP Positions
	output.Tex = input.Tex;

	return output;
}