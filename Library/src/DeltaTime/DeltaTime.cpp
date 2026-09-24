#include "pch.h"
#include "DeltaTime.h"

namespace clc {
	bool DeltaTime::Initialize()
	{
		QueryPerformanceFrequency(&frequency);
		QueryPerformanceCounter(&lastTime);
		return true;
	}
	void DeltaTime::Update()
	{

		QueryPerformanceCounter(&currentTime);

		deltaTime =
			static_cast<double>(currentTime.QuadPart - lastTime.QuadPart)
			/ static_cast<double>(frequency.QuadPart);

		lastTime = currentTime;

		//std::cout << "Delta time: " << deltaTime << " s\n";
	}
	double DeltaTime::get_dt()
	{
		return deltaTime;
	}
}//aex