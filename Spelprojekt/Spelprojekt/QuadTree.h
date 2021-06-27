#pragma once
#include "Object.h"
#include "AABB.h"

class QuadTree
{
	const int MAX_LEVELS = 5;
	const int MAX_OBJECTS = 10;

public:
	QuadTree(AABB bounds);
	QuadTree(size_t level, AABB bounds);

	void Insert(Object* object);
	void Clear();
	bool IsSplit() const { return this->children != nullptr && this->children[0] != nullptr; }
	
	void SetBounds(AABB bounds) { this->bounds = bounds; }
	AABB GetBounds() { return this->bounds; }
	
	std::vector<Object*> GetObjects() { return this->objects; }
	QuadTree** GetChildren() { return this->children; }

private:
	void Split();
	int GetBoundsIndex(Object* object);
	void GetChildBounds(AABB childBounds[]);
	bool AABBOverlapsXZ(AABB a, AABB b);

private:
	std::vector<Object*> objects;
	QuadTree** children;
	size_t level;
	AABB bounds;
};