#ifndef FRAME_HPP
#define FRAME_HPP

// 
// Represents an in-game screen.
//

#include <string>
#include <boost/enable_shared_from_this.hpp>

#include "inputhandler.hpp"
#include "widget.hpp"

class Frame : public InputHandler, public boost::enable_shared_from_this<Frame>
{
public:
	Frame(const std::string &frameName, int priority);
    virtual ~Frame();

	const std::string &getName() const;

	int addWidget(const WidgetPtr &widget);

	void setFocusedWidget(int widgetId);
	//void saveOptions();
	void setEnabled(bool enabled);

	int getPriority() const;

	bool mouseDown(mouse_button button, float clickX, float clickY);
	bool mouseUp(mouse_button button, float clickX, float clickY);
	bool keyDown(keypress key, keymod mod);
	bool keyUp(keypress key, keymod mod);

	virtual void timepass();
	virtual void render();
	void setRenderScriptFunction(const std::string &renderScriptFunction);
	void renderMouseOver(bool mouseOver);

	virtual bool wantsExclusiveKeyboard() const;
	virtual bool isVisible() const;

protected:
	virtual void onEnable();
	virtual void onDisable();
	void setFocusedWidget(const WidgetPtr &newFocus);

	WidgetPtr _focusedWidget;
	typedef std::vector<WidgetPtr> WidgetPool;
	WidgetPool _widgets;
	std::string _frameName;
	std::string _renderScriptFunction;

	bool _enabled;
	int _priority;
};

#endif
