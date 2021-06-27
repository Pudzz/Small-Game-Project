#pragma once
#include <vector>
#include <map>
#include <string>
#include "assimp/scene.h"
#include "MathHelper.h"
#include "Joint.h"
#include "Animation.h"

class Skeleton
{
public:
	Skeleton();
	~Skeleton();

	unsigned int GetNumberOfBones();
	DirectX::XMMATRIX GetGlobalMeshInverse();

	// Get the bone at a given index
	Joint& GetBone(unsigned int boneIndex);

	int GetKeyframe();
	Animation* GetCurrentAnimation();

	void SetGlobalMeshInverseTransform(DirectX::XMMATRIX matrix);
	void AddBoneAmount();

	// Add new bone
	void AddNewBone(Joint newBone);

	// Add keyframe
	void AddKeyframe();

	// Set the first and current animation
	void SetFirstAnimation(Animation* first);
	void SetCurrentAnimation(Animation* anim);

	// Map for the bones, containing and index and a name
	std::map<std::string, int> boneMapping;

	std::vector<Animation*> animations;


private:
	std::vector<Joint> bones;
	unsigned int numberOfBones;
	DirectX::XMMATRIX globalMeshInverseTransform;

	Animation* currentAnimation;
	int whichKeyframe;
};