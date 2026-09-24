#pragma once
//#include "Composition/AEXComponent.h"
#include <string>

namespace clc {
	class Node
	{
		std::string name_;
	public:
		virtual ~Node();
		std::string get_name();
		void set_name(std::string name);
		virtual void OnStateEnter() = 0;
		virtual void OnStateExit() = 0;
		virtual void OnStateUpdate() = 0;
	};
}