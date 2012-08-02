#include "menu.hpp"
#include "scripting.hpp"

Menu::Menu(const std::string &name)
	: Frame(name)
{
	_focusedWidget     = WidgetPtr();
	_onEnteringCommand = "";
}

Menu::~Menu()
{
}

