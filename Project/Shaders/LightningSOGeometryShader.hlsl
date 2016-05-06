struct DS_OUT
{
	float4 Pos : SV_POSITION;
	//float3 Normal : NORMAL;
	float2 BranchLevel : TEXCOORD;
};

struct GS_OUT
{
	float4 Pos : SV_POSITION;
	float2 BranchLevel : TEXCOORD0;
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

float randomizes1(in float2 uv)
{
	float2 noise = (frac(sin(dot(uv, float2(12.9898, 78.233)*2.0)) * 43758.5453));
	return abs(noise.x + noise.y) * 0.5;
}

float clampMinus1To1(float x)	//In value must be 0-1, use saturate first.
{
	float clamped = (x - 1) + x;
	return clamped; 
}

[maxvertexcount(64)]	//defines the output to be a maximum of x vertices.
void GS_main(line DS_OUT input[2], inout PointStream<GS_OUT> OutputStream)
{
	float4 p1 = input[0].Pos; //?
	float4 p2 = input[1].Pos;

	float nrOfSegments = baseLightningSegments;

	float4 v[64];

	for (int i = 0; i < 64; i++)
	{
		v[i] = float4(0.0f, 0.0f, 0.0f, 1);
	}
	
	//Calculate Line Vectors
	float4 center = ((p1 + p2) / 2);
	float3 lineDir = normalize(p2 - p1);
	float3 up = float3(0.0f, 1.0f, 0.0f);
	float3 right = normalize(cross(lineDir, up));
	float3 left = normalize(cross(right, lineDir));

	float lightningLength = distance(p1, p2);
	float extrudeLength = lightningLength / nrOfSegments;

	float randomScale = 1.7f;
	float branchLength = (extrudeLength * 1.7f);
	float nrOfBranchSegments = 4;
	bool branch = false;

	/////////OUTPUT STAGE///////////////	/////////OUTPUT STAGE///////////////	/////////OUTPUT STAGE///////////////
	/////////OUTPUT STAGE///////////////	/////////OUTPUT STAGE///////////////	/////////OUTPUT STAGE///////////////
	GS_OUT output;
	float4 prevPos = p1, prevPos2 = p1;
	float3 prevDir = float3(0, 0, 0);
	int k = 0, branchLevel = 1, prevK = 0;
	for (int k = 0; k < nrOfSegments; k++)
	{
		for (int i = 0; i < 2; i++)	//For each segment
		{
			output.Pos = float4(p1 + (lineDir * (extrudeLength * (k + i))), 1.0f);
			//output.Pos = lerp(p1, p2, k + i);

			float randOffsX = clampMinus1To1(saturate(randomizes1((k + i) * seed + output.Pos.xz)));
			float randOffsZ = clampMinus1To1(saturate(randomizes1((k + i) * seed + output.Pos.zx)));

			if (branch == true && k == 0 && i == 0)
			{
				randOffsX = clampMinus1To1(saturate(randomizes1((prevK + i) * seed + output.Pos.xz)));
				randOffsZ = clampMinus1To1(saturate(randomizes1((prevK + i) * seed + output.Pos.zx)));
			}
			float4 newPos = float4((randOffsX * right) + (randOffsZ * left), 1.0f);

			//if (branch == true && k == 0 && i == 0)
			//	output.Pos = output.Pos + (newPos * randomScale);

			//output.Pos = float4(p1 + (lineDir * (extrudeLength * (k + i))), 1.0f);
			if (k != 0 || k != nrOfSegments)	//All points except the first and last gets a random offset
				output.Pos = output.Pos + (newPos * randomScale);	//Applies Random Offset To Joint

			output.BranchLevel = float2(branchLevel, 0);
			OutputStream.Append(output);

			if (i != 0)
			{
				prevDir = normalize(output.Pos - prevPos);
				prevPos = output.Pos;
				prevPos2 = output.Pos;
			}

			if (k % 3 == true && k != nrOfSegments && k != 0 && i != 0 && branch == false)
			{
				prevK = k;
				k = 0;
				nrOfSegments = nrOfBranchSegments;	//Will make the first for-loop run 2 times. 
				branchLevel = 2;
				p1 = prevPos;
				lineDir = prevDir;
				extrudeLength = branchLength;
				branch = true;
			}
			if (k == nrOfBranchSegments - 1 && branch == true && i != 0)
			{
				//Reset
				k = prevK;
				nrOfSegments = baseLightningSegments;
				branchLevel = 1;
				p1 = input[0].Pos;
				lineDir = normalize(p2 - p1);
				extrudeLength = lightningLength / nrOfSegments;
				branch = false;
			}
		}
		
		//if (k % 3 == true && k != nrOfSegments && k != 0)
		//{
		//	float randOffsX = randomizes1(k + seed - p1.xz);
		//	float randOffsZ = randomizes1(k + seed * p2.xz);
		//	float4 newPos = float4((randOffsX * right) + (randOffsZ * left), 1.0f);

		//	output.Pos = float4((prevPos + (prevDir * branchLength)), 1.0f);
		//	output.Pos = output.Pos + (newPos * 0.2f);
		//	output.BranchLevel = float2(2, 0);
		//	OutputStream.Append(output);
		//		
		//	output.Pos = prevPos2;
		//	output.BranchLevel = float2(2, 0);
		//	OutputStream.Append(output);
		//}
	}
}

//while (k <= nrOfSegments)
//{
//	float randOffsX = randomizes1(k * seed + p1.xz);
//	float randOffsZ = randomizes1(k * seed + p2.xz);
//	randOffsX = saturate(randOffsX);
//	randOffsZ = saturate(randOffsZ);
//	randOffsX = clampMinus1To1(randOffsX);
//	randOffsZ = clampMinus1To1(randOffsZ);
//
//	float4 newPos = float4((randOffsX * right) + (randOffsZ * left), 1.0f);
//
//	if (k == 0)	//If its the first point
//	{
//		output.Pos = input[0].Pos;
//		output.BranchLevel = float2(1, 0);
//		OutputStream.Append(output);
//		prevPos = output.Pos;
//	}
//	else if (k == nrOfSegments)	//If its the last point
//	{
//		output.Pos = input[1].Pos;
//		output.BranchLevel = float2(1, 0);
//		OutputStream.Append(output);
//	}
//	else
//	{
//		for (int i = 0; i < 2; i++)
//		{
//			output.Pos = float4(p1 + (lineDir * (extrudeLength * k)), 1.0f);
//			output.Pos = output.Pos + (newPos * randomScale);	//Applies Random Offset To Joint
//			output.BranchLevel = float2(1, 0);
//
//			OutputStream.Append(output);
//			if (i != 0)
//			{
//				prevDir = normalize(output.Pos - prevPos);
//				prevPos = output.Pos;
//				prevPos2 = output.Pos;
//			}
//		}
//
//		if (k % 3 == true && k != nrOfSegments && k != 0)
//		{
//			randOffsX = randomizes1(k + seed - p1.xz);
//			randOffsZ = randomizes1(k + seed * p2.xz);
//			newPos = float4((randOffsX * right) + (randOffsZ * left), 1.0f);
//
//
//			output.Pos = float4((prevPos + (prevDir * branchLength)), 1.0f);
//			output.Pos = output.Pos + (newPos * 0.2f);
//			output.BranchLevel = float2(2, 0);
//			OutputStream.Append(output);
//
//			output.Pos = prevPos2;
//			output.BranchLevel = float2(1, 0);
//			OutputStream.Append(output);
//		}
//	}
//	k++;
//}



//
//float branches = 1;
//for (int b = 0; b < branches; b++)
//{
//	for (int i = 0; i < 2; i++)	//For each segment
//	{
//		output.Pos = float4(p1 + (lineDir * (extrudeLength * (k + i))), 1.0f);
//		//output.Pos = lerp(p1, p2, k + i);
//
//		float randOffsX = clampMinus1To1(saturate(randomizes1((k + i) * seed + output.Pos.xz)));
//		float randOffsZ = clampMinus1To1(saturate(randomizes1((k + i) * seed + output.Pos.zx)));
//		float4 newPos = float4((randOffsX * right) + (randOffsZ * left), 1.0f);
//
//		//output.Pos = float4(p1 + (lineDir * (extrudeLength * (k + i))), 1.0f);
//		if (k != 0 || k != nrOfSegments)	//All points except the first and last gets a random offset
//			output.Pos = output.Pos + (newPos * randomScale);	//Applies Random Offset To Joint
//		output.BranchLevel = float2(branchLevel, 0);
//		OutputStream.Append(output);
//
//		if (i != 0)
//		{
//			prevDir = normalize(output.Pos - prevPos);
//			prevPos = output.Pos;
//			prevPos2 = output.Pos;
//		}
//
//		if (k % 3 == true && k != nrOfSegments && k != 0 && i != 0)
//		{
//			branches = 2;	//Will make the first for-loop run 2 times. 
//			branchLevel = 2;
//			p1 = prevPos;
//			lineDir = prevDir;
//			extrudeLength = branchLength;
//
//		}
//		if (branches == 2)
//		{
//			//Reset
//			p1 = input[0].Pos;
//			branchLevel = 1;
//			lineDir = normalize(p2 - p1);
//			extrudeLength = lightningLength / nrOfSegments;
//		}
//	}
//}