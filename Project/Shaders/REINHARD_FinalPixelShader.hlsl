Texture2D Texture : register(t0);
Texture2D AvgLumSRV : register(t1);
Texture2D MaxLumSRV : register(t2);
sampler SamplerWrap;

cbuffer mipLevel {
	int4 levels;
};

struct VS_OUT {
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};

float ldexp(float x, float exp) {
	return x * pow(2.0, exp);
}

float3 RGBEToRGB(float4 rgbe) {
	if (rgbe.w > 0.0) {
		rgbe *= 255.0;
		float val = ldexp(1.0, rgbe.w - (128.0 + 8.0));
		return rgbe.xyz * val;
	}
	return float3(0.0, 0.0, 0.0);
}

float GetLuminance(float3 rgb) {
	return (0.2126 * rgb.r) + (0.7152 * rgb.g) + (0.0722 * rgb.b);
}

float3 RGB2xyY(float3 rgb) {
	const float3x3 RGB2XYZ = float3x3(	0.4124, 0.3576, 0.1805, 
										0.2126, 0.7152, 0.0722, 
										0.0193, 0.1192, 0.9505);
	float3 XYZ = mul(RGB2XYZ, rgb);
	return float3(XYZ.x / (XYZ.x + XYZ.y + XYZ.z), XYZ.y / (XYZ.x + XYZ.y + XYZ.z), XYZ.y);
}

float3 xyY2RGB(float3 xyY) {
	float3 XYZ = float3(	xyY.x * (xyY.z / xyY.y),
							xyY.z, 
							(1.0 - xyY.x - xyY.y) * (xyY.z / xyY.y)	);
	const float3x3 XYZ2RGB = float3x3(	3.2406,		-1.5372,	-0.4986,
										-0.9689,	1.8758,		0.0415, 
										0.0557,		-0.2040,	1.0570);
	return mul(XYZ2RGB, XYZ);
}

float4 PS_main(VS_OUT input) : SV_TARGET
{
	////follow nuttys code! http://www.nutty.ca/?page_id=352&link=hdr

	float3 rgb = RGBEToRGB(Texture.Sample(SamplerWrap, input.Tex));
	float lum = GetLuminance(rgb);

	//return float4(levels.x, levels.y, levels.z, 1.0);
	////return float4(XYZ.x, XYZ.y, XYZ.z, 1.0);
	////return float4(tex.r, tex.g, tex.b, 1.0);
	//return float4(finLum, finLum, finLum, 1.0);
	////return float4(mip.r, mip.g, mip.b, 1.0);
	//return float4(rgb, 1.0);
	return MaxLumSRV.SampleLevel(SamplerWrap, input.Tex, 9);// levels.x);

}