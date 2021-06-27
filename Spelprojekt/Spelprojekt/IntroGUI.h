#pragma once
#include "GUI.h"
#include "DirectXHelpers.h"
#include "CameraController.h"
#include "SoundHandler.h"

class IntroScene;
#include "IntroScene.h"


class IntroGUI
{
public:
	IntroGUI(GUI* gui, DX11Handler& dx11, CameraController* cameraController, IntroScene* scenes, Gamemanager* gamemanager); // tabrot soundeffect
	~IntroGUI();

	enum class Menu
	{
		credits,
		howtoplay,
		start,
		options,
		soundtracks,
		highscore,
		quit
	};
			
	void Update();

	void Start();
	void LoadStart();

	void HowToPlay();
	void LoadHowToPlay();

	void Credits();
	void LoadsCredits();

	void Options(); 
	void LoadOptions();

	void Soundtracks();
	void LoadSoundtracks();

	void Highscore();
	void LoadHighscore();

	void Quit();
	void LoadQuit();

private:
	void ClearGUI();

	bool first = true;
	Menu menu = Menu::start;
	
	DX11Handler& dx11;
	GUI* gui;
	Input* input;
	IntroScene* currentScene;

	Gamemanager* gamemanager;

	// Vsync
	std::string lastOn, lastOff;	
	bool vsyncOn = false;
		
	// Music and sound stuff //
	float volumeScale = 0.1f;	
	float maxVolume = 1.0f;
	float currentMusicVolume;
	float currentSoundVolume;
		
	bool trackoneChecked;
	bool tracktwoChecked;
	bool trackthreeChecked;		

	std::string lastEasy, lastMedium, lastHard;
};