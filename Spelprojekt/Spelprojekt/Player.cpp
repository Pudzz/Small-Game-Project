#include "Physics.h"

#include "Player.h"
#include "Scene.h"
#include "SpawnObjects.h"

Player::Player(AssimpHandler::AssimpData modelData, CameraController* controller, GUI* gui, WorldContext* context)
	: Object(ObjectLayer::Player, modelData.mesh, modelData.material), gui(gui), controller(controller), context(context)
{

	this->input = controller->getInput();
	this->currentPosition = { 0,0,0 };
	DirectX::XMStoreFloat3(&currentPosition, GetTransform().GetPosition());

	// weapon shit
	this->leftActionbar = nullptr;
	this->rightActionbar = nullptr;

	this->lefthandFull = false;
	this->righthandFull = false;
	this->leftWeapon = nullptr;
	this->rightWeapon = nullptr;
	this->activeWeapon = nullptr;
	this->arrowRotation = { 0,0,0 };
	this->playerHealth = 100.0f;

	this->winArea = winArea;
	this->arrow = nullptr;

	this->points = 0;
	this->isMoving = false;
	this->rangedAttacking = false;
	this->meleeAttacking = false;

	this->healthTimer = 5.0f;
}

Player::~Player()
{
}

void Player::Update(const float& deltaTime)
{
	UpdateMovement(deltaTime);
	UpdateHeight(deltaTime);
	CheckForPickups();

	UseWeapon();
	UpdateLookAtPosition();
	UpdateAnimations();

	HealthCheck(deltaTime);
}


void Player::TakeDamage(float damage)
{
	if (playerHealth > 0.0f)
	{
		playerHealth -= damage;
		context->gamemanager->GetSoundeffectHandler()->PlaySound("Splash", context->gamemanager->GetCurrentSoundVolume());
	}
}

void Player::UpdateMovement(float fixedDeltaTime)
{
	DirectX::XMFLOAT3 nextPosition;
	DirectX::XMStoreFloat3(&nextPosition, GetTransform().GetPosition());
	currentPosition = nextPosition;

	DirectX::XMFLOAT3 noff = { 0,0,0 };

	if (controller->GetState() == CameraController::State::Follow)
	{
		if (controller->getInput()->GetKey(DirectX::Keyboard::Keys::P))
		{
			GetMesh()->skeleton->SetCurrentAnimation(GetMesh()->skeleton->animations[2]);
			return;
		}

		float dx = 0.0f;
		float dz = 0.0f;

		if (input->GetKey(DirectX::Keyboard::Keys::W)) dz += 1.0f;
		if (input->GetKey(DirectX::Keyboard::Keys::A)) dx -= 1.0f;
		if (input->GetKey(DirectX::Keyboard::Keys::S)) dz -= 1.0f;
		if (input->GetKey(DirectX::Keyboard::Keys::D)) dx += 1.0f;

		float length = sqrtf(dx * dx + dz * dz);

		if (length != 0.0f && !meleeAttacking && !rangedAttacking)
		{
			dx /= length;
			dz /= length;
			noff.x = dx * fixedDeltaTime * movementspeed;
			noff.z = dz * fixedDeltaTime * movementspeed;

			bool walkable = context->spawner->PointIsWalkable(nextPosition.x + noff.x, nextPosition.z + noff.z);
			isMoving = walkable;

			if (walkable)
			{
				DirectX::XMFLOAT3 result = CheckCollisions(fixedDeltaTime, length);
				noff.x += result.x;
				noff.z += result.z;
			}
			else
			{
				noff.x = 0.0f;
				noff.z = 0.0f;
			}
		}
		else
		{
			// IDLE
			isMoving = false;
		}

		nextPosition.x += noff.x;
		nextPosition.z += noff.z;

		bool changedir = (length != 0.0f);
		GetTransform().SmoothRotate(nextPosition, fixedDeltaTime, changedir);
		GetTransform().SetPosition({ nextPosition.x, nextPosition.y, nextPosition.z });
	}
}

void Player::UpdateHeight(float FixedDeltaTime)
{
	float xFloat = DirectX::XMVectorGetByIndex(GetTransform().GetPosition(), 0);
	float zFloat = DirectX::XMVectorGetByIndex(GetTransform().GetPosition(), 2);

	GetTransform().SetPosition({ xFloat,(context->terrain->SampleHeight(xFloat, zFloat) + playerHeight), zFloat });
}


DirectX::XMFLOAT3 Player::CheckCollisions(const float& deltaTime, const float& length)
{
	DirectX::XMFLOAT3 result = { 0,0,0 };
	DirectX::XMVECTOR start = GetTransform().GetPosition();
	float y = DirectX::XMVectorGetByIndex(start, 1);
	y -= (playerHeight / 2.0f);

	DirectX::XMVectorSetByIndex(start, y, 1);
	DirectX::XMVECTOR direction, offset;

	const int rayCount = 64;
	float startAngle = static_cast<float>(rand() % 360);

	for (size_t i = 0; i < rayCount; i++)
	{
		float angle = startAngle + (float)i * (360.0f / rayCount);
		float rad = angle * MathHelper::ToRadians;

		direction = DirectX::XMVector3Normalize({ cosf(rad),0,sinf(rad) });
		offset = DirectX::XMVectorAdd(start, DirectX::XMVectorScale(direction, length));

		RaycastHit hit = context->physics->Raycast(start, offset);
		if (hit.hit)
		{
			DirectX::XMFLOAT3 pos;
			DirectX::XMStoreFloat3(&pos, DirectX::XMVectorSubtract(GetTransform().GetPosition(), hit.position));

			result.x += pos.x * deltaTime;
			result.z += pos.z * deltaTime;
		}
	}

	return result;
}

void Player::CheckForPickups()
{
	if (input->GetKeyDown(DirectX::Keyboard::Keys::E) && (!lefthandFull || !righthandFull))
	{
		std::vector<Object*> pickups = context->entities->GetObjectsInLayer(ObjectLayer::Pickup);
		bool foundPickup = false;

		for (auto i = pickups.begin(); i < pickups.end() && !foundPickup; i++)
		{
			Weapon* obj = static_cast<Weapon*>(*i);

			if (obj != nullptr && obj->IsEnabled() && obj->GetWorldBounds().Overlaps(this->GetWorldBounds()))
			{
				if (!lefthandFull)
				{
					leftWeapon = CopyWeapon(obj);

					this->leftActionbar = new GUIActionbar(*obj->GetWeaponSprite());
					this->leftActionbar->SetPosition(325.0f, 650.0f);
					this->gui->AddGUIObject(this->leftActionbar, "Left Actionbar");

					lefthandFull = true;
				}
				else if (lefthandFull && !righthandFull)
				{
					rightWeapon = CopyWeapon(obj);

					this->rightActionbar = new GUIActionbar(*obj->GetWeaponSprite());
					this->rightActionbar->SetPosition(400.0f, 650.0f);
					this->gui->AddGUIObject(this->rightActionbar, "Right Actionbar");

					righthandFull = true;
				}

				obj->SetEnabled(false);
				context->spawner->RemovePickup(obj);

				foundPickup = true;
			}
		}
	}
}

float Player::ShortestRotation(float currentDir, float nextDir)
{
	float returnValue = 0;
	if (abs(nextDir - currentDir) < MathHelper::PI)
		returnValue = nextDir - currentDir;
	else if (currentDir < nextDir)
		returnValue = nextDir - currentDir - MathHelper::PI * 2.0f;
	else
		returnValue = nextDir - currentDir + MathHelper::PI * 2.0f;
	return returnValue;
}

void Player::UpdateAnimations()
{
	if (this->isMoving)
	{
		this->GetMesh()->skeleton->SetCurrentAnimation(this->GetMesh()->skeleton->animations[0]);
	}

	else if (this->rangedAttacking)
	{
		this->GetMesh()->skeleton->SetCurrentAnimation(this->GetMesh()->skeleton->animations[2]);
	}

	else if (this->meleeAttacking)
	{
		this->GetMesh()->skeleton->SetCurrentAnimation(this->GetMesh()->skeleton->animations[3]);
	}

	else
	{
		this->GetMesh()->skeleton->SetCurrentAnimation(this->GetMesh()->skeleton->animations[1]);
	}
}

void Player::UseWeapon()
{
	// Left hand
	if (input->GetLeftMouseButtonDown() && lefthandFull)
	{
		WeaponUsage(leftWeapon, lefthandFull);

		if (!lefthandFull)
			gui->RemoveGUIObject("Left Actionbar");
	}

	// Right hand
	if (input->GetRightMouseButtonDown() && righthandFull)
	{
		WeaponUsage(rightWeapon, righthandFull);

		if (!righthandFull)
			gui->RemoveGUIObject("Right Actionbar");
	}

	if (this->GetMesh()->skeleton->GetKeyframe() == this->GetMesh()->skeleton->GetCurrentAnimation()->GetLength() - 4)
	{
		this->meleeAttacking = false;
		this->rangedAttacking = false;
	}
}

void Player::WeaponUsage(Weapon* weapon, bool& hand)
{
	DirectX::XMVECTOR aimDirection = GetAimDirection();

	if (weapon->GetType() == WeaponType::Coconut)
	{
		//DirectX::XMVECTOR aimDirection = GetAimDirection();
		weapon->TriggerAttack(GetTransform().GetPosition(), aimDirection);
		weapon->PlaySoundEffect();

		SetActiveWeapon(static_cast<Weapon*>(weapon));
		context->entities->InsertObject(weapon);

		hand = false;
		GetTransform().SetRotation(aimDirection);
		this->rangedAttacking = true;
	}

	if (weapon->GetType() == WeaponType::Spoon)
	{
		weapon->TriggerAttack(GetTransform().GetPosition(), aimDirection);
		weapon->PlaySoundEffect();

		SetActiveWeapon(static_cast<Weapon*>(weapon));
		context->entities->InsertObject(weapon);

		hand = false;
		GetTransform().SetRotation(aimDirection);
		this->rangedAttacking = true;
	}
}

float Player::GetPlayerHealth()
{
	return playerHealth;
}

void Player::IncreaseHealth()
{
	if (GetPlayerHealth() != 100.0f) {
		this->playerHealth += context->gamemanager->GetRecoverHealth();
		if (this->playerHealth > 100.0f)
			this->playerHealth = 100.0f;
	}
}

void Player::HealthCheck(float deltaTime)
{
	if (GetHealth() < 100.0f)
	{
		if (healthTimer <= 0.0f) {
			IncreaseHealth();
			healthTimer = 5.0f;
		}
		else {
			healthTimer -= deltaTime;
		}
	}
	else
	{
		if (healthTimer != 5.0f)
			healthTimer = 5.0f;
	}
}

Weapon* Player::CopyWeapon(Weapon* weapon)
{
	Weapon* curr = nullptr;
	if (weapon->GetType() == WeaponType::Coconut)
	{
		Projectile* proj = static_cast<Projectile*>(weapon);
		curr = new Projectile(*proj);
		curr->SetType(weapon->GetType());
		curr->context = context;
	}
	else if (weapon->GetType() == WeaponType::Spoon)
	{
		Spoon* spoonweap = static_cast<Spoon*>(weapon);
		curr = new Spoon(*spoonweap);
		curr->SetType(weapon->GetType());
		curr->context = context;
	}

	return curr;
}

Weapon* Player::GetActiveWeapon() const
{
	return this->activeWeapon;
}

void Player::SetActiveWeapon(Weapon* weapon)
{
	this->activeWeapon = weapon;
}

void Player::SetTargetAndArrow(Object* arrow, Object* winArea)
{
	this->arrow = arrow;
	this->winArea = winArea;
}

void Player::UpdateLookAtPosition()
{
	if (arrow != nullptr)
	{
		arrow->GetTransform().SetPosition({ GetTransform().GetPosition().m128_f32[0],GetTransform().GetPosition().m128_f32[1],GetTransform().GetPosition().m128_f32[2] });
		arrow->GetTransform().LookAt(winArea->GetWorldBounds().GetCenter());
	}
}

void Player::RemoveActiveWeapon()
{
	if (activeWeapon != nullptr)
	{
		context->entities->RemoveObject(activeWeapon);
		delete activeWeapon;
		activeWeapon = nullptr;
	}
}


DirectX::XMVECTOR Player::GetAimDirection() const
{
	//h�mta ray
	float t = -1.0f;
	DirectX::XMVECTOR playerToMouseDirection = { 0,0,0 };

	MathHelper::Ray ray = context->scene->GetSceneCamera()->ScreenPositionToWorldRay(input->GetMousePosition());
	float denom = DirectX::XMVectorGetByIndex(DirectX::XMVector3Dot({ 0,-1,0 }, ray.direction), 0);
	//ray casta mot plane
	if (denom > 0.000001f)
	{
		DirectX::XMVECTOR originToPlane = DirectX::XMVectorSubtract(GetTransform().GetPosition(), ray.origin);
		t = DirectX::XMVectorGetByIndex(DirectX::XMVector3Dot(originToPlane, { 0,-1,0 }), 0) / denom;
	}
	//h�mta riktning mellan spelar och punkt p� planet
	if (t >= 0.0f)
	{
		DirectX::XMVECTOR pointInWorld = DirectX::XMVectorAdd(ray.origin, DirectX::XMVectorScale(ray.direction, t));
		playerToMouseDirection = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(pointInWorld, GetTransform().GetPosition()));
	}

	DirectX::XMFLOAT3 playerToMouseDir;
	DirectX::XMStoreFloat3(&playerToMouseDir, playerToMouseDirection);

	float yaw = atan2f(playerToMouseDir.x, playerToMouseDir.z) + MathHelper::PI;
	DirectX::XMVECTOR angleToPos = { 0, yaw, 0 };
	return angleToPos;
}

