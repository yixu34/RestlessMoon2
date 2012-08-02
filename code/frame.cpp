#include "frame.hpp"
#include "framemanager.hpp"
#include "input.hpp"
#include "scripting.hpp"

Frame::Frame(const std::string &frameName, int priority)
	: _frameName(frameName), _priority(priority), _enabled(false)
{
}

Frame::~Frame()
{
}

void Frame::setEnabled(bool enabled)
{
	// Don't bother updating anything if we're not changing the enabled status
	if (_enabled == enabled)
		return;

	// Otherwise update the enabled status.
	_enabled = enabled;
	frameManager->setFrameEnabled(_frameName, enabled);

	if (enabled)
		onEnable();
	else
		onDisable();
}

int Frame::addWidget(const WidgetPtr &widget)
{
	bool wasEmpty = _widgets.empty();
	_widgets.push_back(widget);

	if (wasEmpty)
		_focusedWidget = widget;

	//input->setKeyboardExclusive(false);

	return widget->getId();
}

const std::string &Frame::getName() const
{
	return _frameName;
}

bool Frame::keyDown(keypress key, keymod mod)
{
	if (_focusedWidget)
		return _focusedWidget->keyDown(key, mod);
	else
		return false;
}

bool Frame::keyUp(keypress key, keymod mod)
{
	if (_focusedWidget)
		return _focusedWidget->keyUp(key, mod);
	else
		return false;
}

bool Frame::mouseDown(mouse_button button, float clickX, float clickY)
{
	for (WidgetPool::iterator it = _widgets.begin(); it != _widgets.end(); it++)
	{
		const WidgetPtr &currWidget = *it;
		if (currWidget->containsPoint(clickX, clickY) &&
			currWidget->mouseDown(button, clickX, clickY))
		{
			setFocusedWidget(currWidget);
			return true;
		}
	}
	return false;
}

bool Frame::mouseUp(mouse_button button, float clickX, float clickY)
{
	for (WidgetPool::iterator it = _widgets.begin(); it != _widgets.end(); it++)
	{
		const WidgetPtr &currWidget = *it;
		if (currWidget->containsPoint(clickX, clickY) &&
			currWidget->mouseUp(button, clickX, clickY))
		{
			return true;
		}
	}
	return false;
}

void Frame::render()
{
	if (!_enabled)
		return;
	
	if (_renderScriptFunction != "")
		scripting->executeString(_renderScriptFunction);

	if (_frameName == "game")
	{
		for (WidgetPool::iterator it = _widgets.begin(); it != _widgets.end(); it++)
		{
			const WidgetPtr &currWidget = *it;
			currWidget->render();
		}
	}
}

void Frame::setRenderScriptFunction(const std::string &renderScriptFunction)
{
	_renderScriptFunction = renderScriptFunction;
}

void Frame::renderMouseOver(bool mouseOver)
{
	if (input->isMouseExclusive())
		return;

	float cursorX, cursorY;
	input->getMousePosition(cursorX, cursorY);

	for (WidgetPool::iterator it = _widgets.begin(); it != _widgets.end(); it++)
	{
		const WidgetPtr &currWidget = *it;
		if (mouseOver && currWidget->containsPoint(cursorX, cursorY))
		{
			currWidget->render();
			return;
		}
		else if(!mouseOver && !currWidget->containsPoint(cursorX, cursorY))
		{
			currWidget->render();
		}
	}
}

void Frame::timepass()
{
	for (WidgetPool::iterator it = _widgets.begin(); it != _widgets.end();)
	{
		const WidgetPtr &currWidget = *it;
		if (currWidget->shouldRemove())
		{
			it = _widgets.erase(it);
			/*if (_widgets.empty())
				input->setKeyboardExclusive(true);*/
		}
		else
			it++;
	}
}

void Frame::onEnable()
{
}

void Frame::onDisable()
{
}

bool Frame::wantsExclusiveKeyboard() const
{
	return _widgets.empty();
}

bool Frame::isVisible() const
{
	return _enabled;
}

//void Frame::setEnterCommand(const std::string &command)
//{
//	_onEnteringCommand = command;
//}
//
//void Frame::onEntering()
//{
//	if (_onEnteringCommand != "")
//		scripting->executeString(_onEnteringCommand);
//}

//void Frame::onLeaving()
//{
//	for (WidgetPool::iterator it = _widgets.begin(); it != _widgets.end(); it++)
//	{
//		const WidgetPtr &currWidget = *it;
//		currWidget->onLeavingFrame();
//	}
//}

void Frame::setFocusedWidget(int widgetId)
{
	for (WidgetPool::iterator it = _widgets.begin(); it != _widgets.end(); it++)
	{
		const WidgetPtr &currWidget = *it;
		if (currWidget->getId() == widgetId)
		{
			setFocusedWidget(currWidget);
			return;
		}
	}
}

void Frame::setFocusedWidget(const WidgetPtr &newFocus)
{
	if (_focusedWidget)
		_focusedWidget->setKeyFocus(false);

	_focusedWidget = newFocus;

	if (_focusedWidget)
	{
		_focusedWidget->setKeyFocus(true);
		//		_listensToChars = _focusedWidget->listensToChars();
	}
	//else
	//	_listensToChars = false;
}

//void Frame::saveOptions()
//{
//	// TODO:  Implement this.
//}

int Frame::getPriority() const
{
	return _priority;
}


