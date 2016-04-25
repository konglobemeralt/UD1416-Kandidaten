Texture2D Texture	: register(t0);
Texture2D AvgLum	: register(t1);
Texture2D MaxLum	: register(t2);
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

float frexp(float x, out float exp) {
	exp = ceil(log2(x));
	return(x * exp2(-exp));
}

float3 RGBEToRGB(float4 rgbe) {
	if (rgbe.w > 0.0) {
		rgbe *= 255.0;
		float val = ldexp(1.0, rgbe.w - (128.0 + 8.0));
		return rgbe.xyz * val;
	}
	return float3(0.0, 0.0, 0.0);
}

float4 RGBToRGBE(float3 rgb) {
	float val = max(max(rgb.x, rgb.y), rgb.z);
	if (val < 0.00001)
		return float4(0.0, 0.0, 0.0, 0.0);
	else {
		float exp;
		float4 rgbe = float4(0.0, 0.0, 0.0, 0.0);
		val = frexp(val, exp) * 256.0 / val;
		rgbe.xyz = rgb.xyz * val;
		rgbe.w = exp + 128.0;
		return (rgbe / 255.0);
	}
}

float3 RGB2xyY(float3 rgb) {
	const float3x3 RGB2XYZ = float3x3(0.4124, 0.3576, 0.1805,
		0.2126, 0.7152, 0.0722,
		0.0193, 0.1192, 0.9505);
	float3 XYZ = mul(RGB2XYZ, rgb);
	return float3(XYZ.x / (XYZ.x + XYZ.y + XYZ.z), XYZ.y / (XYZ.x + XYZ.y + XYZ.z), XYZ.y);
}

float3 xyY2RGB(float3 xyY) {
	float3 XYZ = float3(xyY.x * (xyY.z / xyY.y),
		xyY.z,
		(1.0 - xyY.x - xyY.y) * (xyY.z / xyY.y));
	const float3x3 XYZ2RGB = float3x3(3.2406, -1.5372, -0.4986,
		-0.9689, 1.8758, 0.0415,
		0.0557, -0.2040, 1.0570);
	return mul(XYZ2RGB, XYZ);
}

float GetLuminance(float3 rgb) {
	return (0.2126 * rgb.r) + (0.7152 * rgb.g) + (0.0722 * rgb.b);
}

//manual mipmap?
float3 BilinearFilter(float2 uv, float mipLvlBias) {
	uint width, heigth;
	Texture.GetDimensions(width, heigth);
	uint imagesize = width * heigth;
	float2 texelSize = float2(1 / width, 1 / heigth);
	
	float2 fUv = frac(uv * imagesize);
	uv = floor(uv * imagesize) / imagesize;

	float3 tl = RGBEToRGB(Texture.SampleLevel(SamplerWrap, uv, mipLvlBias));
	float3 tr = RGBEToRGB(Texture.SampleLevel(SamplerWrap, uv + float2(texelSize.x, 0.0), mipLvlBias));
	float3 bl = RGBEToRGB(Texture.SampleLevel(SamplerWrap, uv + float2(0.0, texelSize.y), mipLvlBias));
	float3 br = RGBEToRGB(Texture.SampleLevel(SamplerWrap, uv + float2(texelSize.x, texelSize.y), mipLvlBias));

	float3 a = lerp(tl, tr, fUv.x);
	float3 b = lerp(bl, br, fUv.x);
	return lerp(a, b, fUv.y);
}	  
//manual mipmap?
float4 maxFilter(float2 uv, float mipLvlBias) {
	uint width, heigth;
	Texture.GetDimensions(width, heigth);
	uint imagesize = width * heigth;
	float2 texelSize = float2(1 / width, 1 / heigth);

	uv = floor(uv * imagesize) / imagesize;

	float4 ptl = Texture.SampleLevel(SamplerWrap, uv, mipLvlBias);
	float4 ptr = Texture.SampleLevel(SamplerWrap, uv + float2(texelSize.x, 0.0), mipLvlBias);
	float4 pbl = Texture.SampleLevel(SamplerWrap, uv + float2(0.0, texelSize.y), mipLvlBias);
	float4 pbr = Texture.SampleLevel(SamplerWrap, uv + float2(texelSize.x, texelSize.y), mipLvlBias);

	float3 tl = RGBEToRGB(ptl);
	float3 tr = RGBEToRGB(ptr);
	float3 bl = RGBEToRGB(pbl);
	float3 br = RGBEToRGB(pbr);

	float ltl = GetLuminance(tl);
	float ltr = GetLuminance(tr);
	float lbl = GetLuminance(bl);
	float lbr = GetLuminance(br);

	float maxLum = max(max(max(ltl, ltr), lbl), lbr);
	if (ltl == maxLum)
		return ptl;
	else if (ltr == maxLum)
		return ptr;
	else if (lbl = maxLum)
		return pbl;
	else
		return pbr;
}

//float4 GetMaxLum(float2 uv) {
//	float newLum = GetLuminance(RGBEToRGB(Texture.Sample(SamplerWrap, uv)));
//	float oldLum = GetLuminance(RGBEToRGB(MaxLum.Sample(SamplerWrap, float2(0.0, 0.0))));
//	if (newLum > oldLum)
//		return float4(newLum, newLum, newLum, 1.0);
//	else return float4(oldLum, oldLum, oldLum, 1.0);
//}

float4 PS_main(VS_OUT input) : SV_TARGET
{
	uint width, heigth;
	Texture.GetDimensions(width, heigth);
	uint imagesize = width * heigth;
	const float delta = 1.0;
	
	//MAX LUM
	if (levels.y == 0) {
		float3 RGB = RGBEToRGB(Texture.Sample(SamplerWrap, input.Tex));
		float3 MaxL = RGBEToRGB(MaxLum.Sample(SamplerWrap, float2(0.0, 0.0)));
		float luminance = GetLuminance(RGB);
		float logLuminance = log(delta + luminance);
		if(MaxL.x < 0.00001)
			return RGBToRGBE(logLuminance);
		if (logLuminance > MaxL.x)
			return RGBToRGBE(float3(logLuminance, logLuminance, logLuminance));//logLuminance);
		else
			return RGBToRGBE(MaxL);
		maxlum not working properly!
		//return maxFilter(input.Tex, levels.x);
	}

	//AVG LUM
	else if (levels.y == 1) {
		float3 RGB = RGBEToRGB(Texture.Sample(SamplerWrap, input.Tex));
		float luminance = GetLuminance(RGB);
		float logLuminance = log(delta + luminance);

		return RGBToRGBE(logLuminance);

		/*float4 rgb = float4(BilinearFilter(input.Tex, levels.x), 1.0);
		return RGBToRGBE(rgb);*/
	}

	//FINAL PASS
	else {		//if (levels.y == 2)
		float avgLum = GetLuminance(RGBEToRGB(AvgLum.SampleLevel(SamplerWrap, input.Tex, levels.x)));
		//avgLum = exp(1/imageSize)
		////follow nuttys code! http://www.nutty.ca/?page_id=352&link=hdr

		//float3 rgb = RGBEToRGB(Texture.Sample(SamplerWrap, input.Tex));
		//float lum = GetLuminance(rgb);


		return AvgLum.Sample(SamplerWrap, input.Tex);
		//return float4(RGBEToRGB(MaxLum.Sample(SamplerWrap, float2(0.0, 0.0))), 1.0);
		//return float4(RGBEToRGB(AvgLum.Sample(SamplerWrap, input.Tex)), 1.0);
		//return float4(RGBEToRGB(MaxLumSRV.SampleLevel(SamplerWrap, input.Tex, levels.x).xyz), 1.0);// levels.x);

		// set a 1x1texture as SRV AND RTV write to it only if the current pixel is brighter than the sample.
		// Reinhard source code: https://www.cs.utah.edu/~reinhard/cdrom/source.html
		// https://msdn.microsoft.com/en-us/library/windows/desktop/bb153294(v=vs.85).aspx
		// http://www.cmap.polytechnique.fr/~peyre/cours/x2005signal/hdr_photographic.pdf
		// http://www.nutty.ca/?page_id=352&link=hdr
	}
}