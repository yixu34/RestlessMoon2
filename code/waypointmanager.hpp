#ifndef WAYPOINTMANAGER_HPP
#define WAYPOINTMANAGER_HPP

#include <vector>
#include <map>
#include <set>
#include <d3dx9.h>
#include <boost/shared_ptr.hpp>

#include "stlastar.hpp"

class Waypoint;
class WaypointManager;
class Path;
typedef boost::shared_ptr<WaypointManager> WaypointManagerPtr;
typedef boost::shared_ptr<Waypoint> WaypointPtr;
typedef boost::shared_ptr<Path> PathPtr;
typedef std::map<int,WaypointPtr> WaypointMap;
typedef std::vector<int> WaypointList;

typedef struct Edge{
	int destination;
	float distance;
} Edge;

typedef boost::shared_ptr<Edge> EdgePtr;

class Waypoint
{
public:
	Waypoint(float x, float y, float z, int id, std::string cmd="");
	Waypoint(const D3DXVECTOR3 &location):_position(location){}
	D3DXVECTOR3 position(){return _position;}
	int getID(){return ID;}
	std::set<EdgePtr> getNeighbors() {return neighbors;}
private:
	D3DXVECTOR3 _position;
	int ID;
	std::set<EdgePtr> neighbors;
	std::string command;

	friend class WaypointManager;
};

class Path
{
public:
	Path();
	WaypointPtr nextLocation();
	void addWaypoint(WaypointPtr wp);
	void resetCurrent();
	void logPath();
	void setOnComplete(const std::string& cmd){onComplete = cmd;}
	std::string getOnComplete(){return onComplete;}
private:
	std::vector<WaypointPtr> path;
	std::vector<WaypointPtr>::iterator currentNode;
	std::string onComplete;
};

class WaypointManager
{
public:
	WaypointManager(void);
	int addWaypoint(float x, float y, float z);
	void addEdge(int id1, int id2);
	PathPtr getPath(WaypointPtr source, WaypointPtr destination);

	WaypointPtr getWaypoint(int wpid);		
	void clear();
	WaypointPtr nearestVisibleWaypoint(D3DXVECTOR3 target);
	WaypointList visibleWaypoints(D3DXVECTOR3 target);

private:
	WaypointMap waypoints;
	static int nextID;

	class Node{
	private:
		WaypointPtr wp;
		float cost;
	public:
		Node();
		Node(WaypointPtr waypoint, float c);

		WaypointPtr getWP(){return wp;}

		float GoalDistanceEstimate( Node &nodeGoal );
//		Return the estimated cost to goal from this node (pass reference to goal node)

		bool IsGoal( Node &nodeGoal );
//		Return true if this node is the goal.
//
		bool GetSuccessors( AStarSearch<Node> *astarsearch, Node * parent );
//		For each successor to this state call the AStarSearch's AddSuccessor call to 
//		add each one to the current search - return false if you are out of memory and the search
//		will fail
//
		float GetCost( Node &successor );
//		Return the cost moving from this state to the state of successor
//
		bool IsSameState( Node &rhs );
//		Return true if the provided state is the same as this state

	};
	

public:

	virtual ~WaypointManager(void);
};

extern WaypointManager waypointManager;

#endif