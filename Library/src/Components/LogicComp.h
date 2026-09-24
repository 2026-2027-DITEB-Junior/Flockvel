#pragma once
#include "../Engine/IComp.h"

namespace clc
{
	// Simple base class for logic component
	// does nothing except adding/removing itself
	// to/from the Logic system. It is thought
	// so that logic components derive from it and 
	// don't have to do it.

	class LogicComp : public IComp
	{
	public:
		void AddToSystem() override;
		void RemoveFromSystem() override;
	};
}