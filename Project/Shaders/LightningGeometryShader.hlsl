struct DS_OUT
{
	float4 Pos : SV_POSITION;
	//float4 Pos2 : POSITION;		//Active if tessellation is active
	//float3 Normal : NORMAL;
	float2 BranchLevel : TEXCOORD;
};

struct GS_OUT
{
	float4 Pos : SV_POSITION;
	//float3 Normal : NORMAL;
	float2 BranchLevel : TEXCOORD;
	//float2 sEndpoints[2] : TEXCOORD2;
	//float2 sPos : TEXCOORD1;
	//float distanceToLine : PSIZE;
};

cbuffer cBuffer : register(b0)
{
	float4x4 WVP;
	float4x4 WV;
	float4x4 World;
	float4x4 projection;
	float4 camPos;
	float lineWidth;
	int density;
	float tessLevel;
	float seed;
	int baseLightningSegments;
};

//float4 crossProduct(float4 v1, float4 v2)
//{
//	//[ V1.y*V2.z - V1.z*V2.y, V1.z*V2.x - V1.x*V2.z, V1.x*V2.y - V1.y*V2.x ]
//	float VX, VY, VZ;
//
//	VX = ((v1.y * v2.z) - (v1.z * v2.y));
//	VY = ((v1.z * v2.x) - (v1.x * v2.z));
//	VZ = ((v1.x * v2.y) - (v1.y * v2.x));
//
//	float4 result = (VX, VY, VZ, 1.0f);
//	return result;
//}

//float2 to_ScreenSpace(float3 vPos)
//{
//	vPos = (mul(viewMatrix, float4(vPos, 1.0f))).xyz;
//	float4 u = mul(projection, float4(vPos, 1.0f));
//	return u.xy / u.w;
//}

//emit/Append function here?

[maxvertexcount(12)]	//defines the output to be a maximum of x vertices.
void GS_main(line DS_OUT input[2], inout TriangleStream<GS_OUT> OutputStream)
{
	const int nrOfPts = 2;	//2 = line, 4 = lineajd;
	float radius = 0.08f / input[1].BranchLevel.x;
	float2 sEndpoints[2];
	float2 sPosition;

	////Pass raytracing inputs to the pixel shader
	//sEndpoints[0] = to_ScreenSpace(input[0].Pos.xyz);
	//sEndpoints[1] = to_ScreenSpace(input[1].Pos.xyz);

	////float4x4 VP = projection * viewMatrix;
	//float4x4 VP = mul(projection, viewMatrix);
	//float4 clipSpace = mul(VP, input[0].Pos);

	float4 p1 = input[0].Pos; //?
	float4 p2 = input[1].Pos;

	float4 center = normalize((p1 + p2) / 2);
	float3 toCam = normalize(camPos - center);
	toCam.y = 0.0f;
	toCam = normalize(toCam);

	float3 up = float3(0.0f, 1.0f, 1.0f);
	float3 right = normalize(cross(toCam, up));

	float3 lineDir = normalize(p2 - p1);
	float3 perpVec = normalize(cross(toCam, lineDir));

	float3 depthVec = normalize(cross(perpVec, lineDir));
	 
	//lineDir = normalize(cross(perpVec, toCam));

	//perpVec = perpVec*radius;
	//lineDir = lineDir*radius;

	float4 v[4];
	float2 uv[4];
	float r = radius;
	//float3 vNor1 = (input[0].Normal);
	//float3 vNor2 = (input[1].Normal);

	//vNor1 = normalize(mul(vNor1, right));
	//vNor2 = normalize(mul(vNor2, right));

	//vNor1 = normalize(cross(vNor1, perpVec));
	//vNor2 = normalize(cross(vNor2, perpVec));

	//Points are perpendicular to the line direction
	//v[0] = p1 + float4(-r * perpVec, 1.0f);
	//v[1] = p1 + float4(r * perpVec, 1.0f);
	//v[2] = p2 + float4(-r * perpVec, 1.0f);
	//v[3] = p2 + float4(r * perpVec, 1.0f);

	////Points Are Lined Up on the Right Axiz
	//v[0] = p1 + float4(-r * right, 1.0f);
	//v[1] = p1 + float4(r * right, 1.0f);
	//v[2] = p2 + float4(-r * right, 1.0f);
	//v[3] = p2 + float4(r * right, 1.0f);

	////Points Are Lined Up along vertex normal
	//v[0] = p1 + float4(-r * vNor1, 1.0f);
	//v[1] = p1 + float4(r * vNor1, 1.0f);
	//v[2] = p2 + float4(-r * vNor2, 1.0f);
	//v[3] = p2 + float4(r * vNor2, 1.0f);

	float overlapDivide = 1.0f;

	////Points are perpendicular to the line direction AND overlap eachother.
	v[0] = (p1 + float4(-r * perpVec, 1.0f)) + float4((-r * overlapDivide) * lineDir, 1.0f);
	v[1] = (p1 + float4(r * perpVec, 1.0f)) + float4((-r * overlapDivide) * lineDir, 1.0f);
	v[2] = (p2 + float4(-r * perpVec, 1.0f)) + float4((r * overlapDivide) * lineDir, 1.0f);
	v[3] = (p2 + float4(r * perpVec, 1.0f)) + float4((r * overlapDivide) * lineDir, 1.0f);

	uv[0] = float2(0.0f, 0.0f);
	uv[1] = float2(0.0f, 1.0f);
	uv[2] = float2(1.0f, 0.0f);
	uv[3] = float2(1.0f, 1.0f);

	/////////OUTPUT STAGE///////////////	/////////OUTPUT STAGE///////////////	/////////OUTPUT STAGE///////////////
	/////////OUTPUT STAGE///////////////	/////////OUTPUT STAGE///////////////	/////////OUTPUT STAGE///////////////
	GS_OUT output;
	//output.sEndpoints[0] = sEndpoints[0];
	//output.sEndpoints[1] = sEndpoints[1];
	//output.sPos = (input[1].Pos.xy / input[1].Pos.w);

	for (uint i = 0; i < 4; i++)
	{
		//output.sEndpoints[0] = sEndpoints[0];
		//output.sEndpoints[1] = sEndpoints[1];
		output.Pos = mul(v[i], WVP);
		//output.Pos = v[i];
		//output.Normal = input[0].Normal;
		if(i < 2)
			output.BranchLevel = input[0].BranchLevel;
		else
			output.BranchLevel = input[1].BranchLevel;
		//output.Tex = input[0].Tex;

		//output.sPos = (v[i].xy / v[i].w);

		//output.distanceToLine = 0.0f;
		OutputStream.Append(output);
	}
	OutputStream.RestartStrip();
}




//v[0] = (input[1].Pos + float4(0, -r, 0.0f, 1.0f));
//v[1] = (input[1].Pos + float4(0, r, 0.0f, 1.0f));
//v[2] = (input[0].Pos + float4(-0, -r, 0.0f, 1.0f));
//v[3] = (input[0].Pos + float4(-0, r, 0.0f, 1.0f));

//int ii = 0;
//for (int j = 0; j < 2; j++)
//{
//	GS_OUT output; //Output of type "struct DS_OUT"
//	output.sEndpoints[0] = sEndpoints[0];
//	output.sEndpoints[1] = sEndpoints[1];
//
//	for (uint i = 0; i < 2; i++)
//	{
//		output.Pos = mul(v[ii], WVP);
//		//output.Pos = v[i];
//		output.Normal = input[0].Normal;
//		output.Tex = input[0].Tex;
//
//		OutputStream.Append(output);
//		ii++;
//	}
//}
//OutputStream.RestartStrip();