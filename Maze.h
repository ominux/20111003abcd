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

#ifndef MAZE_H
#define MAZE_H

#include "MazePoint.h"
#include "Net.h"
#include "PrimMST.h"
#include "RouteTracks.h"
#include <iostream>
#include <vector>
#include <queue>
#include <list>

using namespace std;

class PointCompare;

typedef vector<MazePoint *> ColumnVector;
typedef ColumnVector::iterator ColumnVectorItr;
typedef vector<ColumnVector *> MazeVector;
typedef MazeVector::iterator MazeVectorItr;
typedef priority_queue<MazePoint *,vector<MazePoint *>,PointCompare> PriorityQ;
typedef list<MazePoint *> MazeList;
typedef MazeList::iterator MazeListItr;

class Maze
{

 public:
  
  //constructors
  Maze();
  Maze(int, int);
  Maze(int, int, RouteTracks *);
  ~Maze();

  //editors
  inline void putObstacle(int xValue, int yValue)
  { 
    if(isValid(xValue, yValue) )
      getMazePointAt(xValue,yValue)->setObstacle(); 
  }
  
  inline void putSink(int xValue, int yValue)
  {  
    if(isValid(xValue, yValue) )
      {
	getMazePointAt(xValue,yValue)->setSink();
	getMazePointAt(xValue,yValue)->setRouteHere();
	delete mySink;
	mySink = new Point(xValue, yValue); 
      }
  }

  inline void putSink(Point *sinkPt){ putSink(sinkPt->getX(),sinkPt->getY()); }
  
  inline void putSource(int xValue, int yValue)
  { 
    if(isValid(xValue, yValue) )
      {
	getMazePointAt(xValue,yValue)->setSource(); 
	delete source;
	source = new Point(xValue, yValue);
      }
  }

  inline void putSource(Point *sourcePt) 
  { putSource(sourcePt->getX(), sourcePt->getY()); }
  
  inline void setCostFnToRoutePts() { costFnToRoutePts = true; }
  //inline void setUseCongestionCost(bool setIt) { useCongestionCost = setIt; }

  //accessors
  inline MazePoint * getMazePointAt(int xValue, int yValue)
  {
    if(!isValid(xValue,yValue))
      return 0;
    return (*(*maze)[xValue])[yValue]; 
  }
  inline int valueAt(int xValue, int yValue)
  { 
    if(!isValid(xValue,yValue))
      return 0;
    return getMazePointAt(xValue,yValue)->getPointType(); 
  }
  inline int mazeRouteValueAt(int xValue, int yValue)
    {
      if(!isValid(xValue,yValue))
	return 0;
      return getMazePointAt(xValue,yValue)->getMazeRouteCost();
    }
  inline bool isSource(int xValue, int yValue)
  { 
    if(!isValid(xValue,yValue))
      return false;
    return getMazePointAt(xValue,yValue)->isSource(); 
  }

  inline bool isSink(int xValue, int yValue)
  { 
    if(!isValid(xValue,yValue))
      return false;
    return getMazePointAt(xValue,yValue)->isSink();
  }
  
  inline bool isObstacle(int xValue, int yValue)
  { 
    if(!isValid(xValue,yValue))
      return false;
    return getMazePointAt(xValue,yValue)->isObstacle();
  }
  
  inline int getLength(int xValue, int yValue)
  { 
    if(!isValid(xValue,yValue))
      return 0;
    return getMazePointAt(xValue,yValue)->getPointType(); 
  }  

  inline MazeList * getRoutePoints() { return routePts; }
  inline int getX() { return x; }
  inline int getY() { return y; }

  //misc
  inline bool isValid(int xValue, int yValue)
    { return xValue < x && xValue >= 0 && yValue >= 0 && yValue < y; }
  bool findRoute();
  bool findRoute(Point *sourcePt, Point *sinkPt);
  void pickRoute(MazePoint *);
  void pickRoute(int);
  bool routeNet(Net *);
  bool routeSteinerNet(Net *);
  void clearMaze();
  void print();
  void printRouting(Net *);
  void printSourceSink();
  void printMazeCost();
  

 private:
  int x, y;
  bool sinkFound;
  MazeVector *maze;
  Point *source;
  Point *mySink;
  PriorityQ *costQ;
  MazeList *possibleRoutePts;
  MazeList *routePts;
  Net *mazeNet;
  RouteTracks *routeTracks;
  
  //maze routing switches
  bool costFnToRoutePts;
  //bool useCongestionCost;
  static int count;

  //private fns
  MazePoint * getCostToRoute(MazePoint *, int);
  
};

class PointCompare
{
public:
  bool operator() (MazePoint *, MazePoint *);
};

#endif









