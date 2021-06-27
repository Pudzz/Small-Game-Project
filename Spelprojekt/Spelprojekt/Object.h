#pragma once
#include "Transform.h"
#include "Mesh.h"
#include "Material.h"
#include "AABB.h"
#include "assimpHandler.h"

class Renderer;

enum class ObjectLayer
{
	None = 0,
	Enviroment = 1 << 0,
	Enemy = 1 << 1,
	Player = 1 << 3,
	Pickup = 1 << 4,
	Projectile = 1 << 5,
	Tree = 1 << 6,
	Any = Enviroment | Enemy | Player | Projectile | Pickup | Tree
};

class Object
{
public:

	Object();
	Object(const Object& other);
	Object(ObjectLayer layer, AssimpHandler::AssimpData model);
	Object(ObjectLayer layer, Mesh* mesh, Material* material);
	virtual ~Object();

	const Transform& GetTransform() const  { return this->transform; }
	Transform& GetTransform() { return this->transform; }

	void SetMaterial(Material* material) { this->material = material; }
	Material* GetMaterial() const { return this->material; }
	
	void SetMesh(Mesh* mesh);
	Mesh* GetMesh() const { return this->mesh; }

	AABB GetLocalBounds() const { return this->localBounds; }
	AABB& ScaleLocalBounds() { return this->localBounds; }
	AABB GetWorldBounds() const;

	void UpdateLocalBounds();
	size_t GetID() const { return this->id; }

	virtual void Render(Renderer*, DirectX::XMMATRIX view, DirectX::XMMATRIX projection, DirectX::XMFLOAT3 right, DirectX::XMFLOAT3 up);
	virtual void Update(const float& deltaTime) {};
	virtual void FixedUpdate(const float& fixedDeltaTime) {};

	ObjectLayer GetLayer() const { return this->layer; }
	void SetLayer(ObjectLayer layer) { this->layer = layer; }

	void SetEnabled(bool enabled) { this->enabled = enabled; }
	bool IsEnabled() const { return this->enabled; }
	bool IsVisible() const { return this->visible; }
	void SetVisible(bool visible) { this->visible = visible; }

private:
	Transform transform;
	AABB localBounds;
	ObjectLayer layer;
	Material* material;
	Mesh* mesh;
	size_t id;
	bool enabled;
	bool visible = true;
};

static size_t object_id_counter = 0;
