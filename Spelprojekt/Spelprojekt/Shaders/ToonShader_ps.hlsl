#include "Constants.hlsl"

Texture2D m_albedoMap : register(t0);
Texture2D m_normMap : register(t1);

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

	//
	//output.light = float4(mat_ambient.xyz, mat_shininess);

	output.light = mul(mul(float4(input.worldPosition, 1.0f), sunView), sunProjection);


	// The direction of the diffuse light
	float3 DiffuseLightDirection = -sunDirection;

	// The color of the diffuse light
	float4 DiffuseColor = float4(1, 1, 1, 1);
	// The intensity of the diffuse light
	float DiffuseIntensity = 1.0;

	if (hasAlbedoTexture)
	{
		//output.albedo *= float4(m_albedoMap.Sample(m_samplerState, input.uv));

		float intensity = dot(normalize(input.normal), DiffuseLightDirection );
		if (intensity < 0)
			intensity = 0;


		float4 testColor = float4(m_albedoMap.Sample(m_samplerState, input.uv));
		// Discretize the intensity, based on a few cutoff points
		if (intensity > 0.95)
			testColor = float4(1.0, 1, 1, 1.0) * testColor;
		else if (intensity > 0.5)
			testColor = float4(0.8, 0.8, 0.8, 1.0) * testColor;
		else if (intensity > 0.05)
			testColor = float4(0.5, 0.5, 0.5, 1.0) * testColor;
		else
			testColor = float4(0.3, 0.3, 0.3, 1.0) * testColor;

		//testColor = round(intensity * 5) / 5 * testColor;
		output.albedo = testColor;
	}


	output.normal = float4(input.normal, 1.0f);
	if (hasNormalTexture)
	{
		float4 normalSample = m_normMap.Sample(m_samplerState, input.uv);
		normalSample = ((normalSample * 2.0f) - 1.0f);

		float3 bitangent = cross(input.tangent, input.normal);
		float3x3 TBN = float3x3(input.tangent, bitangent, input.normal);
		output.normal = float4(normalize(mul(normalSample.xyz, TBN)), 1.0f);
	}

	output.position = float4(input.worldPosition, 1.0f);
	return output;
}