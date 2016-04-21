struct DS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};

// Output control point
struct HS_CONTROL_POINT_OUTPUT
{
	float3 vPosition : WORLDPOS; 
};

// Output patch constant data.
struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[2]			: SV_TessFactor; // e.g. would be [4] for a quad domain
	//float InsideTessFactor			: SV_InsideTessFactor; // e.g. would be Inside[2] for a quad domain
	// TODO: change/add other stuff
};

#define NUM_CONTROL_POINTS 2

[domain("isoline")]
DS_OUTPUT DS_main(
	HS_CONSTANT_DATA_OUTPUT input,
	float2 domain : SV_DomainLocation,	//float2 quad patch, float3 tri patch, float2 isoline
	const OutputPatch<HS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> patch)
{
	DS_OUTPUT Output;

	Output.Pos = float4(patch[0].vPosition * domain.x + patch[1].vPosition * domain.y, 1);
	//OLD Below
	//Output.Pos = float4(patch[0].vPosition * domain.x + patch[1].vPosition * domain.y + patch[2].vPosition * domain.z, 1);

	return Output;
}
