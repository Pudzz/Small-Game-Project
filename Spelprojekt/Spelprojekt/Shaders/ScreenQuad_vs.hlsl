#include "Constants.hlsl"

struct VertexInputType
{
	float4 position : POSITION;
	float2 uv : TEXCOORD0;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
};

PixelInputType main(VertexInputType input)
{
	PixelInputType output;
	output.position = input.position;
	output.uv = input.uv;
	return output;
}