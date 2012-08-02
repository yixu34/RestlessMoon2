#include <cctype>

#include "common.hpp"
#include "widget.hpp"
#include "graphics.hpp"
#include "util.hpp"
#include "gameapp.hpp"
#include "scripting.hpp"
#include "input.hpp"
#include "console.hpp"

int Widget::nextId = 0;

Widget::Widget(
	float relX, 
	float relY, 
	float relWidth, 
	float relHeight)
{
	_relX      = relX;
	_relY      = relY;
	_relWidth  = relWidth;
	_relHeight = relHeight;

	_shouldRemove = false;

	onChangingResolution();
	setKeyFocus(false);

	_id = nextId++;
}

Widget::~Widget()
{
}

void Widget::remove()
{
	_shouldRemove = true;
}

bool Widget::shouldRemove() const
{
	return _shouldRemove;
}

bool Widget::keyDown(keypress key, keymod mod)
{
	return false;
}

bool Widget::keyUp(keypress key, keymod mod)
{
	return false;
}

bool Widget::mouseDown(mouse_button button, float clickX, float clickY)
{
	return false;
}

bool Widget::mouseUp(mouse_button button, float clickX, float clickY)
{
	return false;
}

void Widget::setKeyFocus(bool keyFocus)
{
	_keyFocus = keyFocus;
}

bool Widget::hasKeyFocus() const
{
	return _keyFocus;
}

void Widget::onChangingResolution()
{
	graphics->toAbsoluteCoords(_relX, _relY, _absX, _absY);
	graphics->toAbsoluteCoords(_relWidth, _relHeight, _absWidth, _absHeight);
}

bool Widget::containsPoint(float screenX, float screenY) const
{
	return util::rectangleContainsPoint(
		screenX, screenY, _absX, _absY, _absWidth, _absHeight);
}

//void Widget::onLeavingFrame()
//{
//	// EMPTY
//}

int Widget::getId() const
{
	return _id;
}

void Widget::saveOptions()
{
	// EMPTY
}

//////////////////////////////////////////////////////////////////////////

SpriteWidget::SpriteWidget(
		float relX, 
		float relY, 
		float relWidth, 
		float relHeight,
		float startU,
		float startV,
		float endU,
		float endV, 
		const std::string &actionString): Widget(relX, relY, relWidth, relHeight)
{
	_actionString = actionString;

	_screenSpaceQuad = new float[20];

	// Top Left Vertex
	_screenSpaceQuad[0] = (2*relX)-1;
	_screenSpaceQuad[1] = 1-(2*relY);
	_screenSpaceQuad[2] = 0;
	_screenSpaceQuad[3] = startU;
	_screenSpaceQuad[4] = startV;

	// Top Right Vertex
	_screenSpaceQuad[5] = (2*(relX+relWidth))-1;
	_screenSpaceQuad[6] = 1-(2*relY);
	_screenSpaceQuad[7] = 0;
	_screenSpaceQuad[8] = endU;
	_screenSpaceQuad[9] = startV;
	
	// Bottom Right Vertex
	_screenSpaceQuad[10] = (2*(relX+relWidth))-1;
	_screenSpaceQuad[11] = 1-(2*(relY+relHeight));
	_screenSpaceQuad[12] = 0;
	_screenSpaceQuad[13] = endU;
	_screenSpaceQuad[14] = endV;

	// Bottom Left Vertex
	_screenSpaceQuad[15] = (2*relX)-1;
	_screenSpaceQuad[16] = 1-(2*(relY+relHeight));
	_screenSpaceQuad[17] = 0;
	_screenSpaceQuad[18] = startU;
	_screenSpaceQuad[19] = endV;
}

SpriteWidget::~SpriteWidget()
{
	delete [] _screenSpaceQuad;
	_screenSpaceQuad = 0;
}

void SpriteWidget::render()
{
	graphics->getDevice()->DrawPrimitiveUP(D3DPT_TRIANGLEFAN,2,(void*)_screenSpaceQuad,5*sizeof(float));
}

void SpriteWidget::onChangingResolution()
{
	Widget::onChangingResolution();
}

bool SpriteWidget::mouseDown(mouse_button button, float clickX, float clickY)
{
	if (_actionString != "")
	{
		scripting->executeString(_actionString);
		return true;
	}
	else
		return false;
}


//////////////////////////////////////////////////////////////////////////

TextLabel::TextLabel(
	const std::string &text, 
	float relX, 
	float relY, 
	bool centered, 
	const std::string &actionString, 
	const TextParams &textParams)
	: Widget(relX, relY, 0, 0)
{
	_textParams   = &textParams;
	_text         = text;
	_centered     = centered;
	_actionString = actionString;

	graphics->calculateTextDimensions(_text, _absWidth, _absHeight);

	if (_centered)
		_absX -= _absWidth / 2;
}

TextLabel::~TextLabel()
{
}

void TextLabel::render()
{

	// Don't tell the graphics class to center the text, because have already
	// pre-centered it.
	graphics->drawTextAbs(_absX, _absY, _text.c_str(), false, *_textParams);
}

void TextLabel::onChangingResolution()
{
	graphics->toAbsoluteCoords(_relX, _relY, _absX, _absY);
}

bool TextLabel::mouseDown(mouse_button button, float clickX, float clickY)
{
	if (_actionString != "")
	{
		scripting->executeString(_actionString);
		return true;
	}
	else
		return false;
}

//////////////////////////////////////////////////////////////////////////

TextInputBox::TextInputBox(
	 float relX, 
	 float relY, 
	 float relWidth, 
	 float relHeight, 
	 const TextParams &textParams) : Widget(relX, relY, relWidth, relHeight)
{
	_textParams = &textParams;

	// Text input boxes differentiate between upper and lower case characters.
//	_listensToChars = true;
}

TextInputBox::~TextInputBox()
{
}

void TextInputBox::render()
{
	// TODO:  Draw a border around here, possibly using an image.
	graphics->drawTextAbs(
		_absX, 
		_absY, 
		_textInput.getText().c_str(), 
		false, // don't center, since we did that already.
		*_textParams);
}

bool TextInputBox::mouseDown(mouse_button button, float clickX, float clickY)
{
	return true;
}

bool TextInputBox::keyDown(keypress key, keymod mod)
{
	return _textInput.keyDown(key, mod);
}

//void TextInputBox::onLeavingFrame()
//{
//	_textInput.clear();
//}

//////////////////////////////////////////////////////////////////////////

TextLabelMenu::TextLabelMenu(
	float relX, 
	float relY, 
	const MenuItemList &menuItems, 
	const TextParams &textParams /* = fontDefault */)
	: Widget(relX, relY, 0, 0), _menuItems(menuItems)
{
	_pageNumber = 1;
	_numPages   = (menuItems.size() / numEntriesPerPage) + 
				  (menuItems.size() % numEntriesPerPage > 0 ? 1 : 0);
}

TextLabelMenu::~TextLabelMenu()
{
}

bool TextLabelMenu::keyDown(keypress key, keymod mod)
{
	if (key == '.')
	{
		_pageNumber++;
		if (_pageNumber > _numPages)
			_pageNumber = _numPages;

		return true;
	}
	else if (key == ',')
	{
		_pageNumber--;
		if (_pageNumber < 1)
			_pageNumber = 1;

		return true;
	}
	else if (std::isdigit(key))
	{
		executeSelection(key - '0');
		remove();
		return true;
	}
	else if (key == VK_ESCAPE)
	{
		remove();
		return true;
	}

	return false;
}

void TextLabelMenu::executeSelection(unsigned selection)
{
	unsigned menuIndex = ((_pageNumber - 1) * numEntriesPerPage) + selection;
	if (menuIndex < _menuItems.size())
	{
		std::string toExecute = _menuItems[menuIndex].second;
		scripting->executeString(toExecute);	
	}
}

void TextLabelMenu::render()
{
	// Print all of the menu items on the current page.
	unsigned startIndex = (_pageNumber - 1) * numEntriesPerPage;
	unsigned lastIndex  = std::min(startIndex + numEntriesPerPage, _menuItems.size());
	for (unsigned i = startIndex; i < lastIndex; i++)
	{
		std::string currString = util::toString(i % numEntriesPerPage) + " - " + 
			                     _menuItems[i].first;

		graphics->drawTextAbs(
			_absX, 
			_absY + 30 * (i % numEntriesPerPage), 
			currString,
			false);
	}
}