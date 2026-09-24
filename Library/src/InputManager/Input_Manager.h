//This was made whith help of GitHub copilot

#pragma once

//#include <Core/AEXCore.h>
//#include <Main Engine/AEXEngine.h>
//#include <Engine/Platform/AEXInput.h>
#include "../CommonHeaders.h"
#include "../Engine/IBase.h"
#include "../Engine/Singleton.h"
#include <stdlib.h>
#include <windows.h>
#include <Xinput.h>
#pragma comment(lib, "Xinput.lib")
//#include <json/json.hpp>		// nlohmann::json

#ifdef IN_LIBRARY
#define API __declspec(dllexport)
#else
#define API __declspec(dllimport)
#endif

enum menu_state{original, key_map, controller_map};

enum GP_map_options{classic, modern, psycho};

namespace clc 
{
	class API InputManager : public IBase {
		CLC_SINGLETON(InputManager);
	public:
		bool Initialize();
		void Update();
		void UpdateControllerState(int playerIndex = 0);
		void UpdateKeyboardAndMouseState();

		void show_menu();

		// Getters for stick positions
		float GP_getLeftStickX();
		float GP_getLeftStickY();
		float GP_getRightStickX();
		float GP_getRightStickY();

		// Getters for triggers
		float GP_getLeftTrigger();
		float GP_getRightTrigger();

		/* This is for gamepad
		Pressed: Button is currently being held down.
		triggered: Button was just pressed this frame (not pressed in the previous frame).
		*/
		bool GP_ButtonAPressed();
		bool GP_ButtonATriggered();
		bool GP_ButtonBPressed();
		bool GP_ButtonBTriggered();
		bool GP_ButtonXPressed();
		bool GP_ButtonXTriggered();
		bool GP_ButtonYPressed();
		bool GP_ButtonYTriggered();
			 
		bool GP_ButtonLBPressed();
		bool GP_ButtonLBTriggered();
		bool GP_ButtonRBPressed();
		bool GP_ButtonRBTriggered();

		bool GP_ButtonLSPressed();
		bool GP_ButtonLSTriggered();
		bool GP_ButtonRSPressed();
		bool GP_ButtonRSTriggered();
			 
		bool GP_ButtonStartPressed();
		bool GP_ButtonStartTriggered();
		bool GP_ButtonBackPressed();
		bool GP_ButtonBackTriggered();

		bool GP_DPadUpPressed();
		bool GP_DPadDownTriggered();
		bool GP_DPadDownPressed();
		bool GP_DPadUpTriggered();
		bool GP_DPadLeftPressed();
		bool GP_DPadLeftTriggered();
		bool GP_DPadRightPressed();
		bool GP_DPadRightTriggered();

		/* This is for keyboard
		Pressed: Button is currently being held down.
		triggered: Button was just pressed this frame (not pressed in the previous frame).
		char forward_key  = 'W';
		char right_key	  = 'D';
		char left_key	  = 'A';
		char back_key	  = 'S';
		char recharge_key = 'L';
		*/
		bool KB_forwardPressed();
		bool KB_forwardTriggered();

		bool KB_RightPressed();
		bool KB_RightTriggered();

		bool KB_LeftPressed();
		bool KB_LeftTriggered();

		bool KB_BackPressed();
		bool KB_BackTriggered();

		bool KB_RechargePressed();
		bool KB_RechargeTriggered();

		bool KB_SpacePressed();
		bool KB_SpaceTriggered();

		bool KB_SpaceStart();
		bool KB_SpaceMenu();

	private:

		void save_controls() const;
		void load_controls();

		std::string filename = "data\\Controls\\Key_binding.json";

		//menu things :)
		bool show = false;
		menu_state state = original;
		GP_map_options GP_map;
		std::string map_c();

		XINPUT_STATE controllerStates;

		float LX;
		float LY;
		float RX;
		float RY;

		float DEADZONE_L = 0.1f;
		float DEADZONE_R = 0.1f;

		float input_threshold_x = 0.2f;
		float input_threshold_y = 0.4f;

		float LT;
		float RT;

		bool prev_button_a;
		bool button_a;
		bool prev_button_b;
		bool button_b;
		bool prev_button_x;
		bool button_x;
		bool prev_button_y;
		bool button_y;

		bool prev_button_lb;
		bool button_lb;
		bool prev_button_rb;
		bool button_rb;
		bool prev_button_ls;
		bool button_ls;
		bool prev_button_rs;
		bool button_rs;
		
		bool prev_button_start;
		bool button_start;
		bool prev_button_back;
		bool button_back;

		bool prev_cross_up;
		bool cross_up;
		bool prev_cross_down;
		bool cross_down;
		bool prev_cross_left;
		bool cross_left;
		bool prev_cross_right;
		bool cross_right;

		//controls for keyboard and mouse can be added here
		char forward_key  = 'W';
		char right_key	  = 'D';
		char left_key	  = 'A';
		char back_key	  = 'S';
		char recharge_key = 'L';

		bool prev_forward;
		bool forward;
		bool prev_right;
		bool right;
		bool prev_left;
		bool left;
		bool prev_back;
		bool back;
		bool prev_recharge;
		bool recharge;

		int space_key = VK_SPACE;

		bool prev_space = false;
		bool space = false;

		bool input_init = false;
	};
} // namespace aex

// Easy access to singleton
#define clcInputManager (clc::InputManager::GetInstance())