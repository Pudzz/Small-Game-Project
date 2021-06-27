#include "GUIActionbar.h"

GUIActionbar::GUIActionbar(DX11Handler& dx11, std::string spriteFile, float x, float y)
{
	// Store dxhandler
	this->dxHandler = &dx11;
	this->spritebatch = nullptr;

	// Store position
	this->xPosition = x;
	this->yPosition = y;
	// Set position
	this->position = DirectX::XMVectorSet(this->xPosition, this->yPosition, 0, 0);

	// Store scale
	this->xScale = 1.0f;
	this->yScale = 1.0f;
	// Set scale
	this->scale = DirectX::XMVectorSet(this->xScale, this->yScale, 1, 1);

	// Needed for drawfunction //
	this->rotation = 0.0f;								
	this->color = DirectX::XMVectorSet(1, 1, 1, 1);		
	this->origin = DirectX::XMVectorSet(1, 1, 1, 1);	
	// - - - - - - - - - - - - //	

	this->SRV = nullptr;

	HRESULT result;
	std::wstring wsConvert(spriteFile.begin(), spriteFile.end());
	result = DirectX::CreateWICTextureFromFile(dx11.GetDevice(), wsConvert.c_str(), nullptr, &SRV);
	assert(SUCCEEDED(result));
}

void GUIActionbar::Draw(DirectX::SpriteBatch* spritebatch)
{
	spritebatch->Draw(SRV, this->position, nullptr, this->color, rotation, origin, scale, DirectX::SpriteEffects::SpriteEffects_None, 0.0f);
}

void GUIActionbar::SetPosition(float x, float y)
{
	this->xPosition = x;
	this->yPosition = y;
	this->position = DirectX::XMVectorSet(this->xPosition, this->yPosition, 0, 0);
}

void GUIActionbar::SetWICSprite(DX11Handler& dx11, std::string spriteFile)
{
	if (this->SRV != nullptr)
		this->SRV->Release();

	HRESULT result;
	std::wstring wsConvert(spriteFile.begin(), spriteFile.end());
	result = DirectX::CreateWICTextureFromFile(dx11.GetDevice(), wsConvert.c_str(), nullptr, &SRV);
	assert(SUCCEEDED(result));
}