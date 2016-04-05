#define PI (3.141592653589793)

//cbuffer cBuffer1 : register(b0)
//{
//	float4x4 worldMatrix;
//	float4x4 viewMatrix;
//	float4x4 projMatrix;
//	float4x4 viewMatrixInverse;
//	float4x4 lightViewMatrix;
//	float4x4 lightProjMatrix;
//}

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	//float4 Normal : NORMAL;
	float2 Tex : TEXCOORD;
	//float4 Wpos : POSITION;
};

struct GS_OUT
{
	float4 Pos : SV_POSITION;
	//float4 Normal : NORMAL;
	float2 Tex : TEXCOORD;
	//float4 Wpos : POSITION;
	//float4 LightViewPos : POSITION1;
};

[maxvertexcount(3)]
void GS_main(triangle VS_OUT input[3] : SV_POSITION, inout TriangleStream<GS_OUT> output)
{
	//float4 cPos[3];
	//float3 faceNormal;

	//[unroll]
	//for (int i = 0; i < 3; i++){
	//	cPos[i] = mul(input[i].Pos, mul(worldMatrix, mul(viewMatrix, projMatrix)));
	//}
	//
	//faceNormal = normalize(cross(float3(cPos[0].xyz - cPos[2].xyz), float3(cPos[1].xyz - cPos[2].xyz)));

	//if (dot(-cPos[0].xyz, faceNormal) >= 0.0f){
	//	[unroll]
	//	for (uint i = 0; i < 3; i++)
	//	{
	//		GS_OUT element;

	//		element.Pos = cPos[i];
	//		element.Normal = input[i].Normal;
	//		element.Tex = input[i].Tex;
	//		element.Wpos = input[i].Wpos;
	//		element.LightViewPos = mul(input[i].Pos, mul(worldMatrix, mul(lightViewMatrix, lightProjMatrix)));

	//		output.Append(element);
	//	}
	//}

	for (uint i = 0; i < 3; i++)
	{
		GS_OUT element;

		element.Pos = input[i].Pos;
		element.Tex = input[i].Tex;

		output.Append(element);
	}
}