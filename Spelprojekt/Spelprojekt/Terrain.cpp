#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <DirectXMath.h>

#include "Terrain.h"
#include "DX11Handler.h"

Terrain::Terrain() : mesh(nullptr)
{
	this->height = 0;
	this->width = 0;
}

Terrain::~Terrain()
{
}

void Terrain::GenerateMesh(std::string texturePath, ID3D11Device* device, bool water)
{
	int bpp = sizeof(uint8_t) * 4;//RGBA, bits per pixel
	stbi_set_flip_vertically_on_load(1);
	uint8_t* rgb_image = stbi_load(texturePath.data(), &width, &height, &bpp, 1);

	std::vector<MeshVertex> vertList;
	std::vector<unsigned int> indexList;
	size_t amountOfIndecies = 0;
	MeshVertex vertex;

	for (size_t z = 0; z < height; z++)
	{
		for (size_t x = 0; x < width; x++)
		{
			//Vertex locations on x and y axis loaded here.
			vertex.position.x = x * XZScale;
			vertex.position.z = z * XZScale;

			//Load in height of said vertex, only returns 0-1.
			vertex.position.y = (float)rgb_image[z * width + x + 0] / 255.f;  
			vertex.position.y *= verticalScaling;
			if(water)
				vertex.uv = DirectX::XMFLOAT2(((float)x) / width, (height - (float)z) / height); // needs to be calculated when we create a quad
			else
				vertex.uv = DirectX::XMFLOAT2(((float)x) / 10, (height - (float)z) / 10); // needs to be calculated when we create a quad
			vertex.normal = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
			vertex.tangent = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

			vertList.push_back(vertex);

			if ((int)z < height - static_cast<long long>(1) && (int)x < width - static_cast<long long>(1))
			{
				// triangle 1
				indexList.push_back((unsigned int)(amountOfIndecies + width));
				indexList.push_back((unsigned int)(amountOfIndecies + width) + 1);
				indexList.push_back((unsigned int)(amountOfIndecies + 1));
				//triangle 2
				indexList.push_back((unsigned int)(amountOfIndecies + width));
				indexList.push_back((unsigned int)(amountOfIndecies + 1));
				indexList.push_back((unsigned int)(amountOfIndecies));
			}

			amountOfIndecies++;
		}
	}

	delete rgb_image;

	std::vector<DirectX::XMFLOAT3> tempNormal;
	DirectX::XMFLOAT3 unnormalized;
	DirectX::XMVECTOR edge1 = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR edge2 = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	int NumFaces = (width - 1) * (height - 1) * 2;
	float vecX, vecY, vecZ;

	for (size_t i = 0; i < NumFaces; ++i)
	{
		vecX = vertList.at(indexList.at(i * 3)).position.x - vertList.at(indexList.at((i * 3) + 2)).position.x;
		vecY = vertList.at(indexList.at(i * 3)).position.y - vertList.at(indexList.at((i * 3) + 2)).position.y;
		vecZ = vertList.at(indexList.at(i * 3)).position.z - vertList.at(indexList.at((i * 3) + 2)).position.z;
		edge1 = DirectX::XMVectorSet(vecX, vecY, vecZ, 0.0f); 

		vecX = vertList.at(indexList.at((i * 3) + 2)).position.x - vertList.at(indexList.at((i * 3) + 1)).position.x;
		vecY = vertList.at(indexList.at((i * 3) + 2)).position.y - vertList.at(indexList.at((i * 3) + 1)).position.y;
		vecZ = vertList.at(indexList.at((i * 3) + 2)).position.z - vertList.at(indexList.at((i * 3) + 1)).position.z;
		edge2 = DirectX::XMVectorSet(vecX, vecY, vecZ, 0.0f);

		XMStoreFloat3(&unnormalized, DirectX::XMVector3Normalize(DirectX::XMVector3Cross(edge1, edge2)));

		for(size_t j = 0; j < 3; j++)
		{
			vertList.at(indexList.at((i * 3) + j)).normal.x += unnormalized.x;
			vertList.at(indexList.at((i * 3)+ j)).normal.y += unnormalized.y;
			vertList.at(indexList.at((i * 3)+ j)).normal.z += unnormalized.z;
		}
	}


	for (size_t i = 0; i < vertList.size(); ++i)
	{
		DirectX::XMFLOAT3 temp = vertList.at(i).normal;
		DirectX::XMStoreFloat3(&vertList.at(i).normal, DirectX::XMVector3Normalize({ -temp.x,-temp.y,-temp.z }));
	}

	this->mesh = MeshCreator::CreateMesh(vertList, indexList, device);

}

float Terrain::SampleHeight(float x, float z)
{
	DirectX::XMFLOAT3 position = { x,0,z };

	float howFarX = position.x - (int)(position.x / XZScale);
	float howFarZ = position.z - (int)(position.z / XZScale);

	int col = (int)floorf(position.x);
	int row = (int)floorf(position.z);

	//quick exit if we are out of the heightmap	//
	if (row < 0 || col < 0 || row >= GetMapWidth() - 1 || col >= GetMapHeight() - 1)
	{
		return 0;
	}

	bool bottomTriangle = (howFarX + howFarZ) <= 1.f;
	float topRightHeight = GetMesh()->vertexes.at(row * static_cast<long long>(GetMapWidth()) + col).position.y;
	float topLeftHeight = GetMesh()->vertexes.at(row * static_cast<long long>(GetMapWidth()) + col + 1).position.y;
	float bottomLeftHeight = GetMesh()->vertexes.at((row + static_cast<long long>(1)) * static_cast<long long>(GetMapWidth()) + col).position.y;
	float bottomRightTriangle = GetMesh()->vertexes.at((row + static_cast<long long>(1)) * static_cast<long long>(GetMapWidth()) + col + 1).position.y;
	float resultHeight = 0;

	if (bottomTriangle)
	{
		float uy = topLeftHeight - topRightHeight;
		float vy = bottomLeftHeight - topRightHeight;
		resultHeight = topRightHeight + howFarX * uy + howFarZ * vy;
	}

	else
	{
		float uy = bottomLeftHeight - bottomRightTriangle;
		float vy = topLeftHeight - bottomRightTriangle;
		resultHeight = bottomRightTriangle + (1.0f - howFarX) * uy + (1.0f - howFarZ) * vy;
	}

	return resultHeight;
}

DirectX::XMVECTOR Terrain::SampleNormal(float x, float z)
{
	DirectX::XMFLOAT3 position = { x,0,z };
	//DirectX::XMStoreFloat3(&position, GetTransform().GetPosition());

	float howFarX = position.x - (int)(position.x / XZScale);
	float howFarZ = position.z - (int)(position.z / XZScale);

	int col = (int)floorf(position.x);
	int row = (int)floorf(position.z);

	//quick exit if we are out of the heightmap		//
	if (row < 0 || col < 0 || row > 250 ||col > 250)
		return { 0,0,0 };

	DirectX::XMVECTOR bottomLeft = DirectX::XMLoadFloat3(&GetMesh()->vertexes.at(row * static_cast<long long>(GetMapWidth()) + col).normal);
	DirectX::XMVECTOR bottomRight = DirectX::XMLoadFloat3(&GetMesh()->vertexes.at(row * static_cast<long long>(GetMapWidth()) + col + 1).normal);
	DirectX::XMVECTOR topLeft = DirectX::XMLoadFloat3(&GetMesh()->vertexes.at((row + static_cast<long long>(1)) * static_cast<long long>(GetMapWidth()) + col).normal);
	DirectX::XMVECTOR topRight = DirectX::XMLoadFloat3(&GetMesh()->vertexes.at((row + static_cast<long long>(1)) * static_cast<long long>(GetMapWidth()) + col + 1).normal);

	DirectX::XMVECTOR top = DirectX::XMVectorLerp(topLeft, topRight, howFarX);
	DirectX::XMVECTOR bottom = DirectX::XMVectorLerp(bottomLeft, bottomRight, howFarX);

	return DirectX::XMVectorLerp(bottom, top, howFarZ);
}
