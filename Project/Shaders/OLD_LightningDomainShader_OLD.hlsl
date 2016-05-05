struct DS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float3 Normal : NORMAL;
	float2 Tex : TEXCOORD;
};

// Output control point
struct HS_CONTROL_POINT_OUTPUT
{
	float4 Pos : SV_POSITION;
	float3 Normal : NORMAL;
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
DS_OUTPUT DS_main(HS_CONSTANT_DATA_OUTPUT input, float2 domain : SV_DomainLocation,	//float2 quad patch, float3 tri patch, float2 isoline
	/*const */OutputPatch<HS_CONTROL_POINT_OUTPUT, 2> cpoint, uint id : SV_PrimitiveID)
{
	HS_CONTROL_POINT_OUTPUT output;
	float dx = domain.x;
	float dy = domain.y;

	//output.Pos = (cpoint[0].Pos * domain.x + cpoint[1].Pos * domain.y, 1, 1);
	//output.Pos = (cpoint[0].Pos * domain.x + cpoint[1].Pos * domain.y + cpoint[0].Pos * domain.x, 1.0f);


	//float3 pos = pow(1.0f - t, 3.0f) * cpoint[0].Pos + 3.0f * pow(1.0f - t, 2.0f) * t * cpoint[1].Pos + 3.0f * (1.0f - t) * pow(t, 2.0f) * cpoint[2].Pos + pow(t, 3.0f) * cpoint[3].Pos;

	/////GOOD
	//output.Pos = float4((cpoint[0].Pos.x * dx), (cpoint[0].Pos.y * dy), 0.0f, 1.0f);	//Z = DEPTHPOS
	output.Pos = pow(1.0f - dx, 3.0f) * cpoint[0].Pos + 3.0f * pow(1.0f - dx, 2.0f) * dx * cpoint[1].Pos;
	//output.Pos = cpoint[0].Pos * cpoint[1].Pos;
	/////GOOD
	//output.Pos = float4(dx, dy, 0.0f, 1.0f);
	//output.Pos = cpoint[0].Pos * dx * dy;

	//output.Pos = ((cpoint[0].Pos * dx) + (cpoint[0].Pos), dx, 1);
	//output.Pos = float4(pos, 1.0f);

	//output.Pos = (cpoint[0].Pos + cpoint[1].Pos) / 2;
	//output.Pos = dx; 

	//output.Pos = cpoint[1].Pos * (t * 2);

	//output.Pos = float4((cpoint[0].Pos * pos), 1.0f);

	//output.Pos = cpoint[id].Pos * dx;

	output.Normal = cpoint[0].Normal;
	output.Tex = cpoint[0].Tex;
	//Original Backup
	//output.Pos = float4(cpoint[0].vPosition * domain.x + cpoint[1].vPosition * domain.y + cpoint[2].vPosition * domain.z, 1);
	return output;
}
