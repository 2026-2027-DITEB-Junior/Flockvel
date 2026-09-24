#pragma once
#include "Node.h"
#include <map>
#include <iostream>
#include <string>
#include "../Event System/EventSys.h"
//#include <Core/AEXCore.h>

namespace clc {

	//fms_states["IDLE"] = new MC_Idle(this, anim);
	class StateMachine : public subscriber
	{
		
	public:
		std::map<std::string, Node*> fsm_states;
		Node* act_state = nullptr;
		void set_act_state(Node* state);
		void ChangeState(std::string newNode);
		void Update();

		void handle_event(EEvent* e);
	};

}