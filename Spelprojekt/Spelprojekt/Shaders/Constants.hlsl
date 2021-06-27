
#define MAX_LIGHTS 32

cbuffer WorldConstantBuffer : register(b0)
{
	matrix mvp;
	matrix world;
	matrix invWorld;
	matrix invView;
	matrix vp;
	float3 objCentre;
	float wvb_pad0;

	float3 cameraRight;
	float wvb_pad1;

	float3 cameraUp;
	float wvb_pad2;

	float time;
	float3 wvb_pad3;

	matrix boneTransforms[70];

};

//---- light buffer
struct PointLight
{
	float4 color;
	float3 position;
	float radius;
};

cbuffer LightConstantBuffer : register(b1)
{
	matrix worldToView;
	float4 sunColor;
	float3 sunDirection;
	float sunIntensity;

	matrix sunView;
	matrix sunProjection;


	float3 eyePosition;
	int pointLightCount;

	PointLight pointLights[MAX_LIGHTS];

}

cbuffer MaterialBuffer : register (b2)
{
	float4 mat_ambient;
	float4 mat_diffuse;
	float4 mat_specular;

	float mat_shininess;
	float3 mb_pad;
	//float reflectivity;

	// bools f�r vilka textures som anv�nds
	bool hasAlbedoTexture;
	bool hasNormalTexture;
}

cbuffer SSAOBuffer : register (b3)
{
	// SSAO 
	float2 screenSize;
	float ssao_radius;
	float ssao_scale;
	float ssao_bias;
	float ssao_intensity;
	float2 lp_pad0;
}