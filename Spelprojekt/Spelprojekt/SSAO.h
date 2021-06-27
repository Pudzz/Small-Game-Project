#pragma once
#include "DX11Handler.h"
#include "Texture.h"
#include "Mesh.h"
#include "Shader.h"
#include "RenderTarget.h"
#include "ConstantBuffers.h"

class Renderer;

class SSAO
{
public:
	SSAO(size_t width, size_t height);
	virtual ~SSAO();

	void Initialize(DX11Handler* dx11);
	void Pass(Renderer* renderer, RenderTarget* gbuffer);

	ID3D11ShaderResourceView* GetOutputSRV() const;

private:
	RenderTarget* renderTarget;
	Texture* randomTexture;
	Shader* shader;
	ID3D11SamplerState* sampler;

private:
	size_t width, height;
	DX11Handler* dx11;

	SSAOBuffer ssaoData;
	ID3D11Buffer* ssaoBuffer_ptr;
};