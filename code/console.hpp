#ifndef CONSOLE_HPP
#define CONSOLE_HPP

//
// In-game console for setting variables and calling functions.
//

#include <string>
#include <deque>
#include <boost/shared_ptr.hpp>

#include "frame.hpp"
#include "sprite.hpp"
#include "textinput.hpp"
#include "eventlistener.hpp"

class Console;
typedef boost::shared_ptr<Console> ConsolePtr;

class Console : public Frame
{
public:
	Console();
	~Console();

	bool initialize();
	void shutdown();

	void toggle();
	void render();
	void timepass();

	bool keyDown(keypress key, keymod mod);
	bool isDown() const;

	bool isVisible() const;
	bool wantsExclusiveKeyboard() const;

	void write(const std::string &text);

private:
	//void onEnable();
	//void onDisable();

	void renderHistory();
	void executeString(const std::string &str);

	void updateScrolling();
	bool onCommand(const Event &event);

	typedef std::deque<std::string> HistoryList;
	HistoryList _history;

	SpritePtr _background;
	TextInput _textInput;

	bool _isDown;

	int   _historySize;
	float _speed;
	float _currRelHeight;
	float _maxRelHeight;
	float _maxAbsHeight;
};

extern ConsolePtr console;

#endif