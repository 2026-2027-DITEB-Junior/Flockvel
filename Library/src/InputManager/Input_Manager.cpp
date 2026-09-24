#include "pch.h"
#include "Input_Manager.h"

namespace clc {

	enum errors {
		CONTROLLER_NOT_CONNECTED
	};

	std::string InputManager::map_c() {
		switch (GP_map)
		{
		case classic:
			return "classic";
			break;
		case modern:
			return "modern";
			break;
		case psycho:
			return "psycho";
			break;
		default:
			break;
		}
	}

	void InputManager::show_menu()
	{	}

	bool InputManager::Initialize()
	{
		load_controls();
		return true;
	}

	void InputManager::Update() {
		//example of use
		//if(ButtonAPressed()){
		//	std::cout << "Button A Pressed" << std::endl;
		//}
		/*if (clcInput.KeyPressed(Keys::Control) && clcInput.KeyTriggered('I')) show = !show;
		show_menu();*/

		/*if (!input_init) {
			clcInput.Initialize();
			input_init = true;
		}*/

		UpdateKeyboardAndMouseState();

		try {
			UpdateControllerState();
		}
		catch (errors e) {
			if (e == CONTROLLER_NOT_CONNECTED) {
				std::cout << "Controller not connected." << std::endl;
			}
		}
	}

	void InputManager::UpdateControllerState(int playerIndex) {

		// Obtener estado del mando (0 = primer mando)
		DWORD result = XInputGetState(playerIndex, &controllerStates);

		if (result == NO_ERROR) {
			// El mando esta conectado
			auto& pad = controllerStates.Gamepad;

			// STICKS (valores entre -32768 y 32767)
			LX = pad.sThumbLX / 32767.0f;
			LY = pad.sThumbLY / 32767.0f;

			RX = pad.sThumbRX / 32767.0f;
			RY = pad.sThumbRY / 32767.0f;

			//para evitar valores fuera de rango
			if (LX < -1.0f) LX = -1.0f;
			if (LY < -1.0f) LY = -1.0f;
			if (RX < -1.0f) RX = -1.0f;
			if (RY < -1.0f) RY = -1.0f;
			
			// DEADZONE (para evitar drift del mando)
			if (abs(LX) < DEADZONE_L) LX = 0;
			if (abs(LY) < DEADZONE_L) LY = 0;
			if (abs(RX) < DEADZONE_R) RX = 0;
			if (abs(RY) < DEADZONE_R) RY = 0;


			// TRIGGERS (valores entre 0 y 255)
			LT = pad.bLeftTrigger / 255.0f;
			RT = pad.bRightTrigger / 255.0f;

			//set previous butons
			{
				prev_button_a = button_a;
				prev_button_b = button_b;
				prev_button_x = button_x;
				prev_button_y = button_y;
				prev_button_lb = button_lb;
				prev_button_rb = button_rb;
				prev_button_ls = button_ls;
				prev_button_rs = button_rs;
				prev_button_start = button_start;
				prev_button_back = button_back;
				prev_cross_up = cross_up;
				prev_cross_down = cross_down;
				prev_cross_left = cross_left;
				prev_cross_right = cross_right;
			}


			//pasar los botones a bools
			button_a = pad.wButtons & XINPUT_GAMEPAD_A;
			button_b = pad.wButtons & XINPUT_GAMEPAD_B;
			button_x = pad.wButtons & XINPUT_GAMEPAD_X;
			button_y = pad.wButtons & XINPUT_GAMEPAD_Y;

			button_lb = pad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER;
			button_rb = pad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER;

			button_ls = pad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB;
			button_rs = pad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB;

			button_start = pad.wButtons & XINPUT_GAMEPAD_START;
			button_back  = pad.wButtons & XINPUT_GAMEPAD_BACK;

			cross_up    = pad.wButtons & XINPUT_GAMEPAD_DPAD_UP;
			cross_down  = pad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN;
			cross_left  = pad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT;
			cross_right = pad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT;


		}
		else {
			// El mando no est� conectado
			//throw CONTROLLER_NOT_CONNECTED;
			return;
		}
	}

	void InputManager::UpdateKeyboardAndMouseState() {
		//set previous butons
		prev_forward = forward;
		prev_left = left;
		prev_right = right;
		prev_back = back;
		prev_recharge = recharge;
		prev_space = space;

		//original controls
		//GetAsyncKeyState(recharge_key) & 0x8000;
		forward  = GetAsyncKeyState(forward_key)  & 0x8000;
		back     = GetAsyncKeyState(back_key)     & 0x8000;
		left     = GetAsyncKeyState(left_key)     & 0x8000;
		right    = GetAsyncKeyState(right_key)    & 0x8000;
		recharge = GetAsyncKeyState(recharge_key) & 0x8000;
		space    = GetAsyncKeyState(space_key)    & 0x8000;

		//std::cout << "W pressed : " << clcInput.KeyPressed('W') << "\n";
	}

	// Getters for stick positions
	float InputManager::GP_getLeftStickX() { return LX; }
	float InputManager::GP_getLeftStickY() { return LY; }
	float InputManager::GP_getRightStickX() { return RX; }
	float InputManager::GP_getRightStickY() { return RY; }

	// Getters for triggers
	float InputManager::GP_getLeftTrigger() { return LT; }
	float InputManager::GP_getRightTrigger() { return RT; }

	/* This is for gamepad
	Pressed: Button is currently being held down.
	triggered: Button was just pressed this frame (not pressed in the previous frame).
	*/
	bool InputManager::GP_ButtonAPressed() { return button_a; }
	bool InputManager::GP_ButtonATriggered() { return button_a && !prev_button_a; }
	bool InputManager::GP_ButtonBPressed() { return button_b; }
	bool InputManager::GP_ButtonBTriggered() { return button_b && !prev_button_b; }
	bool InputManager::GP_ButtonXPressed() { return button_x; }
	bool InputManager::GP_ButtonXTriggered() { return button_x && !prev_button_x; }
	bool InputManager::GP_ButtonYPressed() { return button_y; }
	bool InputManager::GP_ButtonYTriggered() { return button_y && !prev_button_y; }

	bool InputManager::GP_ButtonLBPressed() { return button_lb; }
	bool InputManager::GP_ButtonLBTriggered() { return button_lb && !prev_button_lb; }
	bool InputManager::GP_ButtonRBPressed() { return button_rb; }
	bool InputManager::GP_ButtonRBTriggered() { return button_rb && !prev_button_rb; }

	bool InputManager::GP_ButtonLSPressed() { return button_ls; }
	bool InputManager::GP_ButtonLSTriggered() { return button_ls && !prev_button_ls; }
	bool InputManager::GP_ButtonRSPressed() { return button_rs; }
	bool InputManager::GP_ButtonRSTriggered() { return button_rs && !prev_button_rs; }

	bool InputManager::GP_ButtonStartPressed() { return button_start; }
	bool InputManager::GP_ButtonStartTriggered() { return button_start && !prev_button_start; }
	bool InputManager::GP_ButtonBackPressed() { return button_back; }
	bool InputManager::GP_ButtonBackTriggered() { return button_back && !prev_button_back; }

	bool InputManager::GP_DPadUpPressed() { return cross_up; }
	bool InputManager::GP_DPadDownTriggered() { return cross_down && !prev_cross_down; }
	bool InputManager::GP_DPadDownPressed() { return cross_down; }
	bool InputManager::GP_DPadUpTriggered() { return cross_up && !prev_cross_up; }
	bool InputManager::GP_DPadLeftPressed() { return cross_left; }
	bool InputManager::GP_DPadLeftTriggered() { return cross_left && !prev_cross_left; }
	bool InputManager::GP_DPadRightPressed() { return cross_right; }
	bool InputManager::GP_DPadRightTriggered() { return cross_right && !prev_cross_right; }

	/* This is for keyboard
	Pressed: Button is currently being held down.
	triggered: Button was just pressed this frame (not pressed in the previous frame).
	*/
	bool InputManager::KB_forwardPressed() { return forward; }
	bool InputManager::KB_forwardTriggered() { return forward && !prev_forward; }

	bool InputManager::KB_RightPressed() { return right; }
	bool InputManager::KB_RightTriggered() { return right && !prev_right; }

	bool InputManager::KB_LeftPressed() { return left; }
	bool InputManager::KB_LeftTriggered() { return left && !prev_left; }

	bool InputManager::KB_BackPressed() { return back; }
	bool InputManager::KB_BackTriggered() { return back && !prev_back; }

	bool InputManager::KB_RechargePressed() { return recharge; }
	bool InputManager::KB_RechargeTriggered() { return recharge && !prev_recharge; }

	bool InputManager::KB_SpacePressed() { return space; }
	bool InputManager::KB_SpaceTriggered() { return space && !prev_space; }

	/*
	funcitons for controls for our game (no copilot here ;-;)
	*/

	void InputManager::save_controls() const
	{}
	void InputManager::load_controls()
	{}
}//aex