#pragma once
#include "DX11Handler.h"
#include "assimpHandler.h"
#include "Player.h"
#include "Entities.h"
#include <ctime>
#include "MathHelper.h"

class Enemy;

class SpawnObjects
{
	struct RespawnPickup
	{
		DirectX::XMVECTOR position;
		DirectX::XMVECTOR rotation;
		WeaponType type;
		float remaningTime;
		int prefabIndex;
	};

	const float RespawnTimer = 30.0f;

	const float CoconutRespawnTime = 30.0f;
	const int CoconutsPerTree = 3;
	const float CoconutOffset = 3.0f;

	const float MinimumSpawnHeight = 4.0f;
	const float EnemySpawnDistanceFromPlayer = 20.0f;

public:
	SpawnObjects(WorldContext* context);
	virtual ~SpawnObjects();

	void Initialize();
	void Clear();

	void SetMaxEnemies(int amount);
	int CountEnemiesRemaining() const { return this->maxEnemies; }

	void SetPickupPrefab(Object* obj, WeaponType type);
	void SetEnemyPrefab(Enemy*);
	void PlaceWagon (Object*);

	void Update(const float& deltaTime);
	void RemovePickup(Object* object);

	bool PointIsWalkable(float x, float z);
	void CheckDistanceForRespawn(float deltaTime);
	void Purge();

private:
	DirectX::XMVECTOR GetRandomSpawnPosition(float heightOffset);
	DirectX::XMVECTOR  GetRandomEnemyPosition();

	void AddEnemyToPool();
	void RemoveEnemy(Enemy*);
	Enemy* SpawnEnemy();

	void UpdateSpawnEnemy();
	void UpdateEnemies(const float& deltaTime);

private:
	float rTimer;

	bool* spawnmap;
	WorldContext* context;

	Object* wagon;
	Object* pickupsPrefabs[2];
	Enemy* enemyPrefab;
	int maxEnemies;
	int enemyCount;

	std::vector<RespawnPickup> respawnTimers;
	std::queue<Enemy*> enemyPool;

};