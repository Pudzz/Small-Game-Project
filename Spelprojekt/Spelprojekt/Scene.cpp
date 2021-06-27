#include "Scene.h"
#include "Enemy.h"

Scene::Scene(std::string name, Renderer* renderer, DX11Handler& dx11, Window& window) :
	sceneName(name), renderer(renderer), window(window), dx11(dx11), entities(nullptr)
{
	this->sceneBounds = AABB({ 0,0,0 }, { 250, 64, 250 });
	this->entities = new Entities(this->sceneBounds);
	this->camera = new Camera(60.0f, window.GetWidth(), window.GetHeight());
	this->nextScene = nullptr;
	this->didWin = false;
}

Scene::~Scene()
{
	delete camera; 
}

void Scene::Unload()
{
	Logger::Write("scene unload");

	auto allEntities = entities->AllEntities();

	for (auto i : allEntities)
		delete i;
	
	entities->Clear();
}

void Scene::Update(const float& deltaTime)
{
	entities->UpdateTree();

	for (auto i : entities->AllEntities())
	{
		if (i->IsEnabled())
		{
			i->Update(deltaTime);
		}
	}
}

void Scene::FixedUpdate(const float& fixedDeltaTime)
{
	for (auto i : entities->AllEntities())
	{
		i->FixedUpdate(fixedDeltaTime);
	}
}

void Scene::Render()
{
	// itererats through the objects and passes the renderer to the object.
	// sorts the objects based on shader -> material properties -> object

	renderer->ShadowPass(this->cameraFocusPosition, sceneBounds, camera, entities);

	renderer->SetDeferredRenderTarget();
	renderer->ClearRenderTarget();

	std::vector<Object*> inView = entities->GetObjectsInView(camera);
	std::sort(inView.begin(), inView.end(), m_CompareRenderList); // O(N�log(N))

	DirectX::XMMATRIX view = camera->GetView();
	DirectX::XMMATRIX projection = camera->GetProjection();

	size_t lastShaderID = -1;
	size_t lastMaterialID = -1;
	Material* currentMaterial = nullptr;

	for (auto i : inView)
	{
		if (i->IsEnabled())
		{
			Material* material = i->GetMaterial();
			size_t shaderID = material->GetShader()->GetID();

			if (lastShaderID != shaderID)
			{
				material->GetShader()->Bind(dx11.GetContext());
				lastShaderID = shaderID;
			}

			if (lastMaterialID != material->GetID())
			{
				if (currentMaterial != nullptr)
					currentMaterial->Unbind(dx11.GetContext());

				currentMaterial = material;
				material->Bind(dx11.GetContext());
				lastMaterialID = material->GetID();
			}
			DirectX::XMFLOAT3 right;
			DirectX::XMStoreFloat3(&right, DirectX::XMVector3Normalize(camera->GetTransform().Right()));
			DirectX::XMFLOAT3 up;
			DirectX::XMStoreFloat3(&up, DirectX::XMVector3Normalize(camera->GetTransform().Up()));
			i->Render(renderer, view, projection, right, up);
		}
	}

	renderer->DisplayFrame(camera);
}

// Sets the state of win or lose, changing to the appropriate endscene
void Scene::setWinOrLose(bool didWin)
{
	this->didWin = didWin;
}

// https://stackoverflow.com/questions/6771374/sorting-an-stl-vector-on-two-values
bool Scene::m_CompareRenderList(Object* a, Object* b)
{
	Material* aMat = a->GetMaterial();
	Material* bMat = b->GetMaterial();
	bool comparison = false;

	if (aMat->GetShader()->GetID() != bMat->GetShader()->GetID())
		comparison = (aMat->GetShader()->GetID() < bMat->GetShader()->GetID());
	else
		comparison = (aMat->GetID() < bMat->GetID());

	return comparison;
}