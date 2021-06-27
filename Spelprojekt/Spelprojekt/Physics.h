#pragma once
#include <reactphysics3d.h>
#include "Object.h"
#include "Transform.h"
#include "Logger.h"
#include <unordered_map>
#include "RaycastHit.h"
#include "RaycastCallback.h"

class Physics
{
public:

	Physics();
	virtual~Physics();

	rp3d::ProxyShape* AddCollision(rp3d::ConvexMeshShape* shape, rp3d::RigidBody* body, float mass);
	rp3d::Transform ConvertTransform(const Transform& transform);
	rp3d::Vector3 Convert(DirectX::XMVECTOR v3);

	rp3d::CollisionBody* CreateCollisionBodyBoxCustom(Object* object,float x, float y, float z);
	rp3d::CollisionBody* CreateCollisionBodyBox(Object* object);
	rp3d::CollisionBody* CreateCollisionBodyCapsule(Object* object, float radius, float xOffset = 0, float zOffset = 0, float height = 100.0f);

	rp3d::CollisionWorld* GetWorld() const { return this->world; }
	RaycastHit Raycast (DirectX::XMVECTOR start, DirectX::XMVECTOR end);

private:
	std::unordered_map<Mesh*, rp3d::ConvexMeshShape*> meshShapeCache;
	rp3d::CollisionWorld* world;
	std::vector< rp3d::CollisionBody*> bodies;
};