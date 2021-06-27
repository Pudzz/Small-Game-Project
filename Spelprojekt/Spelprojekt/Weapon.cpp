#include "Weapon.h"


Weapon::Weapon(WeaponType type, ObjectLayer layer, Mesh* mesh, Material* material, WorldContext* context) : Object(layer, mesh, material), context(context), weaponSprite(nullptr), type(type)
{
	this->direction = { 0,0,0 };
	this->nextPos = { 0,0,0 };

	this->inFlight = false;
	this->damage = 0;
	this->used = 0;
	this->movementspeed = 0;
	this->damage = 0;
}

Weapon::Weapon(WeaponType type, ObjectLayer layer, AssimpHandler::AssimpData model, WorldContext* context) : Weapon(type, layer, model.mesh, model.material, context) {}

Weapon::~Weapon()
{
	if (this->weaponSprite != nullptr)
	{
		delete this->weaponSprite;
		this->weaponSprite = nullptr;
	}
}

void Weapon::Update(const float& deltaTime)
{
	if (inFlight)
	{
		Move(deltaTime);
	}
}

void Weapon::TriggerAttack(DirectX::XMVECTOR pos, DirectX::XMVECTOR rot)
{
	GetTransform().SetPosition(pos);
	GetTransform().SetRotation(rot);
	this->inFlight = true;
	this->direction = rot;

	this->startHeight = DirectX::XMVectorGetByIndex(pos, 1);
	this->yVelocity = START_Y_VELOCITY;
}

void Weapon::Move(const float& deltaTime)
{
	nextPos = {
		(GetTransform().GetPosition().m128_f32[0] + (-std::sinf(direction.m128_f32[1]) * movementspeed) * deltaTime) ,
		GetTransform().GetPosition().m128_f32[1],
		(GetTransform().GetPosition().m128_f32[2] + (-std::cosf(direction.m128_f32[1]) * movementspeed) * deltaTime)
	};	// 30 = speed

	float y = DirectX::XMVectorGetByIndex(nextPos, 1);
	yVelocity -= GRAVITY * deltaTime;
	y += yVelocity * deltaTime;
	nextPos = DirectX::XMVectorSetByIndex(nextPos, y, 1);

	DirectX::XMFLOAT3 posf3;
	DirectX::XMStoreFloat3(&posf3, nextPos);

	float h = context->terrain->SampleHeight(posf3.x, posf3.z);
	if (h > y)
	{
		OnHitGround();
	}

	// kolla height	
	GetTransform().SetPosition(nextPos);
	GetTransform().SetRotation({ (GetTransform().GetRotation().m128_f32[0] + (-8.f * deltaTime)) ,GetTransform().GetRotation().m128_f32[1]  ,GetTransform().GetRotation().m128_f32[2] });
}

void Weapon::OnHitGround()
{
	SetEnabled(false);
}
