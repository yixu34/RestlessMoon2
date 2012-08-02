#ifndef EVENTLISTENER_HPP
#define EVENTLISTENER_HPP

//
// Implement this interface if you want your class to listen to events.
//

#include <boost/shared_ptr.hpp>
//#include <boost/enable_shared_from_this.hpp>
#include <map>

#include "event.hpp"
#include "eventhandlerfunctor.hpp"

//class EventListener;
//typedef boost::shared_ptr<EventListener> EventListenerPtr;

class EventListener //: public boost::enable_shared_from_this<EventListener>
{
public:
	EventListener();
	virtual ~EventListener();

	// Return true if you consume the event, false otherwise.
	bool handleEvent(const Event &event);

	virtual void registerHandlers() = 0;
	
	/*template <typename T>
	void registerHandler(
		const EventType &type, 
		T *instance, 
		bool (T::*HandlerFunc)(const Event &event));*/

	void registerHandler(
		const EventType &type, 
		const EventHandlerFunctorPtr &handler);

	void removeHandler(const EventType &type);

protected:
	typedef std::map<EventType, EventHandlerFunctorPtr> HandlerPool;
	HandlerPool _handlers;
};

//template <typename T>
//inline void EventListener::registerHandler(
//	const EventType &type, 
//	T *instance,
//	bool (T::*HandlerFunc)(const Event &event))
//{
//	EventListenerPtr thisListener = shared_from_this();
//
//	EventHandlerFunctorPtr handler(
//		new BaseEventHandlerFunctor<T>(thisListener, &HandlerFunc));
//
//	_handlers[type] = handler;
//	eventManager->addListener(
//		thisListener, 
//		type);
//}

#endif