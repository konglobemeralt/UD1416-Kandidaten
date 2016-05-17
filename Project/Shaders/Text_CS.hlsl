struct Corners
{
	float2 leftup;
	float2 leftdown;
	float2 rightup;
	float2 rightdown;
};

// Write the new position of every particle
RWStructuredBuffer<Corners> output : register(u0);
//AppendStructuredBuffer<Corners> output : register(u0);
Texture2D tex : register(t0);
SamplerState Linear : register(s0);
 
//[numthreads(32, 19, 1)]
[numthreads(1, 1, 1)]
void CS_main(uint3 id : SV_DispatchThreadID)
{
	float sizeX = 1024.0f;
	float sizeY = 760.0f;
	float small = 0.1f;

	//Corners corners;
	//corners.leftup		=	float2(0.0f, 0.0f);
	//corners.leftdown	=	float2(0.5f, 1.0f);
	//corners.rightup		=	float2(1.0f, 0.0f);
	//corners.rightdown	=	float2(1.0f, 1.0f);
	//output.Append(corners);

	output[0].leftup	=	float2(0.0f, 0.0f);
	output[0].leftdown	=	float2(0.0f, 1.0f);
	output[0].rightup	=	float2(1.0f, 0.0f);
	output[0].rightdown	=	float2(1.0f, 1.0f);

	//float2 pixel = float2(1.0f, 1.0f);
	float2 pixels[9];
	float4 colors[9];
	//pixel.x /= sizeX;
	//pixel.y /= sizeY;
	//float4 uv = tex.SampleLevel(Linear, pixel, 0);
	//if (1.0f - uv.x < small && uv.w > small)
	//{
	//	output[id.x].leftdown.x = id.x / sizeX;
	//	output[0].leftdown.x = output[id.x].leftdown.x;
	//}

	// Pixel layout
	// | 1 | 2 | 3 |
	// | 4 | 0 | 5 |
	// | 6 | 7 | 8 |

	for (unsigned int i = 0; i < sizeX; i++)
	{
		for (unsigned int j = 0; j < sizeY; j++)
		{
			pixels[0] = float2(saturate(float(i)), saturate(float(j)));
			colors[0] = tex.SampleLevel(Linear, pixels[0], 0);

			pixels[1] = float2(saturate(float(i - 1)), saturate(float(j - 1)));
			colors[1] = tex.SampleLevel(Linear, pixels[1], 0);

			pixels[2] = float2(saturate(float(i)), saturate(float(j - 1)));
			colors[2] = tex.SampleLevel(Linear, pixels[2], 0);

			pixels[3] = float2(saturate(float(i + 1)), saturate(float(j + 1)));
			colors[3] = tex.SampleLevel(Linear, pixels[3], 0);

			pixels[4] = float2(saturate(float(i - 1)), saturate(float(j)));
			colors[4] = tex.SampleLevel(Linear, pixels[4], 0);

			pixels[5] = float2(saturate(float(i + 1)), saturate(float(j)));
			colors[5] = tex.SampleLevel(Linear, pixels[5], 0);

			pixels[6] = float2(saturate(float(i - 1)), saturate(float(j + 1)));
			colors[6] = tex.SampleLevel(Linear, pixels[6], 0);

			pixels[7] = float2(saturate(float(i)), saturate(float(j + 1)));
			colors[7] = tex.SampleLevel(Linear, pixels[7], 0);

			pixels[8] = float2(saturate(float(i + 1)), saturate(float(j + 1)));
			colors[8] = tex.SampleLevel(Linear, pixels[8], 0);

			// Left edge
			if (colors[4].w < small && colors[0].w > small)
			{
				output[0].leftdown.x = float(i) / sizeX;
				output[0].leftdown.x = 0.0f;
			}

			//// Left upper corner
			//if (colors[4].w < small && colors[2].w < small && colors[0].w > small)
			//{
			//	output[0].leftdown.x = float(i) / sizeX;
			//	output[0].leftdown.x = 0.0f;
			//}

			//if (0.0f - colors[0].x < small && colors[0].w > small &&
			//	0.0f - colors[1].y < small)
			//{
			//	output[0].leftdown.x = float(i) / sizeX;
			//	output[0].leftdown.x = 0.0f;
			//}
		}
	}
}