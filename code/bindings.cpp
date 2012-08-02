#include "bindings.hpp"
#include "input.hpp"

// REMOVE THESE!!!
#include "scripting.hpp"
#include "console.hpp"

Bindings bindings;

Bindings::Bindings()
{
}

Bindings::~Bindings()
{
}

void Bindings::bind(const std::string &keyName, const std::string &command)
{
	bool isMouseBinding;
	DWORD keyCode = Input::keyNameToCode(keyName, isMouseBinding);
	if (keyCode != Input::invalidKey)
		_bindings[keyCode] = KeyState(Command(command), isMouseBinding);;
}

void Bindings::unbind(const std::string &keyName)
{
	bool unused;
	DWORD keyCode = Input::keyNameToCode(keyName, unused);

	BindingPool::iterator it = _bindings.find(keyCode);
	if (it != _bindings.end())
		_bindings.erase(it);
}

void Bindings::unbindall()
{
	_bindings.clear();
}

std::string Bindings::getCommandString(keypress key) const
{
	BindingPool::const_iterator it = _bindings.find(key);
	if (it != _bindings.end())
		return it->second._command.getString();
	else
		return "";
}

void Bindings::timepass()
{
	for (BindingPool::iterator it = _bindings.begin();
		it != _bindings.end(); it++)
	{
		DWORD currKey       = it->first;
		KeyState &currState = it->second;
		bool isMouseBinding = currState._isMouseBinding;
		bool isPressed  = isMouseBinding ? input->isButtonPressed(static_cast<mouse_button>(currKey)) : input->isKeyPressed(currKey);
		bool isReleased = isMouseBinding ? input->isButtonReleased(static_cast<mouse_button>(currKey)) : input->isKeyReleased(currKey);

		// FIXME:  Arg, fix this hack and add proper support for switching
		// between frames.
		if (isPressed)
		{
			if (currState._command.getString() == "toggleConsole()")
				console->toggle();
			else if (!console->isDown())
			{
				currState._command.setInverted(false);
				scripting->executeCommand(currState._command);
			}
		}
		else if (isReleased)
		{
			currState._command.setInverted(true);
			if (currState._command.isInvertible())
				scripting->executeCommand(currState._command);
		}
	}
}

void Bindings::reset()
{
	for (BindingPool::iterator it = _bindings.begin();
		 it != _bindings.end(); it++)
	{
		Command &currCommand = it->second._command;
		if (currCommand.isInvertible())
		{
			currCommand.setInverted(true);
			scripting->executeCommand(currCommand);
		}
	}
}
