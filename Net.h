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

#ifndef NET_H
#define NET_H

#include <vector>
#include <list>
#include <string.h>
#include <iostream>
#include <algorithm>
#include <fstream>

#include "Pin.h"
#include "Box.h"
#include "Segment.h"
#include "Graph.h"
#include "SteinerTree.h"

using namespace std;

class EdgeHolder
{
public:
  inline EdgeHolder() { horizontal = true; trackNum = edgeNum = 0; }
  inline EdgeHolder(bool horiz, int row_or_col, int edge_num)
    { horizontal = horiz; trackNum = row_or_col; edgeNum = edge_num; }
  int operator==(const EdgeHolder &) const;
  void print();
  bool horizontal;
  int trackNum;
  int edgeNum;
};

typedef list<EdgeHolder *> EdgeList;
typedef EdgeList::iterator EdgeListItr;
typedef vector<Pin *> PinVector;
typedef PinVector::iterator PinVectorItr;
typedef vector<Segment> SegmentVector;
typedef vector<Segment> SegmentVectorItr;

class Net
{

public:
  //constructors
  inline Net() 
    { pinVect = new PinVector(); edgeList = new EdgeList(); permanent = false;
    boundingBox = 0; steinerTree = new SteinerTree();
    }
  inline Net(int numPins) 
    { pinVect = new PinVector(numPins,(Pin *) 0); edgeList = new EdgeList();
    permanent = false; boundingBox = 0; steinerTree = new SteinerTree(numPins);
    }
  inline Net(int numPins, int netID, char *netName)
    { setID(netID); setName(netName); pinVect = new PinVector(numPins); 
    edgeList = new EdgeList(); permanent = false; boundingBox = 0;
    steinerTree = new SteinerTree(numPins);
      }
  inline ~Net()
    { 
      for(PinVectorItr i = pinVect->begin(); i != pinVect->end(); i++)
	delete (*i);
      delete pinVect;
      for(EdgeListItr i = edgeList->begin(); i != edgeList->end(); i++)
	delete (*i);
      delete edgeList;
      delete steinerTree;
    }

  //accessors
  inline Pin * getPin(int pinNum) { return (*pinVect)[pinNum]; }
  inline int getNumPins() { return pinVect->size(); }
  inline int getID() { return id; }
  inline char * getName() { return name; }
  inline PinVector * getPinVector() { return pinVect; }
  inline int getNumSteinerPoints() { return steinerTree->numSteinerPoints(); }
  int getNumSteinerEdges();
  PinTuple getSteinerEdgeAt(int position);
  inline EdgeList * getEdgeList() { return edgeList; }
  inline int getNumEdges() { return edgeList->size(); }
  inline bool isPermanent() { return permanent; }
  Box * getBoundingBox();
  void calculateBoundingBox(); //must be called after new pin insertion for correct result
  SegmentVector * getUpperLSegments();
  SegmentVector * getLowerLSegments();
  bool isSteinerTreeMade();
  void makeSteinerTree();
  void makeUnflexibleSteinerTree();

  //modifiers
  void setPin(int pinNum, Pin * thePin);
  inline void setID(int newID) { id = newID; }
  inline void setName(char *newName) { strcpy(name,newName); }
  void addEdge(bool horiz, int trackNum, int edgeNum);
  void addEdgesBetween(bool horiz, int trackNum, int start, int end);
  void removeEdge(bool horiz, int trackNum, int edgeNum);
  void removeAllEdges();
  inline void setPermanent() { permanent = true; }
  void removeConnectedFromPins();

  //misc
  void removeLastPin();
  void printEdges();
  void printPins();
  void printEdges(ofstream &stream);
  bool isFlexible();
  void makeFlexibleSteinerTree();
  

private:
  PinVector * pinVect;
  SteinerTree * steinerTree; 
  int id;
  char name[64];
  EdgeList *edgeList;
  bool permanent;
  Box *boundingBox;
  
};


#endif
