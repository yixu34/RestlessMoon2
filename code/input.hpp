#ifndef KEYBOARD_HPP
#define KEYBOARD_HPP

//
// Encapsulates input codes and stores key-command bindings.
//

#define DIRECTINPUT_VERSION 0x800

#include <d3dx9.h>
#include <dinput.h>
#include <boost/shared_ptr.hpp>
#include <string>
#include <map>

#include "command.hpp"
#include "inputhandler.hpp"

class Input;
typedef boost::shared_ptr<Input> InputPtr;

class Input
{
public:
	Input();
	~Input();

	bool initialize(HINSTANCE hInstance, HWND hwnd);
	void shutdown();

	void getMouseDelta(float &deltaX, float &deltaY) const;
    void getMousePosition(float &cursorX, float &cursorY) const;

	void timepass();

	void setKeyboardExclusive(bool exclusive);
	bool isKeyboardExclusive() const;

	void setMouseExclusive(bool exclusive);
	bool isMouseExclusive() const;

	bool isButtonPressed(mouse_button button);
	bool isButtonReleased(mouse_button button);

	bool isKeyPressed(keypress key);
	bool isKeyReleased(keypress key);

	static keypress keyNameToCode(const std::string &key, bool &isMouseBinding);
	static keypress convertKeycode(keypress key);

	static const keypress invalidKey;

private:
	bool initializeMouse(bool exclusive);
	bool initializeKeyboard(bool exclusive);

	void updateInput();
	//void updateBindings();
    void updateCursorPosition();

	bool isButtonDown(mouse_button button);
	bool isButtonUp(mouse_button button);

    bool isKeyDown(unsigned char *keyBuffer, keypress key);
    bool isKeyDown(keypress key);
    bool isKeyUp(keypress key);
	
	HWND _hwnd; // given from the game application.

	LPDIRECTINPUT8 _input;

	// Mouse data structures.
	LPDIRECTINPUTDEVICE8 _mouse;
	DIMOUSESTATE         _mouseNew;
	DIMOUSESTATE         _mouseOld;
    D3DXVECTOR2          _cursorPosition;

	bool _mouseExclusive;

	// Keyboard data structures.
	LPDIRECTINPUTDEVICE8 _keyboard;
    static const int numKeys = 256;
    unsigned char _keyboardNew[numKeys];
    unsigned char _keyboardOld[numKeys];

	bool _keyboardExclusive;
};

extern InputPtr input;

#endif
