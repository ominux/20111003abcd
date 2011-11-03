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

#ifndef ROUTEEDGE_H
#define ROUTEEDGE_H

#include "Net.h"
#include <list>

typedef list<Net *> NetList;
typedef NetList::iterator NetListItr;

class RouteEdge
{
  
public:
  //constructors
  RouteEdge();
  RouteEdge(bool horiz,int row_or_col,int edge_num);
  ~RouteEdge();

  //accessors
  inline bool isHorizontal() const { return horizontal; }
  inline bool isVertical() const { return !horizontal; }
  inline int getEdgeNum() const { return edgeNum; }
  inline int getTrackNum() const { return trackNum; }
  inline int getNumNets() const { return numNets; }
  inline NetList * getNetList() { return netList; }
  
  //editors
  void addNet(Net *aNet);
  void removeNet(Net *);
  inline void resetNumNets() { numNets = netList->size(); }

  //misc
  void print();

private:
  bool horizontal;
  int trackNum;
  int edgeNum;
  int numNets;
  NetList *netList;

};

#endif
