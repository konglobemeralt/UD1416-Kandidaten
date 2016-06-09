Texture2D Texture	: register(t0);
Texture2D AvgLum	: register(t1);
sampler SamplerWrap;

cbuffer mipLevel {
	int4 levels;
};

struct VS_OUT {
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};

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
	return (0.27 * rgb.r) + (0.67 * rgb.g) + (0.06 * rgb.b);
	//return (0.2126 * rgb.r) + (0.7152 * rgb.g) + (0.0722 * rgb.b);
}

float globTMO(float key, float lWhite, float2 uv) {
	float locLum = exp(AvgLum.SampleLevel(SamplerWrap, uv, 0));								// Reinhard Equation 1 (Inner Lw(x,y))
	float worldLum = exp(AvgLum.SampleLevel(SamplerWrap, uv, levels.x));					// Reinhard Equation 1
	float lumScale = (key / worldLum) * locLum;												// Reinhard Equation 2
	float finLum = (lumScale * (1 + lumScale / (lWhite * lWhite))) / (1 + lumScale);		// Reinhard Equation 4

	return finLum;
}

float locTMO(float key, float2 uv) {
	float La;
	float factor = key / exp(AvgLum.SampleLevel(SamplerWrap, uv, levels.x).x);
	float epsilon = 0.001;
	float phi = 8.0;
	float scale[7] = { 1, 2, 4, 8, 16, 32, 64 };
	for (unsigned int i = 0; i < 7; ++i) {
		float v1 = exp(AvgLum.SampleLevel(SamplerWrap, uv, i).x - 1.0) * factor;			// Replaces Gaussian Kernel per Reinhard Equation 5 & 6
		float v2 = exp(AvgLum.SampleLevel(SamplerWrap, uv, i + 1).x - 1.0) * factor;		// Replaces Gaussian Kernel per Reinhard Equation 5 & 6
		if (abs(v1 - v2) / ((key * pow(2, phi) / (scale[i] * scale[i])) + v1) > epsilon) {	// Reinhard Equation 7 & 8
			La = v1;
			break;
		}
		else
			La = v2;
	}

	float locLum = exp(AvgLum.SampleLevel(SamplerWrap, uv, 0));								// Reinhard Equation 1 (Inner Lw(x,y))
	float worldLum = exp(AvgLum.SampleLevel(SamplerWrap, uv, levels.x));					// Reinhard Equation 1
	float lumScale = (key / worldLum) * locLum;												// Reinhard Equation 2

	return lumScale / (1.0 + La);															// Reinhard Equation 9
}

float4 PS_main(VS_OUT input) : SV_TARGET
{
	uint width, heigth;
Texture.GetDimensions(width, heigth);
uint imagesize = width * heigth;
float2 uv = floor(input.Tex * imagesize) / imagesize;
const float delta = 0.00001;

// USER SETTINGS
float key = levels.w * 0.01; //0.72; //exp(AvgLum.SampleLevel(SamplerWrap, uv, levels.x));
float lWhite = 1.0;
////////////////

float3 rgb = Texture.Sample(SamplerWrap, uv);

//AVG LUM
if (levels.y == 1) {
	rgb = Texture.Sample(SamplerWrap, uv).xyz;
	float luminance = GetLuminance(rgb);
	float logLuminance = log(delta + luminance);

	return logLuminance;
}

//FINAL PASS
else {
	float finLum = 1.0;

	//// GLOBAL TONE MAP
	if (levels.z == 0 || levels.z == 3) {
		finLum = globTMO(key, lWhite, uv);

		// APPLY LUMINANCE
		float3 xyY = RGB2xyY(rgb);
		xyY.z *= (finLum + 0.5); // some dumbass had *= here before which gave greater contrasts but looked weeeird
		rgb = xyY2RGB(xyY);
	}


	// LOCAL TONE MAP
	else if (levels.z == 1 || levels.z == 4) {
		finLum = locTMO(key, uv);

		// APPLY LUMINANCE
		float3 xyY = RGB2xyY(rgb);
		xyY.z *= (finLum + 0.5);
		rgb = xyY2RGB(xyY);
	}

	// GAMMA CORRECTION
	float gamma = 1.0 / 2.2;
	rgb = pow(rgb, float3(gamma, gamma, gamma));

	if (levels.z == 3 || levels.z == 4)
		return saturate(finLum);
	return float4(rgb, 1.0);

	// set a 1x1texture as SRV AND RTV write to it only if the current pixel is brighter than the sample.
	// Reinhard source code: https://www.cs.utah.edu/~reinhard/cdrom/source.html
	// https://msdn.microsoft.com/en-us/library/windows/desktop/bb153294(v=vs.85).aspx
	// http://www.cmap.polytechnique.fr/~peyre/cours/x2005signal/hdr_photographic.pdf
	// http://www.nutty.ca/?page_id=352&link=hdr
	// https://imdoingitwrong.wordpress.com/2010/08/19/why-reinhard-desaturates-my-blacks-3/
	// https://books.google.se/books?id=30ZOCgAAQBAJ&pg=PA403&lpg=PA403&dq=implementing+reinhard+dodging+and+burning&source=bl&ots=2Yiqg0IDEL&sig=QHGS1IWqgLhRI7U6yWqI0izg3kI&hl=sv&sa=X&ved=0ahUKEwjiuL-ItazMAhXoDZoKHTTbCkQQ6AEIGzAA#v=onepage&q=implementing%20reinhard%20dodging%20and%20burning&f=false
	// http://liu.diva-portal.org/smash/get/diva2:24136/FULLTEXT01.pdf
	// http://www.ceng.metu.edu.tr/~akyuz/files/hdrgpu.pdf
}
}