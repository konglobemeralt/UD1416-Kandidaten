struct GS_OUT {
	float4 Pos : SV_POSITION;
	//float3 Normal : NORMAL;
	float2 BranchLevel : TEXCOORD;
	//float2 sEndpoints[2] : TEXCOORD2;
	//float2 sPos : TEXCOORD1;
	//float distanceToLine : PSIZE;
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

float UVDistance(float u, float v)
{
	float distance;
	float uv = v;

	if (uv < 0.5f)
		distance = 0.5f - uv;
	else if (uv > 0.5f)
		distance = uv - 0.5f;
	else distance = 0.0f;

	return distance;
}

float4 PS_main(GS_OUT input) : SV_TARGET
{
	//float dist = distToLine(input.sPos, input.sEndpoints[0], input.sEndpoints[1]);
	//
	//float color = (1.0f + (0.000012 * dist));

	//return float4(color, color, color, 1.0f);

	//return float4(input.sEndpoints[0].x, input.sEndpoints[0].y, color, 1.0f);


	//float distanceToL = input.distanceToLine;
	//distanceToL = 1.0f - (15.0f * distanceToL);
	//float color = (distanceToL);
	//return float4(color, color, color, 1.0f);

	///////GLOW
	//float2 ep1 = input.sEndpoints[0];
	//float2 ep2 = input.sEndpoints[1];
	//float dist = distToLine(input.sPos, ep1, ep2);
	//dist = 1.0f - (15.0f * dist);
	//float color = (dist);
	//return float4(color, color, color, 1.0f);
	///////GLOW
	
	//float dist = UVDistance(input.BranchLevel.x, input.BranchLevel.y);
	//dist = (dist * 2);
	//float color = (1.0f - dist);

	//return float4(color, (color * 0.8f), (color * 0.5f), 1.0f);
	//return float4(1, 1, 1, 1.0f);

	float color = 1 / input.BranchLevel.x;
	color *= 2;

	return float4(color * 0.9, color * 0.85, color, 1.0f);
	//return float4(1.0f, 1.0f, 1.0f, 1.0f);
}