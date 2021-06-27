#pragma once
#include <unordered_map>
#include "assimpHandler.h"

class Resources
{
public:
	Resources();
	virtual	~Resources();

	template <typename T> void AddResource(std::string key, T* resource);
	template <typename T> T* GetResource(std::string key);
	AssimpHandler::AssimpData GetModel(std::string key);
	void AddModel(std::string key, AssimpHandler::AssimpData* data);

	void RemoveResource(std::string key);

private:
	std::unordered_map<std::string, void*> resourceMap;
};


template<typename T>
inline void Resources::AddResource(std::string key, T* resource)
{
	auto findItem = resourceMap.find(key);
	if (findItem == resourceMap.end())
	{
		resourceMap.insert({ key, static_cast<void*>(resource) });
	}
}

template<typename T>
inline T* Resources::GetResource(std::string key)
{
	T* item = nullptr;
	auto findItem = resourceMap.find(key);
	if (findItem != resourceMap.end())
	{
		void* dd = (*findItem).second;
		item = static_cast<T*>(dd);
	}

	return item;
}
