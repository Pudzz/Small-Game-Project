#include "SoundHandler.h"
#include "Logger.h"



SoundHandler::SoundHandler()
{
	DirectX::AUDIO_ENGINE_FLAGS eflags = DirectX::AudioEngine_Default;
#ifdef _DEBUG
	eflags = eflags | DirectX::AudioEngine_Debug;
#endif
	this->audioEngine = new DirectX::AudioEngine(eflags);
	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	assert(SUCCEEDED(hr));

	this->soundEffect = nullptr;
}

SoundHandler::~SoundHandler()
{
	for (auto i : soundMap) {
		delete i.second;
	}

	soundMap.clear();
}

void SoundHandler::LoadSound(std::string key, std::string filePath)
{
	std::wstring convertString(filePath.begin(), filePath.end());
	if (!HasKey(key))
	{
		soundEffect = new DirectX::SoundEffect(audioEngine, convertString.c_str());
		soundMap.insert({ key, soundEffect });
	}
}

void SoundHandler::PlaySound(std::string key, float volume)
{
	if (HasKey(key))
	{
		soundMap[key]->Play(volume,0,0);		
	}	
}

bool SoundHandler::HasKey(std::string key) const
{
	return soundMap.find(key) != soundMap.end();
}

void SoundHandler::SetGlobalVolume(float volume)
{
	audioEngine->SetMasterVolume(volume);
}

float SoundHandler::GetGlobalVolume()
{
	return audioEngine->GetMasterVolume();
}

void SoundHandler::ResetSound()
{
	audioEngine->Reset(); 
}

void SoundHandler::DeleteTrack(std::string key)
{
	if (HasKey(key))
	{
		soundMap.erase( key );
	}
}
