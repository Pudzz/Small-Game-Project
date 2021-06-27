#include "GUI.h"

GUI::GUI(DX11Handler& dx11) : dx11(dx11)
{
	spriteBatch = new DirectX::SpriteBatch(dx11.GetContext());
}

void GUI::AddGUIObject(GUIObject *addObj, std::string name)
{
	GUIObjects.insert({ name,addObj });
}

void GUI::RemoveGUIObject(std::string name)
{
	auto temp =   GUIObjects.find(name);

	if(temp != GUIObjects.end())
	{
		GUIObjects.erase(temp);
	}
}

void GUI::DrawAll()
{
	spriteBatch->Begin();

	for (auto i : GUIObjects)
	{	
		i.second->Draw(spriteBatch);
	}

	spriteBatch->End();	
}