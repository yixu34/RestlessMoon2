#include "event.hpp"
#include "eventtype.hpp"
#include "timer.hpp"
#include <boost/optional.hpp>

//
// Static event type strings go here!
//
const char *const Ev_Test::eventName         = "ev_test";
const char *const Ev_Play_Sound::eventName   = "ev_play_sound";
const char *const Ev_Play_Music::eventName   = "ev_play_music";
const char *const Ev_Entity_Die::eventName   = "ev_entity_die";
const char *const Ev_Start_Game::eventName   = "ev_start_game";
const char *const Ev_Entity_Hit::eventName   = "ev_entity_hit";
const char *const Ev_Shake_Screen::eventName = "ev_shake_screen";

//////////////////////////////////////////////////////////////////////////

Event::Event(
	const std::string &type, 
	const EventDataPtr &eventData) : _type(type)
{
	_timeCreated = getTimeMilliseconds();
	_eventData   = eventData;
}

Event::~Event()
{
}

const EventType &Event::getType() const
{
	return _type;
}

float Event::getTimeCreated() const
{
	return _timeCreated;
}

EventDataPtr Event::getData() const
{
	return _eventData;
}

bool Event::operator<(const Event &rhs) const
{
	return (getType() < rhs.getType());
}

