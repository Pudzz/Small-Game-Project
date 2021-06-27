#include "Constants.hlsl"

struct VS_INPUT
{
	float4 position : POSITION;
	float2 uv		: TEXCOORD0;
	float3 normal	: NORMAL;
	float3 tangent	: TANGENT;
	float weights : BONEWEIGHTS;
	unsigned int IDS : BONEINDICES;
};

struct VS_OUTPUT
{
	float4 position		 : SV_POSITION;
	float3 normal		 : NORMAL;
	float2 uv			 : TEXCOORD0;
	float3 worldPosition : POSITION;
	float3 tangent		 : TANGENT;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	output.uv = input.uv;

	output.worldPosition = mul(input.position, world).xyz;
	output.position = mul(input.position, mvp);

	output.normal = normalize(mul(input.normal, world).xyz);
	output.tangent = normalize(mul(input.tangent, world).xyz);

	return output;
}

// Entrypoint for animation
VS_OUTPUT animation(VS_INPUT input)
{
	VS_OUTPUT output;

	float4 positions = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float3 normals = float3(0.0f, 0.0f, 0.0f);
	float3 tangents = float3(0.0f, 0.0f, 0.0f);

	// Multiply the vertex position with the bone transform, depending on its ID and weight
	positions += mul(input.position, boneTransforms[input.IDS]) * input.weights;

	// Multiply the vertex normal with the bone transform, depending on its ID and weight
	normals += input.weights * mul(input.normal, (float3x3)boneTransforms[input.IDS]).xyz;

	// Multiply the vertex tangent with the bone transform, depending on its ID and weight
	tangents += input.weights * mul(input.tangent, (float3x3)boneTransforms[input.IDS]).xyz;

	output.worldPosition = mul(positions, world).xyz;
	output.normal = mul(normals, world).xyz;
	output.tangent = mul(tangents, world).xyz;
	output.uv = input.uv;
	output.position = mul(positions, mvp);

	return output;
}