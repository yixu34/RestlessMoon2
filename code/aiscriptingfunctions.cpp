#include "scripting.hpp"
#include "game.hpp"
#include "waypointmanager.hpp"

// moveAIToPos(int uid, float posX,float posY,float posZ,string command)
// moves the AI player with uid to given position
// on arrival, runs the command 
SCRIPTING_FUNCTION(moveAIToPos)
{
	int uid = scripting->getArgument<int>(1);
	float posX = scripting->getArgument<float>(2);
	float posY = scripting->getArgument<float>(3);
	float posZ = scripting->getArgument<float>(4);
	std::string command   = scripting->getArgument<std::string>(5);
	AIPlayerPtr aiPlayer = game->getAIPlayer(uid);

	if(aiPlayer != NULL)
	{
		aiPlayer->setTargetPosition(WaypointPtr(new Waypoint(posX,posY,posZ,-1,command)));
	}
	//else return an error as player doesnt exist
	
	return 0;
}

//moveAIToWaypoint(uid, wpid,command)
//moves the AI with uid to the waypoint with wpid
SCRIPTING_FUNCTION(moveAIToWaypoint)
{
	int uid = scripting->getArgument<int>(1);
	int wpid = scripting->getArgument<int>(2);

	AIPlayerPtr aiPlayer = game->getAIPlayer(uid);
	if(aiPlayer == NULL) return 0; //that player doesnt exist, probably should raise error
	
	if(scripting->getNumArgs() == 3)
	{
		std::string cmd = scripting->getArgument<std::string>(3);
		aiPlayer->setTargetPosition(waypointManager.getWaypoint(wpid),cmd);
	}
	else
		aiPlayer->setTargetPosition(waypointManager.getWaypoint(wpid));
	return 0;
}

//addWayPoint(float x, float y, float z, wpid2= NULL)
//creates a wp at x,y,z and returns the wpid
//if wpid2 exists, creates an edges between wpid2 and the new waypoint
SCRIPTING_FUNCTION(addWayPoint)
{
	float posX = scripting->getArgument<float>(1);
	float posY = scripting->getArgument<float>(2);
	float posZ = scripting->getArgument<float>(3);

	int wp = waypointManager.addWaypoint(posX,posY,posZ);

	if(scripting->getNumArgs() == 4)
		waypointManager.addEdge(wp,scripting->getArgument<int>(4));

	scripting->pushArgument(wp);

	return 1;
}

//setNextWaypoint(wpid1,wpid2)
//simply adds an edge from wpid1 to wpid2
SCRIPTING_FUNCTION(setNextWayPoint)
{
	int id1 = scripting->getArgument<int>(1);
	int id2 = scripting->getArgument<int>(2);

	waypointManager.addEdge(id1,id2);

	return 0;
}

//attackPlayer(attackerID,targetID=human)
//tells ai player attackerID to attack targetID
//if target is not set, attacks the human player
SCRIPTING_FUNCTION(attackPlayer)
{
	int attacker = scripting->getArgument<int>(1);
	AIPlayerPtr aiPlayer = game->getAIPlayer(attacker);
	if(aiPlayer == NULL) return 0; //that player doesnt exist, probably should raise error

	if(scripting->getNumArgs() == 2)
	{
		int target = scripting->getArgument<int>(2);
		AIPlayerPtr aiTarget = game->getAIPlayer(target);

		if(aiTarget == NULL) return 0; //that player doesnt exist, probably should raise error

		aiPlayer->setPlayerTarget(aiTarget);
	}
	else
		aiPlayer->setPlayerTarget(game->player);
	return 0;
}

SCRIPTING_FUNCTION(pathFind)
{
	int unit = scripting->getArgument<int>(1);
	int from = scripting->getArgument<int>(2);
	int to = scripting->getArgument<int>(3);
	
	WaypointPtr fromPtr = waypointManager.getWaypoint(from);
	WaypointPtr toPtr = waypointManager.getWaypoint(to);
	MYASSERT(fromPtr && toPtr);

	PathPtr path = waypointManager.getPath(fromPtr,toPtr);
	path->resetCurrent();
	AIPlayerPtr ai = game->getAIPlayer(unit);
	if(ai!=NULL)
	ai->setPath(path);
	return 0;
}

//setPath(uid,wp1,wp2,...,cmd)
SCRIPTING_FUNCTION(setPath)
{
	int unit = scripting->getArgument<int>(1);
	PathPtr path(new Path());
	AIPlayerPtr ai = game->getAIPlayer(unit);
	if(ai==NULL) return 0;

	for(int i=2;i<scripting->getNumArgs();i++)
	{
		path->addWaypoint(waypointManager.getWaypoint(scripting->getArgument<int>(i)));
	}
	path->setOnComplete(scripting->getArgument<std::string>(scripting->getNumArgs()));
	path->resetCurrent();
	ai->setPath(path);
	return 0;
}

void registerAIScriptingFunctions()
{
	REGISTER_SCRIPTING_FUNCTION(moveAIToPos);
	REGISTER_SCRIPTING_FUNCTION(addWayPoint);
	REGISTER_SCRIPTING_FUNCTION(setNextWayPoint);
	REGISTER_SCRIPTING_FUNCTION(moveAIToWaypoint);
	REGISTER_SCRIPTING_FUNCTION(attackPlayer);
	REGISTER_SCRIPTING_FUNCTION(pathFind);
	REGISTER_SCRIPTING_FUNCTION(setPath);
}