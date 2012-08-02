#include "trigger.hpp"
#include "game.hpp"
#include "scripting.hpp"

Trigger::Trigger()
{
}
void Trigger::triggerActivated()
{
	assert(_triggerCommand != "");
	scripting->executeString(_triggerCommand);
}

//***************************************************************************************************************

PlayerLocationTrigger::PlayerLocationTrigger(const std::string& triggerCommand, const D3DXVECTOR2& minXZ, 
											 const D3DXVECTOR2& maxXZ)
{
	_triggerCommand = triggerCommand;
	_minXZ = minXZ;
	_maxXZ = maxXZ;
}

bool PlayerLocationTrigger::timepass()
{
	D3DXVECTOR3 mainPlayerPos = game->player->getEntity()->getPos();
	if(_minXZ.x <= mainPlayerPos.x && _minXZ.y <= mainPlayerPos.z && _maxXZ.x >= mainPlayerPos.x && _maxXZ.y >= mainPlayerPos.z)
	{
		triggerActivated();
		return true;
	}
	else
		return false;
}

//***************************************************************************************************************

TimeTrigger::TimeTrigger(const std::string& triggerCommand, float timeDelay)
{
	_triggerCommand = triggerCommand;
	_timeDelay = timeDelay;
}

bool TimeTrigger::timepass()
{
	_timeDelay -= getDtSeconds();
	if(_timeDelay < 0)
	{
		triggerActivated();
		return true;
	}
	else
		return false;
}

//***************************************************************************************************************

EntityDeathTrigger::EntityDeathTrigger(const std::string &triggerCommand, const std::string& entityName)
{
	_triggerCommand = triggerCommand;
	_entityName = entityName;
	registerHandlers();
	_done = false;
}

void EntityDeathTrigger::cleanUp()
{
	removeHandler(EventType(Ev_Entity_Die::eventName));
}

void EntityDeathTrigger::registerHandlers()
{
	registerHandler(
		EventType(Ev_Entity_Die::eventName), 
		EventHandlerFunctorPtr(new BaseEventHandlerFunctor<EntityDeathTrigger>(this, &EntityDeathTrigger::processEntityDeath)));
}
bool EntityDeathTrigger::processEntityDeath(const Event& entityDeath)
{
	EvEntityDie_data *eventData = entityDeath.getRawData<EvEntityDie_data>();
	
	if(eventData->_entityName == _entityName)
	{
		triggerActivated();
		_done = true;
		removeHandler(EventType(Ev_Entity_Die::eventName));
	}
	return false;
}