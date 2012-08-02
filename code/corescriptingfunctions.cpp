#include "scripting.hpp"
#include "audio.hpp"
#include "common.hpp"
#include "gameapp.hpp"
#include "console.hpp"
#include "eventmanager.hpp"
#include "event.hpp"
#include "framemanager.hpp"
#include "game.hpp"
#include "camera.hpp"
#include "cvar.hpp"

//////////////////////////////////////////////////////////////////////////
// Miscellaneous/core functions.
//////////////////////////////////////////////////////////////////////////

SCRIPTING_FUNCTION(registerCallback)
{
	std::string eventName    = scripting->getArgument<std::string>(1);
	std::string callbackName = scripting->getArgument<std::string>(2);
	scripting->registerCallbackFunction(eventName, callbackName);

	return 0;
}

SCRIPTING_FUNCTION(scriptLog)
{
	std::string text = scripting->getArgument<std::string>(1);
	log(text.c_str());

	return 0;
}

SCRIPTING_FUNCTION(scriptAssert)
{
	int toAssert = scripting->getArgument<int>(1);

	MYASSERT(toAssert);

	return 0;
}

SCRIPTING_FUNCTION(quit)
{
	gameApp->quit();

	return 0;
}

SCRIPTING_FUNCTION(startNewGame)
{
	std::string levelFile = scripting->getArgument<std::string>(1);
	int type = scripting->getArgument<int>(2);
	eventManager->raiseEvent(EventPtr(new Ev_Start_Game(levelFile, type)));
	return 0;
}

SCRIPTING_FUNCTION(toggleConsole)
{
	console->toggle();
	return 0;
}

SCRIPTING_FUNCTION(getDtMs)
{
	float dtMilliseconds = getDtMilliseconds();
	scripting->pushArgument(dtMilliseconds);
	return 1;
}

SCRIPTING_FUNCTION(getDtSec)
{
	float dtSeconds = getDtSeconds();
	scripting->pushArgument(dtSeconds);
	return 1;
}

SCRIPTING_FUNCTION(set)
{
	std::string varName  = scripting->getArgument<std::string>(1);
	std::string newValue = scripting->getArgument<std::string>(2);

	BaseCVar *cvar = cvarPool->getVar(varName);
	if (cvar != 0)
		cvar->setValueFromString(newValue);
	else
		console->write(varName + " is not a valid cvar!");

	return 0;
}

SCRIPTING_FUNCTION(setMusicVolume)
{
	audio->setMusicVolume(scripting->getArgument<float>(1));
	return 0;
}

SCRIPTING_FUNCTION(setMasterVolume)
{
	audio->setMasterVolume(scripting->getArgument<float>(1));
	return 0;
}

void registerCoreScriptingFunctions()
{
    REGISTER_SCRIPTING_FUNCTION(scriptLog);
    REGISTER_SCRIPTING_FUNCTION(scriptAssert);
    REGISTER_SCRIPTING_FUNCTION(registerCallback);
    REGISTER_SCRIPTING_FUNCTION(quit);
    REGISTER_SCRIPTING_FUNCTION(startNewGame);
    REGISTER_SCRIPTING_FUNCTION(toggleConsole);

	REGISTER_SCRIPTING_FUNCTION(getDtMs);
	REGISTER_SCRIPTING_FUNCTION(getDtSec);

	REGISTER_SCRIPTING_FUNCTION(set);

	REGISTER_SCRIPTING_FUNCTION(setMusicVolume);
	REGISTER_SCRIPTING_FUNCTION(setMasterVolume);
}
