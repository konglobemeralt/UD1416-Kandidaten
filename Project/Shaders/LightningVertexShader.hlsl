struct VS_IN
{
	float3 Pos : POSITION;
	//float3 Normal : NORMAL;
	float2 BranchLevel : TEXCOORD;
};

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	//float3 Normal : NORMAL;
	float2 BranchLevel : TEXCOORD;
};

cbuffer cBuffer : register(b0)
{
	float4x4 WVP;
	float4x4 WV;
	float4x4 World;
	float4x4 projection;
	float4 camPos;
	float lineWidth;
	int density;
	float tessLevel;
	float seed;
	int baseLightningSegments;
};

VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;		//VS_OUT output = (VS_OUT)0; //Från 3D_Space_Project
	
	output.Pos = float4(input.Pos, 1.0f);	//Use This If WVP calc is done in GS.
	//output.Normal = float3(0, 0, 0);
	output.BranchLevel = input.BranchLevel;

	//output.Pos = float4((input.Pos.x * 0.8), (input.Pos.y * 0.8), (input.Pos.z * 0.8), 1.0f);
	//output.Pos = float4((input.Pos.x * lineWidth), (input.Pos.y * lineWidth), (input.Pos.z * lineWidth), 1.0f);
	//output.Pos = mul(float4(input.Pos, 1.0), WVP);	//Calculate Vertex WVP Positions

	//output.Pos = mul(float4(input.Pos, 1.0), World);	//Calculate Vertex V Positions
	//output.Pos = mul(output.Pos, viewMatrix);
	//output.Pos = mul(output.Pos, projection);

	return output;
}