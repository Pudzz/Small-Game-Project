#pragma once
#include <vector>
#include "Entities.h"
#include "Transform.h"

class Spawner
{
	struct RespawnObject
	{
		Transform spawnTransform;
		float remainingTime;
	};

public:
	Spawner(Entities* entities);
	virtual ~Spawner();

	void Update(const float& deltaTime);
	
	bool IsPaused() const { return this->paused; }
	void SetPaused(bool paused) { this->paused = paused; }

private:
	bool paused;
	std::vector<RespawnObject> respawnList;
	Entities* entities;

};