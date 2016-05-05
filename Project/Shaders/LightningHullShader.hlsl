// Input control point
struct VS_POINT_IN
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
};

// Output control point
struct HS_CONTROL_POINT_OUTPUT
{
	float4 Pos : SV_POSITION;
	//float3 Normal : NORMAL;
	float2 BranchLevel : TEXCOORD;
};

// Output patch constant data.
struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[2]			: SV_TessFactor; // e.g. would be [4] for a quad domain
	//float EdgeTessFactor[3]			: SV_TessFactor; // e.g. would be [4] for a quad domain
	//float InsideTessFactor			: InsideTessFactor; // e.g. would be Inside[2] for a quad domain
	// TODO: change/add other stuff
};

#define NUM_CONTROL_POINTS 2

// Patch Constant Function
HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(
	InputPatch<VS_POINT_IN, NUM_CONTROL_POINTS> inputPoints,
	uint PatchID : SV_PrimitiveID)
{
	HS_CONSTANT_DATA_OUTPUT output;
	
	// Insert code to compute Output here
	output.EdgeTessFactor[0] = density; //	Density factor
	output.EdgeTessFactor[1] = tessLevel; // Tessellation factor
	//output.EdgeTessFactor[1] = 4; // Tessellation factor

	return output;
}

[domain("isoline")]		//"tri", "quad", "isoline".
[partitioning("integer")]	//"integer", "fractional_even", "fractional_odd", or "pow2". //Defines the tesselation scheme to be used in the hull shader.
[outputtopology("line")]	//"point", "line", "triangle_cw", "triangle_ccw".
[outputcontrolpoints(2)]	//Defines the number of output control points (per thread) that will be created in the hull shader.
[patchconstantfunc("CalcHSPatchConstants")]	//Defines the function for computing patch constant data.
HS_CONTROL_POINT_OUTPUT HS_main(InputPatch<VS_POINT_IN, NUM_CONTROL_POINTS> inputPoints, uint id : SV_OutputControlPointID)
{
	HS_CONTROL_POINT_OUTPUT output;

	// Insert code to compute Output here
	output.Pos = inputPoints[id].Pos;
	//output.Normal = inputPoints[id].Normal;
	output.BranchLevel = inputPoints[id].BranchLevel;
	//output.Tex = float2(0, 0);

	return output;
}
