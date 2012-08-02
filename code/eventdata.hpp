#ifndef EVENTDATA_HPP
#define EVENTDATA_HPP

#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>
#include <string>
#include <vector>

#include "entity.hpp"
#include "util.hpp"

//
// Represents the low-level _data and types that compose an event.
//

class EventData;
typedef boost::shared_ptr<EventData> EventDataPtr;

typedef std::pair<std::string, bool> EventArgument;
typedef std::vector<EventArgument> EventArgumentList;

// Used to convert event _data members to strings, to be used as arguments
// to a scripting callback function.
inline EventArgument convertArgument(const std::string &arg)
{
	return std::make_pair(arg, false);
}

inline EventArgument convertArgument(double arg)
{
	return std::make_pair(util::toString(arg), true);
}

class EventData
{
public:
	EventData() {};
	virtual ~EventData() = 0 {};

	virtual EventArgumentList getArguments() const;
};

//////////////////////////////////////////////////////////////////////////

class EvData_Test : public EventData
{
public:
	EvData_Test(float x, float y);

	EventArgumentList getArguments() const;

	float _x;
	float _y;
};

//////////////////////////////////////////////////////////////////////////

class EvPlaySound_data : public EventData
{
public:
	EvPlaySound_data(
		const std::string &soundToPlay, 
		const boost::optional<D3DXVECTOR3> &position,
		bool headRelative);

	std::string _soundToPlay;
	boost::optional<D3DXVECTOR3> _position;
	bool _headRelative;
};

//////////////////////////////////////////////////////////////////////////

class EvPlayMusic_data : public EventData
{
public:
	EvPlayMusic_data(const std::string &musicFile, bool interupt);
	std::string _musicFile;
	bool _interupt;
};

//////////////////////////////////////////////////////////////////////////

class EvEntityDie_data : public EventData
{
public:
	EvEntityDie_data(
		const std::string &deathSound, 
		Entity* entityToDie, 
		const std::string &entityName,
		const D3DXVECTOR3 &deathPosition, 
		Entity::death_mode deathMode);

	std::string _soundToPlay;
	std::string _entityName;
	Entity* _entityToDie; 
	D3DXVECTOR3 _deathPosition;
	Entity::death_mode _deathMode;
};

//////////////////////////////////////////////////////////////////////////

class EvEntityHit_data : public EventData
{
public:
	EvEntityHit_data(
		const D3DXVECTOR3 &projectilePosition,
		const D3DXVECTOR3 &projectileVelocity, 
		Entity::on_hit_mode onHitMode);

	D3DXVECTOR3 _projectilePosition;
	D3DXVECTOR3 _projectileVelocity;
	Entity::on_hit_mode _onHitMode;
};

//////////////////////////////////////////////////////////////////////////

class EvMapLoad_data : public EventData
{
public:
	EvMapLoad_data(const std::string &mapName, int type);

	std::string _mapName;
	int _type;
};

//////////////////////////////////////////////////////////////////////////

class EvShakeScreen_data : public EventData
{
public:
	EvShakeScreen_data(float shakeStrength, float shakeTime);

	float _shakeStrength;
	float _shakeTime;
};

#endif