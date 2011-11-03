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

#ifndef GRIDGRAPH_H
#define GRIDGRAPH_H

#include <iostream>

#include "Graph.h"
#include "Pin.h"

using namespace std;

class GridGraph;

typedef struct
{
  Pin *one;
  Pin *two;
} PinTuple;

//----------------------GridVertex Class-----------------------------------
class GridVertex: public Vertex
{
  
public:
  GridVertex();
  GridVertex(char *name);
  GridVertex(Pin *pin);
  GridVertex(char *name, Pin *thePin);
  ~GridVertex();

  //accessors
  Pin * getPin();

  void print();
  
private:
  Pin *pin;

  //JW friend GridGraph;
  friend class GridGraph;//JW
};


//----------------------GridGraph class-------------------------------------
class GridGraph: public Graph
{
  
public:
  GridGraph();
  GridGraph(int);
  ~GridGraph();

  GridVertex * getVertexWithName(char *);
  GridVertex * getVertexAt(int);
  GridVertex * getVertexWithPin(Pin *);

  void addEdgeBetween(Pin *, Pin *);
  void addEdgeBetween(Pin *, Pin *, char *);

  PinTuple getEdgePinsAt(int position);

  
  
  void print();

};

#endif
