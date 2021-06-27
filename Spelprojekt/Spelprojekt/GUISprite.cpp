#include "GUISprite.h"
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>
#include <wrl.h>


GUISprite::GUISprite( DX11Handler& dx11, std::string spriteFile, float x, float y) 
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



	Microsoft::WRL::ComPtr<ID3D11Resource> res;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> tex;
	HRESULT result;
	std::wstring wsConvert(spriteFile.begin(), spriteFile.end());

	result = DirectX::CreateWICTextureFromFile(dx11.GetDevice(), wsConvert.c_str(), &res, &SRV);
	

	result = res.As(&tex);

	D3D11_TEXTURE2D_DESC desc;
	tex->GetDesc(&desc);	

	width = desc.Width;
	height = desc.Height;
	std::cout << width << "  " << height << std::endl;
	assert(SUCCEEDED(result));
}

GUISprite::~GUISprite()
{
	if (SRV != nullptr) {
		SRV->Release();
		SRV = 0;
	}

	if (spritebatch != nullptr) {
		spritebatch = nullptr;
		spritebatch = 0;
	}
}

void GUISprite::Draw(DirectX::SpriteBatch* spritebatch)
{	
	spritebatch->Draw(SRV, this->position, nullptr, this->color, rotation, origin, scale,DirectX::SpriteEffects::SpriteEffects_None, 0.0f);
	
}

void GUISprite::SetPosition(float x, float y)
{
	this->xPosition = x;
	this->yPosition = y;
	this->position = DirectX::XMVectorSet(this->xPosition, this->yPosition, 0, 0);
}

void GUISprite::SetWICSprite(DX11Handler& dx11, std::string spriteFile)
{
	if (this->SRV != nullptr)
		this->SRV->Release();			

	HRESULT result;
	std::wstring wsConvert(spriteFile.begin(), spriteFile.end());
	result = DirectX::CreateWICTextureFromFile(dx11.GetDevice(), wsConvert.c_str(), nullptr, &SRV);
	assert(SUCCEEDED(result));
}

void GUISprite::SetDDSSprite(DX11Handler& dx11, std::string spriteFile)
{
	if (this->SRV != nullptr)
		this->SRV->Release();

	HRESULT result;
	std::wstring wsConvert(spriteFile.begin(), spriteFile.end());
	result = DirectX::CreateDDSTextureFromFile(dx11.GetDevice(), wsConvert.c_str(), nullptr, &SRV);
	assert(SUCCEEDED(result));
}

bool GUISprite::Clicked(Input* input)
{
	if (MouseOver(input) && input->GetLeftMouseButtonDown())
		return true;
	else
		return false;
}

bool GUISprite::MouseOver(Input* input)
{
	if ((input->GetMousePosition().x > xPosition && input->GetMousePosition().x < xPosition + width) && (input->GetMousePosition().y > yPosition && input->GetMousePosition().y < yPosition + height))
	{
		return true;
	}
	else
		return false;

}

void GUISprite::HealthBar(float maxHealth, float currentHealth)
{	
	this->xScale = currentHealth/maxHealth;
	float xs = this->xScale;
	this->scale = DirectX::XMVectorSet(xs, 1, 1, 1);
}

void GUISprite::VolumeBar(float maxVol, float currentVol)
{
	this->xScale = currentVol / maxVol;
	float xs = this->xScale;
	this->scale = DirectX::XMVectorSet(xs, 1, 1, 1);
}

