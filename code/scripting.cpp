#include <boost/lexical_cast.hpp>

#include "scripting.hpp"
#include "event.hpp"
#include "eventmanager.hpp"
#include "command.hpp"

ScriptingPtr scripting;

Scripting::Scripting()
{
	_luaVM = 0;
}

Scripting::~Scripting()
{
	if (_luaVM != 0)
		shutdown();
}

void Scripting::registerHandlers()
{
	registerHandler(
		EventType(Ev_Test::eventName), 
		EventHandlerFunctorPtr(new BaseEventHandlerFunctor<Scripting>(this, &Scripting::onCallback)));
}

bool Scripting::initialize()
{
	_luaVM = lua_open();

	luaopen_base(_luaVM);
	//luaopen_io(_luaVM);
	luaopen_table(_luaVM);
	luaopen_math(_luaVM);
	luaopen_string(_luaVM);

	return true;
}

void Scripting::shutdown()
{
	lua_close(_luaVM);
	_luaVM = 0;
}

void Scripting::executeFile(const std::string &filename)
{
	luaL_dofile(_luaVM, filename.c_str());
}

void Scripting::executeString(const std::string &str)
{
	luaL_dostring(_luaVM, str.c_str());
}

void Scripting::executeCommand(const Command &command)
{
	luaL_dostring(_luaVM, command.getString().c_str());
}

//void Scripting::executeFunction(
//	ScriptingFunction function, 
//	int numArgs, 
//	int numResults)
//{
//	lua_pushcfunction(_luaVM, function);
//	lua_pcall(_luaVM, numArgs, numResults, 0);
//}

int Scripting::calculateStackIndex(int argPosition, int numArgs)
{
	return argPosition - numArgs - 1;
}

int Scripting::setActiveTable(const std::string &tableName)
{
	lua_getglobal(_luaVM, tableName.c_str());
	if (!lua_istable(_luaVM, -1))
	{
		ERRORMSG("This is not a table!");
		return 0;
	}

	// Return the size of the table.
	return luaL_getn(_luaVM, -1);
}

void Scripting::popArguments(int numArgsToPop)
{
	lua_pop(_luaVM, numArgsToPop);
}

int Scripting::getNumArgs()
{
	return lua_gettop(_luaVM);
}

void Scripting::registerFunction(
	const std::string &functionName, 
	ScriptingFunction function)
{
	lua_register(_luaVM, functionName.c_str(), function);
}

void Scripting::registerCallbackFunction(
	const std::string &eventName, 
	const std::string &callbackName)
{
	// Put in a dummy list of callbacks if we haven't seen this event before.
	if (_callbackMap.find(eventName) == _callbackMap.end())
		_callbackMap[eventName] = CallbackFunctionPool();

	_callbackMap[eventName].insert(callbackName);
}

PacketPtr Scripting::writeToPacket(int numArgs, int startIndex)
{
	// Too few arguments, return nothing.
	if (numArgs <= 0)
		return PacketPtr();

	PacketPtr packet(new Packet());
	const int endIndex = numArgs;
	for (int i = startIndex; i <= endIndex; i++)
	{
		// XXX:  Test if the argument is a number first, because numbers 
		// always count as strings.
		if (lua_isnumber(_luaVM, i))
		{
			// Check if the argument is an int.
			float number = getArgument<float>(i);
			try {
				int numAsInt = boost::lexical_cast<int>(number);
				packet->putInt(numAsInt);  // doesn't get called if the cast fails
			}
			catch (boost::bad_lexical_cast &) {
				packet->putFloat(number);
			}
		}
		else if (lua_isstring(_luaVM, i))
		{
			std::string str = getArgument<std::string>(i);
			packet->putString(str);
		}
		else
		{
			ERRORMSG("Invalid packet type.");
			return PacketPtr();  // equivalent to returning NULL.
		}
	}

	return packet;
}

bool Scripting::onCallback(const Event &event)
{
	const std::string &eventName = event.getType().getString();

	// If this event type isn't registered, then ignore it.
	if (_callbackMap.find(eventName) == _callbackMap.end())
		return false;

	// Otherwise, look up all associated callback functions and call them.
	CallbackFunctionPool callbacks = _callbackMap[eventName];
	for (CallbackFunctionPool::iterator it = callbacks.begin();
		 it != callbacks.end(); it++)
	{
		const std::string &currCallback = *it;
		executeCallbackFunction(currCallback, event);
	}

	return true;
}

void Scripting::executeCallbackFunction(
	const std::string &callbackName, 
	const Event &event)
{
	lua_getglobal(_luaVM, callbackName.c_str());

	// Push all of the event arguments onto the stack.
	EventArgumentList eventArgs = event.getData()->getArguments();
	for (EventArgumentList::iterator it = eventArgs.begin();
		 it != eventArgs.end(); it++)
	{
		std::string currArg = it->first;
		bool isNumber       = it->second;

		if (isNumber)
			lua_pushnumber(_luaVM, std::atof(currArg.c_str()));
		else
			lua_pushstring(_luaVM, currArg.c_str());
	}

	lua_pcall(_luaVM, eventArgs.size(), LUA_MULTRET, 0);
}

void Scripting::pushNil()
{
	lua_pushnil(_luaVM);
}

//////////////////////////////////////////////////////////////////////////

