#ifndef AIPLAYER_HPP
#define AIPLAYER_HPP

#include "animatedentity.hpp"
#include "player.hpp"
#include "waypointmanager.hpp"

class AIPlayer;
typedef boost::shared_ptr<AIPlayer> AIPlayerPtr;

class AIPlayer : public Player
{
public:
	AIPlayer(const AnimatedEntityPtr& entity);
	void timepass();
	void setTargetPosition(const WaypointPtr& waypoint, const std::string &cmd = "");
	void setPlayerTarget(const PlayerPtr& player);
	void setPath(const PathPtr& p);
	bool canSee(const D3DXVECTOR3& e);
public:
	virtual ~AIPlayer(void);

private:
	WaypointPtr targetPos;  //The waypoint headed towards, should have LoS
	WaypointPtr startPos;   //Where we return to when nothing else to do
	PlayerPtr target;		//The player its attacking
	PathPtr path;			//Path to follow to get to target
	float lastLoS;
	bool currentLoS;
	std::string onArrive;

	enum state {
		AI_IDLE,			//Sit there and do nothing, no matter what
		AI_MOVE,			//Follow a path
		AI_ATTACK,			//Target is in sight, shoot him
		AI_CHASE,			//Target just went out of site, prepare to attack last known location
		AI_WANDER,			//No valid targets
		AI_RETREAT,			//Better return to starting location
	};

	state _state;
	float lastHealth;
};

#endif