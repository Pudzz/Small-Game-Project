#include "Entities.h"

Entities::Entities() : quadtree(AABB({ 0,0,0 }, { 0,0,0 })) {}
Entities::Entities(AABB worldBounds) : quadtree(worldBounds) {}
Entities::~Entities() {}

void Entities::UpdateTree()
{
	quadtree.Clear();
	for (auto i : allEntities)
		quadtree.Insert(i);
}

void Entities::SetBounds(AABB worldBounds)
{
	this->quadtree.SetBounds(worldBounds);
}

void Entities::InsertObject(Object* object)
{
	ObjectLayer layer = object->GetLayer();
	auto findLayerVector = objectsInLayerMap.find(layer);

	if (findLayerVector == objectsInLayerMap.end())
	{
		objectsInLayerMap.insert({ layer, std::vector<Object*>() });
		objectsInLayerMap[layer].push_back(object);
	}
	else
	{
		auto find0 = std::find(objectsInLayerMap[layer].begin(), objectsInLayerMap[layer].end(), object);
		if (find0 == objectsInLayerMap[layer].end())
		{
			objectsInLayerMap[layer].push_back(object);
		}
	}

	auto find1 = std::find(allEntities.begin(), allEntities.end(), object);
	if (find1 == allEntities.end())
	{
		allEntities.push_back(object);
	}
}

void Entities::RemoveObject(Object* object)
{
	ObjectLayer layer = object->GetLayer();
	auto findLayerVector = objectsInLayerMap.find(layer);

	if (findLayerVector != objectsInLayerMap.end())
	{
		std::vector<Object*>& v = objectsInLayerMap[layer];
		auto g = std::find(v.begin(), v.end(), object);
		if (g != v.end())
			v.erase(g);
	}

	//// all
	auto g = std::find(allEntities.begin(), allEntities.end(), object);
	if (g != allEntities.end())
		allEntities.erase(g);
}

void Entities::ChangeLayer(Object* object, ObjectLayer layer)
{
	RemoveObject(object);
	object->SetLayer(layer);
	InsertObject(object);
}

std::vector<Object*> Entities::GetObjectsInLayer(ObjectLayer layer)
{
	auto findLayerVector = objectsInLayerMap.find(layer);
	if (findLayerVector != objectsInLayerMap.end())
		return objectsInLayerMap[layer];

	return std::vector<Object*>(0);
}

std::vector<Object*> Entities::GetObjectsInRange(DirectX::XMVECTOR center, float radius, ObjectLayer layer)
{
	DirectX::XMFLOAT3 point;
	DirectX::XMStoreFloat3(&point, center);
	float hr = radius / 2.0f;
	AABB bounds = AABB({ point.x - hr, point.y - hr, point.z - hr }, { point.x + hr, point.y + hr, point.z + hr });

	std::vector<Object*> inRange;
	auto inArea = GetObjectsInAABB(bounds, layer);

	for (auto j : inArea)
	{
		if (j->IsEnabled() && (((int)j->GetLayer() & (int)layer) == (int)j->GetLayer()))	
		{
			DirectX::XMVECTOR a = j->GetTransform().GetPosition();
			DirectX::XMVECTOR b = center;

			float sqrDistance = DirectX::XMVectorGetByIndex(DirectX::XMVector3LengthSq(DirectX::XMVectorSubtract(a, b)), 0);

			if (sqrDistance < radius * radius)
				inRange.push_back(j);
		}
	}

	return inRange;
}

std::vector<Object*> Entities::GetObjectsInAABB(const AABB& aabb, ObjectLayer layer)
{
	std::vector<Object*> inArea;
	std::unordered_set<int> visited;
	std::queue<QuadTree*> treeQueue;
	treeQueue.push(&quadtree);

	while (!treeQueue.empty())
	{
		QuadTree* quad = treeQueue.front();
		treeQueue.pop();

		if (quad->GetBounds().Overlaps(aabb))
		{
			if (quad->IsSplit())
			{
				auto d = quad->GetChildren();
				for (size_t i = 0; i < 4; i++)
					treeQueue.push(d[i]);
			}
			else
			{
				auto df = quad->GetObjects();

				for (auto j : df)
				{
					if (visited.find(j->GetID()) == visited.end())
					{
						if (j->IsEnabled() && (((int)j->GetLayer() & (int)layer) == (int)j->GetLayer()))
						{
							inArea.push_back(j);
						}

						visited.insert(j->GetID());
					}
				}
			}
		}
	}

	return inArea;
}

std::vector<Object*> Entities::GetObjectsInView(Camera* camera)
{
	std::vector<Object*> inView;
	std::unordered_set<int> visited;
	std::queue<QuadTree*> treeQueue;
	treeQueue.push(&quadtree);

	while (!treeQueue.empty())
	{
		QuadTree* quad = treeQueue.front();
		treeQueue.pop();

		if (camera->IsBoundsInView(quad->GetBounds()))
		{
			if (quad->IsSplit())
			{
				auto d = quad->GetChildren();
				for (size_t i = 0; i < 4; i++)
					treeQueue.push(d[i]);
			}
			else
			{
				auto df = quad->GetObjects();

				for (auto j : df)
				{
					if (j->IsEnabled() && visited.find(j->GetID()) == visited.end() && camera->IsBoundsInView(j->GetWorldBounds()))
					{
						visited.insert(j->GetID());
						inView.push_back(j);
					}
				}
			}
		}
	}

	return inView;
}

void Entities::Clear()
{
	allEntities.clear();
	objectsInLayerMap.clear();
}
