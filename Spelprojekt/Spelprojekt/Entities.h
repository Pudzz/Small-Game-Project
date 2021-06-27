#pragma once
#include <unordered_map>
#include <unordered_set>
#include "Object.h"
#include "Camera.h"
#include "QuadTree.h"

class Entities
{
public:
	
	Entities();
	Entities(AABB worldBounds);
	virtual ~Entities();

	void UpdateTree();
	
	void SetBounds(AABB worldBounds);
	void InsertObject(Object*);
	void RemoveObject(Object*);
	void ChangeLayer(Object*, ObjectLayer);

	std::vector<Object*> GetObjectsInLayer(ObjectLayer layer);
	std::vector<Object*> GetObjectsInRange(DirectX::XMVECTOR center, float radius, ObjectLayer layer = ObjectLayer::Any);
	std::vector<Object*> GetObjectsInAABB(const AABB& aabb, ObjectLayer layer = ObjectLayer::Any);
	std::vector<Object*> GetObjectsInView(Camera* camera);
	std::vector<Object*> AllEntities() { return this->allEntities; }

	void Clear();

private:
	QuadTree quadtree;
	std::vector<Object*> allEntities;
	std::unordered_map<ObjectLayer, std::vector<Object*>> objectsInLayerMap;
};