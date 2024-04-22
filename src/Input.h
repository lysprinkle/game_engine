#ifndef INPUT_H
#define INPUT_H

#include "SDL2/SDL.h"
#include <unordered_map>
#include <string>
#include <vector>

enum INPUT_STATE { INPUT_STATE_UP, INPUT_STATE_JUST_BECAME_DOWN, INPUT_STATE_DOWN, INPUT_STATE_JUST_BECAME_UP };

const std::unordered_map<std::string, SDL_Scancode> __keycode_to_scancode = {
	// Directional (arrow) Keys
	{"up", SDL_SCANCODE_UP},
	{"down", SDL_SCANCODE_DOWN},
	{"right", SDL_SCANCODE_RIGHT},
	{"left", SDL_SCANCODE_LEFT},

	// Misc Keys
	{"escape", SDL_SCANCODE_ESCAPE},

	// Modifier Keys
	{"lshift", SDL_SCANCODE_LSHIFT},
	{"rshift", SDL_SCANCODE_RSHIFT},
	{"lctrl", SDL_SCANCODE_LCTRL},
	{"rctrl", SDL_SCANCODE_RCTRL},
	{"lalt", SDL_SCANCODE_LALT},
	{"ralt", SDL_SCANCODE_RALT},

	// Editing Keys
	{"tab", SDL_SCANCODE_TAB},
	{"return", SDL_SCANCODE_RETURN},
	{"enter", SDL_SCANCODE_RETURN},
	{"backspace", SDL_SCANCODE_BACKSPACE},
	{"delete", SDL_SCANCODE_DELETE},
	{"insert", SDL_SCANCODE_INSERT},

	// Character Keys
	{"space", SDL_SCANCODE_SPACE},
	{"a", SDL_SCANCODE_A},
	{"b", SDL_SCANCODE_B},
	{"c", SDL_SCANCODE_C},
	{"d", SDL_SCANCODE_D},
	{"e", SDL_SCANCODE_E},
	{"f", SDL_SCANCODE_F},
	{"g", SDL_SCANCODE_G},
	{"h", SDL_SCANCODE_H},
	{"i", SDL_SCANCODE_I},
	{"j", SDL_SCANCODE_J},
	{"k", SDL_SCANCODE_K},
	{"l", SDL_SCANCODE_L},
	{"m", SDL_SCANCODE_M},
	{"n", SDL_SCANCODE_N},
	{"o", SDL_SCANCODE_O},
	{"p", SDL_SCANCODE_P},
	{"q", SDL_SCANCODE_Q},
	{"r", SDL_SCANCODE_R},
	{"s", SDL_SCANCODE_S},
	{"t", SDL_SCANCODE_T},
	{"u", SDL_SCANCODE_U},
	{"v", SDL_SCANCODE_V},
	{"w", SDL_SCANCODE_W},
	{"x", SDL_SCANCODE_X},
	{"y", SDL_SCANCODE_Y},
	{"z", SDL_SCANCODE_Z},
	{"0", SDL_SCANCODE_0},
	{"1", SDL_SCANCODE_1},
	{"2", SDL_SCANCODE_2},
	{"3", SDL_SCANCODE_3},
	{"4", SDL_SCANCODE_4},
	{"5", SDL_SCANCODE_5},
	{"6", SDL_SCANCODE_6},
	{"7", SDL_SCANCODE_7},
	{"8", SDL_SCANCODE_8},
	{"9", SDL_SCANCODE_9},
	{"/", SDL_SCANCODE_SLASH},
	{";", SDL_SCANCODE_SEMICOLON},
	{"=", SDL_SCANCODE_EQUALS},
	{"-", SDL_SCANCODE_MINUS},
	{".", SDL_SCANCODE_PERIOD},
	{",", SDL_SCANCODE_COMMA},
	{"[", SDL_SCANCODE_LEFTBRACKET},
	{"]", SDL_SCANCODE_RIGHTBRACKET},
	{"\\", SDL_SCANCODE_BACKSLASH},
	{"'", SDL_SCANCODE_APOSTROPHE}
};

const std::unordered_map<std::string, SDL_GameControllerButton> __keycode_to_button = {
	// Directional (arrow) Buttons
	{"up", SDL_CONTROLLER_BUTTON_DPAD_UP},
	{"down", SDL_CONTROLLER_BUTTON_DPAD_DOWN},
	{"right", SDL_CONTROLLER_BUTTON_DPAD_RIGHT},
	{"left", SDL_CONTROLLER_BUTTON_DPAD_LEFT},

	// Shoulder Buttons
	{"left_shoulder", SDL_CONTROLLER_BUTTON_LEFTSHOULDER},
	{"right_shoulder", SDL_CONTROLLER_BUTTON_RIGHTSHOULDER},

	// Shoulder Buttons
	{"left_shoulder", SDL_CONTROLLER_BUTTON_LEFTSHOULDER},
	{"right_shoulder", SDL_CONTROLLER_BUTTON_RIGHTSHOULDER},

	// Stick Buttons
	{"left_stick", SDL_CONTROLLER_BUTTON_LEFTSTICK},
	{"right_stick", SDL_CONTROLLER_BUTTON_RIGHTSTICK},

	// Alphabetical Buttons
	{"a", SDL_CONTROLLER_BUTTON_A},
	{"b", SDL_CONTROLLER_BUTTON_B},
	{"x", SDL_CONTROLLER_BUTTON_X},
	{"y", SDL_CONTROLLER_BUTTON_Y},

	// Alphabetical Buttons
	{"a", SDL_CONTROLLER_BUTTON_A},
	{"b", SDL_CONTROLLER_BUTTON_B},
	{"x", SDL_CONTROLLER_BUTTON_X},
	{"y", SDL_CONTROLLER_BUTTON_Y},

	// Other Buttons
	{"start", SDL_CONTROLLER_BUTTON_START},
	{"guide", SDL_CONTROLLER_BUTTON_GUIDE},
	{"back", SDL_CONTROLLER_BUTTON_BACK},
	{"touchpad", SDL_CONTROLLER_BUTTON_TOUCHPAD},
};




class Input
{
public:
	static void Init(); // Call before main loop begins.
	static void ProcessEvent(const SDL_Event& e); // Call every frame at start of event loop.
	static void LateUpdate();

	static bool GetKey(SDL_Scancode keycode);
	static bool GetKeyDown(SDL_Scancode keycode);
	static bool GetKeyUp(SDL_Scancode keycode);

	static bool GetMouse(int button_num);
	static bool GetMouseUp(int button_num);
	static bool GetMouseDown(int button_num);

	static bool GetButton(SDL_GameControllerButton button, int controllerId);
	static bool GetButtonDown(SDL_GameControllerButton button, int controllerId);
	static bool GetButtonUp(SDL_GameControllerButton button, int controllerId);

private:
	static inline std::unordered_map<SDL_Scancode, INPUT_STATE> keyboard_states;
	static inline std::vector<SDL_Scancode> just_became_down_scancodes;
	static inline std::vector<SDL_Scancode> just_became_up_scancodes;

	static inline std::unordered_map < int, std::unordered_map<SDL_GameControllerButton, INPUT_STATE>> button_states;
	static inline std::unordered_map < int, std::vector<SDL_GameControllerButton>> just_became_down_buttons;
	static inline std::unordered_map < int, std::vector<SDL_GameControllerButton>> just_became_up_buttons;

	static inline int mouse_left_status;
	static inline int mouse_right_status;
	static inline int mouse_middle_status;
};

namespace input {
	bool GetKey(std::string);
	bool GetKeyDown(std::string);
	bool GetKeyUp(std::string);
	bool GetMouse(int);
	bool GetMouseDown(int);
	bool GetMouseUp(int);
	bool GetButton(std::string keycode, int controllerId);
	bool GetButtonDown(std::string keycode, int controllerId);
	bool GetButtonUp(std::string keycode, int controllerId);
}


#endif

