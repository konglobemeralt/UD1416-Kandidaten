struct DS_OUT
{
	float4 Pos : SV_POSITION;
	float3 Normal : NORMAL;
	float2 Tex : TEXCOORD;
};

cbuffer cBuffer : register(b0)
{
	float4x4 WVP;
	float4x4 World;
	float4 camPos;
	float lineWidth;
	float tessLevel;
	float2 padding;
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

//float2 to_ScreenSpace(float3 vPos)
//{
//	vPos = (modelView * float4(vPos, 1.0f)).xyz;
//	float4 u = projection * float4(vPos, 1.0f);
//	return u.xy / u.w;
//}

//emit/Append function here??????

[maxvertexcount(12)]	//defines the output to be a maximum of x vertices.
void GS_main(line DS_OUT input[2], inout TriangleStream<DS_OUT> OutputStream)
{
	const int nrOfPts = 2;	//2 = line, 4 = lineajd;
	float radius = 0.1f;
	float3 vPos[nrOfPts];
	float3 vNormal[nrOfPts];
	float2 sPos;
	float2 sEndPoints[2];

	for (int h = 0; h < nrOfPts; h++)
	{
		vPos[h] = input[h].Pos;
		vNormal[h] = input[h].Normal;
	}


	float4 lineDirVec; //Line Direction Vector
	float4 perpendicularVec;

	float lineWidth = 0.1f;
	float O = 0.0f;

	float4 startPos = input[0].Pos;
	float4 endPos = input[1].Pos;

	//Pass raytracing inputs to the pixel shader
	//sEndPoints[0] = to_ScreenSpace(vPos[1]);
	//sEndPoints[1] = to_ScreenSpace(vPos[2]);

	////Compute orientation vectors for the two connecting faces:
	//float3 p0, p1, p2, p4;
	//p0 = input[0].Pos;
	//p1 = input[1].Pos;
	////p2 = input[2].Pos;	//Not using Adjecency //Not 4 vertices
	////p3 = input[3].Pos;

	float3 n0 = normalize(vPos[1] - vPos[0]);
	//float3 n1 = normalize(vPos[2] - vPos[1]);
	//float3 n2 = normalize(vPos[3] - vPos[2]);
	//float3 u = normalize(n0 - n1);
	//float3 v = normalize(n1 - n2);

	lineDirVec = normalize(input[0].Pos - input[1].Pos); //Is normalize needed?
	//perpendicularVec = crossProduct(lineDirVec, OTHERVECTOR);

	//perpendicularVec = XMVector3Orthogonal(lineDirVec);	//Not working in HLSL
	float4 v[8];

	//Compute the eight corners
	float r = radius;



	//v[0] = (input[1].Pos);
	//v[1] = (input[1].Pos);
	//v[2] = (input[0].Pos);
	//v[3] = (input[0].Pos);
	//v[4] = (input[1].Pos);
	//v[5] = (input[1].Pos);
	//v[6] = (input[0].Pos);
	//v[7] = (input[0].Pos);

	v[0] = (input[0].Pos + float4(O, O, O, 1.0f));
	v[1] = (input[0].Pos + float4(O, O, O, 1.0f));
	v[2] = (input[0].Pos + float4(O, O, O, 1.0f));
	v[3] = (input[0].Pos + float4(O, O, O, 1.0f));
	v[4] = (input[1].Pos + float4(O, O, O, 1.0f));
	v[5] = (input[1].Pos + float4(O, O, O, 1.0f));
	v[6] = (input[1].Pos + float4(O, O, O, 1.0f));
	v[7] = (input[1].Pos + float4(O, O, O, 1.0f));

	//v[0] = (input[1].Pos + float4(r, -r, 0.0f, 1.0f));
	//v[1] = (input[1].Pos + float4(r, r, 0.0f, 1.0f));
	//v[2] = (input[0].Pos + float4(-r, -r, 0.0f, 1.0f));
	//v[3] = (input[0].Pos + float4(-r, r, 0.0f, 1.0f));

	//v[0] = (input[1].Pos + float4(0, -r, 0.0f, 1.0f));
	//v[1] = (input[1].Pos + float4(0, r, 0.0f, 1.0f));
	//v[2] = (input[0].Pos + float4(-0, -r, 0.0f, 1.0f));
	//v[3] = (input[0].Pos + float4(-0, r, 0.0f, 1.0f));
	//v[4] = (input[1].Pos + float4(0, 0.0f, -r, 1.0f));
	//v[5] = (input[1].Pos + float4(0, 0.0f, r, 1.0f));
	//v[6] = (input[0].Pos + float4(-0, 0.0f, -r, 1.0f));
	//v[7] = (input[0].Pos + float4(-0, 0.0f, r, 1.0f));

	v[0] = (input[1].Pos + float4(O, -r, O, 1.0f));
	v[1] = (input[1].Pos + float4(O, r, O, 1.0f));
	v[2] = (input[0].Pos + float4(-O, -r, O, 1.0f));
	v[3] = (input[0].Pos + float4(-O, r, O, 1.0f));
	v[4] = (input[1].Pos + float4(-r, O, O, 1.0f));
	v[5] = (input[1].Pos + float4(r, O, O, 1.0f));
	v[6] = (input[0].Pos + float4(-r, -O, O, 1.0f));
	v[7] = (input[0].Pos + float4(r, -O, O, 1.0f));


	//v[0] = (input[1].Pos + (float4(r, -r, 0.0f, 1.0f) - lineDirVec));
	//v[1] = (input[1].Pos + (float4(r, r, 0.0f, 1.0f) - lineDirVec));
	//v[2] = (input[0].Pos + (float4(-r, -r, 0.0f, 1.0f) + lineDirVec));
	//v[3] = (input[0].Pos + (float4(-r, r, 0.0f, 1.0f) + lineDirVec));

	for (uint i = 0; i < 8; i++)
	{
		DS_OUT output; //Output of type "struct DS_OUT"

		output.Pos = mul(v[i], WVP);
		output.Normal = input[0].Normal;
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