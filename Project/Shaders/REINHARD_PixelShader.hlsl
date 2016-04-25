Texture2D FirstSRV;
sampler SamplerWrap;

cbuffer mipLevel {
	int4 levels; //x = miplvl, y == 1 = Avg, y == 0 = max
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

float GetLuminance(float3 rgb) {
	return (0.2126 * rgb.r) + (0.7152 * rgb.g) + (0.0722 * rgb.b);
}

float3 BilinearFilter(float2 uv, float mipLvlBias) {
	uint width, heigth;
	FirstSRV.GetDimensions(width, heigth);
	uint imagesize = width * heigth;
	float2 texelSize = float2(1 / width, 1 / heigth);
	
	float2 fUv = frac(uv * imagesize);
	uv = floor(uv * imagesize) / imagesize;

	float3 tl = RGBEToRGB(FirstSRV.SampleLevel(SamplerWrap, uv, mipLvlBias));
	float3 tr = RGBEToRGB(FirstSRV.SampleLevel(SamplerWrap, uv + float2(texelSize.x, 0.0), mipLvlBias));
	float3 bl = RGBEToRGB(FirstSRV.SampleLevel(SamplerWrap, uv + float2(0.0, texelSize.y), mipLvlBias));
	float3 br = RGBEToRGB(FirstSRV.SampleLevel(SamplerWrap, uv + float2(texelSize.x, texelSize.y), mipLvlBias));

	float3 a = lerp(tl, tr, fUv.x);
	float3 b = lerp(bl, br, fUv.x);
	return lerp(a, b, fUv.y);
}

float4 maxFilter(float2 uv, float mipLvlBias) {
	// try looping through to find brightest
	uint width, heigth;
	FirstSRV.GetDimensions(width, heigth);
	uint imagesize = width * heigth;
	float2 texelSize = float2(1 / width, 1 / heigth);

	uv = floor(uv * imagesize) / imagesize;

	float4 ptl = FirstSRV.SampleLevel(SamplerWrap, uv, mipLvlBias);
	float4 ptr = FirstSRV.SampleLevel(SamplerWrap, uv + float2(texelSize.x, 0.0), mipLvlBias);
	float4 pbl = FirstSRV.SampleLevel(SamplerWrap, uv + float2(0.0, texelSize.y), mipLvlBias);
	float4 pbr = FirstSRV.SampleLevel(SamplerWrap, uv + float2(texelSize.x, texelSize.y), mipLvlBias);

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

float4 PS_main(VS_OUT input) : SV_TARGET
{
	uint width, heigth;
	FirstSRV.GetDimensions(width, heigth);
	uint imagesize = width * heigth;

	if (levels.y == 1) {
		float4 rgb = float4(BilinearFilter(input.Tex, levels.x), 1.0);
		return RGBToRGBE(rgb);
	}
	else {
		return maxFilter(input.Tex, levels.x);
		// set a 1x1texture as SRV AND RTV write to it only if the current pixel is brighter than the sample.
		// Reinhard source code: https://www.cs.utah.edu/~reinhard/cdrom/source.html
		// https://msdn.microsoft.com/en-us/library/windows/desktop/bb153294(v=vs.85).aspx
		// http://www.cmap.polytechnique.fr/~peyre/cours/x2005signal/hdr_photographic.pdf
		// http://www.nutty.ca/?page_id=352&link=hdr
	}
}