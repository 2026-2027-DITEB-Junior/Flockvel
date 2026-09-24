#include "pch.h"
#include "AudioSystem.h"
#include <iostream>

namespace clc
{
	bool AudioSystem::Initialize()
	{
		if (FMOD::System_Create(&mSystem) != FMOD_OK) {
			std::cout << "fmod: cant create the system\n";
			return false;
		}

		if (mSystem->init(mMaxChannels, FMOD_INIT_NORMAL, nullptr) != FMOD_OK) {
			std::cout << "fmod: init failed\n";
			mSystem->release();
			mSystem = nullptr;
			return false;
		}

		mSystem->getMasterChannelGroup(&mMasterGroup);
		return true;
	}

	void AudioSystem::Shutdown()
	{
		if (!mSystem)
			return;

		mSystem->close();
		mSystem->release();

		mSystem = nullptr;
		mMasterGroup = nullptr;
	}

	void AudioSystem::Update()
	{
		if (mSystem)
			mSystem->update();
	}

	FMOD::Sound* AudioSystem::LoadSound(const std::string& path)
	{
		if (!mSystem)
			return nullptr;

		FMOD::Sound* sound = nullptr;
		if (mSystem->createSound(path.c_str(), FMOD_DEFAULT, nullptr, &sound) != FMOD_OK) {
			std::cout << "fmod: cant load " << path << "\n";
			return nullptr;
		}

		return sound;
	}

	FMOD::Channel* AudioSystem::Play(FMOD::Sound* sound, bool loop, float volume, float speed)
	{
		if (!mSystem || !sound)
			return nullptr;

		// starts paused to set it up before playing
		FMOD::Channel* channel = nullptr;
		if (mSystem->playSound(sound, mMasterGroup, true, &channel) != FMOD_OK)
			return nullptr;

		FMOD_MODE mode = FMOD_LOOP_OFF;
		if (loop)
			mode = FMOD_LOOP_NORMAL;

		channel->setMode(mode);
		channel->setVolume(volume);
		channel->setPitch(speed);
		channel->setPaused(false);

		return channel;
	}

	void AudioSystem::StopAll()
	{
		if (mMasterGroup)
			mMasterGroup->stop();
	}

	void AudioSystem::SetMasterVolume(float volume)
	{
		if (mMasterGroup)
			mMasterGroup->setVolume(volume);
	}

	float AudioSystem::GetMasterVolume()
	{
		float volume = 1.0f;
		if (mMasterGroup)
			mMasterGroup->getVolume(&volume);

		return volume;
	}
}