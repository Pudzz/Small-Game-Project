#include "Constants.hlsl"
#include "noiseSimplex.cginc"

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


float GetWaterHeightAtPoint(float x, float z)
{
	const float amplitude = 0.20f;
	const float noiseScale = 30.0f;
	const float speed = 0.2f;

	x = (x / noiseScale) + time * speed;
	z = (z / noiseScale) + time * speed;
	return snoise(float2(x,z)) * amplitude;
}

float3 SampleWaterNormal(float x, float z)
{
	float t0 = GetWaterHeightAtPoint(x + 1, z);
	float t1 = GetWaterHeightAtPoint(x - 1, z);
	float t2 = GetWaterHeightAtPoint(x, z + 1);
	float t3 = GetWaterHeightAtPoint(x, z - 1);

	float3 va = normalize(float3(1.0f, 0.0f, t1 - t0));
	float3 vb = normalize(float3(0.0f, 1.0f, t3 - t2));

	return cross(va, vb).rbg;
}


VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	
	float4 pos = input.position;
	pos.y = GetWaterHeightAtPoint(pos.x, pos.z);
	
	output.uv = input.uv;

	output.worldPosition = mul(pos, world).xyz;
	output.position = mul(pos, mvp);

	float3 normal = SampleWaterNormal(pos.x, pos.z);
	output.normal = normalize(mul(normal, world).xyz);
	output.tangent = normalize(mul(input.tangent, world).xyz);

	return output;
}