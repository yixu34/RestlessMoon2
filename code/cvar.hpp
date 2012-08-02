#ifndef CVAR_HPP
#define CVAR_HPP

#include <string>
#include <map>

#include "util.hpp"

class BaseCVar;

class CVarPool
{
public:
	CVarPool();
	~CVarPool();

	void registerVar(const std::string &name, BaseCVar *cvar);
	BaseCVar *getVar(const std::string &name) const;

private:
	typedef std::map<
		std::string, 
		BaseCVar *, 
		util::CaseInsensitiveLess> CVarMap;
	CVarMap _cvars;
};

extern CVarPool *cvarPool;

//////////////////////////////////////////////////////////////////////////

class BaseCVar
{
protected:
	BaseCVar()  {};
	virtual ~BaseCVar() {}; 

public:
	virtual void setValueFromString(const std::string &valueStr) = 0;
};

template <typename T>
class CVar : public BaseCVar
{
public:
	CVar(const std::string &name, const T &initialValue = T())
		: _value(initialValue)
	{
		if (cvarPool == 0)
			cvarPool = new CVarPool();

		cvarPool->registerVar(name, this);
	}
	~CVar() {};

	inline operator T() { return _value; }
	inline void operator=(const T &other) { _value = other.value; }

	void setValueFromString(const std::string &valueStr);

private:
	T _value;
};

#endif