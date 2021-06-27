#include "IntroScene.h"



IntroScene::IntroScene(Renderer* renderer, DX11Handler& dx11, Window& window, std::vector<Scene*>& scenes, bool& exitGame,  Gamemanager* gamemanager) : Scene("IntroScene", renderer, dx11, window), scenes(scenes), exitGame(exitGame)
{
	this->camera = new Camera(90.0f, window.GetWidth(), window.GetHeight());
	this->controller = new CameraController(camera, window.GetInput(), CameraController::State::None);
	
	window.GetInput()->LockCursor(false);
	this->nextScene = nullptr;

	Lights& lights = renderer->GetLights();	
	lights.SetSunDirection({ 1.0f, -2.0f, 3.0f });
	lights.SetSunColor({ 0.98f, 0.96f, 0.73f, 1.0f });
	lights.SetSunIntensity(0.9f);
		
	// Gamemanager
	this->gamemanager = gamemanager;
	gamemanager->GetSoundeffectHandler()->SetGlobalVolume(gamemanager->GetCurrentSoundVolume());
	gamemanager->GetMusicHandler()->SetGlobalVolume(gamemanager->GetCurrentMusicVolume());	
	
	// Gui
	gui = new GUI(dx11);
	introGUI = nullptr;
}

IntroScene::~IntroScene()
{	
	scenes.clear();
}

void IntroScene::Load()
{	
	// Set music volume from beginning		
	gamemanager->GetMusicHandler()->PlaySound("Monster", gamemanager->GetCurrentMusicVolume());	

	// Gui
	introGUI = new IntroGUI(gui, dx11, controller, this, gamemanager);
	renderer->SetGUI(gui);
		
	this->glass = new Object(ObjectLayer::Player, resources.GetModel("playerModel"));
	this->glass->GetTransform().Translate(-2.5f, 7.5f, -2.5f);
	this->glass->GetTransform().Rotate(0.0f, 0.0f, 0.0f);

	Assimp::Importer imp;
	const aiScene* assimpScene = imp.ReadFile("Animations/Glasse_Intro_Long.fbx", aiProcess_MakeLeftHanded | aiProcess_Triangulate);

	AssimpHandler::saveAnimationData(assimpScene, this->glass->GetMesh()->skeleton, "Idle");

	this->glass->GetMesh()->skeleton->SetFirstAnimation(this->glass->GetMesh()->skeleton->animations[0]);
	entities->InsertObject(this->glass);

	
	Object* wagon = new Object(ObjectLayer::Enviroment, resources.GetModel("wagonModel"));
	wagon->GetTransform().Translate(7.0f, 7.43f, 1.0f);
	wagon->GetTransform().Rotate(0.0f, 0.3f, 0.0f);
	wagon->GetTransform().SetScale(0.4f, 0.4f, 0.4f);
	entities->InsertObject(wagon);

	//// ------- BACKGROUND

	Object* background = new Object(ObjectLayer::Enviroment, resources.GetModel("backgroundPlane"));
	background->GetTransform().Translate(5.0f, 22.0f, 15.0f);
	background->GetTransform().Rotate(-1.5f, 0.0f, 0.0f);
	background->GetTransform().SetScale(100.0f, 2.0f, 35.0f);
	entities->InsertObject(background);

	Terrain ground;
	ground.GenerateMesh("Textures/map_displacement_map_small.png", dx11.GetDevice(), false);
	
	Object* terrainObject = new Object(ObjectLayer::None, ground.GetMesh(), resources.GetResource<Material>("terrainMaterial"));
	terrainObject->GetTransform().SetPosition({ -100.0f, -2.0f+8.5f, -10.0f });
	entities->InsertObject(terrainObject);

	//Camera
	this->camera->GetTransform().SetPosition({0.0f, 7.44f, -4.5f});
	this->camera->GetTransform().SetRotation({-0.1f, 0.1f, 0.0f});
	this->camera->UpdateView();
	
}


void IntroScene::Unload()
{
	auto allEntities = entities->AllEntities();
	for (auto i : allEntities)
	{
		delete i;
	}

	entities->Clear();
}

void IntroScene::LoadResources()
{

	Shader* toonShader = new Shader();
	toonShader->LoadPixelShader(L"Shaders/ToonShader_ps.hlsl", "main", dx11.GetDevice());
	toonShader->LoadVertexShader(L"Shaders/ToonShader_vs.hlsl", "main", dx11.GetDevice());

	Shader* animationShader = new Shader();
	animationShader->LoadPixelShader(L"Shaders/ToonShader_ps.hlsl", "main", dx11.GetDevice());
	animationShader->LoadVertexShader(L"Shaders/ToonShader_vs.hlsl", "animation", dx11.GetDevice());

	// ------- TERRAIN
	Shader* terrainShader = new Shader();
	terrainShader->LoadPixelShader(L"Shaders/Terrain_ps.hlsl", "main", dx11.GetDevice());
	terrainShader->LoadVertexShader(L"Shaders/ToonShader_vs.hlsl", "main", dx11.GetDevice());

	resources.AddResource("toonShader", toonShader);
	resources.AddResource("terrainShader", terrainShader);
	resources.AddResource("animationShader", animationShader);

	/*
		TEXTURES
	*/
	Texture* grass_texture = Texture::CreateTexture("Textures/Grass_ColorTest.png", dx11);
	Texture* grass_normal = Texture::CreateTexture("Textures/Grass_Normal.png", dx11);
	Texture* sand_texture = Texture::CreateTexture("Textures/Sand_Color_Test.png", dx11);
	Texture* sand_normal = Texture::CreateTexture("Textures/Sand_Normal_2.png", dx11);

	resources.AddResource("grassTexture", grass_texture);
	resources.AddResource("grassNormal", grass_normal);
	resources.AddResource("sandTexture", sand_texture);
	resources.AddResource("sandNormal", sand_normal);

	/*
		MATERIAL
	*/

	Material* terrainMat = new Material(terrainShader, dx11);
	terrainMat->SetTexture(0, grass_texture, SHADER_BIND_TYPE::PIXEL);
	terrainMat->SetTexture(1, sand_texture, SHADER_BIND_TYPE::PIXEL);
	terrainMat->SetTexture(2, grass_normal, SHADER_BIND_TYPE::PIXEL);
	terrainMat->SetTexture(3, sand_normal, SHADER_BIND_TYPE::PIXEL);
	terrainMat->SetSampler(0, dx11.GetDefaultSampler(), SHADER_BIND_TYPE::PIXEL);
	terrainMat->GetMaterialData().hasNormalTexture = true;

	resources.AddResource<Material>("terrainMaterial", terrainMat);

	/*
		MODELS
	*/

	resources.AddModel("playerModel", AssimpHandler::loadFbxObject("Animations/Glasse_Idle.fbx", dx11, animationShader));
	resources.AddModel("wagonModel", AssimpHandler::loadFbxObject("Models/Wagon.fbx", dx11, toonShader));
	resources.AddModel("backgroundPlane", AssimpHandler::loadFbxObject("Models/Background_Plane.fbx", dx11, toonShader));

}

void IntroScene::Update(const float& deltaTime)
{
	Scene::Update(deltaTime);
	introGUI->Update();
}

void IntroScene::FixedUpdate(const float& fixedDeltaTime)
{
	Scene::FixedUpdate(fixedDeltaTime);
	this->glass->GetMesh()->skeleton->AddKeyframe();
}

Scene* IntroScene::GetNextScene() const
{
	return nextScene;
}

void IntroScene::setNextScene()
{
	// Change scene logic
	for (int i = 0; i < scenes.size(); i++)
	{
		if (scenes[i]->GetName() == "DevScene") 
		{
			if (threadPtr->joinable())
			{
				threadPtr->join();
			}
			nextScene = scenes[i];
		}			
	}
}
