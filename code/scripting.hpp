#ifndef SCRIPTING_HPP
#define SCRIPTING_HPP

#include <string>
#include <map>
#include <set>
#include <boost/shared_ptr.hpp>

#include "lua.hpp"
#include "util.hpp"
#include "eventlistener.hpp"
#include "packet.hpp"
#include "common.hpp"

class Scripting;
typedef boost::shared_ptr<Scripting> ScriptingPtr;
typedef int (*ScriptingFunction)(lua_State *);

//////////////////////////////////////////////////////////////////////////

void registerScriptingFunctions();

class Command;

class Scripting : public EventListener
{
public:
	Scripting();
	~Scripting();

	void registerHandlers();

	bool initialize();
	void shutdown();

	// WARNING:  DON'T CALL THESE FUNCTIONS -INSIDE- OF A SCRIPTING
	// FUNCTION!!!  THE GAME WILL PROBABLY CRASH.
	void executeString(const std::string &str);
	void executeCommand(const Command &command);
	void executeFile(const std::string &filename);
	//void executeFunction(
	//	ScriptingFunction function, 
	//	int numArgs, 
	//	int numResults);

	int setActiveTable(const std::string &tableName);

	/*template<typename T>
	T getField(const std::string &fieldName);*/

	template<typename T>
	T getField(int index);

	int getNumArgs();

	template<typename T>
	T getArgument(int argPosition);

	template<typename T>
	void pushArgument(const T &argument);
	void pushNil();
	void popArguments(int numArgsToPop);
	
	template<typename T>
	T getGlobal(const std::string &globalName);

	void registerFunction(
		const std::string &functionName, 
		ScriptingFunction function);

	void registerCallbackFunction(
		const std::string &eventName, 
		const std::string &callbackName);

	// Given a number of arguments on the stack, this will go through each
	// one and stuff it into the packet.
	PacketPtr writeToPacket(int numArgs, int startIndex = 1);

private:
	typedef std::set<
		std::string, 
		util::CaseInsensitiveLess> CallbackFunctionPool;
	typedef std::map<
		std::string, 
		CallbackFunctionPool, 
		util::CaseInsensitiveLess> CallbackMap;
	CallbackMap _callbackMap;

	static int calculateStackIndex(int argPosition, int numArgs);

	bool onCallback(const Event &event);

	void executeCallbackFunction(
		const std::string &callbackName, 
		const Event &event);

	lua_State *_luaVM;
};

extern ScriptingPtr scripting;

//////////////////////////////////////////////////////////////////////////

////
//// XXX:  Assumes that the table is on the top of the stack!!!
////
//template <typename T>
//inline T Scripting::getField(const std::string &fieldName)
//{
//	lua_pushstring(_luaVM, fieldName.c_str());
//	lua_gettable(_luaVM, -2); // retrieve the field
//	if (!lua_isnumber(_luaVM, -1))
//		ERRORMSG("Scripting::getField() - this is not a number!");
//		
//	T result = static_cast<T>(lua_tonumber(_luaVM, -1));
//	lua_pop(_luaVM, 1);
//	return result;
//}
//
////
//// XXX:  Assumes that the table is on the top of the stack!!!
////
//template <>
//inline std::string Scripting::getField<std::string>(const std::string &fieldName)
//{
//	lua_pushstring(_luaVM, fieldName.c_str());
//	lua_gettable(_luaVM, -2); // retrieve the field
//	if (!lua_isstring(_luaVM, -1))
//		ERRORMSG("Scripting::getField() - this is not a string!");
//
//	std::string result = lua_tostring(_luaVM, -1);
//	lua_pop(_luaVM, 1);
//	return result;
//}

//
// XXX:  Assumes that the table is on the top of the stack!!!
//
template <typename T>
inline T Scripting::getField(int index)
{
	lua_pushnumber(_luaVM, index);
	lua_gettable(_luaVM, -2);
	if (!lua_isnumber(_luaVM, -1))
		ERRORMSG("Scripting::getField() - this is not a number!");

	T result = static_cast<T>(lua_tonumber(_luaVM, -1));
	lua_pop(_luaVM, 1);
	return result;
}

//
// XXX:  Assumes that the table is on the top of the stack!!!
//
template <>
inline std::string Scripting::getField<std::string>(int index)
{
	lua_pushnumber(_luaVM, index);
	lua_gettable(_luaVM, -2);
	if (!lua_isstring(_luaVM, -1))
		ERRORMSG("Scripting::getField() - this is not a string!");

	std::string result = lua_tostring(_luaVM, -1);
	lua_pop(_luaVM, 1);
	return result;
}

//////////////////////////////////////////////////////////////////////////

template <typename T>
inline T Scripting::getArgument(int argPosition)
{
	int numArgs = getNumArgs();
	if (numArgs <= 0)
		return 0;

	int stackIndex = calculateStackIndex(argPosition, numArgs);
	if (!lua_isnumber(_luaVM, stackIndex))
		ERRORMSG("Scripting::getArgument() - this is not a number!");

	return static_cast<T>(lua_tonumber(_luaVM, stackIndex));
}

template <>
inline std::string Scripting::getArgument<std::string>(int argPosition)
{
	int numArgs = getNumArgs();
	if (numArgs <= 0)
		return "";

	int stackIndex = calculateStackIndex(argPosition, numArgs);
	if (!lua_isstring(_luaVM, stackIndex))
		ERRORMSG("Scripting::getArgument() - this is not a string!");

	return lua_tostring(_luaVM, stackIndex);
}

//////////////////////////////////////////////////////////////////////////

template <typename T>
inline void Scripting::pushArgument(const T &argument)
{
	lua_pushnumber(_luaVM, argument);
}

template <>
inline void Scripting::pushArgument(const std::string &argument)
{
	lua_pushstring(_luaVM, argument.c_str());
}

//////////////////////////////////////////////////////////////////////////

template <typename T>
inline T Scripting::getGlobal(const std::string &globalName)
{
	lua_getglobal(_luaVM, globalName.c_str());
	T ret = static_cast<T>(lua_tonumber(_luaVM, -1));
	lua_remove(_luaVM, -1);
	return ret;
}

template <>
inline std::string Scripting::getGlobal<std::string>(const std::string &globalName)
{
	lua_getglobal(_luaVM, globalName.c_str());
	std::string ret = lua_tostring(_luaVM, -1);
	lua_remove(_luaVM, -1);
	return ret;
}

//////////////////////////////////////////////////////////////////////////

//
// Macros for exposing engine functions to script files.
//
#define SCRIPTING_FUNCTION(function_name) \
	int function_name(lua_State *luaVM)

#define REGISTER_SCRIPTING_FUNCTION(function_name) \
    scripting->registerFunction(#function_name, function_name);

#endif
