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

#include "Graph.h"
#include <string.h>
#include <iostream>
#include <sstream>
#include <cstring>

using namespace std;

//------------------------Vertex functions--------------------------------------

Vertex::Vertex()
{
  edgeList = new GraphEdgeList();
  strcpy(label,"unnamed");
}

Vertex::Vertex(char *name)
{
   edgeList = new GraphEdgeList();
   strcpy(label,name);
}

Vertex::~Vertex()
{
  delete edgeList;
}

void Vertex::addEdge(Edge *e)
{
  edgeList->push_front(e);
}

void Vertex::print()
{
  int counter = 0;
  cout << "Vertex name:" << label << endl;
  for(GraphEdgeListItr i = edgeList->begin(); i != edgeList->end(); i++)
    {
      cout << "    Edge " << counter << " connected to:";
      (*i)->print();
      counter++;
    }
}

//------------------------Edge functions--------------------------------------

Edge::Edge()
{
  vertexList = new list<Vertex *>();
}

Edge::Edge(char *name)
{
  vertexList = new list<Vertex *>();
  strcpy(label,name);
}

Edge::~Edge()
{
  delete vertexList;
}

void Edge::addVertex(Vertex *v)
{
  vertexList->push_front(v);
}

int Edge::getNumberVertices()
{
  return vertexList->size();
}

Vertex * Edge::getVertexAt(int position)
{
  int counter = 0;
  Vertex * returnVert = NULL;
  for(GraphVertexListItr i = vertexList->begin(); i != vertexList->end(); i++)
    {
      if(position == counter)
	returnVert = (*i);
      counter++;
    }
  return returnVert;
}


void Edge::print()
{
  for(GraphVertexListItr i = vertexList->begin(); i != vertexList->end(); i++)
    {
      cout << (*i)->getLabel() << " ";
    }
  cout << endl;
}

//------------------------Graph functions--------------------------------------

Graph::Graph()
{
  numVertices = numEdges = 0;
  vertices = new GraphVertexVect();
  edges = new GraphEdgeList();
}

Graph::Graph(int numVert)
{
  int j = 0;
  numVertices = numVert;
  numEdges = 0;
  vertices = new GraphVertexVect(numVertices);
  
  for(GraphVertexVectItr i = vertices->begin(); i != vertices->end(); i++)
    {
      ostringstream outputString;
      char* temp;
      (*i) = new Vertex();
      
      outputString << "vertex" << j << ends;
      temp = (char*)outputString.str().c_str();
      (*i)->setLabel(temp);
      j++;
    }
  edges = new GraphEdgeList();
}

Graph::~Graph()
{
  //clean up the vertices
  for(GraphVertexVectItr i = vertices->begin(); i != vertices->end(); i++)
    {
      delete (*i);
    }
  delete vertices;

  //clean up the edges
  for(GraphEdgeListItr i = edges->begin(); i != edges->end(); i++)
    delete (*i);
  delete edges;
}

void Graph::addEdgeBetween(int v1, int v2)
{
  Edge *e = new Edge();
  edges->push_front(e);
  numEdges++;

  e->addVertex((*vertices)[v1]);
  e->addVertex((*vertices)[v2]);

  (*vertices)[v1]->addEdge(e);
  (*vertices)[v2]->addEdge(e);
}

void Graph::addEdgeBetween(int v1, int v2, char *name)
{
  Edge *e = new Edge(name);
  edges->push_front(e);
  numEdges++;

  e->addVertex((*vertices)[v1]);
  e->addVertex((*vertices)[v2]);

  (*vertices)[v1]->addEdge(e);
  (*vertices)[v2]->addEdge(e);
}

void Graph::addEdgeBetween(Vertex *v1, Vertex *v2)
{
  Edge *e = new Edge();
  edges->push_front(e);
  numEdges++;

  e->addVertex(v1);
  e->addVertex(v2);

  v1->addEdge(e);
  v2->addEdge(e);
}

void Graph::addEdgeBetween(Vertex *v1, Vertex *v2, char *name)
{
  Edge *e = new Edge(name);
  edges->push_front(e);
  numEdges++;

  e->addVertex(v1);
  e->addVertex(v2);

  v1->addEdge(e);
  v2->addEdge(e);
}

void Graph::addDirectedEdgeBetween(int v1, int v2)
{
  Edge *e = new Edge();
  edges->push_front(e);
  numEdges++;

  e->addVertex((*vertices)[v1]);
  e->addVertex((*vertices)[v2]);

  (*vertices)[v1]->addEdge(e);
}

void Graph::addDirectedEdgeBetween(int v1, int v2, char *name)
{
  Edge *e = new Edge(name);
  edges->push_front(e);
  numEdges++;

  e->addVertex((*vertices)[v1]);
  e->addVertex((*vertices)[v2]);

  (*vertices)[v1]->addEdge(e);
}

void Graph::addDirectedEdgeBetween(Vertex *v1, Vertex *v2)
{
  Edge *e = new Edge();
  edges->push_front(e);
  numEdges++;

  e->addVertex(v1);
  e->addVertex(v2);

  v1->addEdge(e);
}

void Graph::addDirectedEdgeBetween(Vertex *v1, Vertex *v2, char *name)
{
  Edge *e = new Edge(name);
  edges->push_front(e);
  numEdges++;

  e->addVertex(v1);
  e->addVertex(v2);

  v1->addEdge(e);
}

void Graph::addVertex(Vertex *v)
{
  vertices->push_back(v);

  numVertices++;
}

Vertex * Graph::getVertexWithName(char *name)
{
  for(GraphVertexVectItr i = vertices->begin(); i != vertices->end(); i++)
    {
      if(strcmp((*i)->getLabel(),name) == 0)
	return (*i);
    }

  return NULL;
}

int Graph::getVertexPositionWithName(char *name)
{
  int position = 0;
  for(GraphVertexVectItr i = vertices->begin(); i != vertices->end(); i++)
    {
      if(strcmp((*i)->getLabel(),name) == 0)
	return position;
      position++;
    }
  return -1;
}

int Graph::getVertexPosition(Vertex *vert)
{
  int position = 0;
  for(GraphVertexVectItr i = vertices->begin(); i != vertices->end(); i++)
    {
      if((*i) == vert)
	return position;
      position++;
    }
  return -1;
}

Vertex * Graph::getVertexAt(int position)
{
  if(position < vertices->size() && position >= 0)
    return (*vertices)[position];

  else
    {
      cout << "Invalid position number.  Returning NULL" << endl;
      return NULL;
    }
}

Edge * Graph::getEdgeAt(int position)
{
  int counter = 0;
  Edge * returnEdge = NULL;
  for(GraphEdgeListItr i = edges->begin(); i != edges->end(); i++)
    {
      if(counter == position)
	returnEdge = *i;
      counter++;
    }

  return returnEdge;
}

int Graph::getNumEdges()
{
  int returnValue = 0;
  for(GraphEdgeListItr i = edges->begin(); i != edges->end(); i++)
    {
      returnValue++;
    }
  return returnValue;
}

GraphVertexVect * Graph::dfs(Vertex *startVert)
{
  vector<bool> visitedVertex(numVertices, false);
  GraphVertexList queue;
  GraphVertexVect * ordering = new GraphVertexVect();

  queue.push_front(startVert);
  
  Vertex *currentVert;
  
  //int orderingPosition = 0;

  while(!queue.empty())
    {
      //explore children of next vertex on queue
      currentVert = queue.back();
      queue.pop_back();
      
      //set this vertex in it's proper order
      ordering->push_back(currentVert);

      int vertPosition = getVertexPosition(currentVert);

      //set this vertex as visited
      visitedVertex[vertPosition] = true;

      //look at the neighbors of this vertex
      for(GraphEdgeListItr i = currentVert->edgeList->begin(); 
	  i != currentVert->edgeList->end(); i++)
	{
	  Edge * vertEdge = (*i);
	  for(GraphVertexListItr j = vertEdge->vertexList->begin(); 
	      j != vertEdge->vertexList->end(); j++)
	    {
	      Vertex * edgeVert = (*j);
	      int edgeVertPos = getVertexPosition(edgeVert);

	      //if this neighbor is not already visited, add it to queue
	      if(!visitedVertex[edgeVertPos])
		{
		  visitedVertex[edgeVertPos] = true;
		  queue.push_back(edgeVert);
		}
	    }
	}
	  
      
    }

  return ordering;

}

int Graph::getOutDegree(Vertex *aVert)
{
  return aVert->edgeList->size();
}


void Graph::print()
{
  for(GraphVertexVectItr i = vertices->begin(); i != vertices->end(); i++)
    {
      (*i)->print();
      cout << endl;
    }
}
