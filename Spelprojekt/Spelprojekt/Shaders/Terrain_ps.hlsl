#include "Constants.hlsl"

Texture2D m_sandAlbedo : register(t0);
Texture2D m_grassAlbedo : register(t1);

Texture2D m_sandNormal : register(t2);
Texture2D m_grassNormal : register(t3);

SamplerState m_samplerState: register(s0);

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
	output.albedo = mat_diffuse;
	output.light = float4(mat_ambient.xyz, mat_shininess);
	output.normal = float4(input.normal, 1.0f);

	const float grassHeightLevel = 7.0f;
	const float blendHeight = 1.0f;

	float from = grassHeightLevel + blendHeight / 2.0f;
	float to = grassHeightLevel - blendHeight / 2.0f;
	float t = saturate((input.worldPosition.y - from) / (to - from));
	
	if (hasAlbedoTexture)
	{
		float4 grass = float4(m_grassAlbedo.Sample(m_samplerState, fmod(input.uv, float2 (1, 1))));
		float4 sand = float4(m_sandAlbedo.Sample(m_samplerState, fmod(input.uv, float2 (1, 1))));
		
		float4 color = lerp(sand, grass, t);
		

		float3 DiffuseLightDirection = -sunDirection;

		float intensity = dot(normalize(input.normal), DiffuseLightDirection);
		if (intensity < 0)
			intensity = 0;

		output.albedo = color;
	}

	if (hasNormalTexture)
	{
		float3 bitangent = cross(input.tangent, input.normal);
		float3x3 TBN = float3x3(input.tangent, bitangent, input.normal);

		float4 normalSample0 = m_sandNormal.Sample(m_samplerState, input.uv);
		normalSample0 = ((normalSample0 * 2.0f) - 1.0f);
		float4 sandNormal = float4(normalize(mul(normalSample0.xyz, TBN)), 1.0f);


		float4 normalSample1 = m_sandNormal.Sample(m_samplerState, input.uv);
		normalSample1 = ((normalSample1 * 2.0f) - 1.0f);
		float4 grassNormal = float4(normalize(mul(normalSample1.xyz, TBN)), 1.0f);

		output.normal = lerp(sandNormal, grassNormal, t);
	}

	output.position = float4(input.worldPosition, 1.0f);
	return output;
}