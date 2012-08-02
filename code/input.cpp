#include <fstream>
#include <windows.h>
#include <boost/lexical_cast.hpp>

#include "input.hpp"
#include "util.hpp"
#include "common.hpp"
#include "scripting.hpp"
#include "console.hpp"
#include "graphics.hpp"

InputPtr input;
const keypress Input::invalidKey = -999;

Input::Input()
{
	_input    = 0;
	_mouse    = 0;
    _keyboard = 0;
	_hwnd     = 0;

	memset(&_mouseNew, 0, sizeof(DIMOUSESTATE));
	memset(&_mouseOld, 0, sizeof(DIMOUSESTATE));

    memset(_keyboardNew, 0, numKeys * sizeof(unsigned char));
    memset(_keyboardOld, 0, numKeys * sizeof(unsigned char));
}

Input::~Input()
{
}

bool Input::initialize(HINSTANCE hInstance, HWND hwnd)
{
	_hwnd = hwnd;

	if (FAILED(DirectInput8Create(
		hInstance, 
		DIRECTINPUT_VERSION, 
		IID_IDirectInput8, 
		reinterpret_cast<void **>(&_input), 
		0)))
	{
		return false;
	}

	if (!initializeMouse(false))
		return false;

	if (!initializeKeyboard(true))
		return false;

    // Fill in the input data structures for the first frame.
	updateInput();
	updateInput();

	// Place the cursor in the middle of the screen.
	_cursorPosition.x = graphics->getScreenWidth() / 2;
	_cursorPosition.y = graphics->getScreenHeight() / 2;

	return true;
}

bool Input::initializeMouse(bool exclusive)
{
	_mouseExclusive = exclusive;

	// Create and acquire the mouse.
	if (FAILED(_input->CreateDevice(GUID_SysMouse, &_mouse, 0)))
	{
		return false;
	}

	if (FAILED(_mouse->SetDataFormat(&c_dfDIMouse)))
	{
		return false;
	}

	if (FAILED(_mouse->SetCooperativeLevel(
		_hwnd, 
		(exclusive ? DISCL_EXCLUSIVE : DISCL_NONEXCLUSIVE) | DISCL_FOREGROUND)))
	{
		return false;
	}
	_mouse->Acquire();

	return true;
}

bool Input::initializeKeyboard(bool exclusive)
{
	_keyboardExclusive = exclusive;

	// Create and acquire the keyboard.
	if (FAILED(_input->CreateDevice(GUID_SysKeyboard, &_keyboard, 0)))
	{
		return false;
	}

	if (FAILED(_keyboard->SetDataFormat(&c_dfDIKeyboard)))
	{
		return false;
	}

	if (FAILED(_keyboard->SetCooperativeLevel(
		_hwnd, 
		(exclusive ? DISCL_EXCLUSIVE : DISCL_NONEXCLUSIVE) | DISCL_FOREGROUND)))
	{
		return false;
	}
	_keyboard->Acquire();

	return true;
}

void Input::setKeyboardExclusive(bool exclusive)
{
	if (_keyboardExclusive == exclusive)
		return;

	_keyboardExclusive = exclusive;

	if (_keyboard != 0)
	{
		_keyboard->Unacquire();
		_keyboard->Release();
		_keyboard = 0;
	}

	memset(_keyboardNew, 0, numKeys * sizeof(unsigned char));
	memset(_keyboardOld, 0, numKeys * sizeof(unsigned char));

	initializeKeyboard(exclusive);

	updateInput();
	updateInput();
}

void Input::setMouseExclusive(bool exclusive)
{
	if (_mouseExclusive == exclusive)
		return;

	_mouseExclusive = exclusive;

	if (_mouse != 0)
	{
		_mouse->Unacquire();
		_mouse->Release();
		_mouse = 0;
	}

	memset(&_mouseNew, 0, sizeof(DIMOUSESTATE));
	memset(&_mouseOld, 0, sizeof(DIMOUSESTATE));

	initializeMouse(exclusive);

	updateInput();
	updateInput();
}

bool Input::isKeyboardExclusive() const
{
	return _keyboardExclusive;
}

bool Input::isMouseExclusive() const
{
	return _mouseExclusive;
}

void Input::shutdown()
{
    if (_keyboard != 0)
    {
        _keyboard->Unacquire();
        _keyboard->Release();
    }

	if (_mouse != 0)
	{
		_mouse->Unacquire();
		_mouse->Release();
	}

	_input->Release();
}

void Input::getMouseDelta(float &deltaX, float &deltaY) const
{
	deltaX = _mouseNew.lX;
	deltaY = _mouseNew.lY;
}

void Input::getMousePosition(float &cursorX, float &cursorY) const
{
    cursorX = _cursorPosition.x;
    cursorY = _cursorPosition.y;
}

bool Input::isKeyDown(unsigned char *keyBuffer, keypress key)
{
    return (keyBuffer[key] & 0x80);
}

bool Input::isKeyDown(keypress key)
{
    return isKeyDown(reinterpret_cast<unsigned char *>(&_keyboardNew), key);
}

bool Input::isKeyUp(keypress key)
{
    return !isKeyDown(reinterpret_cast<unsigned char *>(&_keyboardNew), key);
}

bool Input::isKeyPressed(keypress key)
{
	return isKeyDown(key) && !isKeyDown(reinterpret_cast<unsigned char *>(&_keyboardOld), key);
}

bool Input::isKeyReleased(keypress key)
{
	return isKeyUp(key) && isKeyDown(reinterpret_cast<unsigned char *>(&_keyboardOld), key);
}

bool Input::isButtonDown(mouse_button button)
{
	return _mouseNew.rgbButtons[button] & 0x80;
}

bool Input::isButtonUp(mouse_button button)
{
	return !(_mouseNew.rgbButtons[button] & 0x80);
}

bool Input::isButtonPressed(mouse_button button)
{
	// If the button number corresponds to an actual mouse button...
	if (button >= 0)
		return isButtonDown(button) && !(_mouseOld.rgbButtons[button] & 0x80);
	else
	{
		 // Negative button numbers correspond to mouse wheel clicks.
		 if (button == -1) // mouse down
			 return _mouseNew.lZ < 0;
		 else // mouse up
			 return _mouseNew.lZ > 0;
	}
}

bool Input::isButtonReleased(mouse_button button)
{
	return isButtonUp(button) && (_mouseOld.rgbButtons[button] & 0x80);
}

void Input::updateInput()
{
	// Update the mouse state.
	memcpy(&_mouseOld, &_mouseNew, sizeof(DIMOUSESTATE));
	HRESULT mouseHr = _mouse->GetDeviceState(
		sizeof(DIMOUSESTATE), 
		reinterpret_cast<void *>(&_mouseNew));

	// If we lost the mouse device, try to reacquire it
	if (mouseHr == DIERR_INPUTLOST || mouseHr == DIERR_NOTACQUIRED)
		_mouse->Acquire();

	// Update the keyboard state.
	memcpy(_keyboardOld, _keyboardNew, numKeys * sizeof(unsigned char));
	HRESULT keyboardHr = _keyboard->GetDeviceState(
								numKeys * sizeof(unsigned char), 
								reinterpret_cast<void *>(&_keyboardNew));

	// If we lost the keyboard device, try to reacquire it.
	if (keyboardHr == DIERR_INPUTLOST || keyboardHr == DIERR_NOTACQUIRED)
		_keyboard->Acquire();
}

void Input::updateCursorPosition()
{
    float deltaX, deltaY;
    getMouseDelta(deltaX, deltaY);

    _cursorPosition.x += deltaX;
    _cursorPosition.y += deltaY;

    // Make sure that the cursor doesn't leave the bounds of the screen.
    _cursorPosition.x = util::clamp(_cursorPosition.x, 0.0f, graphics->getScreenWidth());
    _cursorPosition.y = util::clamp(_cursorPosition.y, 0.0f, graphics->getScreenHeight());
}

void Input::timepass()
{
    updateCursorPosition();
	updateInput();
//	updateBindings();
}

keypress Input::convertKeycode(keypress key)
{
	switch (key)
	{
    case 48: case 41: return DIK_0;
    case 49: case 33: return DIK_1; 
    case 50: case 64: return DIK_2; 
    case 51: case 35: return DIK_3;
    case 52: case 36: return DIK_4;
    case 53: case 37: return DIK_5;
    case 54: case 94: return DIK_6;
    case 55: case 38: return DIK_7;
    case 56: case 42: return DIK_8;
    case 57: case 40: return DIK_9;

    case 'a': case 'A': return DIK_A;
    case 'b': case 'B': return DIK_B;
    case 'c': case 'C': return DIK_C;
    case 'd': case 'D': return DIK_D;
    case 'e': case 'E': return DIK_E;
    case 'f': case 'F': return DIK_F;
    case 'g': case 'G': return DIK_G;
    case 'h': case 'H': return DIK_H;
    case 'i': case 'I': return DIK_I;
    case 'j': case 'J': return DIK_J;
    case 'k': case 'K': return DIK_K;
    case 'l': case 'L': return DIK_L;
    case 'm': case 'M': return DIK_M;
    case 'n': case 'N': return DIK_N;
    case 'o': case 'O': return DIK_O;
    case 'p': case 'P': return DIK_P;
    case 'q': case 'Q': return DIK_Q;
    case 'r': case 'R': return DIK_R;
    case 's': case 'S': return DIK_S;
    case 't': case 'T': return DIK_T;
    case 'u': case 'U': return DIK_U;
    case 'v': case 'V': return DIK_V;
    case 'w': case 'W': return DIK_W;
    case 'x': case 'X': return DIK_X;
    case 'y': case 'Y': return DIK_Y;
    case 'z': case 'Z': return DIK_Z;

	case VK_BACK: return DIK_BACK;

	case VK_OEM_3: return DIK_GRAVE;

    default: return invalidKey;
	};
}

keypress Input::keyNameToCode(const std::string &key, bool &isMouseBinding)
{
	//
	// Some keys that you cannot bind:
	//    CAPS LOCK
	//
	std::string keyLower = key;
	std::transform(keyLower.begin(), keyLower.end(), keyLower.begin(), std::tolower);
	isMouseBinding = false;

	if (keyLower == "mouse1")
	{
		isMouseBinding = true;
		return 0;
	}
	else if (keyLower == "mouse2")
	{
		isMouseBinding = true;
		return 1;
	}
	else if (keyLower == "mouse3")
	{
		isMouseBinding = true;
		return 2;
	}
	else if (keyLower == "mwheeldown")
	{
		isMouseBinding = true;
		return -1;
	}
	else if (keyLower == "mwheelup")
	{
		isMouseBinding = true;
		return -2;
	}
	else if (keyLower == "esc")    return DIK_ESCAPE;
	else if (keyLower == "`")      return DIK_GRAVE;
	else if (keyLower == "ctrl")   return DIK_LCONTROL;
	else if (keyLower == "shift")  return DIK_LSHIFT;
	else if (keyLower == "tab")    return DIK_TAB;
	else if (keyLower == "enter")  return DIK_RETURN;
	else if (keyLower == "space")  return DIK_SPACE;
	else if (keyLower == "ins")    return DIK_INSERT;
	else if (keyLower == "del")    return DIK_DELETE;
	else if (keyLower == "home")   return DIK_HOME;
	else if (keyLower == "end")    return DIK_END;
	else if (keyLower == "pgup")   return DIK_PRIOR;
	else if (keyLower == "pgdn")   return DIK_NEXT;
	else if (keyLower == "up")     return DIK_UP;
	else if (keyLower == "right")  return DIK_RIGHT;
	else if (keyLower == "down")   return DIK_DOWN;
	else if (keyLower == "left")   return DIK_LEFT;
	else if (keyLower == "backspace") return DIK_BACK;
    //
    // Letter bindings.
    //
	else if (keyLower == "a") return DIK_A;
	else if (keyLower == "b") return DIK_B;
	else if (keyLower == "c") return DIK_C;
	else if (keyLower == "d") return DIK_D;
	else if (keyLower == "e") return DIK_E;
	else if (keyLower == "f") return DIK_F;
	else if (keyLower == "g") return DIK_G;
	else if (keyLower == "h") return DIK_H;
	else if (keyLower == "i") return DIK_I;
	else if (keyLower == "j") return DIK_J;
	else if (keyLower == "k") return DIK_K;
	else if (keyLower == "l") return DIK_L;
	else if (keyLower == "m") return DIK_M;
	else if (keyLower == "n") return DIK_N;
	else if (keyLower == "o") return DIK_O;
	else if (keyLower == "p") return DIK_P;
	else if (keyLower == "q") return DIK_Q;
	else if (keyLower == "r") return DIK_R;
	else if (keyLower == "s") return DIK_S;
	else if (keyLower == "t") return DIK_T;
	else if (keyLower == "u") return DIK_U;
	else if (keyLower == "v") return DIK_V;
	else if (keyLower == "w") return DIK_W;
	else if (keyLower == "x") return DIK_X;
	else if (keyLower == "y") return DIK_Y;
	else if (keyLower == "z") return DIK_Z;
    //
    // Number bindings.
    //
    else if (keyLower == "0") return DIK_0;
    else if (keyLower == "1") return DIK_1;
    else if (keyLower == "2") return DIK_2;
    else if (keyLower == "3") return DIK_3;
    else if (keyLower == "4") return DIK_4;
    else if (keyLower == "5") return DIK_5;
    else if (keyLower == "6") return DIK_6;
    else if (keyLower == "7") return DIK_7;
    else if (keyLower == "8") return DIK_8;
    else if (keyLower == "9") return DIK_9;

	else if (keyLower == "escape") return DIK_ESCAPE;

	else
	{
		return invalidKey;

		// TODO:  Map numpad and function keys to DirectInput key codes!
		//// string of form:  numpadX
		//if (key.find("numpad") == 0 && key.size() == 7)
		//{
		//	std::string numpadPrefix = key.substr(0, 6);
		//	int number;
		//	try {
		//		number = boost::lexical_cast<int>(key[6]);
		//		return VK_NUMPAD0 + number;
		//	} catch (boost::bad_lexical_cast &) {
		//		return invalidKey;
		//	}
		//}
		//// string of the form:  fx[y] (the function keys, f1-f12)
		//else if (key.size() >= 2 && key[0] == 'f')
		//{
		//	std::string suffix = key.substr(1, key.size() - 1);
		//	int number;
		//	try {
		//		number = boost::lexical_cast<int>(suffix);
		//		if (number >= 1 && number <= 12)
		//			return VK_F1 + (number - 1);
		//		else
		//			return invalidKey;
		//	} catch (boost::bad_lexical_cast &) {
		//		return invalidKey;
		//	}
		//}
		//else
		//	return invalidKey;
	}
}

//DWORD Input::keyNameToCode(std::string key)
//{
//	//
//	// Some keys that you cannot bind:
//	//    ESC
//	//
//
//	if (key.size() == 1 && std::isalnum(key[0]))
//		return key[0];
//
//	std::transform(key.begin(), key.end(), key.begin(), std::tolower);
//
//	if (key == "`")
//		return VK_OEM_3; // the tilde ~
//	else if (key == "mouse1")
//		return VK_LBUTTON;
//	else if (key == "mouse2")
//		return VK_RBUTTON;
//	else if (key == "mouse3")
//		return VK_MBUTTON;
//	//else if (key == "mwheelup")
//	//	return VK_MWHEELUP;
//	//else if (key == "mwheeldown")
//	//	return VK_MWHEELDOWN;
//	else if (key == "ctrl")
//		return VK_CONTROL;
//	else if (key == "shift")
//		return VK_SHIFT;
//	else if (key == "tab")
//		return VK_TAB;
//	else if (key == "backspace")
//		return VK_BACK;
//	else if (key == "caps")
//		return VK_CAPITAL;
//	else if (key == "enter")
//		return VK_RETURN;
//	else if (key == "space")
//		return VK_SPACE;
//	else if (key == "ins")
//		return VK_INSERT;
//	else if (key == "del")
//		return VK_DELETE;
//	else if (key == "home")
//		return VK_HOME;
//	else if (key == "end")
//		return VK_END;
//	else if (key == "pgup")
//		return VK_PRIOR;
//	else if (key == "pgdn")
//		return VK_NEXT;
//	else if (key == "up")
//		return VK_UP;
//	else if (key == "right")
//		return VK_RIGHT;
//	else if (key == "down")
//		return VK_DOWN;
//	else if (key == "left")
//		return VK_LEFT;
//	else
//	{
//		// string of form:  numpadX
//		if (key.find("numpad") == 0 && key.size() == 7)
//		{
//			std::string numpadPrefix = key.substr(0, 6);
//			int number;
//			try {
//				number = boost::lexical_cast<int>(key[6]);
//				return VK_NUMPAD0 + number;
//			} catch (boost::bad_lexical_cast &) {
//				return invalidKey;
//			}
//		}
//		// string of the form:  fx[y] (the function keys, f1-f12)
//		else if (key.size() >= 2 && key[0] == 'f')
//		{
//			std::string suffix = key.substr(1, key.size() - 1);
//			int number;
//			try {
//				number = boost::lexical_cast<int>(suffix);
//				if (number >= 1 && number <= 12)
//					return VK_F1 + (number - 1);
//				else
//					return invalidKey;
//			} catch (boost::bad_lexical_cast &) {
//				return invalidKey;
//			}
//		}
//		else
//			return invalidKey;
//	}
//}

//void Input::saveBindingsToFile(const std::string &filename)
//{
//	std::ofstream bindingsFile(filename.c_str());
//	if (!bindingsFile.is_open())
//	{
//		ERRORMSG("Couldn't save input bindings to file.");
//		return;
//	}
//
//	// Prepend a text header to the file...
//	bindingsFile<<"------------------------------------------\n";
//	bindingsFile<<"--Generated by Revenant - do not modify!!!\n";
//	bindingsFile<<"------------------------------------------\n\n";
//
//	for (BindingPool::iterator it = _bindings.begin(); 
//		 it != _bindings.end(); it++)
//	{
//		const KeyState &currState = it->second;
//
//		// NOTE:  If the syntax for the 'bind' scripting function changes, then
//		// it must also be changed here.
//		/*bindingsFile<<"bind(\""<<currState._keyName<<"\", \""<<
//		                         currState._command->getName()<<"\")\n";*/
//		/*bindingsFile<<"bind(\""<<currState._keyName<<"\", \""<<
//			          currState._command<<"\")\n";*/
//	}
//}

