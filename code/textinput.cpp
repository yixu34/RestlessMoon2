#include <windows.h>

#include "textinput.hpp"

TextInput::TextInput()
{
	_cursorPos = 0;
	_status    = text_input_pending;
}

TextInput::~TextInput()
{
}

bool TextInput::keyDown(keypress key, keymod mod)
{
	_status = text_input_pending;

	if (key == VK_BACK && _text != "" && _cursorPos > 0)
	{
		std::string beforeCursor = _text.substr(0, _cursorPos);
		std::string afterCursor  = _text.substr(_cursorPos, _text.length() - _cursorPos);

		_text = beforeCursor.substr(0, beforeCursor.length() - 1) + afterCursor;
		_cursorPos--;
	}
	else if (key == VK_SPACE)
	{
		_text += " ";
		_cursorPos++;
	}
	else if (key == VK_RETURN)
	{
		_status = text_input_complete;
	}
	else if (key == VK_ESCAPE)
	{
		_status = text_input_cancelled;
	} 
	else
	{
		_text += static_cast<char>(key);
		_cursorPos++;
	}

	return true;
}

const std::string &TextInput::getText() const
{
	return _text;
}

void TextInput::clear()
{
	_text.clear();
	_cursorPos = 0;
}

TextInput::text_input_status TextInput::getStatus() const
{
	return _status;
}

//
// VK_DELETE is not picked up by the WM_CHAR message.  Oh well.
//
/*else if (key == VK_DELETE && _text != "" && _cursorPos < _text.length())
{
std::string beforeCursor = _text.substr(0, _cursorPos);
std::string afterCursor  = _text.substr(_cursorPos, _text.length() - _cursorPos);

_text = beforeCursor + 
afterCursor.substr(1, afterCursor.length() - 1);
}*/
/*else if (key == VK_LEFT)
{
_cursorPos--;
if (_cursorPos < 0)
_cursorPos = 0;
}
else if (key == VK_RIGHT)
{
_cursorPos++;
if (_cursorPos > _text.length())
_cursorPos = _text.length();
}*/