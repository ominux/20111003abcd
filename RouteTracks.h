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

#ifndef ROUTETRACKS_H
#define ROUTETRACKS_H

#include "RouteEdge.h"
#include "MazePoint.h"
#include "PrimMST.h"
#include <vector>
#include <list>
#include <limits.h>
#include <iostream>
#include <fstream>

using namespace std;

typedef vector<RouteEdge *> RouteEdgeVector;
typedef RouteEdgeVector::iterator RouteEdgeVectorItr;
typedef vector<RouteEdgeVector *> RouteTrackVector;
typedef RouteTrackVector::iterator RouteTrackVectorItr;
typedef list<MazePoint *> MazeList;
typedef MazeList::iterator MazeListItr;

class RouteTracks
{
  
public:
  //constructors
  RouteTracks(int xNum, int yNum);
  ~RouteTracks();

  //accessors
  int isValid(int x, int y);
  RouteEdge * getHorizontalEdgeAt(int x, int y);
  RouteEdge * getVerticalEdgeAt(int x, int y);
  RouteEdge * getEdgeAt(int x, int y, bool horiz);
  RouteEdge * getHorizontalEdgeWith(int trackNumber, int edgeNumber);
  RouteEdge * getVerticalEdgeWith(int trackNumber, int edgeNumber);
  RouteEdge * getEdgeWith(int trackNumber, int edgeNumber, bool horiz);
  inline int getWidth() { return verticalTracks->size(); }
  inline int getHeight() { return horizontalTracks->size(); }
  int getCongestionAt(int x, int y, bool horizontal);
  int getCongestionWith(int trackNumber, int edgeNumber, bool horiz);
  int getOverflowWith(int trackNumber, int edgeNumber, bool horiz);
  int getVerticalCongestionBetween(int trackNum, int begin, int end);
  int getHorizontalCongestionBetween(int trackNum, int begin, int end);
  inline int getRouteLength() const { return routeLength; }
  inline int getHorizontalCapacity() const { return horizontalCapacity; }
  inline int getVerticalCapacity() const { return verticalCapacity; }
  int getOverflow();
  int getWireLength();
  bool isOverflown(int x, int y, bool horiz);
  bool isOverflownWith(int trackNumber, int edgeNumber, bool horiz);
  bool isDensityOneBendRoutable(Net *aNet,int density);
  bool isVerticalDensityRoutableBetween(int trackNum, int begin, int end, int density);
  bool isHorizontalDensityRoutableBetween(int trackNum, int begin, int end, int density);
  

  //editors
  inline void addNetToEdge(int x, int y, bool horiz, Net *aNet)
  { getEdgeAt(x,y,horiz)->addNet(aNet); }
  inline void removeNetFromEdge(int x, int y, bool horiz, Net *aNet)
  { getEdgeAt(x,y,horiz)->removeNet(aNet); }
  inline void addNetToEdgeWith(int track, int edge, bool horiz, Net *aNet)
  { getEdgeWith(track,edge,horiz)->addNet(aNet); }
  inline void removeNetFromEdgeWith(int track, int edge, bool horiz, Net *aNet)
  { getEdgeWith(track,edge,horiz)->removeNet(aNet); }
  inline void setVerticalCapacity(int cap) { verticalCapacity = cap; }
  inline void setHorizontalCapacity(int cap) { horizontalCapacity = cap; }
  void addHorizontalSegment(Net *,int trackNum, int begin, int end);
  void addVerticalSegment(Net *,int trackNum, int begin, int end);
  void removeRoutingFromEdges(Net *);

  //misc
  NetList * ripNets();
  NetList * oldRipNets();
  void printVerticalTracks();
  void printHorizontalTracks();
  void print();
  void printCongestionDisplayFile(ofstream &stream);
  void routeOneBendNet(Net *aNet);
  void findOneBendRoute(Net *aNet,Pin *,Pin *);
  void zRouteNet(Net *);
  void findZRoute(Net *,Pin *,Pin *);
  void routeBBNet(Net *aNet);
  int findRoutingOverflow(Net *aNet);
  int findRoutingCongestion(Net *aNet);
  bool patternRouteSteinerNet(Net *);
  bool zRouteSteinerNet(Net *);

private:
  RouteTrackVector *horizontalTracks;
  RouteTrackVector *verticalTracks;
  int verticalCapacity;
  int horizontalCapacity;
  int routeLength;

  void findVerticalOverflownEdge(RouteEdge **overflownEdge, 
					      int &numOverflows, bool &foundOverflow); 
  void findHorizontalOverflownEdge(RouteEdge **overflownEdge,
						int &numOverflows, bool &foundOverflow);
};

#endif
