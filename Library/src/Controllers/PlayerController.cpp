#include "pch.h"
#include "PlayerController.h"
#include "../StateMachine/MC_states.h"
#include "../Logic/LogicSystem.h"
#include "../InputManager/Input_Manager.h"

namespace clc
{
	void PlayerController::OnCreate() {
		player_sm_ = new StateMachine();
		FSMInitStates();
		add_thrower(Owner()->GetComp<Rigidbody>());
		player_sm_->add_thrower(this);

		original_pos = Owner()->GetTransform().mPosition;
	}

	void PlayerController::handle_event(EEvent* e)
	{
		if (e->name == "Collision") {
			if (FlagEvent* flg = dynamic_cast<FlagEvent*>(e)) {
				if (flg->flag_ == Flags::Wall) {
					bounce_dir = flg->vec_;
					throw_event("Hit");
				}
			}
		}
		else if (e->name == "Strike") {
			throw_event("Strike");
		}
		else if (e->name == "Hit") {
			throw_event("Hit");
		}
	}

	void PlayerController::Update() {
		//if (!player_sm_) // the fsm is not built yet
		//	return;

		if (!rb) {
			rb = Owner()->GetComp<Rigidbody>();
		}

		player_sm_->Update();


		mOwner->mTransform.mRotation += glm::vec3(0.0f, 0.2f, 0.0f);

		//FSMUpdateState();
	}

	void PlayerController::FSMInitStates()
	{
		player_sm_->fsm_states["Ground"] = new MC_Ground(this);
		player_sm_->set_act_state(player_sm_->fsm_states["Ground"]);

		player_sm_->fsm_states["Fall"]     = new MC_Fall(this);
		player_sm_->fsm_states["Strike"]   = new MC_Strike(this);
		player_sm_->fsm_states["Hit"]      = new MC_Hit(this);
		player_sm_->fsm_states["Bounce"]   = new MC_Bounce(this);
		player_sm_->fsm_states["Recharge"] = new MC_Recharge(this);
		player_sm_->fsm_states["Dash"]     = new MC_Dash(this);
		player_sm_->fsm_states["Jump"]     = new MC_Jump(this);
		player_sm_->fsm_states["WalRide"]  = new MC_WallRide(this);
		player_sm_->fsm_states["Grind"]    = new MC_Grind(this);
		player_sm_->fsm_states["Die"]      = new MC_Die(this);
		player_sm_->fsm_states["Revive"]   = new MC_Revive(this);
	}
	void PlayerController::FSMChangeState(std::string newState)
	{
		player_sm_->ChangeState(newState);
	}
	void PlayerController::FSMUpdateState()
	{
		player_sm_->Update();
	}


	// Serialization
	void PlayerController::Serialize(nlohmann::json& j)const
	{
		IBase::Serialize(j); // type, name and enabled

		j["speed"] = mSpeedForce;
		j["spinSpeed"] = mSpinSpeed;
		j["startState"] = mStartState;
	}
	void PlayerController::Deserialize(const nlohmann::json& j)
	{
		IBase::Deserialize(j);

		if (j.contains("speed")) mSpeedForce = j["speed"].get<float>();
		if (j.contains("spinSpeed")) mSpinSpeed = j["spinSpeed"].get<float>();
		if (j.contains("startState")) mStartState = j["startState"].get<std::string>();
	}
}