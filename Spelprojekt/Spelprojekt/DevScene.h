#pragma once
#include "Physics.h"
#include "RaycastCallback.h"

#include "Scene.h"
#include "Logger.h"
#include "Camera.h"
#include <vector>
#include "Input.h"
#include "CameraController.h"
#include "Terrain.h"
#include "assimpHandler.h"

#include "Icecream.h"
#include "Projectile.h"
#include "Spoon.h"
#include "Timer.h"
#include "GUI.h"
#include "AABB.h"
#include "SpawnObjects.h"
#include "Enemy.h"
#include "WorldContext.h"
#include <thread>

class DevScene : public Scene
{
	const char DEBUG_CAMERA_KEY = 'f';

public:
	DevScene(Renderer* renderer, DX11Handler& dx11, Window& window, std::vector<Scene*>& scenes, Gamemanager* gamemanager);
	virtual ~DevScene();

	void Load() override;
	void Unload() override;
	void LoadResources() override;
	void Update(const float& deltaTime) override;
	void FixedUpdate(const float& fixedDeltaTime) override;

	// update time function?
	Scene* GetNextScene() const override;

	// level design
	void CreateSceneObjects();

	std::string getName() { return this->sceneName; };
	void SetNextScene(bool winOrLose);

private:
	void UpdateGUI(const float& deltaTime);

private:
	WorldContext* context;

	CameraController* controller;
	Gamemanager* gamemanager;
	SpawnObjects* spawner;
	Physics physics;

	GUI* gui;
	GUIText* gametimerText;
	GUIText* fpsText;
	GUISprite* gamehub;
	GUISprite* healthFrame;
	GUISprite* healthbar;
	GUIText* totalScore;
	GUIText* totalEnemies;
	GUIText* exitText;

	GUIActionbar* actionbarLeft;
	GUIActionbar* actionbarRight;

	std::vector<Scene*>& scenes;
	Player* player;

	//Enemy* enemyPrefab1;
	Terrain terrainMesh;
	Terrain waterMesh;

	Timer gametimer;
	Timer fpsTimer;
	float timeUntilEnd;
	bool canWin;
	bool exitActive;
	Object* arrow;
	Object* billBoard;

	// For reading FBX file and animations
	Assimp::Importer imp;
	const aiScene* assimpScene;

	Object* tourist[3];
};