#include "Object.h"
#include "Renderer.h"

Object::Object(ObjectLayer layer, Mesh* mesh, Material* material) : mesh(mesh), material(material), enabled(true), layer(layer)
{
	this->id = object_id_counter++;
	SetMesh(mesh);
}

Object::Object() : Object(ObjectLayer::None, nullptr, nullptr) {}

Object::Object(const Object& other) : Object(other.layer, other.mesh, other.material) { this->enabled = other.enabled; }

Object::Object(ObjectLayer layer, AssimpHandler::AssimpData model) : Object(layer, model.mesh, model.material){}

Object::~Object() {}

void Object::SetMesh(Mesh* mesh)
{
	this->mesh = mesh; 
	UpdateLocalBounds();
}

AABB Object::GetWorldBounds() const
{
	DirectX::XMVECTOR min, max, corner;
	DirectX::XMMATRIX worldTransform = transform.GetWorldMatrix();

	DirectX::XMVECTOR corners[8];
	localBounds.GetCorners(corners);

	for (size_t i = 0; i < 8; i++)
	{
		corner = DirectX::XMVector3Transform(corners[i], worldTransform);

		if (i == 0)
			min = max = corner;
		else
		{
			min = DirectX::XMVectorMin(min, corner);
			max = DirectX::XMVectorMax(max, corner);
		}
	}

	return AABB(min, max);
}

void Object::UpdateLocalBounds()
{
	if (this->mesh != nullptr)
	{
		this->localBounds = AABB({ FLT_MAX, FLT_MAX, FLT_MAX }, { -FLT_MAX, -FLT_MAX, -FLT_MAX });
		DirectX::XMVECTOR pos;

		for (size_t i = 0; i < this->mesh->vertexes.size(); i++)
		{
			pos = DirectX::XMLoadFloat3(&this->mesh->vertexes[i].position);
			localBounds.Encapsulate(pos);
		}
	}
}

void Object::Render(Renderer* renderer, DirectX::XMMATRIX view, DirectX::XMMATRIX projection, DirectX::XMFLOAT3 right, DirectX::XMFLOAT3 up)
{
	DirectX::XMFLOAT3 centre = { 0,0,0 };
	DirectX::XMStoreFloat3(&centre, this->transform.GetPosition());

	if(IsVisible())
		renderer->DrawMesh(mesh, transform.GetWorldMatrix(), view, projection, right, up, centre);
}

