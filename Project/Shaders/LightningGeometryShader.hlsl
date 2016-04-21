struct DS_OUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};

float4 crossProduct(float4 v1, float4 v2)
{
	//[ V1.y*V2.z - V1.z*V2.y, V1.z*V2.x - V1.x*V2.z, V1.x*V2.y - V1.y*V2.x ]
	float VX, VY, VZ;

	VX = ((v1.y * v2.z) - (v1.z * v2.y));
	VY = ((v1.z * v2.x) - (v1.x * v2.z));
	VZ = ((v1.x * v2.y) - (v1.y * v2.x));

	float4 result = (VX, VY, VZ, 1.0f);
	return result;
}

[maxvertexcount(12)]	//defines the output to be a maximum of x vertices.
void GS_main(line DS_OUT input[2], inout TriangleStream<DS_OUT> OutputStream)
{
	float4 lineDirVec; //Line Direction Vector
	float4 perpendicularVec;

	float lineWidth = 0.15f;

	float4 startPos = input[0].Pos;
	float4 endPos = input[1].Pos;

	lineDirVec = normalize(input[0].Pos - input[1].Pos); //Is normalize needed?
	//perpendicularVec = crossProduct(lineDirVec, OTHERVECTOR);

	//perpendicularVec = XMVector3Orthogonal(lineDirVec);	//Not working in HLSL
	float4 v[4];

	v[0] = (input[1].Pos + float4(lineWidth, -lineWidth, 0.0f, 1.0f));
	v[1] = (input[1].Pos + float4(lineWidth, lineWidth, 0.0f, 1.0f));
	v[2] = (input[0].Pos + float4(-lineWidth, -lineWidth, 0.0f, 1.0f));
	v[3] = (input[0].Pos + float4(-lineWidth, lineWidth, 0.0f, 1.0f));

	//v[0] = (input[1].Pos + (float4(lineWidth, -lineWidth, 0.0f, 1.0f) - lineDirVec));
	//v[1] = (input[1].Pos + (float4(lineWidth, lineWidth, 0.0f, 1.0f) - lineDirVec));
	//v[2] = (input[0].Pos + (float4(-lineWidth, -lineWidth, 0.0f, 1.0f) + lineDirVec));
	//v[3] = (input[0].Pos + (float4(-lineWidth, lineWidth, 0.0f, 1.0f) + lineDirVec));

	for (uint i = 0; i < 4; i++)
	{
		DS_OUT output; //Output of type "struct DS_OUT"

		output.Pos = v[i];
		output.Tex = input[0].Tex;

		OutputStream.Append(output);
	}
	OutputStream.RestartStrip();


	//for (uint i = 0; i < 2; i++)
	//{
	//	DS_OUT output; //Output of type "struct DS_OUT"

	//	output.Pos = input[i].Pos;
	//	output.Tex = input[i].Tex;

	//	OutputStream.Append(output);
	//}

	//OutputStream.RestartStrip(); //If i want a brand new Triangle
}