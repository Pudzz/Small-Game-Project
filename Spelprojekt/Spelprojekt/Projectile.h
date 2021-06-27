#pragma once
#include "Weapon.h"
#include "Gamemanager.h"
#include "Player.h"

class Player;

class Projectile : public Weapon 
{
public:
	Projectile(Mesh* mesh, Material* material, WorldContext* context);
	Projectile(AssimpHandler::AssimpData modelData, WorldContext* context);
	Projectile(const Projectile& other);
	virtual ~Projectile();

	void PlaySoundEffect() override;
	void PlayBreaksound() override;
};

