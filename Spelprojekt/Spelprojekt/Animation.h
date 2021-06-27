#pragma once
#include "Joint.h"
#include <vector>
#include <string>

class Animation
{
public:
	Animation();
	~Animation();
	unsigned int GetLength();
	unsigned int GetNumberOfBones();
	std::string GetAnimationName();

	// Get the whole vector containing the bones
	std::vector<Joint>& GetBoneVector();

	// Get the bone of a given index
	Joint& GetBone(unsigned int boneIndex);

	void SetLength(unsigned int length);
	void SetBoneAmount(unsigned int amount);
	void SetName(std::string name);
	
private:
	unsigned int animationLength;
	unsigned int numberOfBones;
	std::string animationName;
	std::vector<Joint> bones;
};
