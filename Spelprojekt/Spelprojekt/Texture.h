#pragma once
#include "DX11Handler.h"
#include <d3d11.h>
#include <vector>
#include <assert.h>
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>
#include <DirectXMath.h>
#include <string>
#include <wrl.h>


class Texture
{
public:

	Texture();
	virtual ~Texture();

	static Texture* CreateTexture(std::string filePath, DX11Handler& dx11);

	ID3D11ShaderResourceView* GetSRV() {return this->textureSRV;}
	ID3D11Resource* GetTexture() {return this->texture;}
	D3D11_TEXTURE2D_DESC  GetimageSampleDesc() const {return this->imageSampleDesc;}

private:	
	D3D11_TEXTURE2D_DESC  imageSampleDesc;
	ID3D11ShaderResourceView* textureSRV;
	ID3D11Resource* texture;
};