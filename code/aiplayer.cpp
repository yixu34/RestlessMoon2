#include <d3dx9.h>
#include "aiplayer.hpp"
#include "player.hpp"
#include "animatedentity.hpp"
#include "util.hpp"
#include "scripting.hpp"
#include "game.hpp"
#include "ray.hpp"
#include "kdtree.hpp"

AIPlayer::AIPlayer(const AnimatedEntityPtr& entity) : Player(entity)
{
	lastLoS = 0;
	currentLoS = false;
	//_state = AI_IDLE;
	_state = AI_WANDER;
	target = game->player;
	lastHealth = _entity->getHealth();
}

AIPlayer::~AIPlayer(void)
{
}

void AIPlayer::setTargetPosition(const WaypointPtr& waypoint, const std::string& cmd)
{
	onArrive = cmd;
	targetPos = waypoint;
	_state = AI_MOVE;
}

void AIPlayer::setPlayerTarget(const PlayerPtr& player)
{
	target = player;
	_state = AI_WANDER;
			
	D3DXVECTOR3 pos = _entity->getPos();
	int start = waypointManager.addWaypoint(pos.x,pos.y,pos.z);
	startPos = waypointManager.getWaypoint(start);
/*	WaypointList l = waypointManager.visibleWaypoints(startPos->position());
	for(int i=0;i<l.size();i++)
	{
		waypointManager.addEdge(start,l[i]);
		waypointManager.addEdge(l[i],start);
	}
*/
}

void AIPlayer::setPath(const PathPtr& p)
{
	path = p;
	setTargetPosition(path->nextLocation());
}

bool AIPlayer::canSee(const D3DXVECTOR3& e)
{
	lastLoS+=getDtSeconds();
	if(lastLoS > 1)
	{
		lastLoS = 0;
		Ray r(_entity->getPos()+D3DXVECTOR3(0,300,0),e -_entity->getPos());
		currentLoS = !kdTree.doesRayIntersect(r);
	}
	return currentLoS;
}

void AIPlayer::timepass(void)
{
	if(_state == AI_IDLE || _entity->isDead() || _entity->isReloading() || game->areEntitiesFrozen())
	{
		// Put this call to stop() so that ai players don't play their chasing
		// animations while the game is frozen.
		_entity->stop();
		return;
	}

	D3DXVECTOR3 pos = _entity->getPos();
	D3DXVECTOR3 tPos = game->player->getEntity()->getPos();
	
	if(_state == AI_WANDER)
	{
		if(canSee(tPos) || lastHealth != _entity->getHealth())
		{
			if(target == NULL) target = game->player;
			_state = AI_ATTACK;
			_entity->walkBackward();
		}
	}
	else if(_state == AI_MOVE && targetPos != NULL)
	{
		if(util::toleranceCompare(pos.x,targetPos->position().x,30.0f)&&
		   util::toleranceCompare(pos.z,targetPos->position().z,30.0f))
		{
			WaypointPtr wp;
			if(path!=NULL)
				wp = path->nextLocation(); //FIXME: iterators incompatible
			if(wp != NULL) 
				setTargetPosition(wp);
			else
				scripting->executeString(path->getOnComplete());
			
		}
		else
		{
			_entity->turnTo( targetPos->position());
			_entity->walkForward();
			if(canSee(tPos))
				_state = AI_ATTACK;
		}
	}
	else if(_state == AI_ATTACK)
	{
		float dist = D3DXVec3Length(&(tPos - pos));
		float time = dist/(_entity->getWeapon()->projectileSpeed );
		D3DXVECTOR3 dir = target->getEntity()->getVelocity();
		D3DXVec3Normalize(&dir, &dir);
		D3DXVECTOR3 leadPos = tPos + time * dir * target->getEntity()->getSpeed(); 
        float f = (target->getEntity()->getBoundingBox().max.y + target->getEntity()->getBoundingBox().min.y)/2.0f -getEntity()->getWeapon()->bulletOffset[0].y;
		
		leadPos.y = f;//tPos.y + 200; //I think gravity is screwing up the y direction

		if(_entity->turnTo(leadPos))
		{		
			if(canSee(tPos))
			{
				_entity->shoot();
				if(!_entity->getNumBullets())
					_entity->reload();

				if(target->getEntity()->getVelocity().x < 0)
					_entity->strafeRight();
				else if(target->getEntity()->getVelocity().x > 0)
					_entity->strafeLeft();

				if(( (_entity->getHealth()<_entity->getMaxHealth()/2) && (dist < 1200)) || dist < 600)
					_entity->walkBackward();
				else			
					_entity->walkForward();
					
			}
				
			else
			{
				// targetPos is now the last visible location of AI
				targetPos = WaypointPtr(new Waypoint(tPos));  
				_state = AI_CHASE;
			}
		}
		
	}
	else if(_state == AI_CHASE)
	{
		_entity->turnTo(targetPos->position());
		_entity->walkForward();
		if(canSee(tPos))
		{
			_state = AI_ATTACK;
		}
		else if(util::toleranceCompare(pos.x,targetPos->position().x,30.0f)&&
		        util::toleranceCompare(pos.z,targetPos->position().z,30.0f))
			_state = AI_RETREAT;
		else if(!_entity->getNumBullets())
			_entity->reload();
	}
	else if(_state == AI_RETREAT)
	{
	/*	if(util::Vector3Compare(pos,startPos->position(),30))
		{
			_state = AI_WANDER;
		}
		else
		{*/
		WaypointPtr nearest = waypointManager.nearestVisibleWaypoint(pos);
		if (!nearest)
			_state = AI_WANDER;
		else
		{
			// XXX:  startPos was occasionally null, which caused a crash
			// in getPath().
			if (!startPos)
			{
				int start = waypointManager.addWaypoint(pos.x,pos.y,pos.z);
				startPos = waypointManager.getWaypoint(start);
			}
			MYASSERT(startPos);

			path = waypointManager.getPath(nearest,startPos);
			_state = AI_MOVE;
		}
		//}
	}

	// Make sure that we show a running/shooting animation if we need to.
	/*if (_state != AI_IDLE && _state != AI_WANDER)
	{
		_entity->walkForward();
	}*/
}