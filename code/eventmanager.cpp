#include "common.hpp"
#include "event.hpp"
#include "eventlistener.hpp"
#include "eventmanager.hpp"

EventManagerPtr eventManager;

EventManager::EventManager()
{
}

EventManager::~EventManager()
{
}

void EventManager::addListener(
	EventListener *listenerToAdd, 
	const EventType &type)
{
	// If we haven't seen this event before, then add it to the set.
	if (_knownEventTypes.find(type) == _knownEventTypes.end())
	{
		// Make sure that the insertion succeeded!
		EventSetInsertResult insertSucceeded = _knownEventTypes.insert(type);
		if (!insertSucceeded.second)
		{
			ERRORMSG("Event type insertion failure.");
			return;
		}
	}

	// Check if there are already any listeners of this type.
	EventListenerList &listeners = _listenerRegistry[type.getId()];
	for (EventListenerList::iterator it = listeners.begin();
		 it != listeners.end(); it++)
	{
		// The listener that we are trying to add already exists, 
		// so don't add it.
		if (listenerToAdd == *it)
			return;
	}

	// We haven't found a matching listener for this event type, so we'll 
	// add the new listener now.
	listeners.push_back(listenerToAdd);
}

void EventManager::removeListener(
	EventListener *listenerToRemove, 
	const EventType &type)
{
	_listenersToRemove.push_back(std::make_pair(listenerToRemove, type));
}

void EventManager::removeListeners()
{
	for (EventListenerRemovalList::iterator it = _listenersToRemove.begin();
		 it != _listenersToRemove.end(); it++)
	{
		EventListener *listenerToRemove = it->first;
		const EventType &type           = it->second;

		// If we haven't seen this event type before, then something
		// must have gone wrong...
		if (_knownEventTypes.find(type) == _knownEventTypes.end())
		{
			ERRORMSG("Unknown event type!");
			return;
		}

		EventListenerList &listeners = _listenerRegistry[type.getId()];
		for (EventListenerList::iterator it = listeners.begin();
			it != listeners.end(); it++)
		{
			EventListener *currListener = *it;
			if (currListener == listenerToRemove)
			{
				// Delete the first matching listener and then stop, 
				// because we assume that addListener() prevents inserting
				// duplicate listeners.
				listeners.erase(it);
				break;
			}
		}
	}
	
	// Now we're done removing those listeners.
	_listenersToRemove.clear();
}

void EventManager::raiseEvent(const EventPtr &event)
{
	// If we have not seen this event type before, ignore it, because
	// otherwise no listener would know to handle it.
	if (_knownEventTypes.find(event->getType()) == _knownEventTypes.end())
	{
		ERRORMSG("Unregistered event type.");
		return;
	}

	_eventsToProcess.push_back(event);
}

void EventManager::timepass()
{
	// For each event, look up its listeners.
		// For each listener, handle the current event.
	while (!_eventsToProcess.empty())
	{
		EventPtr currEvent = _eventsToProcess.front();
		_eventsToProcess.pop_front();

		int eventId = currEvent->getType().getId();

		// Find the listeners that care about this event.
		EventListenerMap::iterator wildcardListIt = _listenerRegistry.find(0);
		EventListenerMap::iterator otherListIt    = _listenerRegistry.find(eventId);

		// Send the event to the wildcard listeners first, if there are any.
		if (wildcardListIt != _listenerRegistry.end())
		{
			const EventListenerList &wildcardListeners = wildcardListIt->second;
			for (EventListenerList::const_iterator wcIt = wildcardListeners.begin();
				wcIt != wildcardListeners.end(); wcIt++)
			{
				EventListener *currListener = *wcIt;

				// Stop checking other listeners if the event is consumed.
				if (currListener->handleEvent(*currEvent))
					break;
			}

			// If the event type happened to BE the wildcard type, then 
			// of course skip it and don't bother resending it to anyone.
			if (wildcardListIt == otherListIt)
				continue;
		}

		// Otherwise, send the event to all OTHER (non-wildcard) listeners.
		const EventListenerList &listeners = otherListIt->second;
		for (EventListenerList::const_iterator listIt = listeners.begin();
			 listIt != listeners.end(); listIt++)
		{
			EventListener *currListener = *listIt;

			// Again, don't check other listeners if the event gets consumed.
			if (currListener->handleEvent(*currEvent))
				break;
		}
	}

	removeListeners();
}


void EventManager::clear()
{
	_eventsToProcess.clear();
}