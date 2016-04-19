struct DS_OUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};

[maxvertexcount(3)]	//defines the output to be a maximum of x vertices.
void GS_main(line DS_OUT input[2], inout LineStream<DS_OUT> OutputStream)
{
	for (uint i = 0; i < 2; i++)
	{
		DS_OUT output; //Output of type "struct DS_OUT"

		output.Pos = input[i].Pos;
		output.Tex = input[i].Tex;

		OutputStream.Append(output);
	}
	OutputStream.RestartStrip(); //Is this needed?
}