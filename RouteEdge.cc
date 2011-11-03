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

#include "RouteEdge.h"
#include <iostream>

using namespace std;

RouteEdge::RouteEdge()
{
  horizontal = true;
  trackNum = edgeNum = 0;
  netList = new NetList();
}

RouteEdge::RouteEdge(bool horiz, int row_or_col, int edge_num)
{
  horizontal = horiz;
  trackNum = row_or_col;
  edgeNum = edge_num;
  netList = new NetList();
}

RouteEdge::~RouteEdge()
{
  delete netList;
}

void RouteEdge::addNet(Net *aNet)
{ 
  netList->push_front(aNet); 
  numNets++; 
}

void RouteEdge::removeNet(Net *aNet)
{
  NetListItr sameNet = netList->end();
  for(NetListItr i = netList->begin(); i != netList->end(); i++)
    {
      if(*i == aNet)
	sameNet = i;
    }
  if(sameNet != netList->end())
    {
      numNets--;
      netList->erase(sameNet);
    }
}

void RouteEdge::print()
{
  if(horizontal)
    cout << "horizontal edge ";
  else
    cout << "vertical edge ";
  cout << "on track " << trackNum << " and edge " << edgeNum << endl;
}
