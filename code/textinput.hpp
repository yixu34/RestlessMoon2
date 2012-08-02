#ifndef TEXTINPUT_HPP
#define TEXTINPUT_HPP

#include <string>

#include "inputhandler.hpp"

class TextInput : public InputHandler
{
public:
	TextInput();
	~TextInput();

	enum text_input_status
	{
		text_input_pending, 
		text_input_complete,
		text_input_cancelled,
	};

	bool keyDown(keypress key, keymod mod);
	const std::string &getText() const;
	void clear();

	text_input_status getStatus() const;

private:
	std::string       _text;
	int               _cursorPos;
	text_input_status _status;
};

#endif
