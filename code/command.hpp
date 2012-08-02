#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <string>

class Command
{
public:
	explicit Command(const std::string &str);

	void setInverted(bool inverted);
	const std::string &getString() const;

	bool isInvertible() const;

private:
	std::string _command;
	std::string _inverseCommand;

	bool _invertible;
	bool _inverted;
};

#endif