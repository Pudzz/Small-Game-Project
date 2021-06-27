#pragma once
#include "Scene.h"
#include "Camera.h"
#include "CameraController.h"
#include "assimpHandler.h"
class EndGUI;
#include "EndGUI.h"

class EndScene : public Scene
{
public:
	EndScene(Renderer* renderer, DX11Handler& dx11, Window& window, std::vector<Scene*>& scenes, std::string sceneName, bool& exitGame, /*SoundHandler* soundeffect,*/ Gamemanager* gamemanager);
	virtual ~EndScene();

	void Load() override;
	void Unload() override;
	void LoadResources() override;

	void Update(const float& deltaTime) override;
	Scene* GetNextScene() const override;

	void SetNextScene(std::string whichScene);
	bool GetWinOrLose();

	bool& exitGame;

private:

	Gamemanager* gamemanager;
	CameraController* controller;
	SoundHandler* soundeffects;
	std::vector<Scene*>& scenes;
	EndGUI* endGUI;
	GUI* gui;
};
