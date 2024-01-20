#ifndef MBASE_KEY_INPUT_H
#define MBASE_KEY_INPUT_H

#include <mbase/common.h>
#include <mbase/behaviors.h>
#include <mbase/vector.h>
#include <WinUser.h>

MBASE_BEGIN

enum class key_strokes : U16 {
	KS_BACKSPACE = VK_BACK,
	KS_TAB = VK_TAB,
	KS_CLEAR = VK_CLEAR,
	KS_ENTER = VK_RETURN,
	KS_SHIFT = VK_SHIFT,
	KS_CONTROL = VK_CONTROL,
	KS_ALT = VK_MENU,
	KS_PAUSE = VK_PAUSE,
	KS_CAPSLOCK = VK_CAPITAL,
	KS_ESCAPE = VK_ESCAPE,
	KS_SPACE = VK_SPACE,
	KS_PGUP = VK_PRIOR,
	KS_PGDOWN = VK_NEXT,
	KS_END = VK_END,
	KS_HOME = VK_HOME,
	KS_LEFT = VK_LEFT,
	KS_UP = VK_UP,
	KS_RIGHT = VK_RIGHT,
	KS_DOWN = VK_DOWN,
	KS_SNAPSHOT = VK_SNAPSHOT,
	KS_INSERT = VK_INSERT,
	KS_DELETE = VK_DELETE,
	KS_HELP = VK_HELP,
	/* COMMON KEYS BEGIN */
	KS_NUM0 = 0x30,
	KS_NUM1 = 0x31,
	KS_NUM2 = 0x32,
	KS_NUM3 = 0x33,
	KS_NUM4 = 0x34,
	KS_NUM5 = 0x35,
	KS_NUM6 = 0x36,
	KS_NUM7 = 0x37,
	KS_NUM8 = 0x38,
	KS_NUM9 = 0x39,
	KS_KEY_A = 0x41,
	KS_KEY_B = 0x42,
	KS_KEY_C = 0x43,
	KS_KEY_D = 0x44,
	KS_KEY_E = 0x45,
	KS_KEY_F = 0x46,
	KS_KEY_G = 0x47,
	KS_KEY_H = 0x48,
	KS_KEY_I = 0x49,
	KS_KEY_J = 0x4A,
	KS_KEY_K = 0x4B,
	KS_KEY_L = 0x4C,
	KS_KEY_M = 0x4D,
	KS_KEY_N = 0x4E,
	KS_KEY_O = 0x4F,
	KS_KEY_P = 0x50,
	KS_KEY_Q = 0x51,
	KS_KEY_R = 0x52,
	KS_KEY_S = 0x53,
	KS_KEY_T = 0x54,
	KS_KEY_U = 0x55,
	KS_KEY_V = 0x56,
	KS_KEY_W = 0x57,
	KS_KEY_X = 0x58,
	KS_KEY_Y = 0x59,
	KS_KEY_Z = 0x5A,
	/* COMMON KEYS END */
	KS_LWIN = VK_LWIN,
	KS_RWIN = VK_RWIN,
	KS_NUMP0 = VK_NUMPAD0,
	KS_NUMP1 = VK_NUMPAD1,
	KS_NUMP2 = VK_NUMPAD2,
	KS_NUMP3 = VK_NUMPAD3,
	KS_NUMP4 = VK_NUMPAD4,
	KS_NUMP5 = VK_NUMPAD5,
	KS_NUMP6 = VK_NUMPAD6,
	KS_NUMP7 = VK_NUMPAD7,
	KS_NUMP8 = VK_NUMPAD8,
	KS_NUMP9 = VK_NUMPAD9,
	KS_MUL = VK_MULTIPLY,
	KS_ADD = VK_ADD,
	KS_SUB = VK_SUBTRACT,
	KS_DIV = VK_DIVIDE,
	KS_F1 = VK_F1,
	KS_F2 = VK_F2,
	KS_F3 = VK_F3,
	KS_F4 = VK_F4,
	KS_F5 = VK_F5,
	KS_F6 = VK_F6,
	KS_F7 = VK_F7,
	KS_F8 = VK_F8,
	KS_F9 = VK_F9,
	KS_F10 = VK_F10,
	KS_F11 = VK_F11,
	KS_F12 = VK_F12
};

enum class key_input_err : U16 {
	KEY_INPUT_SUCCESS = 0,
	KEY_INPUT_ERR_OUT_OF_RANGE = 1,
	KEY_INPUT_ERR_INVALID_TYPE = 2,
	KEY_INPUT_INVALID_HANDLER = 3,
	KEY_INPUT_HANDLER_NOT_FOUND = 4
};

/*
	INPUT HANDLER METHODS:
	- SetListenKey(key_stroke, toggle or hold)
	- SetListenType(toggle or hold)
	- GetListenKey()
	- GetListenType()
	- IsToggled() 
	- IsDown()
	- OnToggleIn ---> Won't be called if the listen type is hold
	- OnToggleOut ---> Won't be called if the listen type is hold
	- OnDown ---> Will also be called even if the listen type is toggle
	- OnUp ---> Will also be called even if the listen type is toggle
*/

class keyboard_input_handler {
public:
	enum class listen_type : U8 {
		LT_TOGGLE = 0,
		LT_HOLD = 1
	};

	keyboard_input_handler() : isToggled(false), isPressed(false) {}

	key_input_err SetListenKey(key_strokes in_ks, listen_type in_lt) noexcept {
		if(in_ks < key_strokes::KS_BACKSPACE || in_ks > key_strokes::KS_F12)
		{
			return key_input_err::KEY_INPUT_ERR_OUT_OF_RANGE;
		}

		else if(in_lt != listen_type::LT_TOGGLE && in_lt != listen_type::LT_HOLD)
		{
			return key_input_err::KEY_INPUT_ERR_INVALID_TYPE;
		}

		listenKey = in_ks;
		listenType = in_lt;

		return key_input_err::KEY_INPUT_SUCCESS;
	}

	key_input_err SetListenType(listen_type in_lt) noexcept {
		if (in_lt != listen_type::LT_TOGGLE && in_lt != listen_type::LT_HOLD)
		{
			return key_input_err::KEY_INPUT_ERR_INVALID_TYPE;
		}

		listenType = in_lt;
	}

	key_strokes GetListenKey() const noexcept {
		return listenKey;
	}

	listen_type GetListenType() const noexcept {
		return listenType;
	}

	bool IsToggled() const noexcept {
		return isToggled;
	}

	bool IsPressed() const noexcept {
		return isPressed;
	}

	virtual GENERIC OnToggleIn() noexcept {}
	virtual GENERIC OnToggleOut() noexcept {}
	virtual GENERIC OnDown() noexcept {}
	virtual GENERIC OnUp() noexcept {}

	friend class keyboard_input_manager;

private:
	bool isPressed;
	bool isToggled;
	key_strokes listenKey;
	listen_type listenType;
};

/*
	KEYBOARD MESSAGES:
	- WM_ACTIVE = If the window becomes activated
	- WM_CHAR = Posted to the window with the keyboard focus when a WM_KEYDOWN message is translated by the TranslateMessage function
	- WM_HOTKEY = If ALT, CTRL, SHIFT or WIN key pressed
	- WM_KEYDOWN = If key is pressed
	- WM_KEYUP = If key is up
	- WM_KILLFOCUS = If the window loses keyboard focus
	- WM_SETFOCUS = If the window gains keyboard focus
	- WM_SYSDEADCHAR = RESEARCH
	- WM_SYSKEYDOWN = RESEARCH
	- WM_SYSKEYUP = RESEARCH
*/

/*
	INPUT MANAGER METHODS:
	- DisableInput = Disables keyboard input
	- 
*/

typedef SHORT(*KeyStateFunction)(I32 vKey); // WINDOWS SPECIFIC

class keyboard_input_manager : public non_copymovable {
public:
	keyboard_input_manager() : isListening(true) {
		ksf[0] = GetKeyState;
		ksf[1] = GetAsyncKeyState;
	}

	GENERIC DisableInput() noexcept {
		isListening = false;
	}

	GENERIC EnableInput() noexcept {
		isListening = true;
	}

	bool IsInputActive() const noexcept {
		return isListening;
	}

	key_input_err RegisterKeyHandler(keyboard_input_handler* in_handler) noexcept {
		if(!in_handler)
		{
			return key_input_err::KEY_INPUT_INVALID_HANDLER;
		}

		inputHandlers.push_back(in_handler);
		return key_input_err::KEY_INPUT_SUCCESS;
	}

	GENERIC UnRegisterHandler(keyboard_input_handler* in_handler) noexcept {
		
	}

	GENERIC Update() noexcept {
		if(!isListening)
		{
			return;
		}

		for(mbase::vector<keyboard_input_handler*>::iterator It = inputHandlers.begin(); It != inputHandlers.end(); It++)
		{
			keyboard_input_handler* activeHandler = *It;
			I32 listenType = (I32)activeHandler->GetListenType();
			I32 listenKey = (I32)activeHandler->GetListenKey();

			// IF THE LISTEN TYPE IS LT_TOGGLE(0), GetKeyState will be called
			// IF THE LISTEN TYPE IS LT_HOLD(1), GetAsyncKeyState will be called

			U16 resultState = ksf[listenType](listenKey);
			if(resultState == 1) // It can only be one if the 'ksf' is 'GetKeyState'
			{
				// MEANS TOGGLED
				activeHandler->OnToggleIn();
				activeHandler->isToggled = true;
			}
			else if(resultState >= I16_MAX)
			{
				activeHandler->OnDown();
				activeHandler->isPressed = true;
			}
			else if(!resultState)
			{
				if(activeHandler->isPressed)
				{
					activeHandler->OnUp();
					activeHandler->isPressed = false;
				}
				
				if(activeHandler->isToggled)
				{
					activeHandler->OnToggleOut();
					activeHandler->isToggled = false;
				}
			}
		}
	}

private:
	KeyStateFunction ksf[2];
	bool isListening;
	mbase::vector<keyboard_input_handler*> inputHandlers;
};

MBASE_END

#endif // !MBASE_KEY_INPUT_H
