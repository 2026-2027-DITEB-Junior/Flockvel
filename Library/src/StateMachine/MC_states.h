#pragma once

//#include <MiSpine.h>
//#include <Physics/\ParryDissapearComp.h>
//#include <Input Manager/Input_Manager.h>
//#include <Main Engine/AEXEngine.h>
//#include <Core/AEXCore.h>
#include "StateMachine.h"
#include "Node.h"
#include "bitset"
#include "../Controllers/PlayerController.h"

namespace clc {

	//class PlayerController;

	class MC_Ground : public Node
	{
	public:
		MC_Ground(PlayerController* pc_) : pc(pc_) {}
		void OnStateEnter() override;
		void OnStateExit() override;
		void OnStateUpdate() override;
	private:
		PlayerController* pc;
	};

	class MC_Fall : public Node
	{
	public:
		MC_Fall(PlayerController* pc_) : pc(pc_) {}
		void OnStateEnter() override;
		void OnStateExit() override;
		void OnStateUpdate() override;
	private:
		PlayerController* pc;
	};

	class MC_Strike : public Node
	{
	public:
		MC_Strike(PlayerController* pc_) : pc(pc_) {}
		void OnStateEnter() override;
		void OnStateExit() override;
		void OnStateUpdate() override;
	private:
		PlayerController* pc;
	};

	class MC_Hit : public Node
	{
	public:
		MC_Hit(PlayerController* pc_) : pc(pc_) {}
		void OnStateEnter() override;
		void OnStateExit() override;
		void OnStateUpdate() override;
	private:
		PlayerController* pc;
	};

	class MC_Bounce : public Node
	{
	public:
		MC_Bounce(PlayerController* pc_) : pc(pc_) {}
		void OnStateEnter() override;
		void OnStateExit() override;
		void OnStateUpdate() override;
	private:
		PlayerController* pc;
	};

	class MC_Recharge : public Node
	{
	public:
		MC_Recharge(PlayerController* pc_) : pc(pc_) {}
		void OnStateEnter() override;
		void OnStateExit() override;
		void OnStateUpdate() override;
	private:
		PlayerController* pc;
	};

	class MC_Dash : public Node
	{
	public:
		MC_Dash(PlayerController* pc_) : pc(pc_) {}
		void OnStateEnter() override;
		void OnStateExit() override;
		void OnStateUpdate() override;
	private:
		PlayerController* pc;
	};

	class MC_Jump : public Node
	{
	public:
		MC_Jump(PlayerController* pc_) : pc(pc_) {}
		void OnStateEnter() override;
		void OnStateExit() override;
		void OnStateUpdate() override;
	private:
		PlayerController* pc;
	};

	class MC_WallRide : public Node
	{
	public:
		MC_WallRide(PlayerController* pc_) : pc(pc_) {}
		void OnStateEnter() override;
		void OnStateExit() override;
		void OnStateUpdate() override;
	private:
		PlayerController* pc;
	};

	class MC_Grind : public Node
	{
	public:
		MC_Grind(PlayerController* pc_) : pc(pc_) {}
		void OnStateEnter() override;
		void OnStateExit() override;
		void OnStateUpdate() override;
	private:
		PlayerController* pc;
	};

	class MC_Die : public Node
	{
	public:
		MC_Die(PlayerController* pc_) : pc(pc_) {}
		void OnStateEnter() override;
		void OnStateExit() override;
		void OnStateUpdate() override;
	private:
		PlayerController* pc;
	};

	class MC_Revive : public Node
	{
	public:
		MC_Revive(PlayerController* pc_) : pc(pc_) {}
		void OnStateEnter() override;
		void OnStateExit() override;
		void OnStateUpdate() override;
	private:
		PlayerController* pc;
	};
}
