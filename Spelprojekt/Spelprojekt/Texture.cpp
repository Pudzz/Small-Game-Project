#include "Texture.h"

Texture::Texture() : texture(nullptr), textureSRV(nullptr)
{
}

Texture::~Texture()
{
}

Texture* Texture::CreateTexture(std::string filePath, DX11Handler& dx11)
{
	Texture* texture = new Texture();
	HRESULT hr;
	std::wstring wsConvert(filePath.begin(), filePath.end());

	hr = DirectX::CreateWICTextureFromFile(dx11.GetDevice(), wsConvert.c_str(), &texture->texture, &texture->textureSRV);
	assert(SUCCEEDED(hr));

	ID3D11Texture2D* pTextureInterface = 0;
	texture->texture->QueryInterface<ID3D11Texture2D>(&pTextureInterface);
	pTextureInterface->GetDesc(&texture->imageSampleDesc);
	pTextureInterface->Release();

	return texture;
}