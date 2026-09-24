#include "pch.h"
#include "StateMachine.h"
namespace clc {

	void StateMachine::ChangeState(std::string new_state)
	{
		//std::cout << new_state << std::endl;
		if (act_state->get_name() != new_state)
		{
			act_state->OnStateExit();
			act_state = fsm_states[new_state];
			act_state->OnStateEnter();
		}
	}
	void StateMachine::Update()
	{
		act_state->OnStateUpdate();
	}

	void StateMachine::set_act_state(Node* state)
	{
		act_state = state;
	}

	void StateMachine::handle_event(EEvent* e)
	{
		//for the player state machine
		if (e->name == "Strike") {
			ChangeState("Strike");
		}
		else if (e->name == "Hit") {
			ChangeState("Hit");
		}
	}
}