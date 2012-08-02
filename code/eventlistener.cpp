#include "eventlistener.hpp"
#include "eventmanager.hpp"

EventListener::EventListener()
{
}

EventListener::~EventListener()
{
	// We -could- unregister ourselves from listening to certain event types
	// via the event manager here.
}

bool EventListener::handleEvent(const Event &event)
{
	HandlerPool::iterator it = _handlers.find(event.getType());
	if (it != _handlers.end())
		return it->second->execute(event);

	return false;
}

void EventListener::registerHandler(
	const EventType &type, 
	const EventHandlerFunctorPtr &handler)
{
	_handlers[type] = handler;
	eventManager->addListener(
		this, //shared_from_this(), 
		type);
}

void EventListener::removeHandler(const EventType &type)
{
	if (_handlers.find(type) != _handlers.end())
		_handlers.erase(type);

	eventManager->removeListener(
		this, //shared_from_this(), 
		type);
}
