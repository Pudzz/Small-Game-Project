#pragma once
#include "Weapon.h"
#include "Player.h"

class Player;
class Spoon : public Weapon
{
public:
	Spoon(Mesh* mesh, Material* material, WorldContext* context);
	Spoon(AssimpHandler::AssimpData model, WorldContext* context);
	Spoon(const Spoon& other);
	virtual ~Spoon();

	void TriggerAttack(DirectX::XMVECTOR pos, DirectX::XMVECTOR rot) override;	
	void FollowPlayer(DirectX::XMVECTOR pos, DirectX::XMVECTOR rot);	

	void PlaySoundEffect() override;
	void PlayBreaksound() override;
};