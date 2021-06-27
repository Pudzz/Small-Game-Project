#include "Resources.h"

Resources::Resources()
{
}

Resources::~Resources()
{
	for (auto i : resourceMap)
		delete i.second;

	resourceMap.clear();
}

AssimpHandler::AssimpData Resources::GetModel(std::string key)
{
	AssimpHandler::AssimpData* dd = GetResource<AssimpHandler::AssimpData>(key);

	if (dd == nullptr)	
		throw std::exception("no model");

	return *dd;
}

void Resources::AddModel(std::string key, AssimpHandler::AssimpData* data)
{
	AddResource(key, data);
}

void Resources::RemoveResource(std::string key)
{
	auto findItem = resourceMap.find(key);
	if (findItem != resourceMap.end())
		resourceMap.erase(findItem);
}
