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

#ifndef GRAPH_H
#define GRAPH_H

#include <list>
#include <vector>
#include <cstring>
using namespace std;//JW

//Vertex is a container class that represents a vertex of a graph.  
//Edge is a container class for a hyperedge.
//Should be used as a parent for more specific graphs (hopefully it is 
//generic enough)

class Edge;
class Graph;

typedef list<Edge *> GraphEdgeList;
typedef GraphEdgeList::iterator GraphEdgeListItr;

//---------------------------Vertex Class--------------------------------------
class Vertex
{

public:
  Vertex();
  Vertex(char *);
  ~Vertex();

  void addEdge(Edge *);
  inline char *getLabel() { return label; }
  inline void setLabel(char *l) { strcpy(label,l); }

  void print();

private:
  GraphEdgeList * edgeList;
  char label[64];

  //JW friend Graph;
  friend class Graph;//JW
};

typedef list<Vertex *> GraphVertexList;
typedef GraphVertexList::iterator GraphVertexListItr;
typedef vector<Vertex *> GraphVertexVect;
typedef GraphVertexVect::iterator GraphVertexVectItr;

//---------------------------Edge Class--------------------------------------
class Edge 
{

public:
  Edge();
  Edge(char *);
  ~Edge();

  void addVertex(Vertex *);
  inline char * getLabel() { return label; }
  inline void setLabel(char *l) { strcpy(label,l); }
  int getNumberVertices();
  Vertex * getVertexAt(int position);
  
  void print();

private:
  GraphVertexList * vertexList;
  char label[64];

  //JWfriend Graph;
  friend class Graph;//JW

};

//---------------------------Graph Class--------------------------------------
class Graph
{
 
public:
  Graph();
  Graph(int);
  ~Graph();

  void addEdgeBetween(int, int);
  void addEdgeBetween(int, int, char *);
  void addEdgeBetween(Vertex *, Vertex *);
  void addEdgeBetween(Vertex *, Vertex *, char *);

  /* a directed edge differs from an (undirected) edge in that the child node
  /  does not have a pointer to the edge; the edge has a pointer to both parent
  /  parent and child 
  */
  void addDirectedEdgeBetween(int, int);
  void addDirectedEdgeBetween(int, int, char *);
  void addDirectedEdgeBetween(Vertex *, Vertex *);
  void addDirectedEdgeBetween(Vertex *, Vertex *, char *);  

  void addVertex(Vertex *);
  
  Vertex * getVertexWithName(char *);
  int getVertexPositionWithName(char *);
  int getVertexPosition(Vertex *);
  Vertex * getVertexAt(int);

  Edge * getEdgeAt(int position);
  
  inline int getNumVertices() { return vertices->size(); }
  int getNumEdges();

  GraphVertexVect * dfs(Vertex *);
  int getOutDegree(Vertex *);

  void print();

private:
  int numEdges;
  int numVertices;
  GraphVertexVect * vertices;
  GraphEdgeList * edges;
};

#endif
