struct DS_OUT
{
	float4 Pos : SV_POSITION;
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

float distToLine(float2 pt, float2 a, float2 b)
{
	float dist = distance(a, b);
	float2 v = normalize(b - a);
	float t = dot(v, pt - a);
	float2 spinePoint;

	if (t > dist)
		spinePoint = b;
	else if (t > 0.0f)
		spinePoint = a + (t * v);
	else
		spinePoint = a;

	return distance(pt, spinePoint);
}

float2 to_ScreenSpace(float3 vPos)
{
	vPos = (mul(transpose(WV), float4(vPos, 1.0f))).xyz;
	float4 u = mul(transpose(projection), float4(vPos, 1.0f));
	return u.xy / u.w;
}

[maxvertexcount(48)]	//defines the output to be a maximum of x vertices.
void GS_main(line DS_OUT input[2], inout TriangleStream<GS_OUT> OutputStream)
{
	float radius = 0.08f / input[1].BranchLevel.x;
	float2 sEndpoints[2];
	float2 sPosition;
	float4 p1 = input[0].Pos; //?
	float4 p2 = input[1].Pos;

	//p1 = p1 + float4(seed, 0.0f, seed, 1.0f);
	
	//Pass raytracing inputs to the pixel shader
	sEndpoints[0] = to_ScreenSpace(p1.xyz);
	sEndpoints[1] = to_ScreenSpace(p2.xyz);

	float3 lineDir = normalize(p2 - p1);
	float4 center = ((p1 + p2) / 2);
	//float3 toCam = normalize(camPos - center);
	float3 up = float3(0.0f, 1.0f, 0.0f);
	float3 right = normalize(cross(lineDir, up));

	
	float3 perpVec = normalize(cross(right, lineDir));

	float3 depthVec = normalize(cross(perpVec, lineDir));
	 
	//lineDir = normalize(cross(perpVec, toCam));

	//perpVec = perpVec*radius;
	//lineDir = lineDir*radius;

	GS_OUT v[8];
	float2 uv[8];
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
	//v[0].Pos = p1 + float4((r * perpVec) + (r * depthVec), 1.0f);
	//v[1].Pos = p1 + float4((r * perpVec) + (-r * depthVec), 1.0f);
	//v[2].Pos = p1 + float4((-r * perpVec) + (-r * depthVec), 1.0f);
	//v[3].Pos = p1 + float4((-r * perpVec) + (r * depthVec), 1.0f);

	//v[4].Pos = p2 + float4((r * perpVec) + (r * depthVec), 1.0f);
	//v[5].Pos = p2 + float4((r * perpVec) + (-r * depthVec), 1.0f);
	//v[6].Pos = p2 + float4((-r * perpVec) + (-r * depthVec), 1.0f);
	//v[7].Pos = p2 + float4((-r * perpVec) + (r * depthVec), 1.0f);

	float overlapDivide = 0.5f;

	////Cubes overlap eachother.
	v[0].Pos = p1 + float4((r * perpVec) + (r * depthVec), 1.0f) + float4((-r * overlapDivide) * lineDir, 1.0f);
	v[1].Pos = p1 + float4((r * perpVec) + (-r * depthVec), 1.0f) + float4((-r * overlapDivide) * lineDir, 1.0f);
	v[2].Pos = p1 + float4((-r * perpVec) + (-r * depthVec), 1.0f) + float4((-r * overlapDivide) * lineDir, 1.0f);
	v[3].Pos = p1 + float4((-r * perpVec) + (r * depthVec), 1.0f) + float4((-r * overlapDivide) * lineDir, 1.0f);

	v[4].Pos = p2 + float4((r * perpVec) + (r * depthVec), 1.0f) + float4((r * overlapDivide) * lineDir, 1.0f);
	v[5].Pos = p2 + float4((r * perpVec) + (-r * depthVec), 1.0f) + float4((r * overlapDivide) * lineDir, 1.0f);
	v[6].Pos = p2 + float4((-r * perpVec) + (-r * depthVec), 1.0f) + float4((r * overlapDivide) * lineDir, 1.0f);
	v[7].Pos = p2 + float4((-r * perpVec) + (r * depthVec), 1.0f) + float4((r * overlapDivide) * lineDir, 1.0f);

	////Points Are Lined Up along vertex normal
	//v[0] = p1 + float4(-r * vNor1, 1.0f);
	//v[1] = p1 + float4(r * vNor1, 1.0f);
	//v[2] = p2 + float4(-r * vNor2, 1.0f);
	//v[3] = p2 + float4(r * vNor2, 1.0f);

	uv[0] = float2(0.0f, 0.0f);
	uv[1] = float2(0.0f, 1.0f);
	uv[2] = float2(1.0f, 0.0f);
	uv[3] = float2(1.0f, 1.0f);
	uv[4] = float2(0.0f, 0.0f);
	uv[5] = float2(0.0f, 1.0f);
	uv[6] = float2(1.0f, 0.0f);
	uv[7] = float2(1.0f, 1.0f);

	/////////OUTPUT STAGE///////////////	/////////OUTPUT STAGE///////////////	/////////OUTPUT STAGE///////////////
	/////////OUTPUT STAGE///////////////	/////////OUTPUT STAGE///////////////	/////////OUTPUT STAGE///////////////

	for (uint i = 0; i < 8; i++)
	{
		//v[i].sEndpoints[0] = sEndpoints[0];
		//v[i].sEndpoints[1] = sEndpoints[1];

		v[i].Pos = mul(v[i].Pos, WVP);
		//v[i].Normal = input[i%2].Normal;
		v[i].BranchLevel = input[1].BranchLevel;
		//v[i].BranchLevel = float2(i * 0.1f, i * 0.1f);
		//v[i].BranchLevel = uv[i];

		//v[i].sPos = (v[i].Pos.xy / v[i].Pos.w);
		//v[i].distanceToLine = distToLine(v[i].sPos, v[i].sEndpoints[0], v[i].sEndpoints[1]);
	}

	//OutputStream.Append(v[0]);	OutputStream.Append(v[4]);	OutputStream.Append(v[3]);	OutputStream.Append(v[7]);	OutputStream.RestartStrip();
	//OutputStream.Append(v[1]);	OutputStream.Append(v[5]);	OutputStream.Append(v[0]);	OutputStream.Append(v[4]);	OutputStream.RestartStrip();
	//OutputStream.Append(v[2]);	OutputStream.Append(v[6]);	OutputStream.Append(v[1]);	OutputStream.Append(v[5]);	OutputStream.RestartStrip();
	//OutputStream.Append(v[3]);	OutputStream.Append(v[7]);	OutputStream.Append(v[2]);	OutputStream.Append(v[6]);	OutputStream.RestartStrip();

	//OutputStream.Append(v[0]);	OutputStream.Append(v[3]);	OutputStream.Append(v[1]);	OutputStream.Append(v[2]);	OutputStream.RestartStrip();
	//OutputStream.Append(v[4]);	OutputStream.Append(v[7]);	OutputStream.Append(v[5]);	OutputStream.Append(v[6]);	OutputStream.RestartStrip();



	OutputStream.Append(v[0]);	OutputStream.Append(v[1]);	OutputStream.Append(v[3]);	OutputStream.Append(v[2]);	OutputStream.RestartStrip();
	OutputStream.Append(v[5]);	OutputStream.Append(v[4]);	OutputStream.Append(v[6]);	OutputStream.Append(v[7]);	OutputStream.RestartStrip();
	OutputStream.Append(v[4]);	OutputStream.Append(v[5]);	OutputStream.Append(v[0]);	OutputStream.Append(v[1]);	OutputStream.RestartStrip();
	OutputStream.Append(v[3]);	OutputStream.Append(v[2]);	OutputStream.Append(v[7]);	OutputStream.Append(v[6]);	OutputStream.RestartStrip();

	OutputStream.Append(v[0]);	OutputStream.Append(v[3]);	OutputStream.Append(v[4]);	OutputStream.Append(v[7]);	OutputStream.RestartStrip();
	OutputStream.Append(v[2]);	OutputStream.Append(v[1]);	OutputStream.Append(v[6]);	OutputStream.Append(v[5]);	OutputStream.RestartStrip();
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


//GS_OUT output;
//output.sEndpoints[0] = sEndpoints[0];
//output.sEndpoints[1] = sEndpoints[1];
//output.sPos = (input[1].Pos.xy / input[1].Pos.w);
//
//for (uint i = 0; i < 8; i++)
//{
//	output.Pos = mul(v[i], WVP);
//	//output.Pos = v[i];
//	output.Normal = input[0].Normal;
//	output.Tex = uv[i*0.5];
//	//output.Tex = input[0].Tex;
//
//	OutputStream.Append(output);
//}
//OutputStream.RestartStrip();



//GS_OUT append(GS_OUT v)
//{
//	GS_OUT output;
//
//	output.Pos = v.Pos;
//	output.Normal = v.Normal;
//	output.Tex = v.Tex;
//
//	return output;
//}
//
//GS_OUT connectFace(GS_OUT a, GS_OUT b, GS_OUT c, GS_OUT d)
//{
//	append(a);
//	append(b);
//	append(c);
//	append(d);
//
//	return 
//}