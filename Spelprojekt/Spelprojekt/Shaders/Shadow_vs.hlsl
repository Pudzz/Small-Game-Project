#include "Constants.hlsl"

struct VS_INPUT
{
	float4 position : POSITION;
	float2 uv		: TEXCOORD0;
	float3 normal	: NORMAL;
	float3 tangent	: TANGENT;
	float4 weights : BONEWEIGHTS;
	unsigned int4 IDS : BONEINDICES;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	output.position = mul(input.position, mvp);

	if (input.weights.x > 0)
		output.position = mul(mul(input.position, boneTransforms[input.IDS.x]) * input.weights.x, mvp);

	return output;
}