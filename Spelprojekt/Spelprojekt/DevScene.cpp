#include "DevScene.h"

DevScene::DevScene(Renderer* renderer, DX11Handler& dx11, Window& window, std::vector<Scene*>& scenes, Gamemanager* gamemanager) : Scene("DevScene", renderer, dx11, window), scenes(scenes), gamemanager(gamemanager) // tabort soundeffect
{
	//----- GUI SHIET |  Set gui last |

	Lights& lights = renderer->GetLights();
	lights.SetSunDirection({ 1, -1, 0 });
	lights.SetSunColor({ 0.98f, 0.96f, 0.73f, 1 });
	lights.SetSunIntensity(0.6f);
		
	this->context = new WorldContext();
	this->spawner = new SpawnObjects(context);
		
	// Fps
	fpsText = new GUIText(dx11, "Fps", window.GetWidth() -100.0f, 10);
	
	// Score text
	totalScore = new GUIText(dx11, "Score", 150.0f, 0.0f);
	totalScore->SetFontSize({ 2.0f, 2.0f });
	totalScore->SetFontColor({ 1,0,0,1 });

	// Total enemies
	totalEnemies = new GUIText(dx11, "Enemiesleft", 280, 45);
	totalEnemies->SetFontSize({ 2.0f, 2.0f });
	totalEnemies->SetFontColor({ 1,0,0,1 });

	// Time left
	gametimerText = new GUIText(dx11, "Time until extraction", 70.0f, 97.5f);
	gametimerText->SetFontSize({ 2.0f, 2.0f });
	gametimerText->SetFontColor({ 1,0,0,1 });

	// Exittext
	exitText = new GUIText(dx11, "Move to exit", window.GetWidth()/3.0f+140.0f, 10.0f);
	exitText->SetFontSize({ 1.0f, 1.0f });
	exitText->SetFontColor({ 1,0,0,1 });

	// Controller
	this->controller = new CameraController(GetSceneCamera(), window.GetInput(), CameraController::State::Follow);
	window.GetInput()->LockCursor(false);

	// Health
	gamehub = new GUISprite(dx11, "Sprites/gamehub.png", 10.0f, 10.0f);
	healthFrame = new GUISprite(dx11, "Sprites/Frame.png", 10.0f, 650.0f);
	actionbarLeft = new GUIActionbar(dx11, "Sprites/Actionbar.png", 325.0f, 650.0f);
	actionbarRight = new GUIActionbar(dx11, "Sprites/Actionbar.png", 400.0f, 650.0f);
	healthbar = new GUISprite(dx11, "Sprites/Healthbar.png", 10.0f, 650.0f);
	healthbar->HealthBar(100.0f, 100.0f);
	//--------------------------------

	// New gui
	gui = new GUI(dx11);
		
	this->arrow = nullptr;
	this->assimpScene = nullptr;
	this->billBoard = nullptr;
	this->canWin = false;
	this->exitActive = false;
	this->player = nullptr;
	this->timeUntilEnd = 0.0f;

	for (int i = 0; i < 3; i++)
		tourist[i] = nullptr;

	// Fillcontext
	context->entities = entities;
	context->gamemanager = gamemanager;
	context->resources = &resources;
	context->spawner = spawner;
	context->terrain = &terrainMesh;
	context->dx11 = &dx11;
	context->physics = &physics;
	context->scene = this;
}

DevScene::~DevScene()
{		
	delete context;
	delete controller;
}

void DevScene::Load()
{
	// Exit text ska vara false i början
	if (exitActive)
		exitActive = false;

	this->canWin = false;

	// SET TOTAL ENEMIES AND TOTAL TIME TO EXTRACTION
	this->timeUntilEnd = context->gamemanager->GetTimer();		
	this->spawner->SetMaxEnemies(context->gamemanager->GetTotalEnemies());	

	Timer testSpeed;
	testSpeed.Start();

	// Exit Wagon
	Object* wagon = new Object(ObjectLayer::Enviroment, resources.GetModel("wagonModel"));
	entities->InsertObject(wagon);
	spawner->PlaceWagon(wagon);

	// GROUNDH MESH
	Object* terrainObject = new Object(ObjectLayer::None, terrainMesh.GetMesh(), resources.GetResource<Material>("terrainMaterial"));
	entities->InsertObject(terrainObject);

	// ------- Water Shader
	Object* water = new Object(ObjectLayer::None, waterMesh.GetMesh(), resources.GetResource<Material>("waterMaterial"));
	water->GetTransform().Translate({ 0,5,0, });
	water->GetTransform().SetRotation({ 0,0,0 });
	entities->InsertObject(water);

	// ------ PLAYER
	this->player = new Player(resources.GetModel("playerModel"), controller, gui, context);
	this->player->GetTransform().SetPosition({ 55, 4, 55 });
	this->player->GetTransform().SetScale(2.3, 2.3, 2.3);
	this->player->ScaleLocalBounds().ScaleMinMax(DirectX::XMMatrixScaling(0.5, 0.5, 0.5));
	this->player->SetLayer(ObjectLayer::Player);
	this->controller->SetFollow(&this->player->GetTransform(), { 0, 10.0f, -10.0f });
	entities->InsertObject(this->player);
	context->player = player;
	
	// ------ Leveldesign
	CreateSceneObjects();

	// - - - - - Exit arrow
	arrow = new Object(ObjectLayer::None, resources.GetModel("arrowModel"));
	arrow->GetTransform().Translate(35, 10, 30);
	player->SetTargetAndArrow(arrow, wagon);
	entities->InsertObject(arrow);
	arrow->SetVisible(false);

	// - - - - - GUI OBJECTs sist, pga inget z-värde. 
	// Add objects	
	gui->AddGUIObject(gamehub, "clockicon");
	gui->AddGUIObject(gametimerText, "gametimerText");
	gui->AddGUIObject(fpsText, "fpsText");
	gui->AddGUIObject(actionbarLeft, "actionbarLeft");
	gui->AddGUIObject(actionbarRight, "actionbarRight");
	gui->AddGUIObject(totalScore, "totalscore");
	gui->AddGUIObject(totalEnemies, "totalenemiesleft");
	gui->AddGUIObject(healthbar, "healthbar");
	gui->AddGUIObject(healthFrame, "healthFrame");
	
	// Set GUI
	renderer->SetGUI(gui); 	// Set GUI

	gametimer.Start();	

	// Play scenemusic	
	gamemanager->PlayMusic();

	// PREFABS
	spawner->Initialize();

	testSpeed.Stop();
	std::cout << std::endl << " LoadTime:  " << testSpeed.GetMilisecondsElapsed() << std::endl;
}

void DevScene::Unload()
{
	Scene::Unload();
	Logger::Write("devscene unload");
	spawner->Purge();

	// @TODO
	gametimer.Restart();
	spawner->Clear();
}

void DevScene::LoadResources()
{
	// save the shaders somewhere, remember to clean it up
	Shader* toonShader = new Shader();
	toonShader->LoadPixelShader(L"Shaders/ToonShader_ps.hlsl", "main", dx11.GetDevice());
	toonShader->LoadVertexShader(L"Shaders/ToonShader_vs.hlsl", "main", dx11.GetDevice());

	Shader* terrainShader = new Shader();
	terrainShader->LoadPixelShader(L"Shaders/Terrain_ps.hlsl", "main", dx11.GetDevice());
	terrainShader->LoadVertexShader(L"Shaders/ToonShader_vs.hlsl", "main", dx11.GetDevice());

	Shader* waterShader = new Shader();
	waterShader->LoadPixelShader(L"Shaders/Water_ps.hlsl", "main", dx11.GetDevice());
	waterShader->LoadVertexShader(L"Shaders/Water_vs.hlsl", "main", dx11.GetDevice());

	Shader* animationShader = new Shader();
	animationShader->LoadPixelShader(L"Shaders/ToonShader_ps.hlsl", "main", dx11.GetDevice());
	animationShader->LoadVertexShader(L"Shaders/ToonShader_vs.hlsl", "animation", dx11.GetDevice());

	Shader* billboardShader = new Shader();
	billboardShader->LoadVertexShader(L"Shaders/Billboard_vs.hlsl", "main", dx11.GetDevice());
	billboardShader->LoadPixelShader(L"Shaders/ToonShader_ps.hlsl", "main", dx11.GetDevice());

	resources.AddResource("toonShader", toonShader);
	resources.AddResource("terrainShader", terrainShader);
	resources.AddResource("waterShader", waterShader);
	resources.AddResource("animationShader", animationShader);
	resources.AddResource("billboardShader", billboardShader);


	// ------- TERRAIN

	Texture* grass_texture = Texture::CreateTexture("Textures/Grass_ColorTest.png", dx11);
	Texture* grass_normal = Texture::CreateTexture("Textures/Grass_Normal.png", dx11);
	Texture* sand_texture = Texture::CreateTexture("Textures/Sand_Color_Test.png", dx11);
	Texture* sand_normal = Texture::CreateTexture("Textures/Sand_Normal_2.png", dx11);
	Texture* heightMapTexture = Texture::CreateTexture("Textures/map_displacement_map_small.png", dx11);

	resources.AddResource("grassTexture", grass_texture);
	resources.AddResource("grassNormal", grass_normal);
	resources.AddResource("sandTexture", sand_texture);
	resources.AddResource("sandNormal", sand_normal);
	resources.AddResource("heightMapTexture", heightMapTexture);

	/*
		MATERIALS
	*/

	Material* terrainMat = new Material(terrainShader, dx11);
	terrainMat->SetTexture(0, grass_texture, SHADER_BIND_TYPE::PIXEL);
	terrainMat->SetTexture(1, sand_texture, SHADER_BIND_TYPE::PIXEL);
	terrainMat->SetTexture(2, grass_normal, SHADER_BIND_TYPE::PIXEL);
	terrainMat->SetTexture(3, sand_normal, SHADER_BIND_TYPE::PIXEL);
	terrainMat->SetSampler(0, dx11.GetDefaultSampler(), SHADER_BIND_TYPE::PIXEL);
	terrainMat->GetMaterialData().hasNormalTexture = true;

	Material* waterMat = new Material(waterShader, dx11);
	waterMat->SetTexture(0, heightMapTexture, SHADER_BIND_TYPE::PIXEL);
	waterMat->SetSampler(0, dx11.GetDefaultSampler(), SHADER_BIND_TYPE::PIXEL);

	resources.AddResource("terrainMaterial", terrainMat);
	resources.AddResource("waterMaterial", waterMat);

	/*
		MODELS
	*/

	resources.AddModel("icecreamModel", AssimpHandler::loadFbxObject("Models/Icecream.fbx", dx11, toonShader));
	resources.AddModel("coconutModel", AssimpHandler::loadFbxObject("Models/Coconut.fbx", dx11, toonShader));
	resources.AddModel("spoonModel", AssimpHandler::loadFbxObject("Models/Spoon.fbx", dx11, toonShader));
	resources.AddModel("wagonModel", AssimpHandler::loadFbxObject("Models/Wagon.fbx", dx11, toonShader));
	resources.AddModel("enemyModel", AssimpHandler::loadFbxObject("Animations/enemy.fbx", dx11, animationShader));
	resources.AddModel("arrowModel", AssimpHandler::loadFbxObject("Models/Arrow.fbx", dx11, toonShader));

	resources.AddModel("quadInv", AssimpHandler::loadFbxObject("Models/QuadInv.fbx", dx11, billboardShader));
	resources.AddModel("mountainModel", AssimpHandler::loadFbxObject("Models/Mountain.fbx", dx11, toonShader));
	resources.AddModel("rockModel", AssimpHandler::loadFbxObject("Models/Rocks.fbx", dx11, toonShader));
	resources.AddModel("boatModel", AssimpHandler::loadFbxObject("Models/Boat.fbx", dx11, toonShader));
	resources.AddModel("kioskModel", AssimpHandler::loadFbxObject("Models/Kiosk.fbx", dx11, toonShader));

	resources.AddModel("sunchairModel", AssimpHandler::loadFbxObject("Models/Sunchair.fbx", dx11, toonShader));
	resources.AddModel("parasollModel", AssimpHandler::loadFbxObject("Models/Umbrella_BlueRed.fbx", dx11, toonShader));
	resources.AddModel("surfboardBlueModel", AssimpHandler::loadFbxObject("Models/SurfboardBlue.fbx", dx11, toonShader));
	resources.AddModel("surfboardOrangeModel", AssimpHandler::loadFbxObject("Models/SurfboardOrange.fbx", dx11, toonShader));
	resources.AddModel("surfboardTrippyModel", AssimpHandler::loadFbxObject("Models/SurfboardTrippy.fbx", dx11, toonShader));
	resources.AddModel("beachballRedModel", AssimpHandler::loadFbxObject("Models/Beachball_Red.fbx", dx11, toonShader));
	resources.AddModel("beachballBlueModel", AssimpHandler::loadFbxObject("Models/Beachball_Blue.fbx", dx11, toonShader));
	resources.AddModel("bushModel", AssimpHandler::loadFbxObject("Models/Bush.fbx", dx11, toonShader));
	resources.AddModel("bungalowModel", AssimpHandler::loadFbxObject("Models/Beachbungalow.fbx", dx11, toonShader));
	resources.AddModel("gateModel", AssimpHandler::loadFbxObject("Models/Gate.fbx", dx11, toonShader));
	resources.AddModel("restplaceBase", AssimpHandler::loadFbxObject("Models/Grill_Base.fbx", dx11, toonShader));
	resources.AddModel("restplaceRoof", AssimpHandler::loadFbxObject("Models/Grill_Roof.fbx", dx11, toonShader));
	resources.AddModel("restplaceStairs", AssimpHandler::loadFbxObject("Models/Grill_Stairs.fbx", dx11, toonShader));
	resources.AddModel("benchModel", AssimpHandler::loadFbxObject("Models/Bench.fbx", dx11, toonShader));
	resources.AddModel("palmModel", AssimpHandler::loadFbxObject("Models/Palm.fbx", dx11, toonShader));

	AssimpHandler::AssimpData* playerModel = AssimpHandler::loadFbxObject("Animations/Glasse_Idle.fbx", dx11, resources.GetResource<Shader>("animationShader"));
	resources.AddModel("playerModel", playerModel);


	/*
		ANIMATIONS
		The animations is read from an fbx file, and saved to a mesh skeleton
	*/

	this->assimpScene = imp.ReadFile("Animations/Glasse_Walk_Cycle.fbx", aiProcess_MakeLeftHanded | aiProcess_Triangulate);
	AssimpHandler::saveAnimationData(assimpScene, playerModel->mesh->skeleton, "Walk");

	this->assimpScene = imp.ReadFile("Animations/Glasse_Idle.fbx", aiProcess_MakeLeftHanded | aiProcess_Triangulate);
	AssimpHandler::saveAnimationData(assimpScene, playerModel->mesh->skeleton, "Idle");
	playerModel->mesh->skeleton->SetFirstAnimation(playerModel->mesh->skeleton->animations[1]);

	this->assimpScene = imp.ReadFile("Animations/Glasse_Attack_Right.fbx", aiProcess_MakeLeftHanded | aiProcess_Triangulate);
	AssimpHandler::saveAnimationData(assimpScene, playerModel->mesh->skeleton, "Attack");


	/*
		PREFABS
	*/

	Icecream* icecreamPrefab = new Icecream(resources.GetModel("icecreamModel"), context);
	resources.AddResource<Icecream>("icecreamPrefab", icecreamPrefab);

	Projectile* coconutPrefab = new Projectile(resources.GetModel("coconutModel"), context);
	resources.AddResource<Projectile>("coconutPrefab", coconutPrefab);

	Spoon* spoonPrefab = new Spoon(resources.GetModel("spoonModel"), context);
	resources.AddResource<Spoon>("spoonPrefab", spoonPrefab);

	//-------- ENEMY prefab
	Enemy* enemyPrefab1 = new Enemy(resources.GetModel("enemyModel"), context);
	enemyPrefab1->GetTransform().Translate(30, 7, 35);
	enemyPrefab1->GetTransform().SetScale(0.275f, 0.275f, 0.275f);
	enemyPrefab1->SetEnabled(false);

	// Animation for enemy
	this->assimpScene = imp.ReadFile("Animations/enemy.fbx", aiProcess_MakeLeftHanded | aiProcess_Triangulate);
	AssimpHandler::saveAnimationData(assimpScene, enemyPrefab1->GetMesh()->skeleton, "Enemy");
	enemyPrefab1->GetMesh()->skeleton->SetFirstAnimation(enemyPrefab1->GetMesh()->skeleton->animations[0]);

	resources.AddResource("enemyPrefab1", enemyPrefab1);

	
	// TERRAIN 'N WATER MESH

	terrainMesh.GenerateMesh("Textures/map_displacement_map_small.png", dx11.GetDevice(), false);
	waterMesh.GenerateMesh("Textures/map_displacement_map_small.png", dx11.GetDevice(), true);


	spawner->SetEnemyPrefab(resources.GetResource<Enemy>("enemyPrefab1"));
	spawner->SetPickupPrefab(resources.GetResource<Spoon>("spoonPrefab"), WeaponType::Spoon);
	spawner->SetPickupPrefab(resources.GetResource<Projectile>("coconutPrefab"), WeaponType::Coconut);

	// Tourist
	AssimpHandler::AssimpData* tourist = AssimpHandler::loadFbxObject("Animations/Regular_Tourist_Idle.fbx", dx11, resources.GetResource<Shader>("animationShader"));
	resources.AddModel("touristModel", tourist);

	this->assimpScene = imp.ReadFile("Animations/Regular_Tourist_Idle.fbx", aiProcess_MakeLeftHanded | aiProcess_Triangulate);
	AssimpHandler::saveAnimationData(assimpScene, tourist->mesh->skeleton, "Idle");
	tourist->mesh->skeleton->SetFirstAnimation(tourist->mesh->skeleton->animations[0]);
}

void DevScene::Update(const float& deltaTime)
{
	this->cameraFocusPosition = player->GetTransform().GetPosition();
	//billBoard->GetTransform().SetPosition({ player->GetTransform().GetPosition().m128_f32[0],player->GetTransform().GetPosition().m128_f32[1] + 6, player->GetTransform().GetPosition().m128_f32[2] });

	Scene::Update(deltaTime);

	this->spawner->Update(deltaTime);
	this->controller->Update(deltaTime);
		
	rp3d::Vector3 start = physics.Convert(DirectX::XMVectorAdd(player->GetTransform().GetPosition(), { 0, -100, 0 }));
	rp3d::Vector3 end = physics.Convert(DirectX::XMVectorAdd(player->GetTransform().GetPosition(), { 0, 100, 0 }));
	rp3d::Ray ray(start, end);

	// Create an instance of your callback class 
	RaycastCallback callbackObject;
	physics.GetWorld()->raycast(ray, &callbackObject);

	UpdateGUI(deltaTime);
}

void DevScene::FixedUpdate(const float& fixedDeltaTime)
{
	Scene::FixedUpdate(fixedDeltaTime);

	// Change keyframe in the animations, in order to animate the object
	this->player->GetMesh()->skeleton->AddKeyframe();
	
	for (int i = 0; i < 3; i++)
		this->tourist[i]->GetMesh()->skeleton->AddKeyframe();
}

Scene* DevScene::GetNextScene() const
{
	return nextScene;
}

void DevScene::CreateSceneObjects()
{
	if (true)
	{
		////////////////////////// MOUNTAINS /////////////////////////////
		Object* mountains[2];
		for (int i = 0; i < 2; i++) {
			mountains[i] = new Object(ObjectLayer::Enviroment, resources.GetModel("mountainModel"));
			entities->InsertObject(mountains[i]);
		}

		// Middle mountain
		mountains[0]->GetTransform().Translate(100.0f, 6.0f, 110.0f);

		// Top left mountain
		mountains[1]->GetTransform().Translate(30.0f, 4.0f, 170.0f);
		mountains[1]->GetTransform().Rotate(0.0f, 180.0f, 0.0f);


		////////////////////////// ROCKS /////////////////////////////
		Object* rocks = new Object(ObjectLayer::Enviroment, resources.GetModel("rockModel"));
		entities->InsertObject(rocks);
		rocks->GetTransform().Rotate(0.0f, 180.0f, 0.0f);
		rocks->GetTransform().Translate(195.0f, 5.0f, 145.0f);


		////////////////////////// BOATS /////////////////////////////
		Object* boat = new Object(ObjectLayer::Enviroment, resources.GetModel("boatModel"));
		boat->GetTransform().Translate(205.0f, 6.2f, 115.0f);
		boat->GetTransform().SetRotation({ 0.1f, 1.7f, -0.15f });
		entities->InsertObject(boat);
		physics.CreateCollisionBodyBoxCustom(boat, 4.0f, 10.0f, 1.0f);
		

		////////////////////////// STANDS /////////////////////////////
		// Left beach stand
		Object* beachstands[3];
		for (int i = 0; i < 3; i++) {
			beachstands[i] = new Object(ObjectLayer::Enviroment, resources.GetModel("kioskModel"));
			entities->InsertObject(beachstands[i]);
		}

		beachstands[0]->GetTransform().Translate(135.0f, 7.0f, 48.0f);

		beachstands[1]->GetTransform().Translate(125.0f, 7.0f, 48.0f);
		beachstands[1]->GetTransform().SetRotation({ -0.1f, -0.1f, 0.0f });

		beachstands[2]->GetTransform().Translate(60.0f, 7.0f, 55.0f);
		beachstands[2]->GetTransform().SetRotation({ -0.1f, 0.8f, 0.0f });

		for (int i = 0; i < 3; i++)
		{
			physics.CreateCollisionBodyBox(beachstands[i]);
		}

		////////////////////////// SUNCHAIR /////////////////////////////		
		// Chairs Left beachside

		Object* sunChairs[21];
		for (int i = 0; i < 21; i++) {
			sunChairs[i] = new Object(ObjectLayer::Enviroment, resources.GetModel("sunchairModel"));
			entities->InsertObject(sunChairs[i]);
		}

		sunChairs[0]->GetTransform().Translate(200.0f, 6.5f, 40.0f);
		sunChairs[0]->GetTransform().Rotate(-0.1f, -6.3f, 0.0f);

		sunChairs[1]->GetTransform().Translate(195.0f, 6.5f, 39.8f);
		sunChairs[1]->GetTransform().Rotate(-0.1f, -6.3f, 0.0f);

		sunChairs[2]->GetTransform().Translate(190.0f, 6.5f, 39.6f);
		sunChairs[2]->GetTransform().Rotate(-0.1f, -6.3f, 0.0f);

		sunChairs[3]->GetTransform().Translate(175.0f, 6.5f, 39.6f);
		sunChairs[3]->GetTransform().Rotate(-0.1f, -6.3f, 0.0f);

		sunChairs[4]->GetTransform().Translate(170.0f, 6.5f, 39.4f);
		sunChairs[4]->GetTransform().Rotate(-0.1f, -6.3f, 0.0f);

		sunChairs[5]->GetTransform().Translate(165.0f, 6.5f, 39.2f);
		sunChairs[5]->GetTransform().Rotate(-0.1f, -6.3f, 0.0f);

		sunChairs[6]->GetTransform().Translate(150.0f, 6.5f, 39.2f);
		sunChairs[6]->GetTransform().Rotate(-0.1f, -6.3f, 0.0f);

		sunChairs[7]->GetTransform().Translate(145.0f, 6.5f, 39.2f);
		sunChairs[7]->GetTransform().Rotate(-0.1f, -6.3f, 0.0f);

		sunChairs[8]->GetTransform().Translate(140.0f, 6.5f, 39.2f);
		sunChairs[8]->GetTransform().Rotate(-0.1f, -6.3f, 0.0f);

		sunChairs[9]->GetTransform().Translate(120.0f, 6.5f, 38.0f);
		sunChairs[9]->GetTransform().Rotate(-0.1f, -6.3f, 0.0f);

		sunChairs[10]->GetTransform().Translate(115.0f, 6.5f, 37.6f);
		sunChairs[10]->GetTransform().Rotate(-0.1f, -6.3f, 0.0f);

		sunChairs[11]->GetTransform().Translate(110.0f, 6.5f, 37.4f);
		sunChairs[11]->GetTransform().Rotate(-0.1f, -6.3f, 0.0f);

		sunChairs[12]->GetTransform().Translate(90.0f, 6.5f, 34.0f);
		sunChairs[12]->GetTransform().Rotate(-0.1f, -6.3f, 0.0f);

		sunChairs[13]->GetTransform().Translate(85.0f, 6.5f, 34.0f);
		sunChairs[13]->GetTransform().Rotate(-0.1f, -6.3f, 0.0f);

		sunChairs[14]->GetTransform().Translate(80.0f, 6.5f, 34.0f);
		sunChairs[14]->GetTransform().Rotate(-0.1f, -6.3f, 0.0f);

		sunChairs[15]->GetTransform().Translate(35.0f, 6.5f, 57.0f);
		sunChairs[15]->GetTransform().Rotate(0.0f, -5.7f, 0.0f);

		sunChairs[16]->GetTransform().Translate(40.0f, 6.5f, 55.0f);
		sunChairs[16]->GetTransform().Rotate(0.0f, -5.5f, 0.0f);

		sunChairs[17]->GetTransform().Translate(42.0f, 6.5f, 51.0f);
		sunChairs[17]->GetTransform().Rotate(0.0f, -5.3f, 0.0f);

		sunChairs[18]->GetTransform().Translate(58.0f, 6.5f, 39.0f);
		sunChairs[18]->GetTransform().Rotate(-0.1f, -5.7f, 0.0f);

		sunChairs[19]->GetTransform().Translate(62.0f, 6.5f, 37.0f);
		sunChairs[19]->GetTransform().Rotate(-0.1f, -5.7f, 0.0f);

		sunChairs[20]->GetTransform().Translate(66.0f, 6.5f, 35.0f);
		sunChairs[20]->GetTransform().Rotate(-0.1f, -5.7f, 0.0f);

		for (int i = 0; i < 21; i++)
		{
			physics.CreateCollisionBodyBoxCustom(sunChairs[i], 1.0f, 10.0f, 3.0f);
		}

		////////////////////////// PARASOLLS /////////////////////////////
		// Parasoll left beachside
		Object* parasolls[7];

		for (int i = 0; i < 7; i++)
		{
			parasolls[i] = new Object(ObjectLayer::Enviroment, resources.GetModel("parasollModel"));
			entities->InsertObject(parasolls[i]);
		}

		parasolls[0]->GetTransform().Translate(192.5f, 5.3f, 40.0f);
		parasolls[1]->GetTransform().Translate(172.5f, 5.4f, 40.0f);
		parasolls[2]->GetTransform().Translate(147.5f, 5.5f, 40.0f);
		parasolls[3]->GetTransform().Translate(117.5f, 5.6f, 38.0f);
		parasolls[3]->GetTransform().Rotate(-0.05f, 0.0f, 0.0f);
		parasolls[4]->GetTransform().Translate(87.5f, 5.6f, 35.0f);
		parasolls[4]->GetTransform().Rotate(-0.05f, 0.0f, 0.0f);
		parasolls[5]->GetTransform().Translate(45.0f, 5.6f, 55.0f);
		parasolls[5]->GetTransform().Rotate(-0.05f, 0.0f, 0.0f);
		parasolls[6]->GetTransform().Translate(65.0f, 6.0f, 40.0f);
		parasolls[6]->GetTransform().Rotate(-0.05f, 0.0f, 0.0f);

		for (int i = 0; i < 7; i++)
		{
			physics.CreateCollisionBodyCapsule(parasolls[i], 0.2f, 0.0f, 0.0f);
		}


		////////////////////////// SURFBOARDS /////////////////////////////
		Object* surfboardsBlue[2];
		surfboardsBlue[0] = new Object(ObjectLayer::Enviroment, resources.GetModel("surfboardBlueModel"));
		surfboardsBlue[0]->GetTransform().Translate(130.0f, 7.0f, 35.0f);
		entities->InsertObject(surfboardsBlue[0]);

		surfboardsBlue[1] = new Object(ObjectLayer::Enviroment, resources.GetModel("surfboardBlueModel"));
		surfboardsBlue[1]->GetTransform().Translate(168.0f, 7.3f, 210.0f);
		entities->InsertObject(surfboardsBlue[1]);


		Object* surfboardOrange[2];
		surfboardOrange[0] = new Object(ObjectLayer::Enviroment, resources.GetModel("surfboardOrangeModel"));
		surfboardOrange[0]->GetTransform().Translate(128.0f, 7.0f, 33.0f);
		entities->InsertObject(surfboardOrange[0]);

		surfboardOrange[1] = new Object(ObjectLayer::Enviroment, resources.GetModel("surfboardOrangeModel"));
		surfboardOrange[1]->GetTransform().Translate(168.0f, 7.3f, 205.0f);
		entities->InsertObject(surfboardOrange[1]);


		Object* SurfboardTrippy = new Object(ObjectLayer::Enviroment, resources.GetModel("surfboardTrippyModel"));
		SurfboardTrippy->GetTransform().Translate(130.0f, 7.0f, 37.0f);
		entities->InsertObject(SurfboardTrippy);

		for (int i = 0; i < 2; i++)
		{
			physics.CreateCollisionBodyBoxCustom(surfboardsBlue[i], 0.5f, 10.0f, 1.0f);
		}

		for (int i = 0; i < 2; i++)
		{
			physics.CreateCollisionBodyBoxCustom(surfboardOrange[i], 0.5f, 10.0f, 1.0f);
		}

		physics.CreateCollisionBodyBoxCustom(SurfboardTrippy, 0.5f, 10.0f, 1.0f);

		////////////////////////// BEACHBALLS /////////////////////////////
		// Balls left beachside

		Object* redballs[3];
		redballs[0] = new Object(ObjectLayer::Enviroment, resources.GetModel("beachballRedModel"));
		redballs[0]->GetTransform().Translate(100.0f, 7.0f, 38.0f);
		redballs[0]->GetTransform().Rotate(0.2f, -5.0f, 0.0f);
		entities->InsertObject(redballs[0]);

		redballs[1] = new Object(ObjectLayer::Enviroment, resources.GetModel("beachballRedModel"));
		redballs[1]->GetTransform().Translate(160.0f, 7.0f, 190.0f);
		redballs[1]->GetTransform().Rotate(0.2f, -5.0f, 0.0f);
		entities->InsertObject(redballs[1]);

		redballs[2] = new Object(ObjectLayer::Enviroment, resources.GetModel("beachballRedModel"));
		redballs[2]->GetTransform().Translate(35.0f, 6.0f, 45.0f);
		redballs[2]->GetTransform().Rotate(0.2f, -5.0f, 0.0f);
		entities->InsertObject(redballs[2]);

		Object* blueballs[3];

		blueballs[0] = new Object(ObjectLayer::Enviroment, resources.GetModel("beachballBlueModel"));
		blueballs[0]->GetTransform().Translate(105.0f, 6.5f, 35.0f);
		blueballs[0]->GetTransform().Rotate(0.2f, -5.0f, 0.0f);
		entities->InsertObject(blueballs[0]);

		blueballs[1] = new Object(ObjectLayer::Enviroment, resources.GetModel("beachballBlueModel"));
		blueballs[1]->GetTransform().Translate(155.0f, 6.5f, 195.0f);
		blueballs[1]->GetTransform().Rotate(0.2f, -5.0f, 0.0f);
		entities->InsertObject(blueballs[1]);

		blueballs[2] = new Object(ObjectLayer::Enviroment, resources.GetModel("beachballBlueModel"));
		blueballs[2]->GetTransform().Translate(40.0f, 6.2f, 40.0f);
		blueballs[2]->GetTransform().Rotate(0.2f, -5.0f, 0.0f);
		entities->InsertObject(blueballs[2]);

		for (int i = 0; i < 3; i++)
		{
			physics.CreateCollisionBodyCapsule(redballs[i], 0.5f, 0.0f, 0.0f);
		}

		for (int i = 0; i < 3; i++)
		{
			physics.CreateCollisionBodyCapsule(blueballs[i], 0.5f, 0.0f, 0.0f);
		}



		////////////////////////// BUSHES /////////////////////////////
		Object* bushes[13];
		for (int i = 0; i < 13; i++)
		{
			bushes[i] = new Object(ObjectLayer::Enviroment, resources.GetModel("bushModel"));
			entities->InsertObject(bushes[i]);
		}


		// 2 Stands bottom beach //
		bushes[0]->GetTransform().Translate(142.0f, 7.0f, 50.0f);
		bushes[0]->GetTransform().Rotate(0.05f, -5.0f, 0.0f);

		bushes[1]->GetTransform().Translate(120.0f, 7.0f, 47.0f);
		bushes[1]->GetTransform().Rotate(0.05f, -5.0f, 0.0f);
		////

		// Stand bottom beach left side
		bushes[2]->GetTransform().Translate(75.0f, 6.5f, 43.0f);
		bushes[2]->GetTransform().Rotate(0.05f, -5.0f, 0.0f);

		bushes[3]->GetTransform().Translate(70.0f, 6.5f, 45.0f);
		bushes[3]->GetTransform().Rotate(0.05f, -5.0f, 0.0f);
		////

		// By restplace //
		bushes[4]->GetTransform().Translate(112.0f, 7.0f, 132.0f);
		bushes[4]->GetTransform().Rotate(0.05f, -5.0f, 0.0f);

		bushes[5]->GetTransform().Translate(115.0f, 8.0f, 148.0f);
		bushes[5]->GetTransform().Rotate(0.05f, -5.0f, 0.0f);
		////


		// By rocks //
		bushes[6]->GetTransform().Translate(195.0f, 8.0f, 143.0f);
		bushes[6]->GetTransform().Rotate(0.05f, -5.0f, 0.0f);

		bushes[7]->GetTransform().Translate(200.0f, 8.0f, 152.0f);
		bushes[7]->GetTransform().Rotate(0.05f, -5.0f, 0.0f);

		// Bottom rocks by beach
		bushes[8]->GetTransform().Translate(190.0f, 7.0f, 120.0f);
		bushes[8]->GetTransform().Rotate(0.05f, -5.0f, 0.0f);

		bushes[9]->GetTransform().Translate(195.0f, 7.0f, 120.0f);
		bushes[9]->GetTransform().Rotate(0.05f, -5.0f, 0.0f);
		////

		// By benches - by rocks
		bushes[10]->GetTransform().Translate(165.0f, 7.0f, 122.0f);
		bushes[10]->GetTransform().Rotate(0.05f, -5.0f, 0.0f);
		////

		// Top beach
		bushes[11]->GetTransform().Translate(143.0f, 7.5f, 188.0f);
		bushes[11]->GetTransform().Rotate(0.05f, -5.0f, 0.0f);

		bushes[12]->GetTransform().Translate(163.0f, 7.5f, 185.0f);
		bushes[12]->GetTransform().Rotate(0.05f, -5.0f, 0.0f);

		for (int i = 0; i < 13; i++)
		{
			physics.CreateCollisionBodyCapsule(bushes[i], 1.0f, 0.0f, 0.0f);
		}

		////////////////////////// Fireplace stuff /////////////////////////////

		Object* bungalow = new Object(ObjectLayer::Enviroment, resources.GetModel("bungalowModel"));
		bungalow->GetTransform().Translate(30.0f, 7.0f, 213.0f);
		bungalow->GetTransform().SetRotation({ 0.0f, -0.9f, 0.0f });
		entities->InsertObject(bungalow);
		physics.CreateCollisionBodyBox(bungalow);

		Object* gate = new Object(ObjectLayer::Enviroment, resources.GetModel("gateModel"));
		gate->GetTransform().Translate(60.0f, 6.2f, 198.0f);
		gate->GetTransform().SetRotation({ 0.0f, -2.2f, -0.1f });
		entities->InsertObject(gate);
		physics.CreateCollisionBodyBoxCustom(gate, 1.0f, 10.0f, 2.5f);
		
		Object* restplaceBase = new Object(ObjectLayer::Enviroment, resources.GetModel("restplaceBase"));
		restplaceBase->GetTransform().Translate(107.0f, 7.5f, 140.0f);
		restplaceBase->GetTransform().SetRotation({ 0.0f, -1.5f, 0.0f });
		entities->InsertObject(restplaceBase);
		physics.CreateCollisionBodyBox(restplaceBase);

		Object* restplaceRoof = new Object(ObjectLayer::Enviroment, resources.GetModel("restplaceRoof"));
		restplaceRoof->GetTransform().Translate(107.0f, 7.5f, 140.0f);
		restplaceRoof->GetTransform().SetRotation({ 0.0f, -1.5f, 0.0f });
		entities->InsertObject(restplaceRoof);
		

		Object* restplaceStair = new Object(ObjectLayer::Enviroment, resources.GetModel("restplaceStairs"));
		restplaceStair->GetTransform().Translate(107.0f, 7.5f, 140.0f);
		restplaceStair->GetTransform().SetRotation({ 0.0f, -1.5f, 0.0f });
		entities->InsertObject(restplaceStair);
		physics.CreateCollisionBodyBox(restplaceStair);


		// Benches left beachside
		Object* benches[4];
		for (int i = 0; i < 4; i++)
		{
			benches[i] = new Object(ObjectLayer::Enviroment, resources.GetModel("benchModel"));
			entities->InsertObject(benches[i]);
		}

		benches[0]->GetTransform().Translate(170.0f, 5.5f, 118.0f);
		benches[0]->GetTransform().SetRotation({ 0.0f, 0.4f, 0.0f });

		benches[1]->GetTransform().Translate(157.0f, 5.3f, 122.0f);
		benches[1]->GetTransform().SetRotation({ 0.0f, 0.3f, 0.0f });

		benches[2]->GetTransform().Translate(140.0f, 7.0f, 195.0f);
		benches[2]->GetTransform().SetRotation({ 0.0f, -2.5f, 0.0f });

		benches[3]->GetTransform().Translate(170.0f, 7.0f, 190.0f);
		benches[3]->GetTransform().SetRotation({ 0.0f, 2.5f, 0.0f });

		for (int i = 0; i < 4; i++){
			physics.CreateCollisionBodyBoxCustom(benches[i], 2.5f, 10.0f, 1.0f);
		}
		
		////////////////////////// PALMS /////////////////////////////

		// Palms left beach side 
		Object* palms[22];
		for (int i = 0; i < 22; i++) {
			palms[i] = new Object(ObjectLayer::Tree, resources.GetModel("palmModel"));
			entities->InsertObject(palms[i]);
		}

		// Bottom beach //
		palms[0]->GetTransform().Translate(200.0f, 7.8f, 60.0f);
		palms[1]->GetTransform().Translate(180.0f, 6.6f, 75.0f);
		palms[2]->GetTransform().Translate(160.0f, 7.8f, 60.0f);
		palms[3]->GetTransform().Translate(145.0f, 6.8f, 73.0f);
		palms[4]->GetTransform().Translate(125.0f, 7.8f, 60.0f);
		palms[5]->GetTransform().Translate(105.0f, 7.8f, 75.0f);
		/////

		// bottom beach left
		palms[6]->GetTransform().Translate(85.0f, 7.8f, 58.0f);
		palms[7]->GetTransform().Translate(83.0f, 7.8f, 82.0f);
		palms[8]->GetTransform().Translate(60.0f, 8.0f, 90.0f);
		palms[9]->GetTransform().Translate(60.0f, 7.8f, 65.0f);
		////

		// left of middle rock
		palms[10]->GetTransform().Translate(55.0f, 7.8f, 115.0f);
		palms[11]->GetTransform().Translate(38.0f, 7.8f, 95.0f);
		palms[12]->GetTransform().Translate(45.0f, 7.8f, 140.0f);
		////

		// top rock
		palms[13]->GetTransform().Translate(70.0f, 7.8f, 165.0f);
		palms[14]->GetTransform().Translate(80.0f, 7.8f, 140.0f);
		palms[15]->GetTransform().Translate(105.0f, 7.8f, 165.0f);
		palms[16]->GetTransform().Translate(90.0f, 8.0f, 195.0f);
		palms[17]->GetTransform().Translate(115.0f, 7.8f, 200.0f);
		////

		// Left by rocks
		palms[18]->GetTransform().Translate(145.0f, 8.0f, 175.0f);
		palms[19]->GetTransform().Translate(170.0f, 7.8f, 160.0f);
		palms[20]->GetTransform().Translate(150.0f, 7.8f, 145.0f);
		palms[21]->GetTransform().Translate(200.0f, 7.8f, 180.0f);

		for (int i = 0; i < 22; i++)
		{
			physics.CreateCollisionBodyCapsule(palms[i], 0.5f, 0.0f, 0.0f);
		}




		for (int i = 0; i < 3; i++) {
			this->tourist[i] = new Object(ObjectLayer::None, resources.GetModel("touristModel"));
			entities->InsertObject(tourist[i]);
		}
			
		this->tourist[0]->GetTransform().SetPosition({ 115.0f ,7.5f,40.0f });
		this->tourist[0]->GetTransform().SetScale(1.5f, 1.5f, 1.5f);

		this->tourist[1]->GetTransform().SetPosition({ 90.0f,10.5f,150.0f });
		this->tourist[1]->GetTransform().SetRotation({ 0.0f, 0.1f, 0.0f });
		this->tourist[1]->GetTransform().SetScale(1.5f, 1.5f, 1.5f);

		this->tourist[2]->GetTransform().SetPosition({ 150.0f,8.0f,130.0f });
		this->tourist[2]->GetTransform().SetRotation({ 0.0f, 0.5f, 0.0f });
		this->tourist[2]->GetTransform().SetScale(1.5f, 1.5f, 1.5f);
	}
}

void DevScene::SetNextScene(bool winOrLose)
{
	// Checks the next scene to endScene, with either win or lose as a statement
	for (int i = 0; i < scenes.size(); i++)
	{
		if (scenes[i]->GetName() == "EndScene")
		{
			scenes[i]->setWinOrLose(winOrLose);
			nextScene = scenes[i];
		}
	}
}

void DevScene::UpdateGUI(const float& deltaTime)
{
	//FPS Start timer
	fpsTimer.Start();

	// Player health
	healthbar->HealthBar(100.0f, player->GetHealth());

	// Enemies left and total score
	totalEnemies->SetString(std::to_string(this->spawner->CountEnemiesRemaining()));
	totalScore->SetString(std::to_string(player->GetPoints()));
		
	// Fps stuff
	fpsTimer.Stop();
	fpsText->SetString("FPS: " + std::to_string((int)(1 / ((fpsTimer.GetMicrosecondsElapsed() / 1000000)))));
	fpsTimer.Restart();

	// Timer to end
	gametimerText->SetString(std::to_string(static_cast<int>(gametimer.GetTimeUntilEnd(timeUntilEnd))));

	// Time remaining == 0
	if (gametimer.GetTimeUntilEnd(timeUntilEnd) <= 0.0f) {
		if (!exitActive) {
			gui->AddGUIObject(exitText, "exit");
			exitActive = true;
		}

		arrow->SetVisible(true);
		gametimerText->SetString("0");
		canWin = true;
	}

	// Enemies remaining == 0
	if (this->spawner->CountEnemiesRemaining() == 0) {
		if (!exitActive) {
			gui->AddGUIObject(exitText, "exit");
			exitActive = true;
		}

		arrow->SetVisible(true);
		totalEnemies->SetString("0");
		canWin = true;
	}

	// Health is 0
	if (player->GetPlayerHealth() <= 0.0f)
	{
		gametimer.Stop();
		gui->RemoveGUIObject("exit");

		// SET CURRENTSCORE TO GAMEMANAGER
		gamemanager->SetCurrentScore(player->GetPoints() - 20);
		SetNextScene(false);
	}

	// If you can win and your bounding box overlaps with the exit area
	if (canWin && player->GetWorldBounds().Overlaps(this->player->GetWinArea()->GetWorldBounds()))
	{
		gametimer.Stop();
		gui->RemoveGUIObject("exit");
		// SET CURRENTSCORE TO GAMEMANAGER
		gamemanager->SetCurrentScore(player->GetPoints() + context->gamemanager->GetExtraPoint() + (int)gametimer.GetTimeUntilEnd(timeUntilEnd));
		SetNextScene(true);
	}
}