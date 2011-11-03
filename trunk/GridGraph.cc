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

#include "GridGraph.h"

//--------------------------GridVertex class------------------------------

GridVertex::GridVertex():Vertex()
{ }

GridVertex::GridVertex(char *name):Vertex(name)
{ }

GridVertex::GridVertex(Pin *thePin)
{
  pin = thePin;
}

GridVertex::GridVertex(char *name, Pin *thePin):Vertex(name)
{
  pin = thePin;
}
  
Pin * GridVertex::getPin()
{
  return pin;
}

void GridVertex::print()
{
  cout << "---------GridVertex------------" << endl;
  pin->print();
  Vertex::print();
  cout << "---------GridVertex------------" << endl;
}

//----------------------GridGraph class-------------------------------------

GridGraph::GridGraph():Graph()
{ }

GridGraph::GridGraph(int num):Graph(num)
{ }

GridGraph::~GridGraph()
{ }

GridVertex * GridGraph::getVertexWithName(char *name)
{
  Vertex * temp =  Graph::getVertexWithName(name);
  return (GridVertex *) temp;
}

GridVertex * GridGraph::getVertexAt(int index)
{
  return (GridVertex *) Graph::getVertexAt(index);
}

GridVertex * GridGraph::getVertexWithPin(Pin * aPin)
{
  GridVertex *returnVert = NULL;
  for(int i = 0; i < getNumVertices(); i++)
    {
      if(getVertexAt(i)->getPin() == aPin)
	returnVert = getVertexAt(i);
    }
  return returnVert;
}

void GridGraph::addEdgeBetween(Pin *one, Pin *two)
{
  //search for the edge with this pin
  Vertex *vertOne = NULL;
  Vertex *vertTwo = NULL;
  for(int i = 0; i < getNumVertices(); i++)
    {
      if(getVertexAt(i)->getPin() == one)
	vertOne = getVertexAt(i);
      if(getVertexAt(i)->getPin() == two)
	vertTwo = getVertexAt(i);
    }

  if(vertOne != NULL && vertTwo != NULL)
    Graph::addEdgeBetween(vertOne, vertTwo);
  else
    cout << "can not find vertex with specified pin in function" << 
      "GridGraph::addEdgeBetween(Pin *one, Pin *two)" << endl;
}

void GridGraph::addEdgeBetween(Pin *one, Pin *two, char *name)
{
  GridVertex * vertOne = getVertexWithPin(one);
  GridVertex * vertTwo = getVertexWithPin(two);
  
  if(vertOne != NULL && vertTwo != NULL)
    Graph::addEdgeBetween(vertOne,vertTwo,name);
  else
    cout << "can not find vertex with specified pin in function" << 
      "GridGraph::addEdgeBetween(Pin *one, Pin *two, char *name)" << endl;

}

PinTuple GridGraph::getEdgePinsAt(int position)
{
  Edge *edge = Graph::getEdgeAt(position);
  PinTuple returnTuple;

  returnTuple.one = ((GridVertex *) edge->getVertexAt(0))->getPin();
  returnTuple.two = ((GridVertex *) edge->getVertexAt(1))->getPin();

  return returnTuple;
}

void GridGraph::print()
{
  
  Graph::print();
  for(int i = 0; i < getNumVertices(); i++)
    {
      GridVertex * gridVert = getVertexAt(i);
      cout << gridVert->getLabel();
      gridVert->getPin()->print();
      cout << endl;
    }
} 
