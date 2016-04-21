// Input control point
struct VS_POINT_IN
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};

// Output control point
struct HS_CONTROL_POINT_OUTPUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};

// Output patch constant data.
struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[2]			: SV_TessFactor; // e.g. would be [4] for a quad domain
	//float EdgeTessFactor[3]			: SV_TessFactor; // e.g. would be [4] for a quad domain
	//float InsideTessFactor			: SV_InsideTessFactor; // e.g. would be Inside[2] for a quad domain
	// TODO: change/add other stuff
};

#define NUM_CONTROL_POINTS 2

// Patch Constant Function
HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(
	InputPatch<VS_POINT_IN, NUM_CONTROL_POINTS> inputPoints,
	uint PatchID : SV_PrimitiveID)
{
	HS_CONSTANT_DATA_OUTPUT Output;

	// Insert code to compute Output here
	Output.EdgeTessFactor[0] = 5;
	//Output.EdgeTessFactor[0] = 
	Output.EdgeTessFactor[1] = 5;
	//Output.EdgeTessFactor[2] = 

	//Below -v> Non existant for a isoline i think.
	//Output.InsideTessFactor = 15; // e.g. could calculate dynamic tessellation factors instead

	return Output;
}

[domain("isoline")]		//"tri", "quad", "isoline".
[partitioning("fractional_odd")]	//"integer", "fractional_even", "fractional_odd", or "pow2". //Defines the tesselation scheme to be used in the hull shader.
[outputtopology("line")]	//"point", "line", "triangle_cw", "triangle_ccw".
[outputcontrolpoints(3)]	//Defines the number of output control points (per thread) that will be created in the hull shader.
[patchconstantfunc("CalcHSPatchConstants")]	//Defines the function for computing patch constant data.
HS_CONTROL_POINT_OUTPUT HS_main( 
	InputPatch<VS_POINT_IN, NUM_CONTROL_POINTS> inputPoints,
	uint i : SV_OutputControlPointID,
	uint PatchID : SV_PrimitiveID )
{
	HS_CONTROL_POINT_OUTPUT Output;

	// Insert code to compute Output here
	Output.Pos = inputPoints[i].Pos;
	Output.Tex = inputPoints[i].Tex;

	return Output;
}
