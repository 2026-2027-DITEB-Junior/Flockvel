#include "GameEngine.h"
#include <iostream>
#include <GL/glew.h>
#include "Graphics/Renderable.h"
#include "Physics/Collisions/Collisions.h"

namespace clc
{
	bool GameEngine::SpecificInitialize()
	{
		phys = &clcPhysics;
		log = &clcLogic;
		if (!log->Initialize())
			return false;

		audioSys = &clcAudio;
		if (!audioSys->Initialize()) return false;
		
		mDisplayShader = resMng->GetShader("PresentFrame.shader");
		mDisplayMesh = resMng->GetModel("plane.obj");

		std::cout << "Game still compiles! :]\n";
		return true;
	}

	void GameEngine::SpecificRun()
	{
		gfxSys->PresentFrame(mDisplayMesh, mDisplayShader);

		//fmod needs an update every frame
		audioSys->Update();

		phys->Update(clcDeltaTime.get_dt());
		log->Update();
	}

	void GameEngine::SpecificShutdown()
	{
		if (audioSys)
			audioSys->Shutdown();
	}
}