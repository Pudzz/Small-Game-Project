#include "Skeleton.h"

Skeleton::Skeleton()
{
	this->numberOfBones = 0;
	this->globalMeshInverseTransform = DirectX::XMMatrixIdentity();
	this->whichKeyframe = 0;
	currentAnimation = nullptr;
}

Skeleton::~Skeleton()
{
	currentAnimation = nullptr;

	for (int i = 0; i < animations.size(); i++)
	{
		delete animations[i];
		animations[i] = nullptr;
	}

	animations.clear();
}

// Get amount of bones
unsigned int Skeleton::GetNumberOfBones()
{
	return this->numberOfBones;
}

// Get the global mesh inverse transform, will almost certainly be an identity matrix
DirectX::XMMATRIX Skeleton::GetGlobalMeshInverse()
{
	return this->globalMeshInverseTransform;
}

// Get bone at given index
Joint& Skeleton::GetBone(unsigned int boneIndex)
{
	return this->bones[boneIndex];
}

// Get the current keyframe in the current animation
int Skeleton::GetKeyframe()
{
	return this->whichKeyframe;
}

// Get current animation
Animation* Skeleton::GetCurrentAnimation()
{
	return this->currentAnimation;
}

// Set the global mesh inverse transform
void Skeleton::SetGlobalMeshInverseTransform(DirectX::XMMATRIX matrix)
{
	this->globalMeshInverseTransform = DirectX::XMMATRIX(matrix);
}

// Add 1 to bone amount
void Skeleton::AddBoneAmount()
{
	this->numberOfBones++;
}

// Add new bone 
void Skeleton::AddNewBone(Joint newBone)
{
	this->bones.push_back(newBone);
}

// Change keyframe, if the current keyframe is the last in the current animation, go to keyframe 0
void Skeleton::AddKeyframe()
{
	if (this->whichKeyframe == this->currentAnimation->GetLength() - 1)
	{
		this->whichKeyframe = 0;
	}
	else
	{
		this->whichKeyframe++;
	}
}

// Set the first animation
void Skeleton::SetFirstAnimation(Animation* first)
{
	this->currentAnimation = first;
}

// Set the current animation and go to keyframe 0, if the current animation is the same as the given, do nothing
void Skeleton::SetCurrentAnimation(Animation* anim)
{
	if (this->currentAnimation->GetAnimationName() == anim->GetAnimationName())
	{
		return;
	}
	else
	{
		this->whichKeyframe = 0;
		this->currentAnimation = anim;
	}
}

