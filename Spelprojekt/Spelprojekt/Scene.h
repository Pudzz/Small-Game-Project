#pragma once
#include <algorithm>
#include <string>
#include "Renderer.h"
#include "Input.h"
#include "Object.h"
#include "Camera.h"
#include "SoundHandler.h"
#include "Entities.h"
#include "Resources.h"
#include "Gamemanager.h"

class Scene
{
public:
	Scene(std::string name, Renderer* renderer, DX11Handler& dx11, Window& window);
	virtual ~Scene();

	virtual void Load() = 0;
	virtual void Unload();
	virtual void LoadResources() = 0;

	virtual void Update(const float& deltaTime);
	virtual void FixedUpdate(const float& fixedDeltaTime);
	virtual void Render();

	virtual Renderer* GetRenderer() { return this->renderer; };
	virtual Scene* GetNextScene() const = 0;

	Camera* GetSceneCamera() const { return this->camera; }

	std::string GetName() { return this->sceneName; };
	AABB GetSceneBounds() { return sceneBounds; }

	Window& GetWindow() { return this->window; };
	void setWinOrLose(bool didWin);

	Entities* GetEntities() const { return this->entities; }
	Scene* nextScene;

private:
	static bool m_CompareRenderList(Object* a, Object* b);

protected:


	AABB sceneBounds;
	Camera* camera;
	Renderer* renderer;
	Window& window;
	DX11Handler& dx11;

	DirectX::XMVECTOR cameraFocusPosition;

	bool resourcesIsLoaded;
	Resources resources;

	Entities* entities;

	std::string sceneName;
	bool didWin;
};