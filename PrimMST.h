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

#ifndef PRIMMST_H
#define PRIMMST_H

#include <queue>
#include <vector>
#include "Net.h"

typedef vector<Net *> NetVector;
typedef NetVector::iterator NetVectorItr;

class PrimOrderHolder
{
public:
  inline PrimOrderHolder(Pin *theCurrentPin, Pin * theParent) 
  { parent = theParent; currentPin = theCurrentPin; }
  inline ~PrimOrderHolder() { }

  Pin *parent;
  Pin *currentPin;
};

typedef vector<PrimOrderHolder *> PrimOrderVector;
typedef PrimOrderVector::iterator PrimOrderVectorItr;

class PrimEdge
{

public:
  inline PrimEdge(Pin *first, Pin *second, int edgeWeight)
  { from = first; to = second; weight = edgeWeight; } 
  inline ~PrimEdge() { }

  Pin *to;
  Pin *from;
  int weight;
};

class PrimCompare
{

public:
  inline bool operator() (PrimEdge *first, PrimEdge *second)
  { return first->weight > second->weight; }
  inline ~PrimCompare() { }
};

typedef priority_queue<PrimEdge *,vector<PrimEdge*>,PrimCompare> PrimPriorityQ;

class PrimMST
{

public:
  //constructors
  PrimMST(Net *);
  ~PrimMST();

  //accessors
  inline PinVector * getParentPins() { return parentPin; }
  inline PrimOrderVector * getPinOrder() { return pinOrder; }
  
  //misc
  void findMST();
  void twoTerminalNets(NetVector & twoTremNets);

private:
  Net *theNet;
  PinVector * parentPin;
  PrimOrderVector * pinOrder;
  PrimPriorityQ * primPQ;
};

#endif
