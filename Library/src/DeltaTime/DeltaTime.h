#pragma once

#include "../CommonHeaders.h"
#include "../Engine/IBase.h"
#include "../Engine/Singleton.h"
#include <stdlib.h>
#include <windows.h>

#ifdef IN_LIBRARY
#define API __declspec(dllexport)
#else
#define API __declspec(dllimport)
#endif


namespace clc 
{
	class API DeltaTime : public IBase {
		CLC_SINGLETON(DeltaTime);
	public:

		bool Initialize();
		void Update();

		double get_dt();

	private:
		double deltaTime;
		LARGE_INTEGER frequency;
		LARGE_INTEGER lastTime;
		LARGE_INTEGER currentTime;

	};
} // namespace aex

// Easy access to singleton
#define clcDeltaTime (clc::DeltaTime::GetInstance())