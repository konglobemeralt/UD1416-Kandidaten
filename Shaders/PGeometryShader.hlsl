cbuffer cBuffer1 : register(b0)
{
	float4x4 worldMatrix;
	float4x4 viewMatrix;
	float4x4 projMatrix;
	float4x4 viewMatrixInverse;
	float4x4 lightViewMatrix;
	float4x4 lightProjMatrix;
}

struct PVS_OUT
{
	float4 Pos : SV_POSITION;
	float4 Normal : NORMAL;
	float2 Tex : TEXCOORD;
};

struct PGS_OUT
{
	float4 Pos : SV_POSITION;
	float4 Normal : NORMAL;
	float2 Tex : TEXCOORD0;
};

static const float4 positions[4] =
{
	float4(-0.1f, 0.1f, 0.0f, 0.0f),
	float4(0.1f, 0.1f, 0.0f, 0.0f),
	float4(-0.1f, -0.1f, 0.0f, 0.0f),
	float4(0.1f, -0.1f, 0.0f, 0.0f)
};

static const float2 uv[4] =
{
	float2(0.0f, 1.0f),
	float2(1.0f, 1.0f),
	float2(0.0f, 0.0f),
	float2(1.0f, 0.0f),
};

[maxvertexcount(4)]
void PGS_main(point PVS_OUT input[1], inout TriangleStream<PGS_OUT> output)
{
	PGS_OUT element;

	float4 viewPos = mul(input[0].Pos, viewMatrix);

	for (uint i = 0; i < 4; i++)
	{
		element.Pos = mul(viewPos + positions[i], projMatrix);

		element.Normal = input[0].Normal;;
		element.Tex = uv[i];

		output.Append(element);
	}
}