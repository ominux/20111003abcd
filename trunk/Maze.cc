/* 
Labyrinth - a global router basedon maze routing
Copyright (C) 2001  Ryan Kastner

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

The author can be contacted by:
email: kastner@cs.ucla.edu
snail mail: University of California, Los Angeles 
            Department of Computer Science 
            3256N Boelter Hall 
            Los Angeles, CA 90095 
*/

#include "Maze.h"
#include "MazePoint.h"
#include <math.h>
#include <iomanip>
#include <list>
#include <limits.h>

using namespace std;

bool PointCompare::operator() (MazePoint *one, MazePoint *two)
{
  //return one->getCost() > two->getCost(); 
  return one->getMazeRouteCost() > two->getMazeRouteCost();
}

int Maze::count = 0;

Maze::Maze()
{
  x = 100;
  y = 100;
  source = 0;
  mySink = 0;
  sinkFound = false;
  costFnToRoutePts = false;
  costQ = new PriorityQ();
  possibleRoutePts = new MazeList();
  routePts = new MazeList();
  maze = new MazeVector(100);
  for(MazeVectorItr i = maze->begin(); i != maze->end(); i++)
      (*i) = new ColumnVector(100);
      
}

Maze::Maze(int xValue, int yValue)
{
  x = xValue;
  y = yValue;
  source = 0;
  mySink = 0;
  sinkFound = false;
  costFnToRoutePts = false;
  maze = new MazeVector(xValue);
  int xPlaceholder = 0;
  int yPlaceholder = 0;
  possibleRoutePts = new MazeList();
  routePts = new MazeList();
  costQ = new PriorityQ();
  for(MazeVectorItr i = maze->begin(); i != maze->end(); i++)
    {
      (*i) = new ColumnVector(yValue);
      
      for(ColumnVectorItr j = (*i)->begin(); j != (*i)->end(); j++)
	{
	  *j = new MazePoint(xPlaceholder,yPlaceholder);
	  yPlaceholder++;
	}
      xPlaceholder++;
      yPlaceholder = 0;
    }
}

Maze::Maze(int xValue, int yValue, RouteTracks *rt)
{
  x = xValue;
  y = yValue;
  source = 0;
  mySink = 0;
  sinkFound = false;
  costFnToRoutePts = false;
  maze = new MazeVector(xValue);
  int xPlaceholder = 0;
  int yPlaceholder = 0;
  possibleRoutePts = new MazeList();
  routePts = new MazeList();
  costQ = new PriorityQ();
  routeTracks = rt;
  for(MazeVectorItr i = maze->begin(); i != maze->end(); i++)
    {
      (*i) = new ColumnVector(yValue);
      
      for(ColumnVectorItr j = (*i)->begin(); j != (*i)->end(); j++)
	{
	  *j = new MazePoint(xPlaceholder,yPlaceholder);
	  (*j)->
	    setTop(routeTracks->getEdgeAt(xPlaceholder, yPlaceholder-1,false));
	  (*j)->
	    setBottom(routeTracks->getEdgeAt(xPlaceholder,yPlaceholder,false));
	  (*j)->
	    setLeft(routeTracks->getEdgeAt(xPlaceholder-1,yPlaceholder,true));
	  (*j)->
	    setRight(routeTracks->getEdgeAt(xPlaceholder,yPlaceholder,true));
	  yPlaceholder++;
	}
      xPlaceholder++;
      yPlaceholder = 0;
    }
}
  
  
Maze::~Maze()
{
  if(source != 0)
    delete source;
  if(mySink != 0)
    delete mySink;
  for(MazeVectorItr i = maze->begin(); i != maze->end(); i++)
    {
      for(ColumnVectorItr j = (*i)->begin(); j != (*i)->end(); j++)
	delete (*j);
      
      delete (*i);
    }
  delete maze;
  while(!costQ->empty())
    {
      MazePoint *holder = costQ->top();
      costQ->pop();
      delete holder;
    }
  delete costQ;
  while(!possibleRoutePts->empty())
    {
      MazePoint *frontOfList = possibleRoutePts->front();
      possibleRoutePts->pop_front();
      //JW delete frontOfList;
    }
  delete possibleRoutePts;
  while(!routePts->empty())
    {
      MazePoint *frontOfList = routePts->front();
      routePts->pop_front();
      //JW delete frontOfList;
    }
  delete routePts;
}

bool Maze::findRoute()
{
  //source or sink not initialized
  if(source == 0 || mySink == 0)
    return false;
  sinkFound = false;

  //source and sink at same location
  if(source->getX() == mySink->getX() && source->getY() == mySink->getY())
    return true;
  
  //sink already on a route
  if(getMazePointAt(source->getX(),source->getY())->isSink())
    return true;

  for(MazeListItr i = possibleRoutePts->begin(); i != possibleRoutePts->end(); i++)
    {
      (*i)->clearCoordinate();
    }
  possibleRoutePts->clear();
  while(!costQ->empty() )
    {
      costQ->top()->clearCoordinate();
      costQ->pop();
    }

  int sourceX = source->getX();
  int sourceY = source->getY();
  int routeLength;

  if(costFnToRoutePts)
    {
      MazePoint tempSource(sourceX, sourceY);
      MazePoint *closestNet = getCostToRoute(&tempSource,0);
      if(closestNet != 0)
	putSink(closestNet);
    }

  MazePoint *mPoint = getMazePointAt(sourceX+1,sourceY);
  if(mPoint == 0)
    { }
  else if(mPoint->isSink())
    {
      int mazeCost = 0;
      if(routeTracks->getHorizontalCapacity() <= mPoint->getLeft()->getNumNets())
	{
	  mazeCost = mPoint->getLeft()->getNumNets();
	}
      mPoint->setParent(getMazePointAt(source->getX(), source->getY()));
      mPoint->setMazeRouteCost(mySink, 1,mazeCost);
      mPoint->setRouteLength(1);
      costQ->push(mPoint);
    }
  else if(mPoint->getPointType() == 0)
    {
      int mazeCost = 0;
      if(routeTracks->getHorizontalCapacity() <= mPoint->getLeft()->getNumNets())
	{
	  mazeCost = mPoint->getLeft()->getNumNets();
	}
      mPoint->setParent(getMazePointAt(source->getX(), source->getY()));
      mPoint->setPointType(1);
      mPoint->setMazeRouteCost(mySink, 1,mazeCost);
      costQ->push(mPoint);
    }
  
  mPoint = getMazePointAt(sourceX-1,sourceY);
  if(mPoint == 0)
    { }
  else if(mPoint->isSink())
    {
      int mazeCost = 0;
      if(routeTracks->getHorizontalCapacity() <= mPoint->getRight()->getNumNets())
	{
	  mazeCost = mPoint->getRight()->getNumNets();
	}
      mPoint->setParent(getMazePointAt(source->getX(), source->getY()));
      mPoint->setRouteLength(1);
      mPoint->setMazeRouteCost(mySink,1,mazeCost);
      costQ->push(mPoint);
    }
  else if(mPoint->getPointType() == 0)
    {
      int mazeCost = 0;
      if(routeTracks->getHorizontalCapacity() <= mPoint->getRight()->getNumNets())
	{
	  mazeCost = mPoint->getRight()->getNumNets();
	}
      mPoint->setParent(getMazePointAt(source->getX(), source->getY()));
      mPoint->setPointType(1);
      mPoint->setMazeRouteCost(mySink,1,mazeCost);
      costQ->push(mPoint);
    }

  mPoint = getMazePointAt(sourceX,sourceY+1);
  if(mPoint == 0)
    { }
  else if(mPoint->isSink())
    { 
      int mazeCost = 0;
      if(routeTracks->getVerticalCapacity() <= mPoint->getTop()->getNumNets())
	{
	  mazeCost = mPoint->getTop()->getNumNets();
	}
      mPoint->setParent(getMazePointAt(source->getX(), source->getY()));
      mPoint->setRouteLength(1);
      mPoint->setMazeRouteCost(mySink,1,mazeCost);
      costQ->push(mPoint);
    }   
  else if(mPoint->getPointType() == 0)
    {
      int mazeCost = 0;
      if(routeTracks->getVerticalCapacity() <= mPoint->getTop()->getNumNets())
	{
	  mazeCost = mPoint->getTop()->getNumNets();
	}
      mPoint->setParent(getMazePointAt(source->getX(), source->getY()));
      mPoint->setPointType(1);
      mPoint->setMazeRouteCost(mySink,1,mazeCost);
      costQ->push(mPoint);
    }
  
  mPoint = getMazePointAt(sourceX,sourceY-1);
  if(mPoint == 0)
    { }
  else if(mPoint->isSink())
    { 
      int mazeCost = 0;
      if(routeTracks->getVerticalCapacity() <= mPoint->getBottom()->getNumNets())
	{
	  mazeCost = mPoint->getBottom()->getNumNets();
	}
      mPoint->setParent(getMazePointAt(source->getX(), source->getY()));
      mPoint->setRouteLength(1);
      mPoint->setMazeRouteCost(mySink,1,mazeCost);
      costQ->push(mPoint);
    }
  else if(mPoint->getPointType() == 0)
    {
      int mazeCost = 0;
      if(routeTracks->getVerticalCapacity() <= mPoint->getBottom()->getNumNets())
	{
	  mazeCost = mPoint->getBottom()->getNumNets();
	}
      mPoint->setParent(getMazePointAt(source->getX(), source->getY()));
      mPoint->setPointType(1);
      mPoint->setMazeRouteCost(mySink,1,mazeCost);
      costQ->push(mPoint);
    }
  
  while(!costQ->empty() && !sinkFound)
    {
      MazePoint *nextPoint;
      MazePoint *currentPoint;
      currentPoint = costQ->top();
      costQ->pop();
      possibleRoutePts->push_front(currentPoint);
      int currentX = currentPoint->getX();
      int currentY = currentPoint->getY();
      int currentPointType = currentPoint->getMazeRouteCost() + 1;
      routeLength = currentPoint->getPointType() + 1;

      if(currentPoint->isSink())
	{
	  sinkFound = true;
	  putSink(currentPoint);
	  pickRoute(currentPoint);
	  return sinkFound;
	}
	
      nextPoint = getMazePointAt(currentX+1,currentY);
      if(nextPoint == 0)
	{ }
      else if(nextPoint->isSink())
	{
	  if(nextPoint->getMazeRouteCost() 
	     > nextPoint->calculateMazeRouteCost
	     (mySink,routeLength,nextPoint->getLeft()->getNumNets() 
	      + currentPoint->getRouteCost())  ||
	     nextPoint->getMazeRouteCost() == 0)
	    {
	      int mazeCost = currentPoint->getRouteCost();
	      if(routeTracks->getHorizontalCapacity() 
		 <= nextPoint->getLeft()->getNumNets())
		{
		  mazeCost += nextPoint->getLeft()->getNumNets();
		}
	      nextPoint->setParent(currentPoint);
	      nextPoint->setMazeRouteCost(mySink,routeLength,mazeCost);
	      nextPoint->setRouteLength(routeLength);
	      costQ->push(nextPoint);
	    }
	}
      else if(nextPoint->getPointType() == 0 || nextPoint->getMazeRouteCost() 
	      > nextPoint->calculateMazeRouteCost(mySink,routeLength,
						  nextPoint->getLeft()->
						  getNumNets() + currentPoint
						  ->getRouteCost()))
	{
	  nextPoint->setPointType(routeLength);
	  int mazeCost = currentPoint->getRouteCost();
	  if(routeTracks->getHorizontalCapacity() <= nextPoint->getLeft()->getNumNets())
	    {
	      mazeCost += nextPoint->getLeft()->getNumNets();
	    }
	  nextPoint->setParent(currentPoint);
	  nextPoint->setMazeRouteCost(mySink,routeLength,mazeCost);
	  nextPoint->setRouteLength(routeLength);
	  costQ->push(nextPoint);
	}
      
      nextPoint = getMazePointAt(currentX-1,currentY);
      if(nextPoint == 0)
	{ }
      else if(nextPoint->isSink())
	{ 
	  if(nextPoint->getMazeRouteCost() > nextPoint->calculateMazeRouteCost
	     (mySink,routeLength, nextPoint->getRight()->
	      getNumNets() + currentPoint->getRouteCost())
	      || nextPoint->getMazeRouteCost() == 0)
	    {
	      int mazeCost = currentPoint->getRouteCost();
	      if(routeTracks->getHorizontalCapacity() 
		 <= nextPoint->getRight()->getNumNets())
		mazeCost += nextPoint->getRight()->getNumNets();
	      nextPoint->setParent(currentPoint);
	      nextPoint->setRouteLength(routeLength);
	      nextPoint->setMazeRouteCost(mySink, routeLength,mazeCost);
	      costQ->push(nextPoint);
	    }
	}
      else if(nextPoint->getPointType() == 0 || nextPoint->getMazeRouteCost() 
	      > nextPoint->calculateMazeRouteCost
	      (mySink,routeLength,nextPoint->getRight()->getNumNets() 
	       + currentPoint->getRouteCost()))
	{
	  nextPoint->setPointType(routeLength);
	  int mazeCost = currentPoint->getRouteCost();
	  if(routeTracks->getHorizontalCapacity() <= nextPoint->getRight()->getNumNets())
	    {
	      mazeCost += nextPoint->getRight()->getNumNets();
	    }
	  nextPoint->setParent(currentPoint);
	  nextPoint->setMazeRouteCost(mySink, routeLength,mazeCost);
	  nextPoint->setRouteLength(routeLength);
	  costQ->push(nextPoint);
	}

      nextPoint = getMazePointAt(currentX,currentY+1);
      if(nextPoint == 0)
	{ }
      else if(nextPoint->isSink())
	{
	  if(nextPoint->getMazeRouteCost() > nextPoint->calculateMazeRouteCost
	     (mySink,routeLength, nextPoint->getTop()->getNumNets() 
	      + currentPoint->getRouteCost()) 
	     || nextPoint->getMazeRouteCost() == 0)
	    {
	      int mazeCost = currentPoint->getRouteCost();
	      if(routeTracks->getVerticalCapacity() 
		 <= nextPoint->getTop()->getNumNets())
		{ 
		  mazeCost += nextPoint->getTop()->getNumNets();
		}
	      nextPoint->setParent(currentPoint);
	      nextPoint->setRouteLength(routeLength);
	      nextPoint->setMazeRouteCost(mySink,routeLength,mazeCost);
	      costQ->push(nextPoint);
	    }
	}
      else if(nextPoint->getPointType() == 0 || nextPoint->getMazeRouteCost() 
	      > nextPoint->calculateMazeRouteCost
	      (mySink,routeLength,nextPoint->getTop()->getNumNets()
	       + currentPoint->getRouteCost()))
	{
	  nextPoint->setPointType(routeLength);
	  int mazeCost = currentPoint->getRouteCost() ;
	  if(routeTracks->getVerticalCapacity() <= nextPoint->getTop()->getNumNets())
	    {
	      mazeCost += nextPoint->getTop()->getNumNets();
	    }
	  nextPoint->setParent(currentPoint);
	  nextPoint->setRouteLength(routeLength);
	  nextPoint->setMazeRouteCost(mySink,routeLength,mazeCost);
	  costQ->push(nextPoint);
	}

      nextPoint = getMazePointAt(currentX,currentY-1);
      if(nextPoint == 0)
	{ }
      else if(nextPoint->isSink())
	{
	  if(nextPoint->getMazeRouteCost() > nextPoint->calculateMazeRouteCost
	     (mySink,routeLength, nextPoint->getBottom()-> getNumNets() 
	      + currentPoint->getRouteCost()) 
	     || nextPoint->getMazeRouteCost() == 0)
	    {
	      int mazeCost = currentPoint->getRouteCost();
	      if(routeTracks->getVerticalCapacity()
		 <= nextPoint->getBottom()->getNumNets())
		{
		  mazeCost += nextPoint->getBottom()->getNumNets();
		}
	      nextPoint->setParent(currentPoint);
	      nextPoint-> setMazeRouteCost(mySink,routeLength,mazeCost);
	      nextPoint->setRouteLength(routeLength);
	      costQ->push(nextPoint);
	    }
	}
      else if(nextPoint->getPointType() == 0 || nextPoint->getMazeRouteCost() 
	      > nextPoint->calculateMazeRouteCost(mySink,routeLength,
						  nextPoint->getBottom()
						  ->getNumNets()+currentPoint
						  ->getRouteCost()))
	{
	  nextPoint->setPointType(routeLength);
	  int mazeCost = currentPoint->getRouteCost();
	  if(routeTracks->getVerticalCapacity()<= nextPoint->getBottom()->getNumNets())
	    {
	      mazeCost += nextPoint->getBottom()->getNumNets();
	    }
	  nextPoint->setParent(currentPoint);
	  nextPoint->setRouteLength(routeLength);
	  nextPoint->setMazeRouteCost(mySink,routeLength,mazeCost);
	  costQ->push(nextPoint);
	}
    }
  return sinkFound;
   
}

bool Maze::findRoute(Point *sourcePt, Point *sinkPt)
{
  putSource(sourcePt);
  putSink(sinkPt);
  return findRoute();
}

void Maze::pickRoute(MazePoint *startPoint)
{  
  static int UP = 1;
  static int DOWN = 2;
  static int LEFT = 3;
  static int RIGHT = 4;
  count++;
  MazePoint *currentPoint = startPoint;
  bool processedSource = false;
  
  //just for debugging
  list<RouteEdge *> routeEdgeList;
  bool printList = false;
  
  while(!processedSource)
    {
      int direction;
      MazePoint *nextPoint = currentPoint->getParent();
      int nextPointx = nextPoint->getX();
      int nextPointy = nextPoint->getY();
      int currentPointx = currentPoint->getX();
      int currentPointy = currentPoint->getY();
      if(currentPointx != nextPointx)
	{
	  if(nextPointx-1 == currentPointx)
	    {
	      RouteEdge *tempEdge = currentPoint->getRight();
	      routeEdgeList.push_front(tempEdge);
	      mazeNet->addEdge(tempEdge->isHorizontal(),
			       tempEdge->getTrackNum(),tempEdge->getEdgeNum());
	      tempEdge->addNet(mazeNet);
	    }
	  else if(nextPointx+1 == currentPointx)
	    {
	      RouteEdge *tempEdge = currentPoint->getLeft();
	      routeEdgeList.push_front(tempEdge);
	      mazeNet->addEdge(tempEdge->isHorizontal(),
			       tempEdge->getTrackNum(),tempEdge->getEdgeNum());
	      tempEdge->addNet(mazeNet);
	    }
	}
      else if(currentPointy != nextPointy)
	{
	  if(nextPointy-1 == currentPointy)
	    {
	      RouteEdge *tempEdge = currentPoint->getBottom();
	      routeEdgeList.push_front(tempEdge);
	      mazeNet->addEdge(tempEdge->isHorizontal(),
			       tempEdge->getTrackNum(),tempEdge->getEdgeNum());
	      tempEdge->addNet(mazeNet);
	    }
	  else if(nextPointy+1 == currentPointy)
	    {
	      RouteEdge *tempEdge = currentPoint->getTop();
	      routeEdgeList.push_front(tempEdge);
	      mazeNet->addEdge(tempEdge->isHorizontal(),
			       tempEdge->getTrackNum(),tempEdge->getEdgeNum());
	      tempEdge->addNet(mazeNet);
	    }
	}
      currentPoint->setRouteHere();
      currentPoint = nextPoint;
      if(currentPoint->getX() == source->getX() 
	 && currentPoint->getY() == source->getY())
	processedSource = true;
    }

  if(printList)
    {
      for(list<RouteEdge *>::iterator i = routeEdgeList.begin(); 
	  i != routeEdgeList.end(); i++)
	(*i)->print();
    }
  
  //clear the necessary data structures
  MazePoint *routePoint;
  while(!costQ->empty() )
    {
      routePoint = costQ->top();
      costQ->pop();
      routePoint->clearCoordinate();
    }
  for(MazeListItr i = possibleRoutePts->begin(); 
      i != possibleRoutePts->end(); i++)
      (*i)->clearCoordinate();

}
    

void Maze::pickRoute(int distance)
{
  int currentX = mySink->getX();
  int currentY = mySink->getY();
  int length = distance - 1;
  int nextX;
  int nextY;
  int direction = 0;
  static int UP = 1;
  static int DOWN = 2;
  static int LEFT = 3;
  static int RIGHT = 4;

  //find the route
  while(length > 0)
    {
      int lowestMazeCost = INT_MAX; 
      int foundNext = false;
      if(getLength(currentX, currentY-1) == length)
	{
	  foundNext = true;
	  lowestMazeCost = mazeRouteValueAt(currentX,currentY-1);
	  nextX = currentX;
	  nextY = currentY-1;
	  direction = UP;
	}
      
      if(getLength(currentX, currentY+1) == length && 
	 mazeRouteValueAt(currentX,currentY+1) < lowestMazeCost &&
	 mazeRouteValueAt(currentX,currentY+1) != 0)
	{
	  lowestMazeCost = mazeRouteValueAt(currentX,currentY+1);
	  nextX = currentX;
	  nextY = currentY+1;
	  direction = DOWN;
	}
	
      if(getLength(currentX-1, currentY) == length && 
	 mazeRouteValueAt(currentX-1,currentY) < lowestMazeCost &&
	 mazeRouteValueAt(currentX-1,currentY) != 0)
	{
	  lowestMazeCost = mazeRouteValueAt(currentX-1,currentY);
	  nextX = currentX-1;
	  nextY = currentY;
	  direction = LEFT;
	}  
      if(getLength(currentX+1, currentY) == length && 
	 mazeRouteValueAt(currentX+1,currentY) < lowestMazeCost&&
	 mazeRouteValueAt(currentX+1,currentY) != 0)
	{
	  lowestMazeCost = mazeRouteValueAt(currentX+1,currentY);
	  nextX = currentX+1;
	  nextY = currentY;
	  direction = RIGHT;
	}
      MazePoint *routePt = getMazePointAt(nextX, nextY);
      if(direction == UP)
	{
	  routePt->getBottom()->addNet(mazeNet);
	}
      else if(direction == DOWN)
	{
	  routePt->getTop()->addNet(mazeNet);
	}
      else if(direction == LEFT)
	{
	  routePt->getRight()->addNet(mazeNet);
	}
      else if(direction == RIGHT)
	{
	  routePt->getLeft()->addNet(mazeNet);
	}

      currentX = nextX;
      currentY = nextY;
      length--;
      routePt->setRouteHere();
      routePt->clearCoordinate();
    }
  if(isSource(currentX,currentY-1))
    {
      nextX = currentX;
      nextY = currentY-1;
      direction = UP;
    }
  
  if(isSource(currentX, currentY+1) )
    {
      nextX = currentX;
      nextY = currentY+1;
      direction = DOWN;
    }
	
  if(isSource(currentX-1, currentY))
    {
      nextX = currentX-1;
      nextY = currentY;
      direction = LEFT;
    }  
  if(isSource(currentX+1, currentY))
    {
      nextX = currentX+1;
      nextY = currentY;
      direction = RIGHT;
    }
  MazePoint *routePt = getMazePointAt(nextX, nextY);
  if(direction == UP)
    {
      routePt->getBottom()->addNet(mazeNet);
    }
  else if(direction == DOWN)
    {
      routePt->getTop()->addNet(mazeNet);
    }
  else if(direction == LEFT)
    {
      routePt->getRight()->addNet(mazeNet);
    }
  else if(direction == RIGHT)
    {
      routePt->getLeft()->addNet(mazeNet);
    }

  routePt->setRouteHere();

  //clear the necessary data structures
  MazePoint *routePoint;
  while(!costQ->empty() )
    {
      routePoint = costQ->top();
      costQ->pop();
      if(!routePoint->isRoute())
	routePoint->clearPoint();
    }
  while(!possibleRoutePts->empty() )
    {
      routePoint = possibleRoutePts->front();
      possibleRoutePts->pop_front();
      if(!routePoint->isRoute())
        routePoint->clearPoint();
    }
  
}    

bool Maze::routeNet(Net *theNet)
{
  if(theNet->getNumPins() < 2 || theNet->getNumPins() > 50)
    return true;

  theNet->removeConnectedFromPins();
  mazeNet = theNet;

  //find the Steiner tre
  PrimMST pmst(theNet);
  pmst.findMST();
  PinVector *parentPins = pmst.getParentPins();
  PrimOrderVector *pinOrder = pmst.getPinOrder();

  //route the edges of the net
  bool routed = true;
  for(PrimOrderVectorItr i = pinOrder->begin(); i != pinOrder->end(); i++)
    {
      Pin *nextRtPin = (*i)->currentPin;
      Pin *parentPtPin = (*i)->parent;
      if(!findRoute(nextRtPin, parentPtPin))
	routed = false;
      nextRtPin->setConnected();
    }
  return routed;
}

//routes the net if the steiner points are already specified
bool Maze::routeSteinerNet(Net *theNet)
{
  theNet->removeConnectedFromPins();
  if(!theNet->isSteinerTreeMade())
    theNet->makeSteinerTree();
  //only route nets with  2 < pins num < 50 
  bool routed = true;
  if(theNet->getNumPins() < 2 || theNet->getNumPins() > 50)
    return routed;
  mazeNet = theNet;
  for(int i = 0; i < mazeNet->getNumSteinerEdges(); i++)
    {
      PinTuple tuple = mazeNet->getSteinerEdgeAt(i);
      Pin *sourcePin, *sinkPin;
      //sink pin must not already be connected or routing will stop immediately
      if(!tuple.one->isConnected())
	{
	  sourcePin = tuple.one;
	  sinkPin = tuple.two;
	}
      else if(!tuple.two->isConnected())
	{
	  sourcePin = tuple.two;
	  sinkPin = tuple.one;
	}
      if(!findRoute(sourcePin,sinkPin))
	return false;
      sourcePin->print();
      sinkPin->print();
      cout << endl;
      tuple.one->setConnected();
      tuple.two->setConnected();
    }
  printRouting(mazeNet);
  return routed;
}

void Maze::clearMaze()
{
  for(int yPos = 0; yPos < y; yPos++)
    {
      for(int xPos = 0; xPos < x; xPos++)
	{
	  getMazePointAt(xPos,yPos)->clearPoint();
	}
    }
  while(!routePts->empty())
    routePts->pop_front();
  while(!costQ->empty())
    costQ->pop();
  sinkFound = false;
}
	  

void Maze::print()
{
  for(int yPos = 0; yPos < y; yPos++)
    {
      for(int xPos = 0; xPos < x; xPos++)
	{
	  cout << setw(3) << valueAt(xPos,yPos) << " ";
	}
      cout << endl;
    }
  cout << endl;
}

void Maze::printMazeCost()
{
  for(int yPos = 0; yPos < y; yPos++)
    {
      for(int xPos = 0; xPos < x; xPos++)
	{
	  cout << setw(3) << mazeRouteValueAt(xPos,yPos) << " ";
	}
      cout << endl;
    }
  cout << endl;
}

void Maze::printSourceSink()
{
  cout << "sink is ";
  mySink->print();
  cout << endl << "source is ";
  source->print();
  cout << endl;
}

void Maze::printRouting(Net *aNet)
{
  MazeVector prettyMaze(x);
  int xPlaceholder = 0;
  int yPlaceholder = 0;
  for(MazeVectorItr i = prettyMaze.begin(); i != prettyMaze.end(); i++)
    {
      (*i) = new ColumnVector(y);
      
      for(ColumnVectorItr j = (*i)->begin(); j != (*i)->end(); j++)
	{
	  *j = new MazePoint(xPlaceholder,yPlaceholder);
	  yPlaceholder++;
	}
      xPlaceholder++;
      yPlaceholder = 0;
    }

  EdgeList *edgelist = aNet->getEdgeList();
  
  for(EdgeListItr i = edgelist->begin(); i != edgelist->end(); i++)
    {
      EdgeHolder *edge = (*i);
      if(edge->horizontal)
	{
	  int y = edge->trackNum;
	  int x1 = edge->edgeNum;
	  int x2 = edge->edgeNum+1;
	  (*prettyMaze[x1])[y]->setPointType(2);
	  (*prettyMaze[x2])[y]->setPointType(2);
	}
      else
	{
	  int x = edge->trackNum;
	  int y1 = edge->edgeNum;
	  int y2 = edge->edgeNum+1;
	  (*prettyMaze[x])[y1]->setPointType(2);
	  (*prettyMaze[x])[y2]->setPointType(2);
	}
    }

  
  for(int i = 0; i < aNet->getNumPins(); i++)
    {
      Pin *aPin = aNet->getPin(i);
      (*prettyMaze[aPin->getX()])[aPin->getY()]->setPointType(1);
    }

  for(MazeListItr i = routePts->begin(); i != routePts->end(); i++)
    {
      
     (*prettyMaze[(*i)->getX()])[(*i)->getY()]->setPointType(2);
    }

  
			
  for(int yPos = 0; yPos < y; yPos++)
    {
      for(int xPos = 0; xPos < x; xPos++)
	{
	  cout << setw(3) << (*prettyMaze[xPos])[yPos]->getPointType();
	}
      cout << endl;
    }
  cout << endl;

  //need to clean up
  for(MazeVectorItr i = prettyMaze.begin(); i != prettyMaze.end(); i++)
    {
      for(ColumnVectorItr j = (*i)->begin(); j != (*i)->end(); j++)
	delete (*j);
      
      delete (*i);
    }
}

MazePoint * Maze::getCostToRoute(MazePoint *mPoint, int sourceDist)
{

  //find the cheapest cost to a route point
  int lowestCost = x + y + 10; //larger than any possible route
  MazePoint *closestPt = 0;
  for(MazeListItr i = routePts->begin(); i != routePts->end(); i++)
    {
      if(mPoint->calculateCost((*i), sourceDist) < lowestCost)
	closestPt = *i;
    }

  
  for(int j = 0; j < mazeNet->getNumPins(); j++)
    {
      if(mazeNet->getPin(j)->calculateCost(mPoint, sourceDist) < lowestCost
	 && mazeNet->getPin(j)->isConnected() )
	closestPt = getMazePointAt
	  (mazeNet->getPin(j)->getX(), mazeNet->getPin(j)->getY());
    }
    
  return closestPt;
}
