#ifndef INPUTHANDLER_HPP
#define INPUTHANDLER_HPP

//
// Interface for classes that care about input device actions.
// (e.g. keypress, mouseclick.)
//

#include <d3dx9.h>

typedef DWORD keymod;
typedef DWORD keypress;

enum mouse_button
{
	button_left   = 0, 
	button_right  = 1, 
	button_middle = 2,
};

class InputHandler
{
public:
	InputHandler() {}//: _listensToChars(false) {}
	virtual ~InputHandler() = 0 {}

	// 
	// All of these functions return true if the handler consumed the 
	// input, false otherwise.
	//
	virtual bool keyDown(keypress key, keymod mod)
	{ return false; }

	virtual bool keyUp(keypress key, keymod mod)
	{ return false; }

	virtual bool mouseDown(mouse_button button, float clickX, float clickY)
	{ return false; }

	virtual bool mouseUp(mouse_button button, float clickX, float clickY)
	{ return false; }

//	bool listensToChars() const                 { return _listensToChars;           }
//	void setListensToChars(bool listensToChars) { _listensToChars = listensToChars; }
//
//protected:
//	bool _listensToChars;
};

#endif
