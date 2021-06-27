#include "Projectile.h"

Projectile::Projectile(Mesh* mesh, Material* material, WorldContext* context) : Weapon(WeaponType::Coconut, ObjectLayer::Pickup, mesh, material, context)
{
	this->weaponSprite = new GUIActionbar(*context->dx11, "Sprites/CoconutNew.png", 0.0f, 0.0f);
	this->inFlight = false;
	this->damage = context->gamemanager->GetCoconutDamage();
	this->movementspeed = 15.0f;
}

Projectile::Projectile(AssimpHandler::AssimpData modelData, WorldContext* context) : Projectile(modelData.mesh, modelData.material, context) {}
Projectile::Projectile(const Projectile& other) : Projectile(other.GetMesh(), other.GetMaterial(), other.context) {}
Projectile::~Projectile() {}

void Projectile::PlaySoundEffect()
{
	context->gamemanager->GetSoundeffectHandler()->PlaySound("CoconutThrow", context->gamemanager->GetCurrentSoundVolume());
}

void Projectile::PlayBreaksound() {}
