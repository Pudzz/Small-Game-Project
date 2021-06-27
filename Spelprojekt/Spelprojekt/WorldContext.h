#pragma once

class Scene;
class Player;
class Physics;
class Terrain;
class Entities;
class Resources;
class DX11Handler;
class Gamemanager;
class SpawnObjects;

struct WorldContext
{
	Scene* scene;
	Player* player;
	Physics* physics;
	Terrain* terrain;
	Entities* entities;
	Resources* resources;
	DX11Handler* dx11;
	Gamemanager* gamemanager;
	SpawnObjects* spawner;
};