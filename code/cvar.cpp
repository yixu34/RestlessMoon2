#include <boost/lexical_cast.hpp>

#include "cvar.hpp"
#include "common.hpp"
#include "console.hpp"

CVarPool *cvarPool = 0;

CVarPool::CVarPool()
{
}

CVarPool::~CVarPool()
{
}

void CVarPool::registerVar(const std::string &name, BaseCVar *cvar)
{
	_cvars[name] = cvar;
}

BaseCVar *CVarPool::getVar(const std::string &name) const
{
	CVarMap::const_iterator it = _cvars.find(name);
	if (it != _cvars.end())
		return it->second;
	else
		return 0;
}

//////////////////////////////////////////////////////////////////////////

void CVar<bool>::setValueFromString(const std::string &valueStr)
{
	const char *str = valueStr.c_str();
	if (!strcasecmp(str, "true"))
		_value = true;
	else if (!strcasecmp(str, "false"))
		_value = false;
	else
	{
		try {
			int valAsInt = boost::lexical_cast<int>(valueStr);
			_value = (valAsInt == 0) ? false : true;
		} catch (boost::bad_lexical_cast &) {
			std::stringstream errorMsg;
			errorMsg<<"Cannot set cvar to "<<valueStr;
			console->write(errorMsg.str());
		}
	}
}

void CVar<int>::setValueFromString(const std::string &valueStr)
{
	try {
		int valAsInt = boost::lexical_cast<int>(valueStr);
		_value = valAsInt;
	} catch (boost::bad_lexical_cast &) {
		std::stringstream errorMsg;
		errorMsg<<"Cannot set cvar to "<<valueStr;
		console->write(errorMsg.str());
	}
}

void CVar<float>::setValueFromString(const std::string &valueStr)
{
	try {
		float valAsFloat = boost::lexical_cast<float>(valueStr);
		_value = valAsFloat;
	} catch (boost::bad_lexical_cast &) {
		std::stringstream errorMsg;
		errorMsg<<"Cannot set cvar to "<<valueStr;
		console->write(errorMsg.str());
	}
}

void CVar<std::string>::setValueFromString(const std::string &valueStr)
{
	_value = valueStr;
}