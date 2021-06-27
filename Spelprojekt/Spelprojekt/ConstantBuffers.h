#pragma once
#include <DirectXMath.h>

#define MAX_LIGHTS 32

const int WORLD_CONSTANT_BUFFER_SLOT = 0;
__declspec(align(16))
struct WorldData
{
	DirectX::XMMATRIX mvp;
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX invWorld;
	DirectX::XMMATRIX invView;
	DirectX::XMMATRIX vp;

	DirectX::XMFLOAT3 centre;
	float pad0;
	DirectX::XMFLOAT3 cameraRight;
	float pad1;
	DirectX::XMFLOAT3 cameraUp;
	float pad2;

	float time;
	float pad[3];
	DirectX::XMMATRIX boneTransforms[70];
};

__declspec(align(16))
struct PointLight
{
	DirectX::XMFLOAT4 color;
	DirectX::XMFLOAT3 position;
	float radius;
};

const int LIGHT_CONSTANT_BUFFER_SLOT = 1;
__declspec(align(16))
struct LightData
{
	DirectX::XMMATRIX worldToView;
	DirectX::XMFLOAT4 sunColor;
	DirectX::XMFLOAT3 sunDirection;
	float sunIntensity;

	DirectX::XMMATRIX sunView;
	DirectX::XMMATRIX sunProjection;


	DirectX::XMFLOAT3 eyePosition;
	int pointLightCount;
	PointLight pointLights[MAX_LIGHTS];

};

const int MATERIAL_CONSTANT_BUFFER_SLOT = 2;
__declspec(align(16))
struct MaterialData
{
	DirectX::XMFLOAT4 ambient;
	DirectX::XMFLOAT4 diffuse;
	DirectX::XMFLOAT4 specular;

	float shininess;
	float md_pad[3];

	// bools för vilka textures som används
	bool hasAlbedoTexture;
	bool hasNormalTexture;
};

const int SSAO_CONSTANT_BUFFER_SLOT = 3;
__declspec(align(16))
struct SSAOBuffer
{
	DirectX::XMFLOAT2 screenSize;
	float ssao_radius;
	float ssao_scale;
	float ssao_bias;
	float ssao_intensity;

	float ld_pad[2];
};