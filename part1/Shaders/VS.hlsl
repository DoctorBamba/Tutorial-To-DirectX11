struct VS_INPUT
{
    float2 position_2d : POSITION_2D;
    float4 color : COLOR;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color    : COLOR_;
};


VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
    output.position = float4(input.position_2d, 0.0f, 1.0f);
	output.color	= input.color;
	return output;
}
