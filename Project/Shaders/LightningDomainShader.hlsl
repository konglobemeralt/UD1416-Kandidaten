struct DS_OUTPUT
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
	float dx = domain.x;
	float dy = domain.y;

	//Output.Pos = (patch[0].Pos * domain.x + patch[1].Pos * domain.y, 1, 1);
	//Output.Pos = (patch[0].Pos * domain.x + patch[1].Pos * domain.y + patch[0].Pos * domain.x, 1.0f);

	
	//float3 pos = pow(1.0f - t, 3.0f) * patch[0].Pos + 3.0f * pow(1.0f - t, 2.0f) * t * patch[1].Pos + 3.0f * (1.0f - t) * pow(t, 2.0f) * patch[2].Pos + pow(t, 3.0f) * patch[3].Pos;

	/////GOOD
	//Output.Pos = float4((patch[0].Pos.x * dx), (patch[0].Pos.y * dy), 0.0f, 1.0f);	//Z = DEPTHPOS
	Output.Pos = pow(1.0f - dx, 3.0f) * patch[0].Pos + 3.0f * pow(1.0f - dx, 2.0f) * dx * patch[1].Pos;
	/////GOOD

	//Output.Pos = ((patch[0].Pos * dx) + (patch[0].Pos), dx, 1);
	//Output.Pos = float4(pos, 1.0f);

	//Output.Pos = patch[1].Pos * (t * 2);

	//Output.Pos = float4((patch[0].Pos * pos), 1.0f);

	Output.Tex = patch[0].Tex;
	//Original Backup
	//Output.Pos = float4(patch[0].vPosition * domain.x + patch[1].vPosition * domain.y + patch[2].vPosition * domain.z, 1);
	return Output;
}
