#include "Application.h"

Application::Application(HINSTANCE hInstance) : window(hInstance), pauseGame(false)
{
	srand((int)time(0));
	this->deltaTime = 0.0f;
	this->currentTime = 0.0f;

	this->window.Initialize(); // initializes the win32 window
	this->dx11.Initialize(this->window); // creates swapchain, device, deviceContext
	this->deferredRenderer = new Renderer(this->window.GetWidth(), this->window.GetHeight(), timer, dx11);

	// Opens the console
	Logger::Open();
	Logger::Write(LOG_LEVEL::Info, "Testing text output to console");
		
	this->gamemanager = new Gamemanager(&dx11);

	this->gameScene = new DevScene(this->deferredRenderer, this->dx11, this->window, scenes, gamemanager);
	this->endScene = new EndScene(this->deferredRenderer, this->dx11, this->window, scenes, "EndScene", exitGame, gamemanager);
	this->introScene = new IntroScene(this->deferredRenderer, this->dx11, this->window, scenes, exitGame, gamemanager);

	// loads all scenes
	t1 = new std::thread(&Application::loadScenes, this);
	
	scenes.push_back(endScene);
	scenes.push_back(gameScene);
	scenes.push_back(introScene);
	introScene->LoadResources();
	IntroScene* test = static_cast<IntroScene*>(introScene);
	test->Load();
	test->threadPtr = t1;
	currentScene = introScene;
}

Application::~Application()
{	
	delete gamemanager;
	Logger::Close();
}

void Application::Run()
{
	// starts the message loop for win32
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	timer.Start();

	while (TRUE)
	{
		// Redirects the messages to the static WindowProc in Window.cpp
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
				break;
			if (exitGame)
				break;
		}
		else
		{
			currentTime = static_cast<float>(timer.GetMilisecondsElapsed() / 1000.0f);
			deltaTime = currentTime - timeLastFrame;

			window.GetInput()->Update();

			// call update function
			if (currentScene != nullptr && !pauseGame)
			{
				fixedTimeAccumulation += deltaTime;
				currentScene->Update(deltaTime);
				currentScene->Render();

				while (fixedTimeAccumulation >= TARGET_FIXED_DELTA)
				{
					currentScene->FixedUpdate(TARGET_FIXED_DELTA);
					fixedTimeAccumulation -= TARGET_FIXED_DELTA;
				}

				Scene* next = currentScene->GetNextScene();

				if (next != nullptr)
				{					
					currentScene->Unload();

					currentScene = next;
					currentScene->Load();
					currentScene->nextScene = nullptr;
				}
			}

			timeLastFrame = currentTime;
		}
	}	
}

void Application::loadScenes()
{
	gameScene->LoadResources();
	endScene->LoadResources();

}

