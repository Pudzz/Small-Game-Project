#pragma once

#include <string>
#include "Mesh.h"
#include <algorithm>
#include<DirectXMath.h>


class Terrain
{
	const float XZScale = 1.0f;

public:

	Terrain();
	virtual ~Terrain();

	void GenerateMesh (std::string texturePath, ID3D11Device* device, bool water);

	int GetMapHeight() { return this->height; };
	int GetMapWidth() { return this->width; };

	float GetXZScale (){ return this->XZScale; };
	float GetVerticalScaling() { return this->verticalScaling; };

	Mesh* GetMesh () { return this->mesh; };
	float SampleHeight(float x, float z);
	DirectX::XMVECTOR SampleNormal(float x, float z);

private:
	float verticalScaling = 11.f; // height of the vertecies
	int width, height;
	Mesh* mesh;

};
