#pragma once
#include <DirectXMath.h>

class AABB
{
public:
	AABB();
	AABB(DirectX::XMVECTOR min, DirectX::XMVECTOR max);

	void Encapsulate(const DirectX::XMVECTOR& point);
	void GetCorners(DirectX::XMVECTOR corners[]) const;
	
	DirectX::XMVECTOR GetMin() const { return this->min; }
	DirectX::XMVECTOR GetMax() const { return this->max; }
	void SetMin(const DirectX::XMVECTOR& min) { this->min = min; }
	void SetMax(const DirectX::XMVECTOR& max) { this->max = max; }

	bool ContainsPoint(const DirectX::XMVECTOR& v) const;
	DirectX::XMVECTOR GetSize() const;
	DirectX::XMVECTOR GetCenter() const;

	float GetRadius() const;
	bool Overlaps(const AABB& other);
	float SqrDistanceToPoint(DirectX::XMVECTOR point);
	void ScaleMinMax(DirectX::XMMATRIX scale);

private:
	DirectX::XMVECTOR min;
	DirectX::XMVECTOR max;
};