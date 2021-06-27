#pragma once
#include "Window.h"
#include "DevScene.h"
#include "IntroScene.h"
#include "Logger.h"
#include "Renderer.h"
#include "DX11Handler.h"
#include "Input.h"
#include <stack>
#include "Physics.h"
#include "Timer.h"
#include <vector>
#include "EndScene.h"
#include "Gamemanager.h"
#include <future>

class Application
{
	const float TARGET_FIXED_DELTA = 1.0f / 50.0f;

public:

	//Constructor & destructor
	Application(HINSTANCE);
	virtual ~Application();

	//Functions
	void Run();
	Scene* GetCurrentScene() const { return this->currentScene; }
	Window& GetWindow() { return this->window; }

	bool IsPaused() const { return this->pauseGame; }
	void SetPaused(bool pause) { this->pauseGame = pause; }

	bool exitGame = false;
	std::thread* t1;
	void loadScenes();

private:
	// Variables
	Window window;
	DX11Handler dx11;

	// Gamemanager for... everything
	Gamemanager* gamemanager;

	//Scenes for the game
	Scene* currentScene;
	Scene* gameScene;
	Scene* introScene;
	Scene* endScene;
	std::vector<Scene*>scenes;
	Renderer* deferredRenderer;

	float timeLastFrame = static_cast<float>(timer.GetMilisecondsElapsed() / 1000.0f);
	float fixedTimeAccumulation = 0.0f;
	float currentTime;
	float deltaTime;

	bool pauseGame = true;
	Timer timer;
};


