#pragma once
#include <DirectXMath.h>
#include <math.h>
#include <algorithm>
#include "MathHelper.h"
//#include "Input.h"
class Transform
{
	const float maxPitch = (90.0f - 0.0001f) * MathHelper::ToRadians;

public:
	static constexpr DirectX::XMVECTOR default_right = { 1,0,0 };
	static constexpr DirectX::XMVECTOR default_up = { 0,1,0 };
	static constexpr DirectX::XMVECTOR default_forward = { 0,0,1 };

public:
	Transform();
	Transform(DirectX::XMVECTOR position, DirectX::XMVECTOR rotation, DirectX::XMVECTOR scale);
	virtual ~Transform();

	void SetPosition(DirectX::XMVECTOR position) { this->position = position; }
	DirectX::XMVECTOR GetPosition()const  { return this->position; }
	DirectX::XMVECTOR GetRotation()const { return this->rotation; }

	DirectX::XMVECTOR TransformDirection(DirectX::XMVECTOR dir) const;
	DirectX::XMMATRIX GetWorldMatrix() const;

	DirectX::XMVECTOR Right() const { return TransformDirection(default_right); }
	DirectX::XMVECTOR Up() const { return TransformDirection(default_up); }
	DirectX::XMVECTOR Forward() const { return TransformDirection(default_forward); }
	
	void Rotate(float pitch, float yaw, float roll);
	void SetRotation(DirectX::XMVECTOR rotation) { this->rotation = rotation; }
	void LookAt(DirectX::XMVECTOR position);
	void LookAtCamera(DirectX::XMVECTOR position);
	void Translate(float x, float y, float z);
	void Translate(DirectX::XMVECTOR translation);

	void RandomizeYRotation();

	void SetScale(float x, float y, float z);
	void SetScale(DirectX::XMVECTOR scale);
	DirectX::XMVECTOR GetScale() const { return this->scale; }

	Transform Clone();
	void ApplyToTransform(Transform& transform);

	void SmoothRotate(DirectX::XMFLOAT3 nextPosition, float fixedDeltaTime, bool changeDir);
private:
	DirectX::XMVECTOR position;
	DirectX::XMVECTOR rotation;
	DirectX::XMVECTOR scale;
	float nextDir = 0;
};
