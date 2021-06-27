#pragma once
#include "MathHelper.h"
#include <string>
#include <vector>

class Joint
{
public:
	Joint();
	~Joint();

	void SetInverseBindPose(DirectX::XMMATRIX matrix);
	void SetGlobalTransform(DirectX::XMMATRIX matrix);
	void SetFinalTransformation(DirectX::XMMATRIX matrix);
	void SetBoneName(std::string name);

	DirectX::XMMATRIX GetInverseBindPose();

	// Get the global and final transforms at a given index
	DirectX::XMMATRIX GetGlobalTransform(unsigned int keyframeIndex);
	DirectX::XMMATRIX GetFinalTransformation(unsigned int keyframeIndex);

	// Get the whole vectors containing global and final transforms
	std::vector<DirectX::XMMATRIX>& GetGlobalTransformVector();
	std::vector<DirectX::XMMATRIX>& GetFinalTransformVector();

	std::string GetBoneName();

private:
	std::string boneName;
	DirectX::XMMATRIX inverseBindPose;
	std::vector<DirectX::XMMATRIX> globalTransform;
	std::vector<DirectX::XMMATRIX> finalTransformation;
};
