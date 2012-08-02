#ifndef TRIGGER_HPP
#define TRIGGER_HPP

#include <boost/shared_ptr.hpp>
#include <d3dx9.h>
#include <string>
#include "eventlistener.hpp"

class Trigger;
class PlayerLocationTrigger;
class TimeTrigger;
class EntityDeathTrigger;

typedef boost::shared_ptr<Trigger> TriggerPtr;
typedef boost::shared_ptr<PlayerLocationTrigger> PlayerLocationTriggerPtr;
typedef boost::shared_ptr<TimeTrigger> TimeTriggerPtr;
typedef boost::shared_ptr<EntityDeathTrigger> EntityDeathTriggerPtr;

class Trigger
{
	public:
		Trigger();
		virtual ~Trigger(){;}
		virtual bool timepass() = 0;
		void triggerActivated();
		virtual void cleanUp(){;}

	protected:
		std::string _triggerCommand;
};

class PlayerLocationTrigger: public Trigger
{
	public:
		PlayerLocationTrigger(const std::string& triggerCommand,const D3DXVECTOR2& minXZ, 
			const D3DXVECTOR2& maxXZ);
		~PlayerLocationTrigger() {;}
		bool timepass();
		
	private:
		D3DXVECTOR2 _minXZ;
		D3DXVECTOR2 _maxXZ;
};

class TimeTrigger: public Trigger
{
	public:
		TimeTrigger(const std::string& triggerCommand, float timeDelay);
		~TimeTrigger() {;}
		bool timepass();
		
	private:
		float _timeDelay;

};

class EntityDeathTrigger: public Trigger, public EventListener
{
	public:
		EntityDeathTrigger(const std::string& triggerCommand, const std::string& entityName);
		~EntityDeathTrigger(){;}
		void registerHandlers();
		bool timepass(){return _done;}
		bool processEntityDeath(const Event& entityDeath);
		void cleanUp();

	private:
		std::string _entityName;
		bool _done;
};

#endif