Texture2D FirstSRV;
sampler SamplerWrap;

cbuffer mipLevel {
	int4 levels;
};

struct VS_OUT {
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};

float4 PS_main(VS_OUT input) : SV_TARGET
{
	float key = 0.25; //exposure setting
	float d = 1.0;
	float3 tex = FirstSRV.SampleLevel(SamplerWrap, input.Tex, 0).rgb;
	float locLum = (0.2126 * tex.r) + (0.7152 * tex.g) + (0.0722 * tex.b);
	float3 mip = FirstSRV.SampleLevel(SamplerWrap, input.Tex, levels.x).rgb;
	float avgL = exp(mip.r * log(d + locLum));

	float maxL = mip.g;

	float lumScale = key / avgL * locLum;

	//follow nuttys code! http://www.nutty.ca/?page_id=352&link=hdr
	//float max = max(max(tex.r, tex.g), tex.b);
	//if (max < 0.00001)
	//	max = 0.0;
	float finLum = (lumScale * (1 + lumScale / (maxL * maxL))) / (1 + lumScale);

	float3 XYZ = float3x3(0.4124, 0.3576, 0.1805, 0.2126, 0.7152, 0.0722, 0.0193, 0.1192, 0.9505) * float3x1(tex.x, tex.y, tex.z);
	float3 xyY = float3(XYZ.x / (XYZ.x + XYZ.y + XYZ.z), XYZ.y / (XYZ.x + XYZ.y + XYZ.z), XYZ.y);
	xyY.z *= finLum;

	XYZ = float3(	xyY.x * (xyY.z / xyY.y), 
					xyY.z, 
					(1.0 - xyY.x - xyY.y) * (xyY.z / xyY.y)	);

	float3 RGB = float3x3(	3.2406,		-1.5372,	-0.4986, 
							-0.9689,	1.8758,		0.0415, 
							0.0557,		-0.2040,	1.0570)	 * float3x1(XYZ.x, XYZ.y, XYZ.z);

	return float4(RGB.x, RGB.y, RGB.z, 1.0);
	
	//return float4(XYZ.x, XYZ.y, XYZ.z, 1.0);
	//return float4(tex.r, tex.g, tex.b, 1.0);
	//return float4(avgL, avgL, avgL, 1.0);
	//return float4(mip.r, mip.g, mip.b, 1.0);
	
	
	//return FirstSRV.SampleLevel(SamplerWrap, input.Tex, levels.x);

}