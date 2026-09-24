#include "pch.h"
#include "LogicSystem.h"

namespace clc 
{
	bool LogicSystem::Initialize()
	{
		// nothing to do here
		return true;
	}

	void LogicSystem::AddComp(LogicComp* comp)
	{
		// avoid duplicates
		for (auto c : mComponents)
			if (c == comp)return;

		// add if not already
		mComponents.push_back(comp);
	}

	void LogicSystem::RemoveComp(LogicComp* comp)
	{
		mComponents.remove(comp);
	}

	void LogicSystem::ClearComps()
	{
		mComponents.clear();
	}

	void LogicSystem::Update()
	{
		if (!mEnabled)
			return;

		for (auto comp : mComponents) {
			if(comp->IsEnabled())
				comp->Update();
		}
	}

	void LogicComp::AddToSystem()
	{
		clcLogic.AddComp(this);
	}
	void LogicComp::RemoveFromSystem()
	{
		clcLogic.RemoveComp(this);
	}
}
