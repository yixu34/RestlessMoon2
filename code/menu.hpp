#ifndef MENU_HPP
#define MENU_HPP

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

#include "frame.hpp"
#include "widget.hpp"

class Menu;
typedef boost::shared_ptr<Menu> MenuPtr;

class Menu : public Frame
{
public:
	Menu(const std::string &name);
	~Menu();

	 

private:


	std::string _onEnteringCommand;
};

#endif