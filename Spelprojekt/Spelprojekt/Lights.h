#pragma once
#include <DirectXMath.h>
#include <d3d11_1.h>
#include <dxgi.h>
#include "ConstantBuffers.h"
#include "DX11Handler.h"
#include <unordered_map>
#include "Camera.h"

class Lights
{
public:
	Lights(size_t screenWidth, size_t screenHeight, size_t width, size_t height);
	virtual ~Lights();

	void Initialize(DX11Handler* dx11);

	size_t AddPointLight(DirectX::XMFLOAT3 position, DirectX::XMFLOAT4 color, float attenuation);
	void RemovePointLight(size_t id);

	void SetSun(DirectX::XMFLOAT3 direction, DirectX::XMFLOAT4 color, float intensity);

	void SetSunColor(DirectX::XMFLOAT4 color) { lightConstantBuffer.sunColor = color; }
	void SetSunIntensity(float intensity) { lightConstantBuffer.sunIntensity = intensity; }
	void SetSunDirection(DirectX::XMFLOAT3 direction);

	bool IsDirty() const { return this->dirty; }
	void SetDirty() { this->dirty = true; }

	void UpdateConstantBuffer(Camera* camera, ID3D11DeviceContext*);

	Camera* GetSunCamera() { return this->sunCamera; }
	void UpdateCameras(DirectX::XMVECTOR focus, const AABB& bounds);

private:
	size_t width, height;
	Camera* sunCamera;

	size_t pointLight_ID;
	std::unordered_map<size_t, PointLight> pointLightMap;

	bool dirty;
	LightData lightConstantBuffer;
	ID3D11Buffer* lightBuffer_ptr;
};