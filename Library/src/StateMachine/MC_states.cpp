//Alejandro Vicente

#include "pch.h"
//#include <Physics/CollisionSystem.h>
//#include <Controllers/PlayerController.h>
#include "MC_states.h"
#include "../InputManager/Input_Manager.h"
#include "../DeltaTime/DeltaTime.h"

//#include <Controllers/PlayerController.h>
//#include <Audio/PlayerAudio.h>
//#include <stdlib.h>  // abs

namespace clc {

	//--------------------------- GENERAL ---------------------------//
	void movement(PlayerController* pc) {

		//gamepad
		float left_stick_x = clcInputManager.GP_getLeftStickX();
		float left_stick_y = clcInputManager.GP_getLeftStickY();

		glm::vec3 objectiveVel = glm::vec3(pc->mSpeedForce * left_stick_x, 0.0f, -pc->mSpeedForce * left_stick_y);

		//keyboard
		if (!left_stick_x && !left_stick_y) {

			if (clcInputManager.KB_forwardPressed()) {
				objectiveVel += glm::vec3(0.0f, 0.0f, -pc->mSpeedForce);
			}
			if (clcInputManager.KB_BackPressed()) {
				objectiveVel += glm::vec3(0.0f, 0.0f, pc->mSpeedForce);
			}
			if (clcInputManager.KB_RightPressed()) {
				objectiveVel += glm::vec3(pc->mSpeedForce, 0.0f, 0.0f);
			}
			if (clcInputManager.KB_LeftPressed()) {
				objectiveVel += glm::vec3(-pc->mSpeedForce, 0.0f, 0.0f);
			}
		}

		pc->targetVelocity = glm::mix(pc->targetVelocity, objectiveVel, pc->smoothness * clcDeltaTime.get_dt());

		pc->rb->velocity *= pc->damping;

		pc->rb->AddForce(pc->targetVelocity - pc->rb->velocity);

		if (clcInputManager.KB_SpaceTriggered() || clcInputManager.GP_ButtonYTriggered()) {
			pc->Owner()->mTransform.mPosition = pc->original_pos;
		}
	}


	//--------------------------- GROUND ---------------------------//

	void MC_Ground::OnStateEnter()
	{
		std::cout << "Ground\n";
	}

	void MC_Ground::OnStateExit()
	{
	}

	void MC_Ground::OnStateUpdate()
	{
		movement(pc);

		if (pc->rb->velocity.y < 0) {
			pc->player_sm_->ChangeState("Fall");
		}
	}


	//--------------------------- FALL ---------------------------//
	void MC_Fall::OnStateEnter()
	{
		std::cout << "Fall\n";
	}

	void MC_Fall::OnStateExit()
	{
	}

	void MC_Fall::OnStateUpdate()
	{
		movement(pc);

		if (pc->rb->velocity.y >= 0) {
			pc->player_sm_->ChangeState("Ground");
		}
	}


	//--------------------------- STRIKE ---------------------------//
	void MC_Strike::OnStateEnter()
	{
		//damage and set bounce direction

		pc->player_sm_->ChangeState("Bounce");
	}

	void MC_Strike::OnStateExit()
	{
	}

	void MC_Strike::OnStateUpdate()
	{
	}


	//--------------------------- HIT ---------------------------//
	void MC_Hit::OnStateEnter()
	{
		//damage and set bounce direction

		pc->player_sm_->ChangeState("Bounce");
	}

	void MC_Hit::OnStateExit()
	{
	}

	void MC_Hit::OnStateUpdate()
	{
	}


	//--------------------------- BOUNCE ---------------------------//
	void MC_Bounce::OnStateEnter()
	{
		//calcular direccion de la fuerza y aplicarla
		
		glm::vec3 force = pc->bounce_dir * pc->bounce_force;
		force.y = pc->up_bounce_force;

		pc->rb->AddForce(force);
	}

	void MC_Bounce::OnStateExit()
	{

	}

	void MC_Bounce::OnStateUpdate()
	{
		//esperar a caer para ir a fall
		if (pc->rb->velocity.y <= 0.0f) {
			pc->player_sm_->ChangeState("Fall");
		}
	}


	//--------------------------- RECHARGE ---------------------------//
	void MC_Recharge::OnStateEnter()
	{
	}

	void MC_Recharge::OnStateExit()
	{
	}

	void MC_Recharge::OnStateUpdate()
	{
	}


	//--------------------------- DASH ---------------------------//
	void MC_Dash::OnStateEnter()
	{
	}

	void MC_Dash::OnStateExit()
	{
	}

	void MC_Dash::OnStateUpdate()
	{
	}


	//--------------------------- JUMP ---------------------------//
	void MC_Jump::OnStateEnter()
	{
	}

	void MC_Jump::OnStateExit()
	{
	}

	void MC_Jump::OnStateUpdate()
	{
	}


	//--------------------------- WALL RIDE ---------------------------//
	void MC_WallRide::OnStateEnter()
	{
	}

	void MC_WallRide::OnStateExit()
	{
	}

	void MC_WallRide::OnStateUpdate()
	{
	}


	//--------------------------- GRIND ---------------------------//
	void MC_Grind::OnStateEnter()
	{
	}

	void MC_Grind::OnStateExit()
	{
	}

	void MC_Grind::OnStateUpdate()
	{
	}


	//--------------------------- DIE ---------------------------//
	void MC_Die::OnStateEnter()
	{
	}

	void MC_Die::OnStateExit()
	{
	}

	void MC_Die::OnStateUpdate()
	{
	}


	//--------------------------- REVIVE ---------------------------//
	void MC_Revive::OnStateEnter()
	{
		// Revive
	}

	void MC_Revive::OnStateExit()
	{
	}

	void MC_Revive::OnStateUpdate()
	{
	}
}