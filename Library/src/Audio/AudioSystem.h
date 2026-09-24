#pragma once
#include "../API.h"
#include "../Engine/Singleton.h"
#include <string>
#include <fmod/fmod.hpp>

namespace clc
{
	class API AudioSystem
	{
		CLC_SINGLETON(AudioSystem);

	public:
		bool Initialize();
		void Shutdown();

		// fmod needs update
		void Update();

		FMOD::Sound* LoadSound(const std::string& path); // for the resource manager

		FMOD::Channel* Play(FMOD::Sound* sound, bool loop, float volume, float speed = 1.0f); // AudioSource is meant to use it
		void StopAll();

		void SetMasterVolume(float volume);
		float GetMasterVolume();

		FMOD::System* GetSystem() { return mSystem; }

	private:
		FMOD::System* mSystem = nullptr;
		FMOD::ChannelGroup* mMasterGroup = nullptr;
		int mMaxChannels = 64;
	};
}

#define clcAudio (clc::AudioSystem::GetInstance())