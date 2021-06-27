#pragma once
#include <WICTextureLoader.h>
#include "GUIObject.h"
#include "Input.h"
class GUISprite: public GUIObject
{
public:
	GUISprite(DX11Handler&, std::string spriteFile, float x, float y);
	~GUISprite();

	void Draw(DirectX::SpriteBatch*) override;
	void SetPosition(float x, float y) override;
	
	void SetWICSprite(DX11Handler& dx11, std::string spriteFile);
	void SetDDSSprite(DX11Handler& dx11, std::string spriteFile);
	bool Clicked(Input* input);
	bool MouseOver(Input* input);
	int GetTextureHeight() { return this->height; };
	int GetTextureWidth() { return this->width; };

	// Update from player/enemies class current health
	void HealthBar(float maxHealth, float currentHealth);	
	void VolumeBar(float maxVol, float currentVol);	

private:
	DX11Handler* dxHandler;
	DirectX::SpriteBatch* spritebatch;	
	ID3D11ShaderResourceView* SRV;
	int width, height;
	float xPosition, yPosition;
	float xScale, yScale;
	float rotation; 

	DirectX::XMVECTOR position, color, origin, scale; 
};