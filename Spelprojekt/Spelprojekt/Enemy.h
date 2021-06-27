#pragma once
#include "RaycastHit.h"
#include "Scene.h"
#include "Terrain.h"
#include "assimpHandler.h"
#include "Icecream.h"
class Player;

#include "Player.h"

class Enemy : public Object
{
	const float ATTACK_RANGE = 13.0f;
	const float MOVEMENT_SPEED = 1.5f;
	const int POINT_VALUE = 5;
	const float TELEPORTATION_COOLDOWN = 2.0f;

public:
	Enemy(Mesh* mesh, Material* material, WorldContext* context);
	Enemy(AssimpHandler::AssimpData modelData, WorldContext* context);
	Enemy(const Enemy& other);
	virtual ~Enemy();

	void Update(const float& deltaTime) override;
	void FixedUpdate(const float& fixedDeltaTime) override;

	void SetHeight(float height) { this->enemyHeight = height; };
	DirectX::XMVECTOR GetVelocity();

	void HitSound();
	int GetPointValue() const { return POINT_VALUE; }
	void UpdateAttackPlayer();
	void DeactivateWeapon();

	// health enemy
	void TakeDamage(float damage);
	void ResetHealth();
	float GetHealthLeft() { return this->health; }
	

	void ResetTeleportationTimer() { this->teleportationTimer = TELEPORTATION_COOLDOWN; }
	void UpdateAnimations();
	float teleportationTimer;

private:
	
	DirectX::XMFLOAT3 CheckCollisions(const float& deltaTime, const float& length);

	void UpdateHeight(float fixedDeltaTime);
	void UpdateMovement(float fixedDeltaTime);
	DirectX::XMVECTOR BoidsAlgorithm(ObjectLayer object);
	DirectX::XMVECTOR Separation(DirectX::XMVECTOR offset, DirectX::XMVECTOR distance, float distF);

private:
	WorldContext* context;
	DirectX::XMVECTOR velocity;
	DirectX::XMVECTOR tVelocity;

	DirectX::XMFLOAT3 currentPosition;
	float nextDir = 0.0f;
	float enemyHeight = 4.65f;

	float cooldownTimer;
	float health;
	Icecream* weapon;
	bool moving;
};