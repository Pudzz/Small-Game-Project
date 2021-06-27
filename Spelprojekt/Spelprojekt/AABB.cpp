#include "AABB.h"

AABB::AABB() : min(DirectX::XMVectorZero()), max(DirectX::XMVectorZero())
{
}

AABB::AABB(DirectX::XMVECTOR min, DirectX::XMVECTOR max) : min(min), max(max)
{
}

void AABB::Encapsulate(const DirectX::XMVECTOR& point)
{
	min = DirectX::XMVectorMin(min, point);
	max = DirectX::XMVectorMax(max, point);
}

void AABB::GetCorners(DirectX::XMVECTOR corners[]) const
{
	DirectX::XMFLOAT3 minf;
	DirectX::XMFLOAT3 maxf;
	DirectX::XMStoreFloat3(&minf, min);
	DirectX::XMStoreFloat3(&maxf, max);

	float dx, dy, dz;
	size_t i = 0;

	for (size_t z = 0; z <= 1; z++)
	{
		for (size_t y = 0; y <= 1; y++)
		{
			for (size_t x = 0; x <= 1; x++)
			{
				dx = minf.x + (maxf.x - minf.x) * (float)x;
				dy = minf.y + (maxf.y - minf.y) * (float)y;
				dz = minf.z + (maxf.z - minf.z) * (float)z;

				corners[i] = { dx, dy, dz };
				i++;
			}
		}
	}
}

bool AABB::ContainsPoint(const DirectX::XMVECTOR& v) const
{
	DirectX::XMFLOAT3 p, minf, maxf;
	DirectX::XMStoreFloat3(&p, v);
	DirectX::XMStoreFloat3(&minf, min);
	DirectX::XMStoreFloat3(&maxf, max);

	return p.x >= minf.x && p.x <= maxf.x && p.y >= minf.y && p.y <= maxf.y && p.z >= minf.z && p.z <= maxf.z;
}

DirectX::XMVECTOR AABB::GetSize() const
{
	return DirectX::XMVectorSubtract(max, min);
}

DirectX::XMVECTOR AABB::GetCenter() const
{
	return DirectX::XMVectorAdd(min, DirectX::XMVectorScale(DirectX::XMVectorSubtract(max, min), 0.5f));
}

float AABB::GetRadius() const
{
	DirectX::XMFLOAT3 size;
	DirectX::XMStoreFloat3(&size, GetSize());

	return sqrtf(size.x * size.x + size.y * size.y + size.z * size.z);
}

bool AABB::Overlaps(const AABB& other)
{
	DirectX::XMFLOAT3 minf, maxf, otherMin, otherMax;
	DirectX::XMStoreFloat3(&minf, GetMin());
	DirectX::XMStoreFloat3(&maxf, GetMax());
	DirectX::XMStoreFloat3(&otherMin, other.GetMin());
	DirectX::XMStoreFloat3(&otherMax, other.GetMax());
	if (maxf.x <= otherMin.x || minf.x >= otherMax.x)
		return false;
	if (maxf.y <= otherMin.y || minf.y >= otherMax.y)
		return false;
	if (maxf.z <= otherMin.z || minf.z >= otherMax.z)
		return false;

	return true;
}

// https://www.amazon.com/dp/1558607323
float AABB::SqrDistanceToPoint(DirectX::XMVECTOR point)
{
	float sqDist = 0.0f;

	DirectX::XMFLOAT3 p;
	DirectX::XMStoreFloat3(&p, point);

	for (size_t i = 0; i < 3; i++)
	{
		float v = DirectX::XMVectorGetByIndex(point, i);
		float bmin = DirectX::XMVectorGetByIndex(min, i);
		float bmax= DirectX::XMVectorGetByIndex(max, i);

		if (v < bmin)
			sqDist += (bmin - v) * (bmin - v);

		if (v > bmax)
			sqDist += (v - bmax) * (v - bmax);
	}

	return sqDist;
}

void AABB::ScaleMinMax(DirectX::XMMATRIX scale)
{
	this->min = DirectX::XMVector3Transform(this->min, scale);
	this->max = DirectX::XMVector3Transform(this->max, scale);
}
