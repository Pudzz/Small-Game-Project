#pragma once
#include "Scene.h"
#include "Camera.h"
#include "CameraController.h"
#include "assimpHandler.h"
#include "Resources.h"
#include "Gamemanager.h"

class IntroGUI;
#include "IntroGui.h"
#include "Terrain.h"
#include <thread>
class IntroScene : public Scene
{

public:
	IntroScene(Renderer* renderer, DX11Handler& dx11, Window& window, std::vector<Scene*>& scenes, bool &exitGame, Gamemanager* gamemanager);
	virtual ~IntroScene();

	void Load() override;
	void Unload() override;
	void LoadResources() override;

	void Update(const float& deltaTime) override;
	void FixedUpdate(const float& fixedDeltaTime) override;

	Scene* GetNextScene() const override;

	CameraController* controller;
	void setNextScene();
	bool& exitGame;
	std::thread* threadPtr;
private:
	IntroGUI* introGUI;
	GUI* gui;
	std::vector<Scene*>& scenes;		
	Gamemanager* gamemanager;
	Object* glass;

};

