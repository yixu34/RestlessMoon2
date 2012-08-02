#ifndef EVENTMANAGER_HPP
#define EVENTMANAGER_HPP

//
// Keeps track of all events in game, and forwards them to the appropriate
// event listeners.
//

#include <boost/shared_ptr.hpp>
#include <set>
#include <map>
#include <list>

#include "eventtype.hpp"

class Event;
class EventListener;
class EventManager;

typedef boost::shared_ptr<Event>         EventPtr;
typedef boost::shared_ptr<EventListener> EventListenerPtr;
typedef boost::shared_ptr<EventManager>  EventManagerPtr;

void registerEvents();

class EventManager
{
public:
	EventManager();
	~EventManager();

	void addListener(
		EventListener *listenerToAdd, 
		const EventType &type);

	void removeListener(
		EventListener *listenerToRemove, 
		const EventType &type);

	void raiseEvent(const EventPtr &event);

	void timepass();

	void clear();

private:
	void removeListeners();

	// A collection of all known unique event types.
	typedef std::set<EventType> EventSet;
	EventSet _knownEventTypes;

	// The result of inserting a new event type to the set of known types.
	typedef std::pair<EventSet::iterator, bool> EventSetInsertResult;

	// Mapping from event id to a list of event listeners.
	typedef std::list<EventListener *> EventListenerList;
	typedef std::map<int, EventListenerList> EventListenerMap;
	EventListenerMap _listenerRegistry;

	// List of event listeners that are scheduled for removal.
	typedef std::pair<EventListener *, EventType> EventListenerRemoval;
	typedef std::list<EventListenerRemoval> EventListenerRemovalList;
	EventListenerRemovalList _listenersToRemove;

	// Enqueued events that the manager will process during the next frame.
	typedef std::list<EventPtr> EventQueue;
	EventQueue _eventsToProcess;
};

extern EventManagerPtr eventManager;

#endif
