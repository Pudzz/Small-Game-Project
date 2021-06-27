#pragma once
#include <d3d11.h>  
#include <DirectXMath.h>
#include <vector>
#include <map>
#include <string>
#include "Skeleton.h"

struct MeshVertex
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 uv;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT3 tangent;
	float weights;
	int IDS;

	MeshVertex() : position(0, 0, 0), uv(0, 0), normal(0, 0, 0), tangent(0, 0, 0), weights(0.0f), IDS(-1) {}
	MeshVertex(DirectX::XMFLOAT3 position, DirectX::XMFLOAT2 uv, DirectX::XMFLOAT3 normal, DirectX::XMFLOAT3 tangent)
		: position(position), uv(uv), normal(normal), tangent(tangent), weights(0.0f), IDS(-1) {}

};

struct Mesh
{
	ID3D11Buffer* vertexBuffer;
	std::vector<MeshVertex> vertexes;

	ID3D11Buffer* indexBuffer;
	std::vector<unsigned int> indices;

	// Skeleton for animation
	Skeleton* skeleton;

	Mesh()
	{
		vertexBuffer = nullptr;
		indexBuffer = nullptr;
		skeleton = nullptr;
	}

	virtual ~Mesh()
	{
		if (vertexBuffer) 
			vertexBuffer->Release();
		if (indexBuffer)
			indexBuffer->Release();
		if (skeleton)
			delete skeleton;
	}
};

namespace MeshCreator
{
	inline Mesh* CreateMesh(std::vector<MeshVertex> vertices, std::vector<unsigned int> indices, ID3D11Device* device)
	{
		Mesh* mesh = new Mesh();

		//mesh->vertexBuffer = nullptr;
		mesh->vertexes = vertices;
		//mesh->indexBuffer = nullptr;
		mesh->indices = indices;

		// creates vertex buffer
		D3D11_BUFFER_DESC vertexBufferDescription;
		ZeroMemory(&vertexBufferDescription, sizeof(vertexBufferDescription));
		vertexBufferDescription.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDescription.ByteWidth = (UINT)(sizeof(MeshVertex) * mesh->vertexes.size());
		vertexBufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA vertexBuffer_subResource;
		ZeroMemory(&vertexBuffer_subResource, sizeof(D3D11_SUBRESOURCE_DATA));
		vertexBuffer_subResource.pSysMem = vertices.data();
		vertexBuffer_subResource.SysMemPitch = 0;
		vertexBuffer_subResource.SysMemSlicePitch = 0;

		HRESULT vertexBufferResult = device->CreateBuffer(&vertexBufferDescription, &vertexBuffer_subResource, &mesh->vertexBuffer);
		assert(SUCCEEDED(vertexBufferResult));

		// creates index buffer
		D3D11_BUFFER_DESC indexBufferDescription;
		indexBufferDescription.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDescription.ByteWidth = (UINT)(sizeof(unsigned int) * indices.size());
		indexBufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDescription.CPUAccessFlags = 0;
		indexBufferDescription.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA indexBuffer_subResource;
		indexBuffer_subResource.pSysMem = indices.data();
		indexBuffer_subResource.SysMemPitch = 0;
		indexBuffer_subResource.SysMemSlicePitch = 0;

		HRESULT indexBufferResult = device->CreateBuffer(&indexBufferDescription, &indexBuffer_subResource, &mesh->indexBuffer);
		assert(SUCCEEDED(indexBufferResult));
		return mesh;
	}

	inline Mesh* CreateScreenQuad(ID3D11Device* device)
	{
		const float size = 1.0f;
		std::vector<MeshVertex> vertices =
		{
			MeshVertex({ -size, -size, 0 }, { 0,1 }, { 0,0,0 }, { 0,0,0 }),		// 0,0
			MeshVertex({ size, -size, 0 }, { 1,1 }, { 0,0,0 }, { 0,0,0 }),		// 0, w
			MeshVertex({ size, size, 0 }, { 1,0 }, { 0,0,0 }, { 0,0,0 }),		// h, w
			MeshVertex({ -size, size, 0 }, { 0,0 }, { 0,0,0 }, { 0,0,0 })		// h, 0
		};

		std::vector<unsigned int> indices = { 3,2,1, 3,1,0 };
		return CreateMesh(vertices, indices, device);
	}
}