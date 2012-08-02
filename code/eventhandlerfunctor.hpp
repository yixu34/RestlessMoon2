#ifndef EVENTHANDLERFUNCTOR_HPP
#define EVENTHANDLERFUNCTOR_HPP

#include <boost/shared_ptr.hpp>

class EventListener;
class EventHandlerFunctor;

typedef boost::shared_ptr<EventListener> EventListenerPtr;
typedef boost::shared_ptr<EventHandlerFunctor> EventHandlerFunctorPtr;

class EventHandlerFunctor
{
public:
	EventHandlerFunctor() {}
	virtual ~EventHandlerFunctor() = 0 {}

	virtual bool execute(const Event &event) = 0;
};

template <typename T>
class BaseEventHandlerFunctor : public EventHandlerFunctor
{
public:
	typedef bool (T::*HandlerFunc)(const Event &event);

	BaseEventHandlerFunctor(T *instance, HandlerFunc handler)
		: _instance(instance), _handler(handler)
	{}

	/*BaseEventHandlerFunctor(
		const EventListenerPtr &listener, 
		HandlerFunc handler)
		: _listener(listener), _handler(handler)
	{}*/

	bool execute(const Event &event)
	{
		return (*_instance.*_handler)(event);
		//return (*_listener.*_handler)(event);
	}

private:
	//EventListenerPtr _listener;
	T            *_instance;
	HandlerFunc   _handler;
};


#endif