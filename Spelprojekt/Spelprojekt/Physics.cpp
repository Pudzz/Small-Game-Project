#include "Physics.h"

Physics::Physics()
{
	// Create the world 
	world = new rp3d::CollisionWorld();
}

Physics::~Physics()
{
	for (int i = bodies.size() - 1; i >= 0; i--)
	{
		delete bodies[i];
	}
}

rp3d::ProxyShape* Physics::AddCollision(rp3d::ConvexMeshShape* shape, rp3d::RigidBody* body, float massValue)
{
	// Add the collision shape to the rigid body
	rp3d::decimal mass = rp3d::decimal(massValue);
	rp3d::ProxyShape* proxyShape;
	proxyShape = body->addCollisionShape(shape, body->getTransform(), mass);
	return proxyShape;
}

rp3d::Transform Physics::ConvertTransform(const Transform& transform)
{
	DirectX::XMFLOAT3 pos, rot;
	DirectX::XMStoreFloat3(&pos, transform.GetPosition());
	DirectX::XMStoreFloat3(&rot, transform.GetRotation());

	rp3d::Vector3 position(pos.x, pos.y, pos.z);
	rp3d::Quaternion rotation = rp3d::Quaternion::fromEulerAngles(rot.x, rot.y, rot.z);
	return rp3d::Transform(position, rotation);
}

rp3d::Vector3 Physics::Convert(DirectX::XMVECTOR v3)
{
	DirectX::XMFLOAT3 v;
	DirectX::XMStoreFloat3(&v, v3);
	return rp3d::Vector3(v.x, v.y, v.z);
}

rp3d::CollisionBody* Physics::CreateCollisionBodyBoxCustom(Object* object, float x, float y, float z)
{
	rp3d::Transform rpTransform = ConvertTransform(object->GetTransform());
	rp3d::CollisionBody* body = world->createCollisionBody(rpTransform);// rp3d::Transform::identity());
	const rp3d::Vector3 halfExtents(x, y, z);
	rp3d::BoxShape* box = new rp3d::BoxShape(halfExtents);

	bodies.push_back(body);
	body->addCollisionShape(box, rp3d::Transform::identity());
	return body;
}

rp3d::CollisionBody* Physics::CreateCollisionBodyBox(Object* object)
{
	rp3d::Transform rpTransform = ConvertTransform(object->GetTransform());
	rp3d::CollisionBody* body = world->createCollisionBody(rpTransform);// rp3d::Transform::identity());

	const rp3d::Vector3 extends = Convert(DirectX::XMVectorScale(object->GetLocalBounds().GetSize(), 0.5f));
	rp3d::BoxShape* box = new rp3d::BoxShape(extends);

	bodies.push_back(body);
	body->addCollisionShape(box, rp3d::Transform::identity());
	return body;
}

rp3d::CollisionBody* Physics::CreateCollisionBodyCapsule(Object* object, float radius, float xOffset, float zOffset, float height)
{
	rp3d::Transform rpTransform = ConvertTransform(object->GetTransform());
	rp3d::Vector3 pos = rpTransform.getPosition();
	pos.x += xOffset;
	pos.z += zOffset;
	rpTransform.setPosition(pos);

	rp3d::CollisionBody* body = world->createCollisionBody(rpTransform);
	rp3d::CapsuleShape* capsule = new rp3d::CapsuleShape(radius, height);

	bodies.push_back(body);
	body->addCollisionShape(capsule, rp3d::Transform::identity());
	return body;
}

RaycastHit Physics::Raycast(DirectX::XMVECTOR start, DirectX::XMVECTOR end)
{
	rp3d::Vector3 s = Convert(start);
	rp3d::Vector3 e = Convert(end);

	rp3d::Ray ray(s, e);

	RaycastCallback callback;
	callback.hitInfo.hit = false;

	world->raycast(ray, &callback);
	return callback.hitInfo;
}
