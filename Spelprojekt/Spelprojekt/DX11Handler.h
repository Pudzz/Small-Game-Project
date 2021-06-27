#pragma once

#include <d3d11_1.h>
#include <dxgi.h> 
#include <assert.h>
#include <d3dcompiler.h>

#include "Window.h"
#include "RenderTarget.h"

#pragma comment(lib, "gdi32")
#pragma comment(lib, "d3d11") 
#pragma comment( lib, "dxgi" )   
#pragma comment(lib, "d3dcompiler.lib")

class DX11Handler
{
public:

	DX11Handler();
	virtual ~DX11Handler();

	void Initialize(const Window& window);

	ID3D11Device* GetDevice() const { return this->device; }
	ID3D11DeviceContext* GetContext() const { return this->context; }
	IDXGISwapChain* GetSwapChain() const { return this->swapchain; }
	RenderTarget* GetBackbufferRenderTarget() const { return this->backbuffer; }

	ID3D11RasterizerState* GetShadowRasterizerState() { return this->shadowRasterizerState; }
	ID3D11RasterizerState* GetMainRasterizerState() { return this->mainRasterizerState; }

	ID3D11SamplerState* CreateSampler(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE mode);
	ID3D11SamplerState* GetDefaultSampler() const { return this->defaultSampler; }

	template <typename T> ID3D11Buffer* CreateBuffer (T& data);
	
private:
	void CreateBackbufferRenderTarget(size_t width, size_t height);

private:

	ID3D11RenderTargetView* backbufferRTV;
	ID3D11Device* device;
	ID3D11DeviceContext* context;
	IDXGISwapChain* swapchain;
	
	ID3D11RasterizerState* mainRasterizerState;
	ID3D11RasterizerState* shadowRasterizerState;

	DXGI_SWAP_CHAIN_DESC swapChainDescription;
	ID3D11Texture2D* backBufferPtr;
	RenderTarget* backbuffer;

	ID3D11SamplerState* defaultSampler;
};


template<typename T>
inline ID3D11Buffer* DX11Handler::CreateBuffer(T& data)
{
	static_assert(sizeof(data) % 16 == 0, "struct is wrong size");
	ID3D11Buffer* buffer;
	ZeroMemory(&buffer, sizeof(ID3D11Buffer));

	HRESULT hr;
	D3D11_BUFFER_DESC bufferDescription;
	D3D11_SUBRESOURCE_DATA subresourceData;
	ZeroMemory(&bufferDescription, sizeof(D3D11_BUFFER_DESC));
	ZeroMemory(&subresourceData, sizeof(D3D11_SUBRESOURCE_DATA));

	bufferDescription.Usage = D3D11_USAGE_DEFAULT;
	bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDescription.CPUAccessFlags = 0;

	// WORLD BUFFER
	bufferDescription.ByteWidth = sizeof(data);
	subresourceData.pSysMem = &data;
	hr = GetDevice()->CreateBuffer(&bufferDescription, &subresourceData, &buffer);
	assert(SUCCEEDED(hr));

	return buffer;
}
