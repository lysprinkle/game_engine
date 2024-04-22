#include "Input.h"
#include "iostream"

void Input::Init() {
	for (int code = SDL_SCANCODE_UNKNOWN; code < SDL_NUM_SCANCODES; code++) {
		keyboard_states[static_cast<SDL_Scancode>(code)] = INPUT_STATE_UP;
	}

	mouse_left_status = 1;
	mouse_right_status = 1;
	mouse_middle_status = 1;
}

void Input::ProcessEvent(const SDL_Event& e) {
	if (e.type == SDL_KEYDOWN) {
		keyboard_states[e.key.keysym.scancode] = INPUT_STATE_JUST_BECAME_DOWN;
		just_became_down_scancodes.push_back(e.key.keysym.scancode);
	}

	else if (e.type == SDL_KEYUP) {
		keyboard_states[e.key.keysym.scancode] = INPUT_STATE_JUST_BECAME_UP;
		just_became_up_scancodes.push_back(e.key.keysym.scancode);
	}

	else if (e.type == SDL_MOUSEBUTTONDOWN) {
		if (e.button.button == SDL_BUTTON_LEFT) {
			mouse_left_status = 2;
		}
		else if (e.button.button == SDL_BUTTON_RIGHT) {
			mouse_right_status = 2;
		}
		else if (e.button.button == SDL_BUTTON_MIDDLE) {
			mouse_middle_status = 2;
		}
	}

	else if (e.type == SDL_MOUSEBUTTONUP) {
		if (e.button.button == SDL_BUTTON_LEFT) {
			mouse_left_status = 4;
		}
		else if (e.button.button == SDL_BUTTON_RIGHT) {
			mouse_right_status = 4;
		}
		else if (e.button.button == SDL_BUTTON_MIDDLE) {
			mouse_middle_status = 4;
		}
	}

	else if (e.type == SDL_CONTROLLERBUTTONDOWN) {
		SDL_JoystickID controllerId = e.cbutton.which;
		button_states[controllerId][static_cast<SDL_GameControllerButton>(e.cbutton.button)] = INPUT_STATE_JUST_BECAME_DOWN;
		just_became_down_buttons[controllerId].push_back(static_cast<SDL_GameControllerButton>(e.cbutton.button));
	}

	else if (e.type == SDL_CONTROLLERBUTTONUP) {
		SDL_JoystickID controllerId = e.cbutton.which;
		button_states[controllerId][static_cast<SDL_GameControllerButton>(e.cbutton.button)] = INPUT_STATE_JUST_BECAME_UP;
		just_became_up_buttons[controllerId].push_back(static_cast<SDL_GameControllerButton>(e.cbutton.button));
	}

}
void Input::LateUpdate() {
	for (const SDL_Scancode& code : just_became_down_scancodes) {
		keyboard_states[code] = INPUT_STATE_DOWN;
	}
	just_became_down_scancodes.clear();
	 
	for (const SDL_Scancode& code : just_became_up_scancodes) {
		keyboard_states[code] = INPUT_STATE_UP;
	}
	just_became_up_scancodes.clear();

	for (auto pair : just_became_down_buttons) {

		for (const SDL_GameControllerButton& button : pair.second) {
			button_states[pair.first][button] = INPUT_STATE_DOWN;
		}

		just_became_down_buttons[pair.first].clear();
	}

	for (auto pair : just_became_up_buttons) {

		for (const SDL_GameControllerButton& button : pair.second) {
			button_states[pair.first][button] = INPUT_STATE_UP;
		}

		just_became_up_buttons[pair.first].clear();
	}

	if (mouse_left_status == 2) {
		mouse_left_status = 3;
	}
	else if (mouse_right_status == 2) {
		mouse_right_status = 3;
	}
	else if (mouse_middle_status == 2) {
		mouse_middle_status = 3;
	}

	if (mouse_left_status == 4) {
		mouse_left_status = 1;
	}
	else if (mouse_right_status == 4) {
		mouse_right_status = 1;
	}
	else if (mouse_middle_status == 4) {
		mouse_middle_status = 1;
	}

}

bool Input::GetKey(SDL_Scancode keycode) {
	return (keyboard_states[keycode] == INPUT_STATE_DOWN || keyboard_states[keycode] == INPUT_STATE_JUST_BECAME_DOWN);
}

bool Input::GetKeyDown(SDL_Scancode keycode) {
	return (keyboard_states[keycode] == INPUT_STATE_JUST_BECAME_DOWN);
}

bool Input::GetKeyUp(SDL_Scancode keycode) {
	return (keyboard_states[keycode] == INPUT_STATE_JUST_BECAME_UP);
}

bool Input::GetMouse(int button_num) {
	if (button_num == 1) {
		return ((mouse_left_status == 2) || (mouse_left_status == 3));
	}
	else if (button_num == 2) {
		return ((mouse_middle_status == 2) || (mouse_middle_status == 3));
	}
	else if (button_num == 3) {
		return ((mouse_right_status == 2) || (mouse_right_status == 3));
	}

	return false;
}

bool Input::GetMouseDown(int button_num) {
	if (button_num == 1) {
		return (mouse_left_status == 2);
	}
	else if (button_num == 2) {
		return (mouse_middle_status == 2);
	}
	else if (button_num == 3) {
		return (mouse_right_status == 2);
	}

	return false;
}

bool Input::GetMouseUp(int button_num) {
	if (button_num == 1) {
		return (mouse_left_status == 4);
	}
	else if (button_num == 2) {
		return (mouse_middle_status == 4);
	}
	else if (button_num == 3) {
		return (mouse_right_status == 4);
	}

	return false;
}

bool Input::GetButton(SDL_GameControllerButton button, int controllerId) {
	return (button_states[controllerId][button] == INPUT_STATE_DOWN || button_states[controllerId][button] == INPUT_STATE_JUST_BECAME_DOWN);
}

bool Input::GetButtonDown(SDL_GameControllerButton button, int controllerId) {
	return (button_states[controllerId][button] == INPUT_STATE_JUST_BECAME_DOWN);
}

bool Input::GetButtonUp(SDL_GameControllerButton button, int controllerId) {
	return (button_states[controllerId][button] == INPUT_STATE_JUST_BECAME_UP);
}

namespace input {
	bool GetKey(std::string keycode) {
		SDL_Scancode key_code;
		if (__keycode_to_scancode.find(keycode) != __keycode_to_scancode.end()) {
			key_code = __keycode_to_scancode.at(keycode);
		}
		else {
			return false;
		}

		return Input::GetKey(key_code);
	}

	bool GetKeyDown(std::string keycode) {
		SDL_Scancode key_code;
		if (__keycode_to_scancode.find(keycode) != __keycode_to_scancode.end()) {
			key_code = __keycode_to_scancode.at(keycode);
		}
		else {
			return false;
		}

		return Input::GetKeyDown(key_code);
	}

	bool GetKeyUp(std::string keycode) {
		SDL_Scancode key_code;
		if (__keycode_to_scancode.find(keycode) != __keycode_to_scancode.end()) {
			key_code = __keycode_to_scancode.at(keycode);
		}
		else {
			return false;
		}

		return Input::GetKeyUp(key_code);
	}

	bool GetMouse(int button_num) {
		return Input::GetMouse(button_num);
	}

	bool GetMouseDown(int button_num) {
		return Input::GetMouseDown(button_num);
	}

	bool GetMouseUp(int button_num) {
		return Input::GetMouseUp(button_num);
	}

	bool GetButton(std::string keycode, int controllerId) {
		SDL_GameControllerButton button;
		if (__keycode_to_button.find(keycode) != __keycode_to_button.end()) {
			button = __keycode_to_button.at(keycode);
		}
		else {
			return false;
		}

		return Input::GetButton(static_cast<SDL_GameControllerButton>(button), controllerId);
	}

	bool GetButtonDown(std::string keycode, int controllerId) {
		SDL_GameControllerButton button;
		if (__keycode_to_button.find(keycode) != __keycode_to_button.end()) {
			button = __keycode_to_button.at(keycode);
		}
		else {
			return false;
		}

		return Input::GetButtonDown(static_cast<SDL_GameControllerButton>(button), controllerId);
	}

	bool GetButtonUp(std::string keycode, int controllerId) {
		SDL_GameControllerButton button;
		if (__keycode_to_button.find(keycode) != __keycode_to_button.end()) {
			button = __keycode_to_button.at(keycode);
		}
		else {
			return false;
		}

		return Input::GetButtonUp(static_cast<SDL_GameControllerButton>(button), controllerId);
	}
}
