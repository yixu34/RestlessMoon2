#include "scripting.hpp"
#include "widget.hpp"
#include "menu.hpp"
#include "framemanager.hpp"
#include "frame.hpp"
#include "input.hpp"
#include "graphics.hpp"
#include "game.hpp"

//////////////////////////////////////////////////////////////////////////
// User interface functions.
//////////////////////////////////////////////////////////////////////////

SCRIPTING_FUNCTION(createFrame)
{
	std::string frameName = scripting->getArgument<std::string>(1);
	int framePriority     = scripting->getArgument<int>(2);
	frameManager->addFrame(frameName, framePriority);

	return 0;
}

SCRIPTING_FUNCTION(createTextLabelMenu)
{
	int numArgs = scripting->getNumArgs();

	std::string frameName = scripting->getArgument<std::string>(1);
	float relX            = scripting->getArgument<float>(2);
	float relY            = scripting->getArgument<float>(3);

	int numStrings = numArgs - 3;
	if (numStrings % 2 != 0)
	{
		log("Need an even number of menu item strings!");
		return 0;
	}

	int numStringPairs = numStrings / 2;
	TextLabelMenu::MenuItemList menuItems(numStringPairs);
	for (int i = 4; i < numStrings + 4; i += 2)
	{
		unsigned menuIndex     = (i - 4) / 2;
		std::string currText   = scripting->getArgument<std::string>(i);
		std::string currAction = scripting->getArgument<std::string>(i + 1);

		menuItems[menuIndex].first  = currText;
		menuItems[menuIndex].second = currAction;
	}

	WidgetPtr menu(new TextLabelMenu(relX, relY, menuItems));
	FramePtr frame = frameManager->getFrame(frameName);
	ASSERTMSG(frame, "createTextLabelMenu() - frame is NULL.");

	frame->addWidget(menu);

	return 0;
}

SCRIPTING_FUNCTION(createSpriteWidget)
{
	std::string frameName       = scripting->getArgument<std::string>(1);
	//std::string textureFilename = scripting->getArgument<std::string>(2);
	float relX                  = scripting->getArgument<float>(2);
	float relY                  = scripting->getArgument<float>(3);
	float relWidth              = scripting->getArgument<float>(4);
	float relHeight             = scripting->getArgument<float>(5);
	float startU                = scripting->getArgument<float>(6);
	float startV                = scripting->getArgument<float>(7);
	float endU                  = scripting->getArgument<float>(8);
	float endV                  = scripting->getArgument<float>(9);
	std::string actionString    = scripting->getArgument<std::string>(10);

	FramePtr menu = frameManager->getFrame(frameName);
	ASSERTMSG(menu, "createSpriteWidget() - frame is NULL");

	int widgetId = menu->addWidget(WidgetPtr(new SpriteWidget(
		relX, 
		relY, 
		relWidth, 
		relHeight,
		startU,
		startV,
		endU,
		endV, 
		actionString)));
	scripting->pushArgument(widgetId);

	return 1;
}

SCRIPTING_FUNCTION(createTextLabel)
{
	const int numArgs     = scripting->getNumArgs();
	std::string frameName = scripting->getArgument<std::string>(1);
	std::string text      = scripting->getArgument<std::string>(2);
	float relX            = scripting->getArgument<float>(3);
	float relY            = scripting->getArgument<float>(4);
	bool centered         = scripting->getArgument<bool>(5);

	std::string actionStr;
	if (numArgs > 5) actionStr = scripting->getArgument<std::string>(6);
	else             actionStr = "";

	FramePtr frame = frameManager->getFrame(frameName);
	ASSERTMSG(frame, "createTextLabel() - frame is NULL");

	int widgetId = frame->addWidget(WidgetPtr(new TextLabel(
		text, 
		relX, 
		relY, 
		centered, 
		actionStr)));
	scripting->pushArgument(widgetId);

	return 1;
}

SCRIPTING_FUNCTION(createTextInput)
{
	std::string frameName = scripting->getArgument<std::string>(1);
	float relX            = scripting->getArgument<float>(2);
	float relY            = scripting->getArgument<float>(3);
	float relWidth        = scripting->getArgument<float>(4);
	float relHeight       = scripting->getArgument<float>(5);

	FramePtr frame = frameManager->getFrame(frameName);
	ASSERTMSG(frame, "createTextInput() - frame is NULL.");

	int widgetId = frame->addWidget(WidgetPtr(new TextInputBox(
		relX, 
		relY, 
		relWidth, 
		relHeight)));
	scripting->pushArgument(widgetId);

	return 1;
}

SCRIPTING_FUNCTION(setFrameEnabled)
{
	std::string frameName = scripting->getArgument<std::string>(1);
	bool frameEnabled     = scripting->getArgument<bool>(2);

	FramePtr frame = frameManager->getFrame(frameName);
	ASSERTMSG(frame, "setFrameEnabled() - frame is NULL.");
	frame->setEnabled(frameEnabled);
	
	return 0;
}

SCRIPTING_FUNCTION(setFocusedWidget)
{
	std::string frameName = scripting->getArgument<std::string>(1);
	int widgetId          = scripting->getArgument<int>(2);

	frameManager->getFrame(frameName)->setFocusedWidget(widgetId);

	return 0;
}

SCRIPTING_FUNCTION(setRenderScriptFunction)
{
	std::string frameName      = scripting->getArgument<std::string>(1);
	std::string renderFunction = scripting->getArgument<std::string>(2);

	FramePtr frame = frameManager->getFrame(frameName);
	ASSERTMSG(frame, "setRenderScriptFunction() - frame is NULL.");
	frame->setRenderScriptFunction(renderFunction);

	return 0;
}

SCRIPTING_FUNCTION(renderMouseOver)
{
	std::string frameName = scripting->getArgument<std::string>(1);
	bool mouseOver        = scripting->getArgument<bool>(2);
	FramePtr frame = frameManager->getFrame(frameName);

	ASSERTMSG(frame, "renderMouseOver() - frame is NULL");

	frame->renderMouseOver(mouseOver);

	return 0;
}

SCRIPTING_FUNCTION(getCursorPos)
{
	float cursorX, cursorY;
	input->getMousePosition(cursorX, cursorY);
	
	cursorX = (2 * cursorX / graphics->getScreenWidth()) - 1;
	cursorY = 1-(2 * cursorY / graphics->getScreenHeight());

	scripting->pushArgument(cursorX);
	scripting->pushArgument(cursorY);

	return 2;
}

SCRIPTING_FUNCTION(processLoadGame)
{
    WIN32_FIND_DATA entry;
	float yOffset = 0.35f;
	const std::string frameName = scripting->getArgument<std::string>(1);
	const std::string exceStringFirst = "createTextLabel(\"" + frameName  + "\", ";
	std::string execString;
    HANDLE handle = FindFirstFile("lev/*.lev", &entry);
    if (handle == INVALID_HANDLE_VALUE) 
		assert(false);
	std::string fileName = entry.cFileName;
	execString = exceStringFirst + "\"" + fileName.substr(0,fileName.length()-4) + "\"" + ", 0.65, " + util::toString(yOffset) 
										+ ", 1, 'startNewGame(\"lev/" + fileName + "\", 1)')";
	yOffset += 0.045;
	scripting->executeString(execString);

    while(FindNextFile(handle,&entry))
    {
        fileName = entry.cFileName;
		execString = exceStringFirst + "\"" + fileName.substr(0,fileName.length()-4) + "\"" + ", 0.65, " + util::toString(yOffset) 
										+ ", 1, 'startNewGame(\"lev/" + fileName + "\", 1)')";
		yOffset += 0.045;
		scripting->executeString(execString);
    }
    FindClose(handle);
	std::string returnScreen = game->isGameInProgress() ? "continue_game" : "main_menu";
	std::string final = exceStringFirst +  "\"Back\", 0.65, " + util::toString(yOffset) 
							+ ", 1, 'setFrameEnabled(\"load_game\", 0); setFrameEnabled(getMainMenuScreen(), 1)')";

 	scripting->executeString(final);
	return 0;
}

SCRIPTING_FUNCTION(getMainMenuScreen)
{
	scripting->pushArgument<std::string>(game->isGameInProgress() ? "continue_game" : "main_menu");
	return 1;
}

void registerUIScriptingFunctions()
{
	// UI functions.
	REGISTER_SCRIPTING_FUNCTION(createFrame);
	REGISTER_SCRIPTING_FUNCTION(createTextLabelMenu);
	REGISTER_SCRIPTING_FUNCTION(createSpriteWidget);
	REGISTER_SCRIPTING_FUNCTION(createTextLabel);
	REGISTER_SCRIPTING_FUNCTION(createTextInput);
	REGISTER_SCRIPTING_FUNCTION(setFrameEnabled);
	REGISTER_SCRIPTING_FUNCTION(setFocusedWidget);
	REGISTER_SCRIPTING_FUNCTION(setRenderScriptFunction);

	REGISTER_SCRIPTING_FUNCTION(renderMouseOver);
	REGISTER_SCRIPTING_FUNCTION(getCursorPos);
	REGISTER_SCRIPTING_FUNCTION(processLoadGame);

	REGISTER_SCRIPTING_FUNCTION(getMainMenuScreen);
}
