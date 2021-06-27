#pragma once
#include "GUIObject.h"

class GUIText : public GUIObject
{
public:
	GUIText();
	GUIText(DX11Handler&, const std::string& display, float x, float y);
	~GUIText();
	void Draw(DirectX::SpriteBatch*) override;	
	void SetPosition(float x, float y) override;

	void SetDXHandler(DX11Handler&);
	void SetFont(const wchar_t* font);
	void SetString(const std::string& text);
	void SetFontSize(DirectX::XMFLOAT2 fontSize);
	void SetFontColor(DirectX::XMVECTORF32 fontColor);
	std::string GetString() const;

private:
	DX11Handler* dxhandler;
	DirectX::SpriteFont* spriteFont;
	std::string text;
	std::wstring txtDisplay;
	float x, y;
	DirectX::XMFLOAT2 fontSize;
	DirectX::XMVECTORF32 fontColor;
};