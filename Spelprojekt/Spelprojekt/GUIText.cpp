#include "GUIText.h"

GUIText::GUIText()
{
	this->dxhandler = nullptr;
	this->spriteFont = 0;
	this->text = "";
	this->txtDisplay = L"";	
	this->x = 0.0f;
	this->y = 0.0f;
	this->fontColor = DirectX::Colors::White;
	this->fontSize = DirectX::XMFLOAT2(1.0f, 1.0f);
}

GUIText::GUIText(DX11Handler& dx11, const std::string& display, float x, float y)
{
	this->x = x;
	this->y = y;
	this->SetString(display);
	this->spriteFont = new DirectX::SpriteFont(dx11.GetDevice(),  L"SpriteFonts/comic_sans_ms_16.spritefont");
	this->fontColor = DirectX::Colors::White;
	this->fontSize = DirectX::XMFLOAT2(1.0f, 1.0f);
}

GUIText::~GUIText()
{	
	if (dxhandler != nullptr) {
		dxhandler = nullptr;
	}

	if (spriteFont != nullptr) {
		spriteFont = nullptr;
	}
}

void GUIText::Draw(DirectX::SpriteBatch* spriteBatch)
{
	spriteFont->DrawString(
		spriteBatch,
		txtDisplay.c_str(),
		DirectX::XMFLOAT2(x, y),
		this->fontColor,
		0.0f,
		DirectX::XMFLOAT2(0.0f, 0.0f),
		this->fontSize
	);
}

std::string GUIText::GetString() const
{
	return text;
}

void GUIText::SetDXHandler(DX11Handler& dx11)
{
	this->dxhandler = &dx11;
}

void GUIText::SetFont(const wchar_t* font)
{
	this->spriteFont = new DirectX::SpriteFont(this->dxhandler->GetDevice(), font);
}

void GUIText::SetString(const std::string& text)
{
	txtDisplay = std::wstring(text.begin(), text.end());
	this->text = text;
}

void GUIText::SetFontSize(DirectX::XMFLOAT2 fontSize)
{
	this->fontSize = fontSize;
}

void GUIText::SetFontColor(DirectX::XMVECTORF32 fontColor)
{
	this->fontColor = fontColor;
}

void GUIText::SetPosition(float x, float y)
{
	this->x = x;
	this->y = y;
}
