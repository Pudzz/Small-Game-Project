#include "Constants.hlsl"
Texture2D m_heightMap : register(t0);
SamplerState m_heightMapState: register(s0);

struct VS_OUTPUT
{
	float4 position		 : SV_POSITION;
	float3 normal		 : NORMAL;
	float2 uv			 : TEXCOORD0;
	float3 worldPosition : POSITION;
	float3 tangent		 : TANGENT;
};

struct GBUFFER
{
	float4 albedo	: SV_Target0;
	float4 light	: SV_Target1;
	float4 normal	: SV_Target2;
	float4 position : SV_Target3;
};


GBUFFER main(VS_OUTPUT input) : SV_TARGET
{
	GBUFFER output;
	float4 heightMap = m_heightMap.Sample(m_heightMapState, float2(input.uv)); 
	float4 DiffuseColor = float4(1, 1, 1, 1);
	float3 DiffuseLightDirection = -sunDirection;

	float4 testColor = float4(0.30f, 0.63f, 0.93f, 1.0f);
	
	float intensity = (heightMap.x + heightMap.y + heightMap.z) / 3;
	if (intensity < 0)
		intensity = 0;
	
	intensity += ((input.worldPosition.y-(5.5f)) / 50);
	intensity = saturate(intensity);


	// Discretize the intensity, based on a few cutoff points
	if (intensity > 0.4)
	testColor = float4(1.0, 1, 1, 1.0) * float4(1,1,1,1);
	else if (intensity > 0.3)
	testColor = float4(0.95, 0.95, 0.95, 1.0) * testColor;
	else if (intensity > 0.2)
	testColor = float4(0.88, 0.88, 0.88, 1.0) * testColor;
	else
	testColor = float4(0.8, 0.8, 0.8, 1.0) * testColor;

	testColor = float4(testColor.xyz + (input.worldPosition.y/30),1);
	output.albedo = testColor;

	output.light = float4(mat_ambient.xyz, 0);
	output.normal = float4(input.normal, 1.0f);
	output.position = float4(input.worldPosition, 1.0f);

	return output;
}