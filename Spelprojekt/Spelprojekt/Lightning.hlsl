#include "Constants.hlsl"

float4 CalculatePointLight(PointLight light, float3 normal, float3 position, float3 viewDirection)
{
	float3 lightDirection = normalize(light.position - position);
	float diff = saturate(dot(normal, lightDirection));
	float3 reflectDir = reflect(-lightDirection, normal);

	// shinyness
	float spec = pow(max(dot(viewDirection, reflectDir), 0.0), 32);
	float distance = length(light.position - position);

	// https://gamedev.stackexchange.com/questions/56897/glsl-light-attenuation-color-and-intensity-formula
	float attenuation = saturate(1.0 - distance / light.radius);
	attenuation *= attenuation;

	float4 ambient = light.color * attenuation * 0.1f;
	float4 diffuse = light.color * diff * attenuation;
	float4 specular = light.color * spec * attenuation;

	return (ambient + diffuse + specular);
}
