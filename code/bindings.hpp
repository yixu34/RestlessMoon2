#ifndef BINDINGS_HPP
#define BINDINGS_HPP

//
// Stores a table of keyboard and mouse bindings, allowing for customizable
// controls.
//

#include <d3dx9.h>
#include <string>
#include <map>

#include "command.hpp"
#include "inputhandler.hpp"

class Bindings
{
public:
	Bindings();
	~Bindings();

	void bind(const std::string &keyName, const std::string &command);
	void unbind(const std::string &keyName);
	void unbindall();
	void reset();

	std::string getCommandString(keypress key) const;

	void timepass();

private:
	struct KeyState
	{
		KeyState(
			const Command &command = Command(""), 
			bool isMouseBinding = false)
			: _command(command), _isMouseBinding(isMouseBinding)
		{}

		bool    _isMouseBinding;
		Command _command;
	};

	typedef std::map<keypress, KeyState> BindingPool;
	BindingPool _bindings;
};

extern Bindings bindings;

#endif