#include "SpawnObjects.h"
#include "Enemy.h"

SpawnObjects::SpawnObjects(WorldContext* context) : context(context), enemyPrefab(nullptr), maxEnemies(0), enemyCount(0)
{
	this->pickupsPrefabs[0] = nullptr;
	this->pickupsPrefabs[1] = nullptr;
	this->spawnmap = nullptr;
	this->wagon = nullptr;

	this->rTimer = 10.0f;
}

SpawnObjects::~SpawnObjects()
{
	while (!enemyPool.empty())
	{
		Enemy* e = enemyPool.front();
		enemyPool.pop();
		delete e;
	}
}

void SpawnObjects::Initialize()
{
	// find trees and place N around
	Weapon* clone = nullptr;
	Projectile* coconut = static_cast<Projectile*>(pickupsPrefabs[(int)WeaponType::Coconut]);

	std::vector<Object*> trees = context->entities->GetObjectsInLayer(ObjectLayer::Tree);

	for (auto i : trees)
	{
		float startAngle = static_cast<float>(rand() % 360);
		DirectX::XMVECTOR position = i->GetTransform().GetPosition();

		for (size_t j = 0; j < CoconutsPerTree; j++)
		{
			DirectX::XMFLOAT3 pos;
			DirectX::XMStoreFloat3(&pos, position);

			float angle = startAngle + ((float)j * (360.0f / CoconutsPerTree));
			float x = pos.x + cosf(angle * MathHelper::ToRadians) * CoconutOffset;
			float z = pos.z + sinf(angle * MathHelper::ToRadians) * CoconutOffset;
			float y = context->terrain->SampleHeight(x, z) + 0.4f;

			clone = new Projectile(*coconut);
			clone->GetTransform().SetPosition({ x,y,z });
			context->entities->InsertObject(clone);
		}
	}

	// SPOOOOOONS
	std::vector<DirectX::XMVECTOR> spoonPositions = {
		{130, 8, 40},
		{28, 7, 47},
		{145.0f, 8.5f, 193.0f},
		{115.0f, 8.5f, 138.0f},
		{195, 7.0f, 115}
	};

	Spoon* spoon = static_cast<Spoon*>(pickupsPrefabs[(int)WeaponType::Spoon]);
	for (auto i : spoonPositions)
	{
		clone = new Spoon(*spoon);

		clone->SetType(WeaponType::Spoon);
		clone->GetTransform().SetPosition(GetRandomSpawnPosition(1.0f));
		context->entities->InsertObject(clone);
	}

	wagon->GetTransform().SetScale(0.5f, 0.5f, 0.5f);
	wagon->GetTransform().Translate(GetRandomSpawnPosition(1.0f));
	wagon->GetTransform().Rotate(0.05f, -5, 0);
}

void SpawnObjects::Clear()
{
	this->enemyCount = 0;
}

void SpawnObjects::SetMaxEnemies(int amount)
{
	this->maxEnemies = amount;
}

void SpawnObjects::Update(const float& deltaTime)
{
	for (int i = respawnTimers.size() - 1; i >= 0; i--)
	{
		respawnTimers[i].remaningTime -= deltaTime;
		if (respawnTimers[i].remaningTime <= 0.0f)
		{
			Projectile* source = static_cast<Projectile*>(pickupsPrefabs[respawnTimers[i].prefabIndex]);
			Projectile* clone = new Projectile(*source);

			clone->GetTransform().SetPosition(respawnTimers[i].position);
			clone->GetTransform().SetRotation(respawnTimers[i].rotation);
			context->entities->InsertObject(clone);

			respawnTimers.erase(respawnTimers.begin() + i);
		}
	}

	UpdateSpawnEnemy();
	UpdateEnemies(deltaTime);
}

void SpawnObjects::SetPickupPrefab(Object* obj, WeaponType type)
{
	this->pickupsPrefabs[(int)type] = obj;
}

void SpawnObjects::SetEnemyPrefab(Enemy* enemy)
{
	this->enemyPrefab = enemy;
}

void SpawnObjects::PlaceWagon(Object* wagon)
{
	this->wagon = wagon;
}

void SpawnObjects::RemovePickup(Object* object)
{
	context->entities->RemoveObject(object);

	RespawnPickup respawn;
	Weapon* w = static_cast<Weapon*>(object);
	respawn.position = object->GetTransform().GetPosition();
	respawn.rotation = object->GetTransform().GetRotation();
	respawn.remaningTime = RespawnTimer;
	respawn.prefabIndex = (int)w->GetType();
	respawn.type = w->GetType();

	respawnTimers.push_back(respawn);
}


bool SpawnObjects::PointIsWalkable(float x, float z)
{
	DirectX::XMVECTOR dot = DirectX::XMVector3Dot(context->terrain->SampleNormal(x, z), { 0,1,0 });
	float height = context->terrain->SampleHeight(x, z);

	return !(height < 0.8f || DirectX::XMVectorGetByIndex(dot, 0) < 0.85f);
}

void SpawnObjects::CheckDistanceForRespawn(float deltaTime)
{
	Enemy* e = nullptr;
	DirectX::XMVECTOR vecPlayer = context->player->GetTransform().GetPosition();
	auto enemies = context->entities->GetObjectsInLayer(ObjectLayer::Enemy);
	for (auto i : enemies)
	{
		e = static_cast<Enemy*>(i);
		DirectX::XMVECTOR vecEnemy = e->GetTransform().GetPosition();
		DirectX::XMVECTOR riktVec = DirectX::XMVectorSubtract(vecPlayer, vecEnemy);
		DirectX::XMVECTOR dist = DirectX::XMVector3Length(riktVec);
		float distance = DirectX::XMVectorGetByIndex(dist, 0);

		if (distance > 30.0f) {
			if (rTimer > 0.0f) {
				rTimer -= deltaTime;
			}
			else {
				e->GetTransform().SetPosition(GetRandomSpawnPosition(1.0f));
				rTimer = 10.0f;
			}
		}
		else {
			if (rTimer != 10.0f)
				rTimer = 10.0f;
		}
	}
}

DirectX::XMVECTOR SpawnObjects::GetRandomSpawnPosition(float heightOffset)
{
	bool found = false;
	int maxIteraitor = 70;
	DirectX::XMVECTOR position = { 0,0,0 };

	while (!found && maxIteraitor > 0)
	{
		float x = static_cast<float>(rand() % context->terrain->GetMapWidth());
		float z = static_cast<float>(rand() % context->terrain->GetMapHeight());
		float y = context->terrain->SampleHeight(x, z);

		if (y >= MinimumSpawnHeight)
		{
			position = { x, y + heightOffset, z };
			found = true;
		}

		maxIteraitor--;
	}

	return position;
}

DirectX::XMVECTOR SpawnObjects::GetRandomEnemyPosition()
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMStoreFloat3(&pos, context->player->GetTransform().GetPosition());

	float angle = (float)(rand() % 16) * (360.0f / CoconutsPerTree);
	float x = pos.x + cosf(angle * MathHelper::ToRadians) * EnemySpawnDistanceFromPlayer;
	float z = pos.z + sinf(angle * MathHelper::ToRadians) * EnemySpawnDistanceFromPlayer;
	float y = context->terrain->SampleHeight(x, z) + 0.4f;

	return { x,y,z };
}

void SpawnObjects::AddEnemyToPool()
{
	Player* player = static_cast<Player*>(context->entities->GetObjectsInLayer(ObjectLayer::Player)[0]);
	Enemy* enemy = new Enemy(*enemyPrefab);

	enemyPool.push(enemy);
}


void SpawnObjects::RemoveEnemy(Enemy* enemy)
{
	enemy->SetEnabled(false);
	context->entities->RemoveObject(enemy);
	enemyPool.push(enemy);
}

Enemy* SpawnObjects::SpawnEnemy()
{
	if (enemyPool.empty())
		AddEnemyToPool();

	Enemy* e = enemyPool.front();
	enemyPool.pop();
	e->ResetHealth();
	e->SetEnabled(true);
	context->entities->InsertObject(e);

	e->GetTransform().SetPosition(GetRandomEnemyPosition());
	e->GetTransform().SetScale(0.275f, 0.275f, 0.275f);

	return e;
}

void SpawnObjects::Purge()
{
	while (!enemyPool.empty())
	{
		Enemy* e = enemyPool.front();
		enemyPool.pop();
		delete e;
	}
}

void SpawnObjects::UpdateSpawnEnemy()
{
	if (enemyCount < context->gamemanager->GetActiveEnemies() && maxEnemies != 0)
	{
		SpawnEnemy();
		enemyCount++;
	}
}

void SpawnObjects::UpdateEnemies(const float& deltaTime)
{
	auto enemies = context->entities->GetObjectsInLayer(ObjectLayer::Enemy);

	Enemy* e = nullptr;
	Player* player = context->player;

	for (auto i : enemies)
	{
		e = static_cast<Enemy*>(i);
		AABB enemyBounds = e->GetWorldBounds();

		if (e != nullptr)
		{
			if (player->GetActiveWeapon() != nullptr && player->GetActiveWeapon()->GetWorldBounds().Overlaps(enemyBounds))
			{
				e->HitSound();
				e->TakeDamage(player->GetActiveWeapon()->AttackDamage());
				player->RemoveActiveWeapon();

				if (e->GetHealthLeft() <= 0.0f)
				{
					if (CountEnemiesRemaining() != 0) 
					{
						player->IncreasePoints(e->GetPointValue());
						maxEnemies--;
					}					

					RemoveEnemy(e);
					enemyCount--;						
				}

				continue;
			}

			if (!context->scene->GetSceneCamera()->IsBoundsInView(enemyBounds))
			{
				e->teleportationTimer -= deltaTime;
				if (e->teleportationTimer <= 0.0f)
				{
					e->GetTransform().SetPosition(GetRandomEnemyPosition());
					e->ResetTeleportationTimer();
				}
			}
			else
			{
				e->ResetTeleportationTimer();
							
			}
		}
	}
}
