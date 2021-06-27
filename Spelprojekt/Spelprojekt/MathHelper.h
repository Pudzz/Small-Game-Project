#pragma once
#include <DirectXMath.h>
#include <algorithm>
#include <cmath>

namespace MathHelper
{
	struct Ray
	{
		DirectX::XMVECTOR origin;
		DirectX::XMVECTOR direction;
	};

	constexpr float PI = 3.14159265359f;
	constexpr float ToRadians = 0.0174532925f;
	constexpr float ToDegrees = 57.2957795f;

	inline float ShortestRotation(float currentDir, float nextDir)
	{
		float returnValue = 0;

		if (abs(nextDir - currentDir) < MathHelper::PI)
			returnValue = nextDir - currentDir;
		else if (currentDir < nextDir)
			returnValue = nextDir - currentDir - MathHelper::PI * 2.0f;
		else
			returnValue = nextDir - currentDir + MathHelper::PI * 2.0f;
		return returnValue;
	}

}