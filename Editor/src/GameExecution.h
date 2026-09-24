#pragma once
#include "Engine/Singleton.h"
#include "string"
#include <SDL3/SDL_opengl.h>

namespace clc
{
	class ExecutionSystem
	{
		CLC_SINGLETON(ExecutionSystem);

	public:
		bool Initialize();
		void Shutdown();

		bool IsExecuting() const;
		void StartExecution();
		void StopExecution();
		

	private:
		bool mExecuting = false;
		std::wstring mProgramPath;

		STARTUPINFO si;
		PROCESS_INFORMATION pi;
	};
}

#define clcExe clc::ExecutionSystem::GetInstance()