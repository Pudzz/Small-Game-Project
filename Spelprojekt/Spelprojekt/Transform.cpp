#include "Transform.h"


Transform::Transform() : position(DirectX::XMVectorZero()), rotation(DirectX::XMVectorZero()), scale({1,1,1})
{

}

Transform::Transform(DirectX::XMVECTOR position, DirectX::XMVECTOR rotation, DirectX::XMVECTOR scale) : position(position), rotation(rotation), scale(scale)
{
}

Transform::~Transform()
{

}

DirectX::XMVECTOR Transform::TransformDirection(DirectX::XMVECTOR dir) const
{
	DirectX::XMMATRIX rot = DirectX::XMMatrixRotationRollPitchYawFromVector(this->rotation);		// rotation matrix
	return DirectX::XMVector3Normalize(DirectX::XMVector3TransformNormal(dir, rot));	// rotates the direction with the matrix
}

DirectX::XMMATRIX Transform::GetWorldMatrix() const
{
	return (DirectX::XMMatrixMultiply(DirectX::XMMatrixScalingFromVector(this->scale), DirectX::XMMatrixRotationRollPitchYawFromVector(this->rotation) * DirectX::XMMatrixTranslationFromVector(this->position)));
}

void Transform::Rotate(float pitch, float yaw, float roll)
{
	DirectX::XMFLOAT3 rot(pitch, yaw, roll);
	this->rotation = DirectX::XMVectorAdd(this->rotation, DirectX::XMLoadFloat3(&rot));

	float p = std::fmaxf(-maxPitch, std::fminf (DirectX::XMVectorGetByIndex(this->rotation, 0), maxPitch));
	this->rotation = DirectX::XMVectorSetByIndex(this->rotation, p, 0);
}

void Transform::LookAt(DirectX::XMVECTOR lookPosition)
{
	DirectX::XMVECTOR offset = DirectX::XMVectorSubtract(lookPosition, position);

	DirectX::XMVECTOR playerPos = position;
	DirectX::XMVECTOR testWinPos = lookPosition;

	DirectX::XMFLOAT3 direction;
	DirectX::XMStoreFloat3(&direction, DirectX::XMVector3Normalize(offset));
	float dX = testWinPos.m128_f32[0] - playerPos.m128_f32[0];
	float dY = testWinPos.m128_f32[1] - playerPos.m128_f32[1];
	float dZ = testWinPos.m128_f32[2] - playerPos.m128_f32[2];

	float angle = atan2f(dX, dZ);
	float yaw = angle;// atan2f(direction.x / direction.y);

	float pitch = atan2f(sqrtf(dZ * dZ + dX * dX), dY);
	this->rotation = { pitch, yaw, 0.0f };
}



void Transform::LookAtCamera(DirectX::XMVECTOR lookPosition)
{
	DirectX::XMVECTOR offset = DirectX::XMVectorSubtract(lookPosition, position);

	DirectX::XMVECTOR playerPos = position;
	DirectX::XMVECTOR testWinPos = lookPosition;

	DirectX::XMFLOAT3 direction;
	DirectX::XMStoreFloat3(&direction, DirectX::XMVector3Normalize(offset));
	float dX = testWinPos.m128_f32[0] - playerPos.m128_f32[0];
	float dZ = testWinPos.m128_f32[2] - playerPos.m128_f32[2];
	float dY = testWinPos.m128_f32[1] - playerPos.m128_f32[1];

	float angle = atan2f(dX, dZ);
	float yaw = angle;

	float pitch = atan2(sqrtf(dZ * dZ + dX * dX), dY) - MathHelper::PI;
	this->rotation = { -pitch, yaw, 0.0f };
}

void Transform::Translate(float x, float y, float z)
{
	Translate({ x,y,z });
}

void Transform::Translate(DirectX::XMVECTOR translation)
{
	this->position = DirectX::XMVectorAdd(this->position, translation);
}

void Transform::RandomizeYRotation()
{
	float y = rand() % 360;
	this->rotation = { 0,y,0 };
}

void Transform::SetScale(float x, float y, float z)
{
	SetScale({ x,y,z });
}

void Transform::SetScale(DirectX::XMVECTOR scale)
{
	this->scale = scale;
}

Transform Transform::Clone()
{
	return Transform(position, rotation, scale);
}

void Transform::ApplyToTransform(Transform& transform)
{
	this->position = transform.position;
	this->rotation = transform.rotation;
	this->scale = transform.scale;
}

void Transform::SmoothRotate(DirectX::XMFLOAT3 nextPosition, float fixedDeltaTime, bool changeDir)
{
	
	DirectX::XMFLOAT3 currentPosition;
	DirectX::XMStoreFloat3(&currentPosition, position);

	float currentDir = DirectX::XMVectorGetByIndex(rotation, 1);

	DirectX::XMVECTOR directionVector = { currentPosition.x - nextPosition.x,0, currentPosition.z - nextPosition.z };
	//Checks if WASD is pressed. True sets new direction
	if (changeDir)
		nextDir = atan2(DirectX::XMVectorGetByIndex(directionVector, 0), DirectX::XMVectorGetByIndex(directionVector, 2));

	//Rotates to shortest angle(in rad)
	Rotate(0, MathHelper::ShortestRotation(currentDir, nextDir) * (fixedDeltaTime * 3.14f), 0);

	//removes rotations bigger and smaller than 360 & -360
	if (DirectX::XMVectorGetByIndex(GetRotation(), 1) < -MathHelper::PI * 2)
		SetRotation({ 0, DirectX::XMVectorGetByIndex(GetRotation(), 1) + MathHelper::PI * 2, 0 });
	if (DirectX::XMVectorGetByIndex(GetRotation(), 1) > MathHelper::PI * 2)
		SetRotation({ 0, DirectX::XMVectorGetByIndex(GetRotation(), 1) - MathHelper::PI * 2, 0 });

}
