#pragma once
#include "assimpHandler.h"
#include "DX11Handler.h"
#include "Terrain.h"
#include "Object.h"
#include "GUIActionbar.h"
#include "Gamemanager.h"
#include "Entities.h"
#include "WorldContext.h"

class Player;

enum class WeaponType
{
	Coconut = 0,
	Spoon = 1,
	Icecream = 2,
};

class Weapon : public Object
{
	const float GRAVITY = 12.0f;
	const float START_Y_VELOCITY = 3.0f;

public:
	Weapon(WeaponType type, ObjectLayer layer, Mesh* mesh, Material* material, WorldContext* context);
	Weapon(WeaponType type, ObjectLayer layer, AssimpHandler::AssimpData model, WorldContext* context);
	virtual ~Weapon();
		
	virtual void Update(const float& deltaTime) override;
	virtual void TriggerAttack(DirectX::XMVECTOR pos, DirectX::XMVECTOR rot);
	
	float GetWeaponDamage() { return this->damage; }	
	GUIActionbar* GetWeaponSprite() { return this->weaponSprite; }		

	WeaponType GetType() const { return this->type; }
	void SetType(WeaponType type) { this->type = type; }

	virtual void PlaySoundEffect() = 0;
	virtual void PlayBreaksound() = 0;

	int CheckUsage() { return this->used; }
	virtual void Use() {}; 

	void SetWeaponSpeed(int value) { this->movementspeed = value; };
	float AttackDamage() { return this->damage; }
	void Move(const float& deltaTime);
	
	virtual void OnHitGround();
	WorldContext* context;

private:
	DirectX::XMVECTOR direction;
	DirectX::XMVECTOR nextPos;
	
protected:	
	int movementspeed;	

	WeaponType type;
	GUIActionbar* weaponSprite;	
	bool inFlight;
	float damage;
	int used;

	float yVelocity;
	float startHeight;
};

