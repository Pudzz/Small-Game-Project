#include "EndScene.h"

EndScene::EndScene(Renderer* renderer, DX11Handler& dx11, Window& window, std::vector<Scene*>& scenes, std::string sceneName, bool& exitGame, Gamemanager* gamemanager) : Scene(sceneName, renderer, dx11, window), scenes(scenes), exitGame(exitGame)
{
	this->camera = new Camera(60.0f, window.GetWidth(), window.GetHeight());
	this->nextScene = nullptr;
	this->controller = new CameraController(GetSceneCamera(), window.GetInput(), CameraController::State::Follow);
	window.GetInput()->LockCursor(false);

	this->didWin = false;

	this->gamemanager = gamemanager;	
	this->gui = new GUI(dx11);
}

EndScene::~EndScene()
{
	delete gui;
	delete endGUI;
	delete controller;
}

void EndScene::Load()
{		
	this->endGUI = new EndGUI(gui, dx11, controller, this, gamemanager);
	renderer->SetGUI(gui);
}

void EndScene::Unload()
{
	this->gamemanager->GetMusicHandler()->ResetSound();
	this->gamemanager->GetSoundeffectHandler()->ResetSound();
}

void EndScene::LoadResources()
{
}

void EndScene::Update(const float& deltaTime)
{
	Scene::Update(deltaTime);
	controller->Update(deltaTime);
	endGUI->Update();
}

Scene* EndScene::GetNextScene() const
{
	return nextScene;
}

void EndScene::SetNextScene(std::string whichScene)
{
	for (int i = 0; i < scenes.size(); i++)
	{
		if (whichScene == "game")
		{
			if (scenes[i]->GetName() == "DevScene")
				nextScene = scenes[i];
		}
		
		if (whichScene == "intro")
		{
			if (scenes[i]->GetName() == "IntroScene")
			{
				nextScene = scenes[i];
			}
		}
	}
}

bool EndScene::GetWinOrLose()
{
	return this->didWin;
}

