#include "Joint.h"

Joint::Joint()
{
	this->boneName = "";
	this->inverseBindPose = DirectX::XMMatrixIdentity();
}

Joint::~Joint()
{
}

// Set inverse bind pose of the bone at binding time
void Joint::SetInverseBindPose(DirectX::XMMATRIX matrix)
{
	this->inverseBindPose = DirectX::XMMATRIX(matrix);
}

// Set global transform for the bone
void Joint::SetGlobalTransform(DirectX::XMMATRIX matrix)
{
	this->globalTransform.push_back(DirectX::XMMATRIX(matrix));
}

// Set the final transformation
void Joint::SetFinalTransformation(DirectX::XMMATRIX matrix)
{
	this->finalTransformation.push_back(DirectX::XMMATRIX(matrix));
}

// Set the name of the bone
void Joint::SetBoneName(std::string name)
{
	this->boneName = name;
}

// Get the inverse bind pose 
DirectX::XMMATRIX Joint::GetInverseBindPose()
{
	return this->inverseBindPose;
}

// Get the global transform
DirectX::XMMATRIX Joint::GetGlobalTransform(unsigned int keyframeIndex)
{
	return this->globalTransform[keyframeIndex];
}

// Get the final transformation
DirectX::XMMATRIX Joint::GetFinalTransformation(unsigned int keyframeIndex)
{
	//return this->finalTransformation[keyframeIndex];
	return this->finalTransformation[keyframeIndex];
}

// Get the vector containing the global transforms, as reference
std::vector<DirectX::XMMATRIX>& Joint::GetGlobalTransformVector()
{
	return this->globalTransform;
}

// Get the vector containing the final transforms, as reference
std::vector<DirectX::XMMATRIX>& Joint::GetFinalTransformVector()
{
	return this->finalTransformation;
}

// Get the bones name
std::string Joint::GetBoneName()
{
	return this->boneName;
}
