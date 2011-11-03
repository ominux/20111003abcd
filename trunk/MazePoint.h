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

#ifndef MAZEPOINT_H
#define MAZEPOINT_H

#include "Point.h"
#include "RouteEdge.h"

#define ROUTECOST 20

class MazePoint : public Point
{

public:
  //constructors
  inline MazePoint() : Point() 
    { pointType = 0; cost = 0; OBSTACLE = -1; SINK = -2; SOURCE = -3; 
    isRouteHere = false; mazeRouteCost = 0; top = bottom = left = right = 0;
    routeLength = 0; parent = 0;}
  inline MazePoint(int xValue, int yValue) : Point(xValue,yValue) 
  { pointType = 0; cost = 0; OBSTACLE = -1;  SINK = -2; SOURCE = -3; 
  isRouteHere = false; mazeRouteCost = 0; top = bottom = left = right = 0; 
  routeLength = 0; parent = 0;}
  inline ~MazePoint() { }

  
  //modifiers
  inline void setObstacle() { pointType = OBSTACLE; }
  inline void setSink() { pointType = SINK; }
  inline void setSource() { pointType = SOURCE; }
  inline void setPointType(int ptype) { pointType = ptype; }
  inline void setCost(Point *sink, int sourceDist)
  { cost = mazeRouteCost = getManhattanDistance(sink) + sourceDist; }
  inline void setExplicitCost(int exCost)
  { cost = exCost; }
  inline void setRouteHere() { isRouteHere = true; }
  inline void clearPoint() { isRouteHere = false; pointType = 0; cost = 0; }
  inline void clearCoordinate() { pointType = 0; cost = 0; mazeRouteCost = 0;}
  inline void setMazeRouteCost(Point *sink, int sourceDist, int routeCost)
  { setCost(sink,sourceDist); mazeRouteCost = routeCost; }
  inline void setTop(RouteEdge *edge) { top = edge; }
  inline void setBottom(RouteEdge *edge) { bottom = edge; }
  inline void setLeft(RouteEdge *edge) { left = edge; }
  inline void setRight(RouteEdge *edge) { right = edge; }
  inline void setRouteLength(int length) { routeLength = length; }
  inline void setParent(MazePoint *pt) { parent = pt; }
    

  //accessors 
  inline int getPointType() { return pointType; }
  inline bool isObstacle() { return pointType == OBSTACLE; }
  inline bool isSink() { return pointType == SINK || isRouteHere; }
  inline bool isSource() { return pointType == SOURCE; }
  inline int getCost() { return cost; }
  inline int calculateCost(Point *sink, int sourceDist)
  { cost = getManhattanDistance(sink) + sourceDist; return cost; }
  inline bool isExplored() { return pointType != 0; }
  inline int getObstacle() { return OBSTACLE; }
  inline int getSink() { return SINK; }
  inline int getSource() { return SOURCE; }
  inline bool isRoute() { return isRouteHere; }
  inline int getMazeRouteCost() { return mazeRouteCost*ROUTECOST + cost; }
  inline int getRouteCost() { return mazeRouteCost; }
  inline RouteEdge * getTop() { return top; }
  inline RouteEdge * getBottom() { return bottom; }
  inline RouteEdge * getLeft() { return left; }
  inline RouteEdge * getRight() { return right; }
  inline int getRouteLength() { return routeLength; }
  inline MazePoint* getParent() { return parent; }
  inline int calculateMazeRouteCost(Point *sink, int sourceDist, int routeCost)
    { 
      return getManhattanDistance(sink) + sourceDist + ROUTECOST*(routeCost); 
    }

 private:
  int pointType;
  int routeLength;
  int cost;
  bool isRouteHere;
  int mazeRouteCost;
  MazePoint *parent;
  RouteEdge *top;
  RouteEdge *left;
  RouteEdge *right;
  RouteEdge *bottom;
  
  //static variables
  int OBSTACLE;
  int SINK;
  int SOURCE;
};


#endif
