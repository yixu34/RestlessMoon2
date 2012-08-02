#ifndef EVENT_HPP
#define EVENT_HPP

//
// Base type for an event that can be signaled during the game.
// An event consists of a type and _data, which are stored in separate classes.
//

#include <boost/shared_ptr.hpp>
#include <boost/optional/optional_fwd.hpp>
#include <string>

#include "eventtype.hpp"
#include "eventdata.hpp"
#include "timer.hpp"
#include "entity.hpp"
#include "packetconstructable.hpp"
#include <string>

class Event;
typedef boost::shared_ptr<Event> EventPtr;

class Event
{
public:
	Event(
		const std::string &type, 
		const EventDataPtr &eventData);

	virtual ~Event() = 0;

	const EventType &getType() const;
	float getTimeCreated() const;
	EventDataPtr getData() const;

	bool operator<(const Event &rhs) const;

	template<typename T>
	T *getRawData() const
	{	return (reinterpret_cast<T *>(_eventData.get())); }

protected:
	EventType _type;
	float _timeCreated;
	EventDataPtr _eventData;
};

//////////////////////////////////////////////////////////////////////////

class Ev_Test : public Event
{
public:
	Ev_Test(float x, float y)
		: Event(eventName, EventDataPtr(new EvData_Test(x, y)))
	{}

	static const char *const eventName;
};

//////////////////////////////////////////////////////////////////////////

class Ev_Play_Sound : public Event
{
public:
	Ev_Play_Sound(
		const std::string &soundToPlay, 
		const boost::optional<D3DXVECTOR3> &position, 
		bool headRelative)
		: Event(eventName, EventDataPtr(new EvPlaySound_data(soundToPlay, position, headRelative)))
	{}

	static const char *const eventName;
};

//////////////////////////////////////////////////////////////////////////

class Ev_Play_Music : public Event
{
public:
	Ev_Play_Music(const std::string &musicFile, bool interupt)
		: Event(eventName, EventDataPtr(new EvPlayMusic_data(musicFile, interupt)))
	{}

	static const char *const eventName;
};

//////////////////////////////////////////////////////////////////////////

class Ev_Entity_Die : public Event
{
public:
	Ev_Entity_Die(
		const std::string &deathSound, 
		Entity* entityToDie, 
		const std::string &entityName, 
		const D3DXVECTOR3 &deathPosition, 
		Entity::death_mode deathMode)
		: Event(eventName, EventDataPtr(new EvEntityDie_data(
											deathSound, 
											entityToDie, 
											entityName,
											deathPosition, 
											deathMode)))
	{}

	static const char *const eventName;
};

//////////////////////////////////////////////////////////////////////////

class Ev_Entity_Hit : public Event
{
public:
	Ev_Entity_Hit(
		const D3DXVECTOR3 &projectilePosition, 
		const D3DXVECTOR3 &projectileVelocity, 
		Entity::on_hit_mode onHitMode)
	    : Event(eventName, EventDataPtr(new EvEntityHit_data(
	                                              projectilePosition, 
			                                      projectileVelocity, 
												  onHitMode)))
	{}

	static const char *const eventName;
};


//////////////////////////////////////////////////////////////////////////

class Ev_Start_Game : public Event
{
public:
	Ev_Start_Game(const std::string &mapName, int type)
		: Event(eventName, EventDataPtr(new EvMapLoad_data(mapName, type)))
	{}

	static const char *const eventName;
};

//////////////////////////////////////////////////////////////////////////

class Ev_Shake_Screen : public Event
{
public:
	Ev_Shake_Screen(float shakeStrength, float shakeTime)
		: Event(eventName, EventDataPtr(new EvShakeScreen_data(shakeStrength, shakeTime)))
	{}

	static const char *const eventName;
};

#endif
