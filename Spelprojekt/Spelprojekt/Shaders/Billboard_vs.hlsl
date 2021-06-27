#include "Constants.hlsl"

struct VS_INPUT
{
	float4 position : POSITION;
	float2 uv		: TEXCOORD0;
	float3 normal	: NORMAL;
	float3 tangent	: TANGENT;
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

	//Fuck this line of code in particular
	float3 testPos = (objCentre + (input.position.x * cameraRight * 1) + (input.position.y * cameraUp * 1));

	output.position = mul(float4(testPos, 1), vp);
	output.worldPosition = mul(input.position, world).xyz;

	output.tangent = normalize(mul(input.tangent, world).xyz);

	output.normal = input.normal;

	return output;
}