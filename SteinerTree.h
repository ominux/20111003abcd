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

#ifndef STEINERTREE_H
#define STEINERTREE_H

#include <algorithm>
#include <string.h>
#include <iostream>
#include <stdio.h>

#include "GridGraph.h"
#include "Pin.h"

using namespace std;

typedef vector<Pin *> PinVector;
typedef PinVector::iterator PinVectorItr;

class PinXSortSmallToLarge
{
public:
  inline bool operator()(Pin *pin1, Pin *pin2)
    { return pin1->getX() < pin2->getX(); }
};
 
class PinYSortSmallToLarge
{
public:
  inline bool operator() (Pin *pin1, Pin *pin2)
    { return pin1->getY() < pin2->getY(); }
};

class SteinerTree 
{

public:
  //con(de)structors
  SteinerTree();
  SteinerTree(int numPins);
  SteinerTree(PinVector *thePins);
  SteinerTree(PinVector *thePins, PinVector *steinerPts);
  ~SteinerTree();

  //modifiers
  void addPin(Pin *aPin);
  void setPinAt(int pinNum, Pin * thePin);
  void removeLastPin();

  //accessors
  int numPins();
  int numSteinerPoints();
  Pin * getPin(int index);
  Pin * getSteinerPoint(int index);
  double getFlexibility();
  PinTuple getEdgePinsAt(int position);
  int getNumTreeEdges();
  bool isTreeMade();

  //misc
  int isFlexible();
  void makeSteinerTree();
  void findFlexibleTree();
  void findUnflexibleTree();
  void clearSteinerPoints();
 
private:
  GridGraph * steinerGraph;
  PinVector * pins;
  PinVector * steinerPoints;
  double flexibility;
  bool treeAvailable;
};

#endif
