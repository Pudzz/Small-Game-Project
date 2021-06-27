#include "CameraController.h"

CameraController::CameraController(Camera* camera, Input* input, State state) : camera(camera), input(input), currentState(state), target(nullptr), freeSpeed(2.0f)
{
	assert(input != nullptr);
	assert(camera != nullptr);
}

CameraController::~CameraController() {}

void CameraController::SetFollow(Transform* target, DirectX::XMVECTOR offset)
{
	this->target = target;
	this->offset = offset;
}

void CameraController::Update(const float& deltaTime)
{
	if (input->GetKeyDown(DEBUG_CAMERA_KEY))
	{
		input->LockCursor(!input->IsCursorLocked());
		bool following = GetState() == CameraController::State::Follow;
		SetState(following ? CameraController::State::Free : CameraController::State::Follow);
	}

	switch (currentState)
	{
	case CameraController::State::Follow:
		UpdateFollow(deltaTime);
		break;
	case CameraController::State::Free:
		UpdateFree(deltaTime);
		break;

	case CameraController::State::None:
	default:
		break;
	}
}

void CameraController::UpdateFree(const float& deltaTime)
{
	Transform& transform = camera->GetTransform();
	POINTS delta = input->GetMouseDelta();
	transform.Rotate(delta.y *  deltaTime, delta.x * deltaTime, 0);

	const float cameraSpeed = 55.0f;	// 12.0f innan
	float forwardDelta = 0.0f;
	float rightDelta = 0.0f;

	if (input->GetKey(DirectX::Keyboard::Keys::W)) forwardDelta += 1.0f;
	if (input->GetKey(DirectX::Keyboard::Keys::A)) rightDelta -= 1.0f;
	if (input->GetKey(DirectX::Keyboard::Keys::S)) forwardDelta -= 1.0f;
	if (input->GetKey(DirectX::Keyboard::Keys::D)) rightDelta += 1.0f;

	if (forwardDelta != 0.0f || rightDelta != 0.0f)
	{
		float len = sqrtf(forwardDelta * forwardDelta + rightDelta * rightDelta);

		DirectX::XMVECTOR translation;
		translation = DirectX::XMVectorScale(transform.Forward(), (forwardDelta / len));
		translation = DirectX::XMVectorAdd(translation, DirectX::XMVectorScale(transform.Right(), (rightDelta / len)));
		translation = DirectX::XMVectorScale(DirectX::XMVector3Normalize(translation), cameraSpeed * deltaTime);
		transform.Translate(translation);
	}
	camera->UpdateView();
}

void CameraController::UpdateFollow(const float& deltaTime)
{

	if (target != nullptr)
	{
		DirectX::XMVECTOR position = DirectX::XMVectorAdd(target->GetPosition(), offset);

		DirectX::XMVECTOR direction = DirectX::XMVectorSubtract(target->GetPosition(), camera->GetTransform().GetPosition());
		direction = DirectX::XMVector3Normalize(direction);

		camera->GetTransform().SetPosition(position);

		camera->GetTransform().LookAtCamera(target->GetPosition());
		camera->UpdateView();
	}
}
