#include "Animation.h"

Animation::Animation()
{
	this->numberOfBones = 0;
	this->animationLength = 0;
	this->animationName = " ";
}

Animation::~Animation()
{
	bones.clear();
}

// Get animation length
unsigned int Animation::GetLength()
{
	return this->animationLength;
}

// Get number of bones corresponding to this animation
unsigned int Animation::GetNumberOfBones()
{
	return this->numberOfBones;
}

// Get animation name
std::string Animation::GetAnimationName()
{
	return this->animationName;
}

// Get the whole vector containing the bones
std::vector<Joint>& Animation::GetBoneVector()
{
	return this->bones;
}

// Get specifik bone by given index
Joint& Animation::GetBone(unsigned int boneIndex)
{
	return this->bones[boneIndex];
}

// Set animation length
void Animation::SetLength(unsigned int length)
{
	this->animationLength = length;
}

// Set bone amount
void Animation::SetBoneAmount(unsigned int amount)
{
	this->numberOfBones = amount;
}

// Set name
void Animation::SetName(std::string name)
{
	this->animationName = name;
}
