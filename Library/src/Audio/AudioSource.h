#pragma once
#include "../Engine/IComp.h"
#include "AudioSystem.h"

namespace clc
{
	class API AudioSource : public IComp
	{
	public:
		AudioSource();
		AudioSource(const std::string& path);
		~AudioSource();

		AudioSource* Clone() override;

		void OnCreate() override;
		void Start() override;
		void RemoveFromSystem() override;

		// playback functions
		void Play();
		void Stop();
		void SetPaused(bool paused);
		bool IsPlaying();

		void SetLoop(bool loop);
		void SetVolume(float volume);
		void RaiseVolume(float amount = 0.1f);
		void LowerVolume(float amount = 0.1f);
		void SetSpeed(float speed);

		void Serialize(nlohmann::json& j) const override;
		void Deserialize(const nlohmann::json& j) override;

		std::string mSoundPath;
		float mVolume = 1.0f;
		float mSpeed = 1.0f;
		bool mLoop = false;
		bool mPlayOnStart = false;

	private:
		// the resource manager owns it
		FMOD::Sound* mSound = nullptr;

		// null while it is not playing
		FMOD::Channel* mChannel = nullptr;
	};
}