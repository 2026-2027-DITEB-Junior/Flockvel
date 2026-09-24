#pragma once
#include <list>
#include "../API.h"
#include "../AllSingletons.h"
#include "IBase.h"

#define WIN32_LEAN_AND_MEAN // exclude win32 features that are rarely used. 
#include <windows.h>
//#include <AEX.h>
#include <iostream>
//#include "Game States.h"
#include "../Controllers/PlayerController.h"

namespace clc
{
	class API EngineSystem
	{
	public:
		// These are the functions that will be used by both the game and editor
		bool Initialize();
		void Run();
		void Shutdown();

		// These are the functions that will be used by only one between the game and editor
		virtual bool SpecificInitialize() { return true; }
		virtual void SpecificRun() {}
		virtual void SpecificShutdown() {}

		virtual bool RendersOwnFrame() const { return false; }

	protected:
		SDL_Window* m_window = nullptr;
		SDL_GLContext m_glContext = nullptr;
		bool m_initialized = false;
		bool m_running = false;

		SceneManager* scnMng = nullptr;
		Factory* fct = nullptr;
		ResourceManager* resMng = nullptr;
		Graphics* gfxSys = nullptr;
		Window* windowSys = nullptr;
	};
}