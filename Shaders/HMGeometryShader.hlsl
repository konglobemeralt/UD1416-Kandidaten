#define PI (3.141592653589793)

cbuffer cBuffer1 : register(b0)
{
	float4x4 worldMatrix;
	float4x4 viewMatrix;
	float4x4 projMatrix;
	float4x4 viewMatrixInverse;
	float4x4 lightViewMatrix;
	float4x4 lightProjMatrix;
}

struct HMVS_OUT
{
	float4 Pos : SV_POSITION;
	float3 Normal : NORMAL;
	float2 Tex : TEXCOORD;
	float2 BlendTex : TEXCOORD1;
};

struct HMGS_OUT
{
	float4 Pos : SV_POSITION;
	float4 Normal : NORMAL;
	float2 Tex : TEXCOORD0;
	float2 BlendTex : TEXCOORD1;
	float4 LightViewPos : TEXCOORD2;
};

[maxvertexcount(6)]
void HMGS_main(triangle HMVS_OUT input[3] : SV_POSITION, inout TriangleStream<HMGS_OUT> output)
{
	float4 cPos[3];
	float3 faceNormal;

	[unroll]
	for (int i = 0; i < 3; i++){
		cPos[i] = mul(input[i].Pos, mul(worldMatrix, mul(viewMatrix, projMatrix)));
	}

	faceNormal = normalize(cross(float3(cPos[0].xyz - cPos[2].xyz), float3(cPos[1].xyz - cPos[2].xyz)));

	if (dot(-cPos[0].xyz, faceNormal) >= 0.0f){
		[unroll]
		for (uint i = 0; i < 3; i++)
		{
			HMGS_OUT element;

			element.Pos = mul(input[i].Pos, mul(worldMatrix, mul(viewMatrix, projMatrix)));
			element.Normal = float4(input[i].Normal, 0.0f);
			element.Tex = input[i].Tex;
			element.BlendTex = input[i].BlendTex;
			element.LightViewPos = mul(input[i].Pos, mul(worldMatrix, mul(lightViewMatrix, lightProjMatrix)));

			output.Append(element);
		}
	}
}