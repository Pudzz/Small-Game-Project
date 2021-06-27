#pragma once
#include <DirectXMath.h>
#include "Input.h"
#include "Camera.h"

class CameraController
{
	const DirectX::Keyboard::Keys DEBUG_CAMERA_KEY = DirectX::Keyboard::Keys::F;

public:

	enum class State
	{
		None,
		Follow,
		Free
	};

	CameraController(Camera* camera, Input* input, State state = State::None);
	virtual ~CameraController();

	void SetState(State state) { this->currentState = state; }
	State GetState() const { return this->currentState; }
	void SetFollow(Transform* transform, DirectX::XMVECTOR offset);
	void SetFreeCameraSpeed(float speed) { this->freeSpeed = speed; }
	Input* getInput() const { return this->input; };
	void Update(const float& deltaTime);

private:
	void UpdateFree(const float& deltaTime);
	void UpdateFollow(const float& deltaTime);
private:
	Camera* camera;
	Input* input;
	State currentState;

	// free movement
	float freeSpeed;

	// follow
	Transform* target;
	DirectX::XMVECTOR offset;
};