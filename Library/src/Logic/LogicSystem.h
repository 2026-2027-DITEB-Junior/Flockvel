#pragma once
#include "../API.h"
#include "../Components/LogicComp.h"
#include "../Engine/Singleton.h"
#include <vector>
#include <list>

namespace clc {
	class API LogicSystem
	{
		CLC_SINGLETON(LogicSystem);

	public: 
		bool Initialize();
		void Update();

		void AddComp(LogicComp* comp);
		void RemoveComp(LogicComp* comp);

		void ClearComps();

		bool mEnabled = true;

	private:
		std::list<LogicComp*> mComponents;
	};
}

// Easy access to singleton
#define clcLogic (clc::LogicSystem::GetInstance())