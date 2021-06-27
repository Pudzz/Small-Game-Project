#include "Icecream.h"
#include "Enemy.h"

Icecream::Icecream(Mesh* mesh, Material* material, WorldContext* context) : Weapon(WeaponType::Icecream, ObjectLayer::Projectile, mesh, material, context)
{
	this->weaponSprite = nullptr;

	this->inFlight = false;
	this->movementspeed = 10;

	this->damage = context->gamemanager->GetEnemyDamage();
}

Icecream::Icecream(AssimpHandler::AssimpData modelData, WorldContext* context) : Icecream(modelData.mesh, modelData.material, context) {}

Icecream::Icecream(const Icecream& other) : Icecream(other.GetMesh(), other.GetMaterial(), other.context)
{
	if (other.GetType() != WeaponType::Icecream)
		throw std::exception();

	GetTransform().SetPosition(other.GetTransform().GetPosition());
	GetTransform().SetRotation(other.GetTransform().GetRotation());
}

Icecream::~Icecream()
{
}

void Icecream::Update(const float& deltaTime)
{
	Weapon::Update(deltaTime);
	UpdateHitPlayer();
}

void Icecream::PlaySoundEffect()
{
	context->gamemanager->GetSoundeffectHandler()->PlaySound("Swoosh", context->gamemanager->GetCurrentSoundVolume());
}

void Icecream::PlayBreaksound()
{
}

void Icecream::UpdateHitPlayer()
{
	if (IsEnabled() && context->player != nullptr && this != nullptr)
	{
		if (this->GetWorldBounds().Overlaps(context->player->GetWorldBounds())) 
		{
			context->player->TakeDamage(Weapon::AttackDamage());
			this->SetEnabled(false);
		}
	}
}
