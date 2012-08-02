#include "console.hpp"
#include "input.hpp"
#include "command.hpp"
#include "scripting.hpp"
#include "bindings.hpp"
#include "graphics.hpp"
#include "timer.hpp"
#include "framemanager.hpp"

ConsolePtr console;

Console::Console()
	: Frame("console", 1)
{
	_isDown        = false;
	_historySize   = 10;
	_speed         = 0.001f;
	_currRelHeight = 0.0f;
	_maxRelHeight  = 0.4f;
}

Console::~Console()
{
	shutdown();
}

bool Console::initialize()
{
	frameManager->addFrame(shared_from_this());
	setEnabled(true);

	_maxAbsHeight = _maxRelHeight * graphics->getScreenHeight();

	_background = spriteCache.get("media/tex_consoleback.jpg");
	return (_background);
}

void Console::shutdown()
{
	if (_background )
		_background->unload();
}

void Console::executeString(const std::string &str)
{
	scripting->executeString(str);
}

void Console::toggle()
{
	_isDown = !_isDown;

	_textInput.clear();
	input->setKeyboardExclusive(!_isDown);
}

//void Console::onEnable()
//{
//	input->setKeyboardExclusive(false);
//}
//
//void Console::onDisable()
//{
//	_textInput.clear();
//	input->setKeyboardExclusive(true);
//}

void Console::updateScrolling()
{
	if (!isDown() && _currRelHeight > 0.0f)
	{
		_currRelHeight -= _speed * getDtMilliseconds();

		if (_currRelHeight < 0.0f)
			_currRelHeight = 0.0f;

	}
	else if (isDown() && _currRelHeight < _maxRelHeight)
	{
		// If the console is marked as partially down, but hasn't reached
		// the bottom yet, then gradually move it down.
		_currRelHeight += _speed * getDtMilliseconds();

		if (_currRelHeight / _maxRelHeight < 0.1f)
			_textInput.clear();

		if (_currRelHeight > _maxRelHeight)
			_currRelHeight = _maxRelHeight;
	}
}

void Console::render()
{
	// Don't render the console if we can't see it.
	if (_currRelHeight <= 0.0f)
		return;

	Frame::render();

	float consoleTopHeight = _currRelHeight - _maxRelHeight;
	_background->render(0.0f, consoleTopHeight, 1.0f, _maxRelHeight);

	float textWidth, textHeight;
	graphics->calculateTextDimensions(
		_textInput.getText().c_str(),
		textWidth, 
		textHeight);

	float relX = 0.0f; // ignored
	float relY = _currRelHeight;
	float absX;
	float absY;
	graphics->toAbsoluteCoords(relX, relY, absX, absY);
	graphics->drawTextAbs(0.0f, absY - textHeight, _textInput.getText().c_str());

	renderHistory();
}

void Console::timepass()
{
	Frame::timepass();

	updateScrolling();
}

void Console::renderHistory()
{
	float textWidth, textHeight;
	graphics->calculateTextDimensions(
		_textInput.getText().c_str(),
		textWidth, 
		textHeight);

	int i = 2;
	float currAbsHeight = _currRelHeight * graphics->getScreenHeight();
	for (HistoryList::reverse_iterator it = _history.rbegin();
		 it != _history.rend(); it++)
	{
		const std::string &currLine = *it;
		
		float currTextY = -(i * textHeight) + currAbsHeight;
		graphics->drawTextAbs(0.0f, currTextY, currLine.c_str());

		i++;
	}
}

bool Console::keyDown(keypress key, keymod mod)
{
	if (!isDown())
		return false;

	// If we intercept a toggleConsole() command, then execute it.
	keypress convertedKey     = Input::convertKeycode(key);
	std::string commandString = bindings.getCommandString(convertedKey);
	if (commandString == "toggleConsole()")
	{
		toggle();
		return true;
	}

	_textInput.keyDown(key, mod);
	TextInput::text_input_status status = _textInput.getStatus();
	if (status == TextInput::text_input_complete)
	{
		// Add the completed string to the history.
		// Front = top    = oldest,
		// Back  = bottom = newest
		std::string finishedText = _textInput.getText();
		write(finishedText);

		if (_history.size() > _historySize)
			_history.pop_front();
		
		executeString(finishedText);
		_textInput.clear();
	}
	else if (status == TextInput::text_input_cancelled)
		_textInput.clear();

	return true;
}

bool Console::isDown() const
{
	return _isDown;
}

bool Console::isVisible() const
{
	return _enabled && isDown();
}

bool Console::wantsExclusiveKeyboard() const
{
	return false;
}

void Console::write(const std::string &text)
{
	_history.push_back(text);
}
