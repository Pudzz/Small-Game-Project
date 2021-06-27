#include "QuadTree.h"

QuadTree::QuadTree(AABB bounds) : QuadTree(0, bounds)
{
}

QuadTree::QuadTree(size_t level, AABB bounds) : level(level), bounds(bounds), children(new QuadTree* [4])
{
	for (size_t i = 0; i < 4; i++)
		children[i] = nullptr;
}

void QuadTree::Insert(Object* object)
{
	if (IsSplit())
	{
		int index = GetBoundsIndex(object);
		if (index != 0)
		{
			for (int f = 0; f < 4; f++)
			{
				if ((index & (1 << f)) == (1 << f))
				{
					children[f]->Insert(object);
				}
			}

			return;
		}
	}

	objects.push_back(object);

	if (objects.size() >= MAX_OBJECTS && level < MAX_LEVELS)
	{
		if (!IsSplit())
			Split();

		for (int i = objects.size() - 1; i >= 0; i--)
		{
			int index = GetBoundsIndex(objects[i]);

			if (index != 0)
			{
				for (int f = 0; f < 4; f++)
				{
					if ((index & (1 << f)) == (1 << f))
						children[f]->Insert(objects[i]);
				}

				objects.erase(objects.begin() + i);
			}
		}
	}
}

void QuadTree::Clear()
{
	objects.clear();

	if (IsSplit())
	{
		for (size_t i = 0; i < 4; i++)
		{
			children[i]->Clear();
			delete children[i];
			children[i] = nullptr;
		}

		delete[]children;
		children = nullptr;
	}
}

void QuadTree::Split()
{
	children = new QuadTree * [4];
	AABB childBounds[4];
	GetChildBounds(childBounds);

	for (size_t i = 0; i < 4; i++)
	{
		children[i] = new QuadTree(level + 1, childBounds[i]);
	}
}

int QuadTree::GetBoundsIndex(Object* object)
{
	int flag = 0;
	AABB childBounds[4];
	GetChildBounds(childBounds);

	for (int i = 0; i < 4; i++)
	{
		if (AABBOverlapsXZ(object->GetWorldBounds(), childBounds[i]))
			flag |= (1 << i);
	}

	return flag;
}

void QuadTree::GetChildBounds(AABB childBounds[])
{
	DirectX::XMFLOAT3 size, min, max;
	DirectX::XMStoreFloat3(&size, bounds.GetSize());
	DirectX::XMStoreFloat3(&min, bounds.GetMin());
	DirectX::XMStoreFloat3(&max, bounds.GetMax());

	float width = size.x / 2.0f;
	float depth = size.z / 2.0f;

	childBounds[0] = AABB({ min.x + width,	min.y,	min.z },			{ max.x,			max.y,		min.z + depth });
	childBounds[1] = AABB({ min.x,			min.y,	min.z },			{ min.x + width,	max.y,		min.z + depth });
	childBounds[2] = AABB({ min.x,			min.y,	min.z + depth },	{ min.x + width,	max.y,		max.z });
	childBounds[3] = AABB({ min.x + width,	min.y,	min.z + depth },	{ max.x,			max.y,		max.z });
}

bool QuadTree::AABBOverlapsXZ(AABB a, AABB b)
{
	DirectX::XMFLOAT3 amin, amax, bmin, bmax;
	DirectX::XMStoreFloat3(&amin, a.GetMin());
	DirectX::XMStoreFloat3(&amax, a.GetMax());
	DirectX::XMStoreFloat3(&bmin, b.GetMin());
	DirectX::XMStoreFloat3(&bmax, b.GetMax());

	return !(amin.x > bmax.x || amax.x < bmin.x || amin.z > bmax.z || amax.z < bmin.z);
}
