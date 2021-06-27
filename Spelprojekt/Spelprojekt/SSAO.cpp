#include "SSAO.h"
#include "Renderer.h"

SSAO::SSAO(size_t width, size_t height) : width(width), height(height), dx11(nullptr), shader(nullptr), randomTexture(nullptr)
{
	ssaoData.ssao_scale = 2.0f;
	ssaoData.ssao_bias = 0.01f;
	ssaoData.ssao_intensity = 3.2f;
	ssaoData.ssao_radius = 1.2f;
	ssaoData.screenSize.x = static_cast<float>(width);
	ssaoData.screenSize.y = static_cast<float>(height);
}

SSAO::~SSAO()
{
	delete this->shader;
	delete this->randomTexture;
}

void SSAO::Initialize(DX11Handler* dx11)
{
	this->dx11 = dx11;
	this->randomTexture = Texture::CreateTexture("Textures/ssaoRandom.jpg", *dx11);
	this->sampler = dx11->GetDefaultSampler();

	this->renderTarget = new RenderTarget(1, width, height, false);
	this->renderTarget->Initalize(dx11->GetDevice());

	this->shader = new Shader();
	this->shader->LoadVertexShader(L"Shaders/ScreenQuad_vs.hlsl", "main", dx11->GetDevice());
	this->shader->LoadPixelShader(L"Shaders/SSAO_ps.hlsl", "main", dx11->GetDevice());

	ssaoBuffer_ptr = dx11->CreateBuffer<SSAOBuffer>(ssaoData);

	dx11->GetContext()->UpdateSubresource(ssaoBuffer_ptr, 0, 0, &ssaoData, 0, 0);
	dx11->GetContext()->PSSetConstantBuffers(SSAO_CONSTANT_BUFFER_SLOT, 1, &ssaoBuffer_ptr);
}

void SSAO::Pass(Renderer* renderer, RenderTarget* gBuffer)
{
	renderer->SetRenderTarget(renderTarget);
	renderer->ClearRenderTarget();

	// bind ssao_random texture + normal and position buffer
	ID3D11ShaderResourceView* srv = randomTexture->GetSRV();
	ID3D11ShaderResourceView** buffers = gBuffer->GetShaderResourceViews();

	dx11->GetContext()->PSSetShaderResources(0, 1, &buffers[2]);
	dx11->GetContext()->PSSetShaderResources(1, 1, &buffers[3]);
	dx11->GetContext()->PSSetShaderResources(2, 1, &srv);
	dx11->GetContext()->PSSetSamplers(0, 1, &sampler);

	// bind SSAO shader
	shader->Bind(dx11->GetContext());
	renderer->DrawScreenQuad();

	ID3D11ShaderResourceView* pSRV[1] = { nullptr };
	ID3D11SamplerState* ssrf[1] = { nullptr };
	dx11->GetContext()->PSSetShaderResources(0, 1, pSRV);
	dx11->GetContext()->PSSetShaderResources(1, 1, pSRV);
	dx11->GetContext()->PSSetShaderResources(2, 1, pSRV);
	dx11->GetContext()->PSSetSamplers(0, 1, ssrf);
}

ID3D11ShaderResourceView* SSAO::GetOutputSRV() const
{
	return this->renderTarget->GetShaderResourceViews()[0];
}
