#pragma once
#include <DirectXMath.h>

struct RaycastHit
{
	DirectX::XMVECTOR position;
	DirectX::XMVECTOR normal;
	bool hit;
};