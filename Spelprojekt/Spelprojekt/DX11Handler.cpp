#include "DX11Handler.h"

DX11Handler::DX11Handler() : device(nullptr), context(nullptr), swapchain(nullptr), mainRasterizerState(nullptr)
{
}

DX11Handler::~DX11Handler()
{
	device->Release();
	device = 0;

	context->Release();
	context = 0;

	swapchain->Release();
	swapchain = 0;

	mainRasterizerState->Release();
	mainRasterizerState = 0;
}

void DX11Handler::Initialize(const Window& window)
{
	/////////////////				SWAPCHAIN INITIALIZE				/////////////////
	
	// Swapchain: swaps between two buffers
	ZeroMemory(&swapChainDescription, sizeof(DXGI_SWAP_CHAIN_DESC));

	swapChainDescription.BufferCount = 2;	 // one back buffer

	swapChainDescription.BufferDesc.Width = window.GetWidth();
	swapChainDescription.BufferDesc.Height = window.GetHeight();
	swapChainDescription.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDescription.BufferDesc.RefreshRate.Numerator = 60; // sets framerate to 60 as max
	swapChainDescription.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDescription.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDescription.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	swapChainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT; // DXGI_USAGE_SHADER_INPUT = can be used as a texture input in a hlsl file. 
	swapChainDescription.OutputWindow = window.GetHWND();
	swapChainDescription.SampleDesc.Count = 1;
	swapChainDescription.SampleDesc.Quality = 0;
	swapChainDescription.Windowed = TRUE;
	swapChainDescription.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;
	swapChainDescription.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	UINT swapflags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef  _DEBUG
	swapflags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevel[] = { D3D_FEATURE_LEVEL_11_0 };	// 
	HRESULT resultCreateDevAndSwap = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, swapflags, featureLevel, 1, D3D11_SDK_VERSION, &swapChainDescription, &swapchain, &device, nullptr, &context);
	assert(SUCCEEDED(resultCreateDevAndSwap));

	/////////////////				END SWAPCHAIN INITIALIZE				/////////////////

	CreateBackbufferRenderTarget(window.GetWidth(), window.GetHeight());
	//SetWireframeMode(false);

	/////////////////				RASTERDESC INITIALIZE				/////////////////

	D3D11_RASTERIZER_DESC rasterizerDescription;
	ZeroMemory(&rasterizerDescription, sizeof(D3D11_RASTERIZER_DESC));

	rasterizerDescription.FillMode = D3D11_FILL_SOLID;
	rasterizerDescription.CullMode = D3D11_CULL_BACK;
	rasterizerDescription.DepthClipEnable = true;
	rasterizerDescription.FrontCounterClockwise = false;
	rasterizerDescription.ScissorEnable = false;
	rasterizerDescription.DepthBias = 0;
	rasterizerDescription.DepthBiasClamp = 0.0f;

	HRESULT resultCreateRasterizer = device->CreateRasterizerState(&rasterizerDescription, &mainRasterizerState);
	assert(SUCCEEDED(resultCreateRasterizer));


	// SHADOW RASTERIZER STATE
	rasterizerDescription.CullMode = D3D11_CULL_FRONT;
	resultCreateRasterizer = device->CreateRasterizerState(&rasterizerDescription, &shadowRasterizerState);
	assert(SUCCEEDED(resultCreateRasterizer));

	context->RSSetState(mainRasterizerState);

	this->defaultSampler = CreateSampler(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP);
}

ID3D11SamplerState* DX11Handler::CreateSampler(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE mode)
{
	D3D11_SAMPLER_DESC samplerDescription;
	ZeroMemory(&samplerDescription, sizeof(D3D11_SAMPLER_DESC));
	samplerDescription.Filter = filter;
	samplerDescription.AddressU = mode;
	samplerDescription.AddressV = mode;
	samplerDescription.AddressW = mode;
	samplerDescription.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;

	for (size_t i = 0; i < 4; i++)
		samplerDescription.BorderColor[i] = 1.0f;

	ID3D11SamplerState* samplerState;
	ZeroMemory(&samplerState, sizeof(ID3D11SamplerState));
	device->CreateSamplerState(&samplerDescription, &samplerState);
	return samplerState;
}


void DX11Handler::CreateBackbufferRenderTarget(size_t width, size_t height)
{
	this->backBufferPtr;
	swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);

	// assert backbufferPtr instead

	if (backBufferPtr != nullptr)
	{
		backbufferRTV;
		device->CreateRenderTargetView(backBufferPtr, nullptr, &backbufferRTV);
		backBufferPtr->Release();

		if (backbufferRTV != nullptr)
		{
			ID3D11DepthStencilView* dsv;
			HRESULT hr;

			// VIEWPORT
			D3D11_VIEWPORT viewport;
			ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
			viewport.TopLeftX = 0.0f;
			viewport.TopLeftY = 0.0f;
			viewport.Width = static_cast<float>(width);
			viewport.Height = static_cast<float>(height);
			viewport.MinDepth = 0.0f;
			viewport.MaxDepth = 1.0f;

			// DEPTH TEXTURE
			D3D11_TEXTURE2D_DESC depthTexDesc;
			depthTexDesc.Width = width;
			depthTexDesc.Height = height;
			depthTexDesc.MipLevels = 1;
			depthTexDesc.ArraySize = 1;
			depthTexDesc.SampleDesc.Count = 1;
			depthTexDesc.SampleDesc.Quality = 0;
			depthTexDesc.Format = DXGI_FORMAT_D32_FLOAT;
			depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
			depthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			depthTexDesc.CPUAccessFlags = 0;
			depthTexDesc.MiscFlags = 0;

			ID3D11Texture2D* depthTexture;
			hr = device->CreateTexture2D(&depthTexDesc, 0, &depthTexture);
			assert(SUCCEEDED(hr));

			D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
			dsvDesc.Format = depthTexDesc.Format;
			dsvDesc.Flags = 0;
			dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			dsvDesc.Texture2D.MipSlice = 0;

			hr = device->CreateDepthStencilView(depthTexture, &dsvDesc, &dsv);
			assert(SUCCEEDED(hr));

			depthTexture->Release();
			depthTexture = nullptr;

			this->backbuffer = new RenderTarget(backbufferRTV, nullptr, dsv, viewport);
		}
	}
}
