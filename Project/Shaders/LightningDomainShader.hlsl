struct DS_OUTPUT
{
	float4 Pos : SV_POSITION;
	//float3 Normal : NORMAL;
	float2 BranchLevel : TEXCOORD;
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
	//float InsideTessFactor			: SV_InsideTessFactor; // e.g. would be Inside[2] for a quad domain
	// TODO: change/add other stuff
};

float randomizes(in float2 uv)
{
	float2 noise = (frac(sin(dot(uv, float2(12.9898, 78.233)*2.0)) * 43758.5453));
	return abs(noise.x + noise.y) * 0.5;
}

#define NUM_CONTROL_POINTS 2

[domain("isoline")]
DS_OUTPUT DS_main(HS_CONSTANT_DATA_OUTPUT input, float2 uv : SV_DomainLocation,	//float2 quad patch, float3 tri patch, float2 isoline
	const OutputPatch<HS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> cpoint, uint id : SV_PrimitiveID)
{
	DS_OUTPUT output;

	float4 p1 = cpoint[0].Pos; 
	float4 p2 = cpoint[1].Pos;

	//Calculate Line Vectors
	float3 lineDir = normalize(p2 - p1);
	float3 up = float3(0.0f, 1.0f, 0.0f);
	float3 right = normalize(cross(lineDir, up));
	float3 left = normalize(cross(right, lineDir));

	//float x = uv.x + uv.y * input.EdgeTessFactor[0];
	float x = uv.x / input.EdgeTessFactor[0] + uv.y;	//http://xboxforums.create.msdn.com/forums/p/54604/331853.aspx

	//Case1
	//float randOffsX = saturate(randomizes(uv + p1.xz));
	//float randOffsZ = saturate(randomizes(uv + p2.xz));

	float randOffsX = clamp((randomizes(uv + p1.xz)), -1.0f, 1.0f);
	float randOffsZ = clamp((randomizes(uv + p2.xz)), -1.0f, 1.0f);


	float4 newPos = float4((randOffsX * right) + (randOffsZ * left), 1.0f);



	float randScale = 0.7;


	float3 vPos = lerp(cpoint[0].Pos, cpoint[1].Pos, x);

	if(uv.x == 0 || uv.x == 1)
		output.Pos = float4(vPos, 1.0f);
	else
		output.Pos = float4((vPos + (newPos * randScale)), 1.0f);
	
	//output.Normal = cpoint[0].Normal;
	output.BranchLevel = cpoint[1].BranchLevel;
	//output.BranchLevel = float2(0, 0);

	return output;
}
