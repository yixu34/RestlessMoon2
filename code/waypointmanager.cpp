#include <set>
#include <vector>
#include <algorithm>

#include "waypointmanager.hpp" 
#include "scripting.hpp"
#include "log.hpp"
#include "ray.hpp"
#include "kdtree.hpp"

WaypointManager waypointManager;
int WaypointManager::nextID = 0;

WaypointManager::Node::Node()
:cost(0),wp(WaypointPtr())
{
}

WaypointManager::Node::Node(WaypointPtr waypoint, float c)
:cost(c),wp(waypoint)
{
}

float WaypointManager::Node::GetCost(WaypointManager::Node &successor)
{
	return successor.cost;
}

bool WaypointManager::Node::GetSuccessors(AStarSearch<Node> *astarsearch, Node * parent)
{
	std::set<EdgePtr> neighbors = wp->getNeighbors();
	for(std::set<EdgePtr>::iterator it = neighbors.begin(); it!=neighbors.end();it++)
	{
		Node n(waypointManager.getWaypoint((*it)->destination),(*it)->distance);
		astarsearch->AddSuccessor(n);
	}
	return true;
}

float WaypointManager::Node::GoalDistanceEstimate( Node &nodeGoal )
{
	return D3DXVec3Length(&(wp->position()-nodeGoal.wp->position()));
}

bool WaypointManager::Node::IsGoal(Node &nodeGoal)
{
	return wp->getID() == nodeGoal.wp->getID();
}

bool WaypointManager::Node::IsSameState(Node &rhs)
{
	return wp->getID() == rhs.wp->getID();
}


Path::Path()
{

}

void Path::addWaypoint(WaypointPtr wp)
{
	path.push_back(wp);
}

WaypointPtr Path::nextLocation()
{
	if(path.size()==0) return WaypointPtr();
	if(currentNode!= path.begin())
		currentNode--;
	else
	{
		scripting->executeString(onComplete);
		return WaypointPtr();
	}
	return *currentNode;
}

void Path::logPath()
{
	log("Path data");
	for(int i=0;i<path.size();i++)
		log("Path[%i]=%i",i,path[i]->getID());
}

void Path::resetCurrent()
{
	currentNode = path.end();
	
}

Waypoint::Waypoint(float x,float y,float z, int id, std::string cmd)
{
	ID = id;
	_position = D3DXVECTOR3(x,y,z);
	command = cmd;
}

WaypointManager::WaypointManager(void)
{
}

WaypointManager::~WaypointManager(void)
{
	
}

int WaypointManager::addWaypoint(float x, float y, float z)
{
	WaypointPtr wp(new Waypoint(x,y,z,nextID++));
	waypoints[wp->ID] = wp;
	return wp->ID;	
}

void WaypointManager::addEdge(int id1, int id2)
{
	EdgePtr edge(new Edge());
	edge->destination = id2;
	edge->distance =  D3DXVec3Length(&(waypoints[id1]->_position - waypoints[id2]->_position));
	waypoints[id1]->neighbors.insert(edge);
}


WaypointPtr WaypointManager::getWaypoint(int wpid)
{
	return waypoints[wpid];
}

PathPtr WaypointManager::getPath(WaypointPtr source, WaypointPtr destination)
{
	PathPtr path(new Path());
	int state;
	Node s(source,0);
	Node d(destination,0);
	AStarSearch<Node> astarsearch;
	astarsearch.SetStartAndGoalStates(s,d);
	while((state=astarsearch.SearchStep()) == AStarSearch<Node>::SEARCH_STATE_SEARCHING)
	{
		log("Astar search stepped");
	}
	if(state == AStarSearch<Node>::SEARCH_STATE_SUCCEEDED)
	{
		log("AStar found path");
		Node * n = astarsearch.GetSolutionStart();
		while(( n = astarsearch.GetSolutionNext() )!= NULL)
			path->addWaypoint(n->getWP());
	}

	astarsearch.CancelSearch();

	return path;

}

void WaypointManager::clear()
{
	waypoints.clear();
	nextID = 0;
}
	
WaypointPtr WaypointManager::nearestVisibleWaypoint(D3DXVECTOR3 target)
{
	WaypointPtr wp;
	float minDist = 65535;
	WaypointList seen = visibleWaypoints(target);
	if(seen.size()==0) return WaypointPtr();
	for(int i=0;i<seen.size();i++)
	{
		float d = D3DXVec3Length(&(waypoints[seen[i]]->_position - target));
		if(d<minDist)
		{
			wp = waypoints[seen[i]];
			minDist = d;
		}
	}
	return wp;
}

WaypointList WaypointManager::visibleWaypoints(D3DXVECTOR3 target)
{
		WaypointList wps;
		for(int i=0;i<waypoints.size();i++)
		{
			Ray r(waypoints[i]->_position,target -waypoints[i]->_position);
			if(!kdTree.doesRayIntersect(r))
				wps.push_back(i);
		}
		return wps;
}