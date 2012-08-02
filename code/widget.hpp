#ifndef WIDGET_HPP
#define WIDGET_HPP

//
// Represents any user interface related screen element.
// (e.g. menu buttons, text labels, HUD elements, etc.)
//

#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>

#include "inputhandler.hpp"
#include "event.hpp"
#include "sprite.hpp"
#include "text.hpp"
#include "textinput.hpp"

class Widget;
typedef boost::shared_ptr<Widget> WidgetPtr;

class Widget : public InputHandler
{
public:
	Widget(
		float relX, 
		float relY, 
		float relWidth, 
		float relHeight);

	virtual ~Widget();

	bool keyDown(keypress key, keymod mod);
	bool keyUp(keypress key, keymod mod);
	bool mouseDown(mouse_button button, float clickX, float clickY);
	bool mouseUp(mouse_button button, float clickX, float clickY);

	void remove();
	bool shouldRemove() const;

	void setKeyFocus(bool keyFocus);
	bool hasKeyFocus() const;
	bool containsPoint(float screenX, float screenY) const;

	virtual void render() = 0;
	virtual void onChangingResolution();
	//virtual void onLeavingFrame();

	// TODO PLACEHOLDER
	// This gets called when you click OK on a menu to save all of the options
	// that you changed in that menu screen.
	virtual void saveOptions();  

	int getId() const;

protected:
	float _absX, _absY;
	float _relX, _relY;

	float _absWidth, _absHeight;
	float _relWidth, _relHeight;

	bool _keyFocus;
	bool _shouldRemove;
	int _id;

private:
	static int nextId;
};

//////////////////////////////////////////////////////////////////////////

// Represents a 2D image.
class SpriteWidget : public Widget
{
public:
	SpriteWidget(
		float relX, 
		float relY, 
		float relWidth, 
		float relHeight,
		float startU,
		float startV,
		float endU,
		float endV, 
		const std::string &actionString);

	~SpriteWidget();

	void render();
	void onChangingResolution();
	bool mouseDown(mouse_button button, float clickX, float clickY);

private:
	std::string _actionString;
	float *_screenSpaceQuad;
};

//////////////////////////////////////////////////////////////////////////

// Static text label.  Doubles as a button.
class TextLabel : public Widget
{
public:
	TextLabel(
		const std::string &text, 
		float relX, 
		float relY, 
		bool centered, 
		const std::string &actionString, 
		const TextParams &textParams = fontDefault);

	~TextLabel();

	bool mouseDown(mouse_button button, float clickX, float clickY);

	void render();
	void onChangingResolution();

private:
	//void calculateFontDimensions();

	std::string _text;
	std::string _actionString;
	const TextParams *_textParams;
	bool _centered;
};

//////////////////////////////////////////////////////////////////////////

// A box into which you can type text.
class TextInputBox : public Widget
{
public:
	TextInputBox(
		float relX, 
		float relY, 
		float relWidth, 
		float relHeight, 
		const TextParams &textParams = fontDefault);

	~TextInputBox();

	bool mouseDown(mouse_button button, float clickX, float clickY);
	bool keyDown(keypress key, keymod mod);

	void render();
	//void onLeavingFrame();

private:
	TextInput         _textInput;
	const TextParams *_textParams;
};

//////////////////////////////////////////////////////////////////////////

class TextLabelMenu : public Widget
{
public:
	typedef std::vector<std::pair<std::string, std::string> > MenuItemList;

	TextLabelMenu(
		float relX, 
		float relY, 
		const MenuItemList &menuItems, 
		const TextParams &textParams = fontDefault);

	~TextLabelMenu();

	bool keyDown(keypress key, keymod mod);

	void render();

private:
	void executeSelection(unsigned selection);

	MenuItemList _menuItems;

	static const unsigned numEntriesPerPage = 10;

	unsigned _pageNumber;
	unsigned _numPages;
};

#endif
