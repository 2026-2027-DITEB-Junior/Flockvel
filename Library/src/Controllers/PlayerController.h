#pragma once
#include "../StateMachine/StateMachine.h"
#include "../Logic/LogicSystem.h"
#include "../Physics/Dynamics/RigidBody.h"
#include "../Event System/EventSys.h"

namespace clc
{
	struct PlayerController : public LogicComp, public subscriber, public thrower
	{
	public:
		void OnCreate()override;
		void Update()override;

		void FSMInitStates();
		void FSMChangeState(std::string newState);
		void FSMUpdateState();

		void handle_event(EEvent* e);

		// Serialization
		void Serialize(nlohmann::json& j)const override;
		void Deserialize(const nlohmann::json& j) override;


		Transform* trm = nullptr;
		Rigidbody* rb = nullptr;
		StateMachine* player_sm_ = nullptr;

		// tweakable values, the editor and the json read these
		float mSpeedForce = 120.0f;      //save these
		glm::vec3 targetVelocity = glm::vec3(0.0f);
		float damping = 0.95f;
		float smoothness = 5.7f;

		float mSpinSpeed = 360.0f;
		std::string mStartState = "Ground";


		glm::vec3 original_pos;

		glm::vec3 bounce_dir = glm::vec3(1, 0, 0);  
		float bounce_force = 2400.0f;  //save these
		float up_bounce_force = 800.0f;
		
	private:
		
		//Collider* cc = nullptr;
		
		//Renderable* renderable = nullptr;
	};
}