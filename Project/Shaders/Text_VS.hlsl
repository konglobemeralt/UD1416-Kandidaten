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

cbuffer Matrix : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
	float4 useMatrices;
}

VS_OUT VS_main(VS_IN input)
{
	VS_OUT output;
	if(useMatrices.x == 1)
		output.Pos = mul(float4(input.Pos, 1.0f), world);
	else if(useMatrices.x == 2)
		output.Pos = mul(float4(input.Pos, 1.0f), mul(world, view));
	else if (useMatrices.x == 3)
		output.Pos = mul(float4(input.Pos, 1.0f), mul(world, mul(view, projection)));
	else
		output.Pos = float4(input.Pos, 1.0f);
	output.Tex = input.Tex;

	return output;
}