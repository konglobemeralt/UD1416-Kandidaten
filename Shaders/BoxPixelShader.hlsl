struct BVS_OUT
{
	float4 Pos : SV_POSITION;
};

struct BPS_OUT
{
	float4 Color : SV_Target0;
};

BPS_OUT BPS_main(BVS_OUT input) : SV_Target
{
	BPS_OUT output;

	output.Color = float4(0.0f, 1.0f, 0.0f, 1.0f);

	return output;
}