#include "RenderTarget.h"
#include <iostream>

RenderTarget::RenderTarget(ID3D11RenderTargetView* rtv, ID3D11ShaderResourceView* srv, ID3D11DepthStencilView* dsv, D3D11_VIEWPORT viewport) : bufferCount(1), viewport(viewport), dss(nullptr)
{
	this->rtv = new ID3D11RenderTargetView * [1]{ rtv };
	this->srv = new ID3D11ShaderResourceView * [1]{ srv };
	this->dsv = dsv;

	this->bufferCount = 1;
}

RenderTarget::RenderTarget(size_t bufferCount, size_t width, size_t height, bool createDepthBuffer) :
	bufferCount(bufferCount), width(width), height(height), createDepthBuffer(createDepthBuffer), dss(nullptr)
{

}

RenderTarget::~RenderTarget()
{
	Cleanup();
}

void RenderTarget::Initalize(ID3D11Device* device)
{
	rtv = new ID3D11RenderTargetView * [bufferCount];
	srv = new ID3D11ShaderResourceView * [bufferCount];

	HRESULT hr;

	/*
		VIEWPORT
	*/
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = static_cast<float>(width);
	viewport.Height = static_cast<float>(height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	if (this->createDepthBuffer)
	{
	
		D3D11_DEPTH_STENCIL_DESC depthStencilStateDsc;
		ZeroMemory(&depthStencilStateDsc, sizeof(D3D11_DEPTH_STENCIL_DESC));

		depthStencilStateDsc.DepthEnable = true;
		depthStencilStateDsc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilStateDsc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

		depthStencilStateDsc.StencilEnable = false;
		depthStencilStateDsc.StencilReadMask = 0xFF;
		depthStencilStateDsc.StencilWriteMask = 0xFF;

		// Stencil operations if pixel is front-facing.s
		depthStencilStateDsc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilStateDsc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilStateDsc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthStencilStateDsc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// Stencil operations if pixel is back-facing.
		depthStencilStateDsc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilStateDsc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilStateDsc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthStencilStateDsc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		HRESULT createDepthStencilResult = device->CreateDepthStencilState(&depthStencilStateDsc, &dss);
		assert(SUCCEEDED(createDepthStencilResult));

		ID3D11Texture2D* depthTextureDX11;
		ZeroMemory(&depthTextureDX11, sizeof(ID3D11Texture2D));

		// DEPTH TEXTURE
		D3D11_TEXTURE2D_DESC depthTextureDescription;
		ZeroMemory(&depthTextureDescription, sizeof(D3D11_TEXTURE2D_DESC));

		depthTextureDescription.Width = width;
		depthTextureDescription.Height = height;
		depthTextureDescription.MipLevels = 1;
		depthTextureDescription.ArraySize = 1;
		depthTextureDescription.SampleDesc.Count = 1;
		depthTextureDescription.SampleDesc.Quality = 0;
		depthTextureDescription.Format = DXGI_FORMAT_R24G8_TYPELESS; //DXGI_FORMAT_R32_TYPELESS;//DXGI_FORMAT_D24_UNORM_S8_UINT; // DXGI_FORMAT_R32G32B32A32_UINT; // DXGI_FORMAT_R24G8_TYPELESS;		//DXGI_FORMAT_R32G32B32A32_FLOAT; // DXGI_FORMAT_D32_FLOAT; // DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		depthTextureDescription.Usage = D3D11_USAGE_DEFAULT;
		depthTextureDescription.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		depthTextureDescription.CPUAccessFlags = 0;
		depthTextureDescription.MiscFlags = 0;

		HRESULT textureResult = device->CreateTexture2D(&depthTextureDescription, 0, &depthTextureDX11);
		assert(SUCCEEDED(textureResult));

		// DEPTH STENCIL VIEW
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilDescription;
		ZeroMemory(&depthStencilDescription, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

		depthStencilDescription.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; //DXGI_FORMAT_D32_FLOAT;//DXGI_FORMAT_D24_UNORM_S8_UINT; // DXGI_FORMAT_D32_FLOAT;//DXGI_FORMAT_D24_UNORM_S8_UINT; // DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDescription.Flags = 0;
		depthStencilDescription.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilDescription.Texture2D.MipSlice = 0;

		// SHADER RESOURCE
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceDescription;
		ZeroMemory(&shaderResourceDescription, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

		shaderResourceDescription.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS; ///DXGI_FORMAT_R24_UNORM_X8_TYPELESS; // DXGI_FORMAT_R32_UINT;//depthTexDesc.Format;//DXGI_FORMAT_R32G32B32A32_FLOAT;// DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		shaderResourceDescription.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		//shaderResourceDescription.Texture2D.MostDetailedMip = 0;
		shaderResourceDescription.Texture2D.MipLevels = 1;


		HRESULT depthStencilResult = device->CreateDepthStencilView(depthTextureDX11, &depthStencilDescription, &dsv);
		assert(SUCCEEDED(depthStencilResult));

		HRESULT shaderResourceView = device->CreateShaderResourceView(depthTextureDX11, &shaderResourceDescription, &depth_srv);

	}

	/*
		RENDER TEXTURES
	*/
	ID3D11Texture2D** targetTextures = new ID3D11Texture2D * [bufferCount];
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	for (size_t i = 0; i < bufferCount; i++)
	{
		hr = device->CreateTexture2D(&textureDesc, nullptr, &targetTextures[i]);
		assert(SUCCEEDED(hr));
	}

	/*
		RENDER TARGET VIEWS
	*/
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	ZeroMemory(&rtvDesc, sizeof(rtvDesc));
	rtvDesc.Format = textureDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	// Create the render target views.
	for (size_t i = 0; i < bufferCount; i++)
	{
		rtvDesc.Texture2DArray.FirstArraySlice = i;
		hr = device->CreateRenderTargetView(targetTextures[i], &rtvDesc, &rtv[i]);
		assert(SUCCEEDED(hr));
	}

	/*
		SHADER RESOURCE VIEWS
	*/
	D3D11_SHADER_RESOURCE_VIEW_DESC shrDesc;
	ZeroMemory(&shrDesc, sizeof(shrDesc));

	shrDesc.Format = textureDesc.Format;
	shrDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shrDesc.Texture2D.MostDetailedMip = 0;
	shrDesc.Texture2D.MipLevels = 1;

	// Create the shader resource views.
	for (size_t i = 0; i < bufferCount; i++)
	{
		hr = device->CreateShaderResourceView(targetTextures[i], &shrDesc, &srv[i]);
		assert(SUCCEEDED(hr));
	}


	for (size_t i = 0; i < bufferCount; i++)
	{
		if (targetTextures[i])
		{
			targetTextures[i]->Release();
			targetTextures[i] = 0;
		}
	}
	delete[] targetTextures;
}

void RenderTarget::Bind(ID3D11DeviceContext* context)
{

	for (size_t i = 0; i < bufferCount; i++)
		context->PSSetShaderResources(i, 1, &srv[i]);
}

void RenderTarget::Unbind(ID3D11DeviceContext* context)
{
	for (size_t i = 0; i < bufferCount; i++)
	{
		ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
		context->PSSetShaderResources(i, 1, nullSRV);
	}
}

void RenderTarget::Cleanup()
{
	for (size_t i = 0; i < bufferCount; i++)
	{
		if (srv[i])
		{
			srv[i]->Release();
			srv[i] = 0;
		}

		if (rtv[i])
		{
			rtv[i]->Release();
			rtv[i] = 0;
		}
	}

	delete dsv;
	delete dss;

	delete[] srv;
	delete[] rtv;
}
