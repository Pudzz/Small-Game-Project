#include "Gamemanager.h"

Gamemanager::Gamemanager(DX11Handler* dx11) : dxhandler(dx11)
{
	this->music = new SoundHandler();
	this->musicVol = 0.5f;
	this->music->SetGlobalVolume(this->musicVol);

	this->soundeffect = new SoundHandler();
	this->soundVol = 1.0f;
	this->soundeffect->SetGlobalVolume(this->soundVol);

	// Load sounds
	LoadSoundEffects();
	LoadMusicTracks();

	// Level soundtrack from start
	this->currentTrack = 1;			// 1 | 2 | 3 - Track1 | Track2 | Track3	

	this->vsyncState = false;

	// difficulty settings
	this->difficultystate = 1;		// 1 = easy | 2 = medium | 3 = hard
	this->timer = 120.0f;			// 120.0f	| 160.0f	 | 200.0f 
	this->nrOfEnemies = 20;			// 20st		| 40st		 | 60st
	this->activeEnemies = 5;		// 5st		| 7st		 | 9st
	this->enemyDamage = 5;			// 5		| 10		 | 15
	this->enemyHealth = 10;			// 10		| 20		 | 30


	// Get latest easy highscore list ---
	highscoreFiles.open("Datafiles/easyHighscore.txt");

	for (int i = 0; i < MAXSCORES; i++) {
		highscoreFiles >> this->easyHighscorePoints[i];
		highscoreFiles >> this->easyHighscoreName[i];
	}

	highscoreFiles.close();

	// Display current textfile
	for (int i = 0; i < MAXSCORES; i++) {
		displayEasyPoints[i] = new GUIText(*dx11, std::to_string(static_cast<int>(easyHighscorePoints[i])), 0.0f, 0.0f);
		displayEasyNames[i] = new GUIText(*dx11, easyHighscoreName[i], 0.0f, 0.0f);
	}
	// Get latest easy highscore list ---

	// --------------------------------------------

	// Get latest normal highscore list ---
	highscoreFiles.open("Datafiles/normalHighscore.txt");

	for (int i = 0; i < MAXSCORES; i++) {
		highscoreFiles >> this->normalHighscorePoints[i];
		highscoreFiles >> this->normalHighscoreName[i];
	}

	highscoreFiles.close();

	for (int i = 0; i < MAXSCORES; i++) {
		displayNormalPoints[i] = new GUIText(*dx11, std::to_string(static_cast<int>(normalHighscorePoints[i])), 0.0f, 0.0f);
		displayNormalNames[i] = new GUIText(*dx11, normalHighscoreName[i], 0.0f, 0.0f);
	}
	// Get latest normal highscore list ---

	// --------------------------------------------

	// Get latest hard highscore list ---
	highscoreFiles.open("Datafiles/hardHighscore.txt");

	for (int i = 0; i < MAXSCORES; i++) {
		highscoreFiles >> this->hardHighscorePoints[i];
		highscoreFiles >> this->hardHighscoreName[i];
	}

	highscoreFiles.close();

	for (int i = 0; i < MAXSCORES; i++) {
		displayHardPoints[i] = new GUIText(*dx11, std::to_string(static_cast<int>(hardHighscorePoints[i])), 0.0f, 0.0f);
		displayHardNames[i] = new GUIText(*dx11, hardHighscoreName[i], 0.0f, 0.0f);
	}
	// Get latest hard highscore list ---
}

Gamemanager::~Gamemanager()
{
	//delete soundeffect;
	//delete music;
}

void Gamemanager::LoadSoundEffects()
{
	this->soundeffect->LoadSound("WinSound", "SoundEffects/tadaWin.wav");		// done
	this->soundeffect->LoadSound("FailSound", "SoundEffects/Fail.wav");			// done
	this->soundeffect->LoadSound("CoconutThrow", "SoundEffects/Explo1.wav");	// done
	this->soundeffect->LoadSound("Swoosh", "SoundEffects/Swoosh.wav");			// done
	this->soundeffect->LoadSound("Splash", "SoundEffects/Splash.wav");			// done
	this->soundeffect->LoadSound("EnemyHit", "SoundEffects/Punch.wav");			// done
	this->soundeffect->LoadSound("PlayerHit", "SoundEffects/playerHit.wav");	// done
	this->soundeffect->LoadSound("HereWeGo", "SoundEffects/herewego.wav");		// done
}

void Gamemanager::LoadMusicTracks()
{
	this->music->LoadSound("Monster", "SoundEffects/MonstersInc.wav");		// done
	this->music->LoadSound("Ben", "SoundEffects/Ben.wav");					// done
	this->music->LoadSound("Duck", "SoundEffects/FluffingDuck.wav");		// done
	this->music->LoadSound("Cup", "SoundEffects/Cuphead.wav");				// done
}

void Gamemanager::PlayMusic()
{
	this->music->ResetSound();

	switch (GetCurrentTrack()) {
	case 1:
		this->music->PlaySound("Ben", GetCurrentMusicVolume());
		break;
	case 2:
		this->music->PlaySound("Duck", GetCurrentMusicVolume());
		break;
	case 3:
		this->music->PlaySound("Cup", GetCurrentMusicVolume());
		break;
	}
}

void Gamemanager::UpdateHighscore(GUI* gui, int score)
{
	switch (GetDifficultyState()) {
	case 1:	// EASY HIGHSCORE
		if (score > easyHighscorePoints[4]) {
			GUISprite* newHighscore = new GUISprite(*dxhandler, "Sprites/newhighscore.png", 655.0f, 180.0f);
			gui->AddGUIObject(newHighscore, "highscore");

			NewHighscore(easyHighscoreName, easyHighscorePoints, displayEasyNames, displayEasyPoints);

			// Write to file again
			writeToHighscore.open("Datafiles/easyHighscore.txt");

			for (int i = 0; i < MAXSCORES; i++) {
				writeToHighscore << easyHighscorePoints[i] << std::endl;
				writeToHighscore << easyHighscoreName[i] << std::endl;
			}

			writeToHighscore.close();
		}
		break;
	case 2: // NORMAL HIGHSCORE
		if (score > normalHighscorePoints[4]) {
			GUISprite* newHighscore = new GUISprite(*dxhandler, "Sprites/newhighscore.png", 655.0f, 180.0f);
			gui->AddGUIObject(newHighscore, "highscore");

			NewHighscore(normalHighscoreName, normalHighscorePoints, displayNormalNames, displayNormalPoints);

			// Write to file again
			writeToHighscore.open("Datafiles/normalHighscore.txt");

			for (int i = 0; i < MAXSCORES; i++) {
				writeToHighscore << normalHighscorePoints[i] << std::endl;
				writeToHighscore << normalHighscoreName[i] << std::endl;
			}

			writeToHighscore.close();
		}
		break;
	case 3:	// HARD HIGHSCORE
		if (score > hardHighscorePoints[4]) {
			GUISprite* newHighscore = new GUISprite(*dxhandler, "Sprites/newhighscore.png", 655.0f, 180.0f);
			gui->AddGUIObject(newHighscore, "highscore");

			NewHighscore(hardHighscoreName, hardHighscorePoints, displayHardNames, displayHardPoints);

			// Write to file again
			writeToHighscore.open("Datafiles/hardHighscore.txt");

			for (int i = 0; i < MAXSCORES; i++) {
				writeToHighscore << hardHighscorePoints[i] << std::endl;
				writeToHighscore << hardHighscoreName[i] << std::endl;
			}

			writeToHighscore.close();
		}
		break;
	}
}

void Gamemanager::NewHighscore(std::string highscoreName[], int highscoreScore[], GUIText* displayHighscorename[], GUIText* displayHighscorePoints[])
{
	// Init all to OLD. 
	for (int i = 0; i < MAXSCORES; i++) {
		highscoreName[i] = "OLD";
	}

	// Change 6th place in array before sortation
	highscoreScore[5] = GetCurrentScore();
	highscoreName[5] = "LATEST";

	// Sort highscore
	SortHighscore(highscoreName, highscoreScore, MAXSCORES);

	// Rename and set new scores to guitexts
	for (int i = 0; i < MAXSCORES; i++) {
		displayHighscorePoints[i]->SetString(std::to_string(static_cast<int>(highscoreScore[i])));
		displayHighscorename[i]->SetString(highscoreName[i]);
	}
}

void Gamemanager::DisplayHighscore(GUI* gui)
{
	// EASY DISPLAY HIGHSCORE
	GUIText* newEasyHighscoreName[5];
	for (int i = 0; i < 5; i++) {
		newEasyHighscoreName[i] = new GUIText(*displayEasyNames[i]);
		newEasyHighscoreName[i]->SetFontSize({ 1.3f, 1.3f });
	}

	newEasyHighscoreName[0]->SetPosition(145, 260);
	newEasyHighscoreName[1]->SetPosition(145, 305);
	newEasyHighscoreName[2]->SetPosition(145, 350);
	newEasyHighscoreName[3]->SetPosition(145, 395);
	newEasyHighscoreName[4]->SetPosition(145, 440);

	gui->AddGUIObject(newEasyHighscoreName[0], "easyfirstname");
	gui->AddGUIObject(newEasyHighscoreName[1], "easysecondname");
	gui->AddGUIObject(newEasyHighscoreName[2], "easythirdname");
	gui->AddGUIObject(newEasyHighscoreName[3], "easyfourthname");
	gui->AddGUIObject(newEasyHighscoreName[4], "easyfifthname");

	GUIText* newEasyHighscore[5];
	for (int i = 0; i < 5; i++) {
		newEasyHighscore[i] = new GUIText(*displayEasyPoints[i]);
		newEasyHighscore[i]->SetFontSize({ 1.3f, 1.3f });
	}

	newEasyHighscore[0]->SetPosition(310, 260);
	newEasyHighscore[1]->SetPosition(310, 305);
	newEasyHighscore[2]->SetPosition(310, 350);
	newEasyHighscore[3]->SetPosition(310, 395);
	newEasyHighscore[4]->SetPosition(310, 440);

	gui->AddGUIObject(newEasyHighscore[0], "easyfirstscore");
	gui->AddGUIObject(newEasyHighscore[1], "easysecondscore");
	gui->AddGUIObject(newEasyHighscore[2], "easythirdscore");
	gui->AddGUIObject(newEasyHighscore[3], "easyfourthscore");
	gui->AddGUIObject(newEasyHighscore[4], "easyfifthscore");

	for (int i = 0; i < 5; i++) {
		if (newEasyHighscoreName[i]->GetString() == "LATEST") {
			newEasyHighscoreName[i]->SetFontColor({ 0,0.5f,1,1 });
			newEasyHighscore[i]->SetFontColor({ 0,0.5f,1,1 });
		}
		else {
			newEasyHighscoreName[i]->SetFontColor({ 1,0,0,1 });
			newEasyHighscore[i]->SetFontColor({ 1,0,0,1 });
		}
	}

	//------------------------------------------------
	// NORMAL HIGHSCORE DISPLAY

	GUIText* newNormalHighscoreName[5];
	for (int i = 0; i < 5; i++) {
		newNormalHighscoreName[i] = new GUIText(*displayNormalNames[i]);
		newNormalHighscoreName[i]->SetFontSize({ 1.3f, 1.3f });
	}

	newNormalHighscoreName[0]->SetPosition(550, 260);
	newNormalHighscoreName[1]->SetPosition(550, 305);
	newNormalHighscoreName[2]->SetPosition(550, 350);
	newNormalHighscoreName[3]->SetPosition(550, 395);
	newNormalHighscoreName[4]->SetPosition(550, 440);

	gui->AddGUIObject(newNormalHighscoreName[0], "normalfirstname");
	gui->AddGUIObject(newNormalHighscoreName[1], "normalsecondname");
	gui->AddGUIObject(newNormalHighscoreName[2], "normalthirdname");
	gui->AddGUIObject(newNormalHighscoreName[3], "normalfourthname");
	gui->AddGUIObject(newNormalHighscoreName[4], "normalfifthname");

	GUIText* newNormalHighscore[5];
	for (int i = 0; i < 5; i++) {
		newNormalHighscore[i] = new GUIText(*displayNormalPoints[i]);
		newNormalHighscore[i]->SetFontSize({ 1.3f, 1.3f });
	}

	newNormalHighscore[0]->SetPosition(720, 260);
	newNormalHighscore[1]->SetPosition(720, 305);
	newNormalHighscore[2]->SetPosition(720, 350);
	newNormalHighscore[3]->SetPosition(720, 395);
	newNormalHighscore[4]->SetPosition(720, 440);

	gui->AddGUIObject(newNormalHighscore[0], "normalfirstscore");
	gui->AddGUIObject(newNormalHighscore[1], "normalsecondscore");
	gui->AddGUIObject(newNormalHighscore[2], "normalthirdscore");
	gui->AddGUIObject(newNormalHighscore[3], "normalfourthscore");
	gui->AddGUIObject(newNormalHighscore[4], "normalfifthscore");

	for (int i = 0; i < 5; i++) {
		if (newNormalHighscoreName[i]->GetString() == "LATEST") {
			newNormalHighscoreName[i]->SetFontColor({ 0,0.5f,1,1 });
			newNormalHighscore[i]->SetFontColor({ 0,0.5f,1,1 });
		}
		else {
			newNormalHighscoreName[i]->SetFontColor({ 1,0,0,1 });
			newNormalHighscore[i]->SetFontColor({ 1,0,0,1 });
		}
	}

	//------------------------------------------
	// HARD HIGHSCORE

	GUIText* newHardHighscoreName[5];
	for (int i = 0; i < 5; i++) {
		newHardHighscoreName[i] = new GUIText(*displayHardNames[i]);
		newHardHighscoreName[i]->SetFontSize({ 1.3f, 1.3f });
	}

	newHardHighscoreName[0]->SetPosition(950, 260);
	newHardHighscoreName[1]->SetPosition(950, 305);
	newHardHighscoreName[2]->SetPosition(950, 350);
	newHardHighscoreName[3]->SetPosition(950, 395);
	newHardHighscoreName[4]->SetPosition(950, 440);

	gui->AddGUIObject(newHardHighscoreName[0], "hardfirstname");
	gui->AddGUIObject(newHardHighscoreName[1], "hardsecondname");
	gui->AddGUIObject(newHardHighscoreName[2], "hardthirdname");
	gui->AddGUIObject(newHardHighscoreName[3], "hardfourthname");
	gui->AddGUIObject(newHardHighscoreName[4], "hardfifthname");

	GUIText* newHardHighscore[5];
	for (int i = 0; i < 5; i++) {
		newHardHighscore[i] = new GUIText(*displayHardPoints[i]);
		newHardHighscore[i]->SetFontSize({ 1.3f, 1.3f });
	}

	newHardHighscore[0]->SetPosition(1110, 260);
	newHardHighscore[1]->SetPosition(1110, 305);
	newHardHighscore[2]->SetPosition(1110, 350);
	newHardHighscore[3]->SetPosition(1110, 395);
	newHardHighscore[4]->SetPosition(1110, 440);

	gui->AddGUIObject(newHardHighscore[0], "hardfirstscore");
	gui->AddGUIObject(newHardHighscore[1], "hardsecondscore");
	gui->AddGUIObject(newHardHighscore[2], "hardthirdscore");
	gui->AddGUIObject(newHardHighscore[3], "hardfourthscore");
	gui->AddGUIObject(newHardHighscore[4], "hardfifthscore");

	for (int i = 0; i < 5; i++) {
		if (newHardHighscoreName[i]->GetString() == "LATEST") {
			newHardHighscoreName[i]->SetFontColor({ 0,0.5f,1,1 });
			newHardHighscore[i]->SetFontColor({ 0,0.5f,1,1 });
		}
		else {
			newHardHighscoreName[i]->SetFontColor({ 1,0,0,1 });
			newHardHighscore[i]->SetFontColor({ 1,0,0,1 });
		}
	}
}

void Gamemanager::SortHighscore(std::string name[], int points[], int totalscores)
{
	for (int i = 0; i < totalscores; i++)
	{
		int min = i;

		for (int k = i + 1; k < totalscores; k++)
		{
			if (points[k] > points[min])
			{
				min = k;
			}
		}

		int oldMin;
		std::string oldnameMin;

		oldMin = points[i];
		oldnameMin = name[i];

		points[i] = points[min];
		name[i] = name[min];
		points[min] = oldMin;
		name[min] = oldnameMin;
	}
}
