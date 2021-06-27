#pragma once
#include <d3d11_1.h>
#include <dxgi.h>     // anv�nds f�r swapchain och device / deviceContext
#include <DirectXMath.h>

#include "Material.h"
#include "Mesh.h"
#include "DX11Handler.h"
#include "RenderTarget.h"
#include "ConstantBuffers.h"
#include "Lights.h"
#include "SSAO.h"
#include "GUI.h"
#include "Entities.h"

constexpr float CLEAR_COLOR[3] = { 0.592f, 0.824f, 0.949f };

class Renderer
{
	const float SHADOW_MAP_SIZE = 2048;

public:
	Renderer(size_t width, size_t height, Timer&, DX11Handler&);
	virtual ~Renderer();

	void SetDeferredRenderTarget();
	void SetRenderTarget(RenderTarget* renderTarget);

	void ClearRenderTarget();
	void DrawMesh(Mesh*, DirectX::XMMATRIX world, DirectX::XMMATRIX view, DirectX::XMMATRIX projection, DirectX::XMFLOAT3 right, DirectX::XMFLOAT3 up, DirectX::XMFLOAT3 centre);

	Lights& GetLights() { return this->lights; }

	void SetGUI(GUI* gui) { this->gui = gui; }

	void ShadowPass(DirectX::XMVECTOR focus, const AABB& bounds, Camera* camera, Entities* entities);
	void DisplayFrame(Camera* camera);
	void setVsync(bool vsync) { this->vSync = vSync; }
	void DrawScreenQuad();

private:
	void DrawMesh(Mesh*);

private:
	bool vSync = false;
	DX11Handler& dx11;
	Timer& timer;

	Lights lights;
	SSAO ssao;

	Shader* deferredLightShader;
	Mesh* screenQuad;

	Material* meshMat;
	GUI* gui;

	RenderTarget* shadowRenderTarget;
	Shader* shadowShader;

	ID3D11Buffer* worldBuffer_ptr;
	WorldData cb_world;

	RenderTarget* backbufferRenderTarget;
	RenderTarget* gbufferRenderTarget;
	RenderTarget* currentRenderTarget;

	ID3D11SamplerState* gbuffersampler;
};