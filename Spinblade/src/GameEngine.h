#pragma once
#include <list>
#include "Engine/Engine.h"

namespace clc
{
	class GameEngine : public EngineSystem
	{
		CLC_SINGLETON(GameEngine);

	public:
		// Returns false if it fails to initialize, true otherwise
		bool SpecificInitialize() override;
		void SpecificRun() override;
		void SpecificShutdown() override;

	private:
		PhysicsSystem* phys = nullptr;
		LogicSystem* log = nullptr;
		//AudioSystem* audioSys = nullptr;

		// The mesh in which we will print the mColorBuffer texture
		GL::Model* mDisplayMesh = nullptr;
		// The shader that will be called to print the mColorBuffer texture
		GL::ShaderProgram* mDisplayShader = nullptr;

		AudioSystem* audioSys = nullptr;
	};
}

#define clcEngine (clc::GameEngine::GetInstance())