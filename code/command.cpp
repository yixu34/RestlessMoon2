#include "command.hpp"

Command::Command(const std::string &str)
{
	_inverted = false;

	if (str == "")
	{
		_command = _inverseCommand = "";
		return;
	}

	char firstChar = str[0];
	if (firstChar == '+' || firstChar == '-')
	{
		std::string withoutFirst  = str.substr(1, str.length() - 1);
		std::string withoutParens = withoutFirst.substr(0, withoutFirst.size() - 2);

		_command        = withoutParens + "(1)";
		_inverseCommand = withoutParens + "(0)";
		_invertible     = true;
	}
	else
	{
		// Ordinary string, so it doesn't have a different inverse.
		_command    = _inverseCommand = str;
		_invertible = false;
	}
}

void Command::setInverted(bool inverted)
{
	_inverted = inverted;
}

const std::string &Command::getString() const
{
	if (_inverted)
		return _inverseCommand;
	else
		return _command;
}

bool Command::isInvertible() const
{
	return _invertible;
}