#include "Physics.h"
#include "Enemy.h"

Enemy::Enemy(Mesh* mesh, Material* material, WorldContext* context) : Object(ObjectLayer::Enemy, mesh, material), context(context), weapon(nullptr)
{
	this->velocity = { 2,0,2 };
	this->tVelocity = { 0,0,0 };

	this->cooldownTimer = 0.0f;

	this->currentPosition = { 0,0,0 };
	DirectX::XMStoreFloat3(&currentPosition, GetTransform().GetPosition());

	// Health for enemy
	this->health = context->gamemanager->GetEnemyHealth();
	ResetTeleportationTimer();

	Icecream* prefab = context->resources->GetResource<Icecream>("icecreamPrefab");

	// PLEASE KILL 
	weapon = new Icecream(*prefab);
	weapon->SetEnabled(false);
	context->entities->InsertObject(weapon);

	this->moving = false;
}

Enemy::Enemy(AssimpHandler::AssimpData modelData, WorldContext* context) : Enemy(modelData.mesh, modelData.material, context) {}
Enemy::Enemy(const Enemy& other) : Enemy(other.GetMesh(), other.GetMaterial(), other.context) {}

Enemy::~Enemy()
{	
}

void Enemy::Update(const float& deltaTime)
{
	if (cooldownTimer > 0.0f)
		cooldownTimer -= deltaTime;

	UpdateHeight(deltaTime);
	UpdateMovement(deltaTime);

	UpdateAttackPlayer();
	UpdateAnimations();
}

void Enemy::FixedUpdate(const float& fixedDeltaTime)
{
	this->GetMesh()->skeleton->AddKeyframe();
}

void Enemy::UpdateMovement(float fixedDeltaTime)
{
	// kolla distants

	if (!GetWorldBounds().Overlaps(context->player->GetWorldBounds()))
	{
		this->moving = true;

		tVelocity = DirectX::XMVectorAdd(tVelocity, BoidsAlgorithm(ObjectLayer::Enemy));
		tVelocity = DirectX::XMVectorScale(tVelocity, fixedDeltaTime);

		GetTransform().Translate(tVelocity);

		DirectX::XMFLOAT3 nextPos;
		DirectX::XMStoreFloat3(&nextPos, GetTransform().GetPosition());
		currentPosition = nextPos;


		DirectX::XMFLOAT3 pPos;
		DirectX::XMStoreFloat3(&pPos, context->player->GetTransform().GetPosition());

		DirectX::XMFLOAT3 off = { 0,0,0 };

		if (pPos.z >= nextPos.z)
			nextPos.z += fixedDeltaTime * DirectX::XMVectorGetByIndex(velocity, 2);
		if (pPos.x <= nextPos.x)
			nextPos.x -= fixedDeltaTime * DirectX::XMVectorGetByIndex(velocity, 0);
		if (pPos.z <= nextPos.z)
			nextPos.z -= fixedDeltaTime * DirectX::XMVectorGetByIndex(velocity, 2);
		if (pPos.x >= nextPos.x)
			nextPos.x += fixedDeltaTime * DirectX::XMVectorGetByIndex(velocity, 0);

		GetTransform().SmoothRotate(nextPos, fixedDeltaTime, true);
		GetTransform().SetPosition({ nextPos.x, nextPos.y, nextPos.z });
	}

	else
	{
		this->moving = false;
	}
}

void Enemy::UpdateAnimations()
{
	if (moving)
	{
		this->GetMesh()->skeleton->SetCurrentAnimation(this->GetMesh()->skeleton->animations[0]);
	}
}

DirectX::XMFLOAT3 Enemy::CheckCollisions(const float& deltaTime, const float& length)
{
	DirectX::XMFLOAT3 result = { 0,0,0 };
	DirectX::XMVECTOR start = GetTransform().GetPosition();
	float y = DirectX::XMVectorGetByIndex(start, 1);
	y -= (1.0f / 2.0f);

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

			result.x += pos.x;
			result.z += pos.z;
		}
	}

	return result;
}


DirectX::XMVECTOR Enemy::BoidsAlgorithm(ObjectLayer object)
{
	float separationRadius = 3.0f;
	float alignRadius = 15.0f;
	DirectX::XMVECTOR velocity = { 0,0,0 };
	DirectX::XMVECTOR avgDir = DirectX::XMVector3Normalize(GetVelocity());
	DirectX::XMVECTOR avgPosition = GetTransform().GetPosition();
	int objInRadius = 0;
	auto checkEntity = context->entities->GetObjectsInLayer(object);
	for (auto entity : checkEntity)
	{
		if (GetID() == entity->GetID())
			continue;

		DirectX::XMVECTOR offset = DirectX::XMVectorSubtract(GetTransform().GetPosition(), entity->GetTransform().GetPosition());
		DirectX::XMVECTOR distance = DirectX::XMVector3Length(offset);
		
		float distF = DirectX::XMVectorGetByIndex(distance, 0);
		
		if (distF < separationRadius)
		{
			velocity = DirectX::XMVectorAdd(velocity, Separation(offset, distance, distF));
		}
		if (distF < alignRadius)
		{
			avgDir =  DirectX::XMVectorAdd(avgDir, static_cast<Enemy*>(entity)->GetVelocity());
			avgPosition = DirectX::XMVectorAdd(avgPosition, entity->GetTransform().GetPosition());

			objInRadius++;
		}
	}
	avgDir = DirectX::XMVector3Normalize(DirectX::XMVectorScale(avgDir, 1.0f / (objInRadius + 1)));
	avgDir = DirectX::XMVectorScale(avgDir, context->gamemanager->GetEnemySpeed());
	DirectX::XMVECTOR align = DirectX::XMVectorSubtract(avgDir, GetVelocity());

	avgPosition = DirectX::XMVectorScale(avgPosition, 1.0f / (objInRadius + 1));
	DirectX::XMVECTOR cohesion = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(avgPosition, GetTransform().GetPosition()));
	cohesion = DirectX::XMVectorScale(cohesion, context->gamemanager->GetEnemySpeed());
	
	velocity = DirectX::XMVectorAdd(velocity, align);
	velocity = DirectX::XMVectorAdd(velocity, cohesion);
	
	return velocity;
}

DirectX::XMVECTOR Enemy::Separation(DirectX::XMVECTOR offset, DirectX::XMVECTOR distance, float distF)
{
	DirectX::XMVECTOR steering = { 0,0,0 };
	if (distF < 0.001f)	{
		
		float randX = (static_cast <float>(rand() % 200) - 100.0f) / 100.0f;
		float randZ = (static_cast <float>(rand() % 200) - 100.0f) / 100.0f;
		steering = { randX, 0, randZ};
	}
	else
	{
		steering = DirectX::XMVectorScale(DirectX::XMVector3Normalize(offset), 4.0f);
	}
	return steering;
}

DirectX::XMVECTOR Enemy::GetVelocity()
{
	return tVelocity;
}

void Enemy::HitSound()
{
	context->gamemanager->GetSoundeffectHandler()->PlaySound("EnemyHit", context->gamemanager->GetCurrentSoundVolume());
}

void Enemy::UpdateAttackPlayer()
{
	DirectX::XMVECTOR vecPlayer = context->player->GetTransform().GetPosition();
	DirectX::XMVECTOR vecEnemy = GetTransform().GetPosition();
	DirectX::XMVECTOR riktVec = DirectX::XMVectorSubtract(vecPlayer, vecEnemy);

	DirectX::XMVECTOR dist = DirectX::XMVector3Length(riktVec);
	float distance = DirectX::XMVectorGetByIndex(dist, 0);

	if (distance < ATTACK_RANGE)
	{
		if (cooldownTimer <= 0.0f) 
		{
			weapon->TriggerAttack(GetTransform().GetPosition(), GetTransform().GetRotation());
			weapon->PlaySoundEffect();
			weapon->SetType(WeaponType::Icecream);
			weapon->SetEnabled(true);
			cooldownTimer = 5.0f;
		}
	}
}

void Enemy::DeactivateWeapon()
{
	weapon->SetEnabled(false);
}

void Enemy::TakeDamage(float damage)
{
	if (this->health > 0.0f) {
		this->health -= damage;
	}
}

void Enemy::ResetHealth()
{
	this->health = context->gamemanager->GetEnemyHealth();
}

void Enemy::UpdateHeight(float fixedDeltaTime)
{
	float xFloat = DirectX::XMVectorGetByIndex(GetTransform().GetPosition(), 0);
	float zFloat = DirectX::XMVectorGetByIndex(GetTransform().GetPosition(), 2);

	GetTransform().SetPosition({ xFloat,(context->terrain->SampleHeight(xFloat, zFloat) + enemyHeight), zFloat });
}