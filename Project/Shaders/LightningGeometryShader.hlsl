struct DS_OUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};

[maxvertexcount(8)]	//defines the output to be a maximum of x vertices.
void GS_main(triangle DS_OUT input[3], inout TriangleStream<DS_OUT> OutputStream)
{
	float4 lineDirVec; //Line Direction Vector
	float4 perpendicularVec;

	float4 startPos = input[0].Pos;
	float4 endPos = input[1].Pos;

	lineDirVec = normalize(input[0].Pos - input[1].Pos); //Is normalize needed?

	//perpendicularVec = XMVector3Orthogonal(lineDirVec);	//Not working in HLSL


	//float4 v[4];
	//v[0] = float4(input[1].Pos + float3(0.1f, -0.1f, 0.0f), 1.0f);
	//v[1] = float4(input[1].Pos + float3(0.1f, 0.1f, 0.0f), 1.0f);
	//v[2] = float4(input[0].Pos + float3(-0.1f, -0.1f, 0.0f), 1.0f);
	//v[3] = float4(input[0].Pos + float3(-0.1f, 0.1f, 0.0f), 1.0f);

	//for (uint i = 0; i < 4; i++)
	//{
	//	DS_OUT output; //Output of type "struct DS_OUT"

	//	output.Pos = v[i];
	//	output.Tex = input[0].Tex;

	//	OutputStream.Append(output);
	//}
	//OutputStream.RestartStrip();


	for (uint i = 0; i < 3; i++)
	{
		DS_OUT output; //Output of type "struct DS_OUT"

		output.Pos = input[i].Pos;
		output.Tex = input[i].Tex;

		OutputStream.Append(output);
	}

	OutputStream.RestartStrip(); //If i want a brand new Triangle
}