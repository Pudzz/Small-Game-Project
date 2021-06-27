#include "Lights.h"

Lights::Lights(size_t screenWidth, size_t screenHeight, size_t width, size_t height) : lightBuffer_ptr(nullptr), width(width), height(height)
{
	this->pointLight_ID = 0;
	this->dirty = false;
	this->lightConstantBuffer = LightData();
	sunCamera = new Camera(90.0f, width, height);
}

Lights::~Lights() {}

void Lights::Initialize(DX11Handler* dx11)
{
	lightBuffer_ptr = dx11->CreateBuffer<LightData>(lightConstantBuffer);
}

size_t Lights::AddPointLight(DirectX::XMFLOAT3 position, DirectX::XMFLOAT4 color, float radius)
{
	size_t id = pointLight_ID++;
	PointLight pl;
	pl.position = position;
	pl.color = color;
	pl.radius = radius;

	pointLightMap.insert({ id, pl });

	dirty = true;
	return id;
}

void Lights::RemovePointLight(size_t id)
{
	auto findResult = pointLightMap.find(id);
	if (findResult != pointLightMap.end())
	{
		pointLightMap.erase(findResult);
		dirty = true;
	}
}

void Lights::SetSun(DirectX::XMFLOAT3 direction, DirectX::XMFLOAT4 color, float intensity)
{
	SetSunDirection(direction);
	SetSunColor(color);
	SetSunIntensity(intensity);
}

void Lights::SetSunDirection(DirectX::XMFLOAT3 direction)
{
	// normalize the direction
	float l = std::sqrtf(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
	lightConstantBuffer.sunDirection = { direction.x / l, direction.y / l, direction.z / l };
}

void Lights::UpdateConstantBuffer(Camera* camera, ID3D11DeviceContext* context)
{
	auto it = pointLightMap.begin();
	size_t index = 0;

	while (it != pointLightMap.end() && index < MAX_LIGHTS)
	{
		lightConstantBuffer.pointLights[index] = (*it).second;
		index++;
		it++;
	}

	lightConstantBuffer.sunView = DirectX::XMMatrixTranspose(sunCamera->GetView());
	lightConstantBuffer.sunProjection = DirectX::XMMatrixTranspose(sunCamera->GetOrthographic());
	lightConstantBuffer.worldToView = DirectX::XMMatrixTranspose(camera->GetView());
	lightConstantBuffer.pointLightCount = (int)pointLightMap.size();

	DirectX::XMStoreFloat3(&lightConstantBuffer.eyePosition, camera->GetTransform().GetPosition());
	context->UpdateSubresource(lightBuffer_ptr, 0, 0, &lightConstantBuffer, 0, 0);
	context->PSSetConstantBuffers(LIGHT_CONSTANT_BUFFER_SLOT, 1, &lightBuffer_ptr);
}

void Lights::UpdateCameras(DirectX::XMVECTOR focus, const AABB& bounds)
{
	const float sunOffsetDistance = 100.0f;
	DirectX::XMVECTOR sunDirection = DirectX::XMLoadFloat3(&lightConstantBuffer.sunDirection);
	sunDirection = { 1,-1,0 };

	DirectX::XMVECTOR offset = DirectX::XMVectorAdd(focus, DirectX::XMVectorScale(DirectX::XMVector3Normalize(sunDirection), sunOffsetDistance * -1));

	sunCamera->GetTransform().SetPosition(offset);
	sunCamera->GetTransform().LookAtCamera(focus);
	sunCamera->UpdateView();
}
