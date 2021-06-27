#pragma once
#include <WICTextureLoader.h>
#include "GUIObject.h"

class GUIActionbar : public GUIObject
{
	public:
		GUIActionbar(DX11Handler&, std::string spriteFile, float x, float y);
		void Draw(DirectX::SpriteBatch*) override;
		void SetPosition(float x, float y) override;
		void SetWICSprite(DX11Handler& dx11, std::string spriteFile);

	private:
		DX11Handler* dxHandler;
		DirectX::SpriteBatch* spritebatch;
		ID3D11ShaderResourceView* SRV;

		float xPosition, yPosition;
		float xScale, yScale;
		float rotation;

		DirectX::XMVECTOR position, color, origin, scale;
};