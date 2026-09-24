#include "pch.h"
#include "AudioSource.h"
#include "../Resources/ResourceManager.h"
#include "../Engine/Serialization.h"

namespace clc
{
	AudioSource::AudioSource() {}
	AudioSource::AudioSource(const std::string& path) { mSoundPath = path; }
	AudioSource::~AudioSource() { Stop(); }

	AudioSource* AudioSource::Clone()
	{
		AudioSource* copy = new AudioSource(*this);
		copy->mChannel = nullptr; // the copy starts silent
		return copy;
	}

	void AudioSource::OnCreate()
	{
		if (mSoundPath.empty())
			return;

		// sound from the rm
		mSound = clcResources.GetSound(mSoundPath);
	}

	void AudioSource::Start()
	{
		if (mPlayOnStart)
			Play();
	}

	void AudioSource::RemoveFromSystem()
	{
		Stop();
	}

	// playback
	void AudioSource::Play()
	{
		if (!mSound)
			return;

		Stop(); // one channel per source
		mChannel = clcAudio.Play(mSound, mLoop, mVolume, mSpeed);
	}

	void AudioSource::Stop()
	{
		if (!mChannel || !clcAudio.GetSystem())
			return;

		mChannel->stop();
		mChannel = nullptr;
	}

	void AudioSource::SetPaused(bool paused)
	{
		if (mChannel)
			mChannel->setPaused(paused);
	}

	bool AudioSource::IsPlaying()
	{
		if (!mChannel)
			return false;

		bool playing = false;
		mChannel->isPlaying(&playing);
		return playing;
	}

	void AudioSource::SetLoop(bool loop)
	{
		mLoop = loop;
		if (!mChannel)
			return;

		// the one already playing changes too
		FMOD_MODE mode = FMOD_LOOP_OFF;
		if (mLoop)
			mode = FMOD_LOOP_NORMAL;

		mChannel->setMode(mode);
	}

	void AudioSource::SetVolume(float volume)
	{
		mVolume = volume;

		// fmod wants it between 0 and 1
		if (mVolume < 0.0f)
			mVolume = 0.0f;
		if (mVolume > 1.0f)
			mVolume = 1.0f;

		if (mChannel)
			mChannel->setVolume(mVolume);
	}

	void AudioSource::RaiseVolume(float amount)
	{
		SetVolume(mVolume + amount);
	}

	void AudioSource::LowerVolume(float amount)
	{
		SetVolume(mVolume - amount);
	}

	void AudioSource::SetSpeed(float speed)
	{
		mSpeed = speed;
		if (mChannel)
			mChannel->setPitch(mSpeed);
	}

	void AudioSource::Serialize(nlohmann::json& j) const
	{
		IBase::Serialize(j);

		j["sound"] = mSoundPath;
		j["volume"] = mVolume;
		j["speed"] = mSpeed;
		j["loop"] = mLoop;
		j["playOnStart"] = mPlayOnStart;
	}

	void AudioSource::Deserialize(const nlohmann::json& j)
	{
		IBase::Deserialize(j);

		if (j.contains("sound")) mSoundPath = j["sound"].get<std::string>();
		if (j.contains("volume")) mVolume = j["volume"].get<float>();
		if (j.contains("speed")) mSpeed = j["speed"].get<float>();
		if (j.contains("loop")) mLoop = j["loop"].get<bool>();
		if (j.contains("playOnStart")) mPlayOnStart = j["playOnStart"].get<bool>();
	}
}