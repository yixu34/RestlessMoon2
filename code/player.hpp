#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "animatedentity.hpp"

class Player;
typedef boost::shared_ptr<Player> PlayerPtr;

class Player
{
public:
	Player();
	Player(const AnimatedEntityPtr& entity);
	virtual void timepass() = 0;
	AnimatedEntityPtr getEntity() { return _entity; }
	void setEntity(const AnimatedEntityPtr& entity) { _entity = entity; }

	int getIDNum() { return _entity->getIDNum(); }

protected:
	//EntityPtr _entity;
	AnimatedEntityPtr _entity;

public:
	virtual ~Player(void);
};

#endif