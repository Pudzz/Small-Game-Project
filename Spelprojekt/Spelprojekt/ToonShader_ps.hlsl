#include "Constants.hlsl"

Texture2D m_albedoMap : register(t0);
Texture2D m_normMap : register(t1);

SamplerState m_samplerState: register(s0);
SamplerState m_normSamplerState : register(s1);

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

	if (hasAlbedoTexture)
	{

		float4 test = float4(m_albedoMap.Sample(m_samplerState, input.uv));


	}

	if (hasNormalTexture)
	{
		float4 normalSample = m_normMap.Sample(m_normSamplerState, input.uv);
		normalSample = ((normalSample * 2.0f) - 1.0f);

		float3 bitangent = cross(input.tangent, input.normal);
		float3x3 TBN = float3x3(input.tangent, bitangent, input.normal);
		output.normal = float4(normalize(mul(normalSample.xyz, TBN)), 1.0f);
	}

	output.position = float4(input.worldPosition, 1.0f);
	return output;
}