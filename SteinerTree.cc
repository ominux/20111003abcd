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

#include "SteinerTree.h"

SteinerTree::SteinerTree()
{
  steinerGraph = new GridGraph();
  pins = new PinVector();
  steinerPoints = new PinVector();
  treeAvailable = false;
}

SteinerTree::SteinerTree(int numPins)
{
  steinerGraph = new GridGraph();
  pins = new PinVector(numPins);
  steinerPoints = new PinVector();
  treeAvailable = false;
}

SteinerTree::SteinerTree(PinVector *pins)
{
  steinerGraph = new GridGraph();
  pins = new PinVector(*pins);
  steinerPoints = new PinVector();
  treeAvailable = false;
}

SteinerTree::SteinerTree(PinVector *thePins, PinVector *steinerPts)
{
  steinerGraph = NULL;
  pins = new PinVector(*thePins);
  steinerPoints = new PinVector(*steinerPts);
  treeAvailable = false;
}

SteinerTree::~SteinerTree()
{
  delete steinerGraph;
  delete pins;
  delete steinerPoints;
}

void SteinerTree::addPin(Pin *aPin)
{
  pins->push_back(aPin);
  treeAvailable = false;
}

void SteinerTree::setPinAt(int pinNum, Pin * thePin)
{
  (*pins)[pinNum] = thePin;
  treeAvailable = false;
}

int SteinerTree::numPins()
{
  return pins->size();
}

int SteinerTree::numSteinerPoints()
{
  return steinerPoints->size();
}

Pin * SteinerTree::getPin(int index)
{
  return (*pins)[index];
}

Pin * SteinerTree::getSteinerPoint(int index)
{
  return (*steinerPoints)[index];
}

double SteinerTree::getFlexibility()
{
  return flexibility;
}

PinTuple SteinerTree::getEdgePinsAt(int position)
{
  return steinerGraph->getEdgePinsAt(position);
}

int SteinerTree::getNumTreeEdges()
{
  return steinerGraph->getNumEdges();
}

void SteinerTree::removeLastPin()
{
  PinVector *oldPinVect = pins;
  pins = new PinVector(oldPinVect->size()-1);
  PinVectorItr j = oldPinVect->begin();
  for(PinVectorItr i = pins->begin(); i != pins->end(); i++)
    {
      if(*j != 0)
	(*i) = *j;
      j++;
    }
  delete oldPinVect;
  treeAvailable = false;
}

bool SteinerTree::isTreeMade()
{
  return treeAvailable;
}

//returns true if the net can be made into a flexible steiner tree
//currently only works for 4 terminal nets
//returns 0 if the net can not be made flexible
//""      1 "" ""  ""  is flexible in the x direction, middle points are higher
//""      2 "" ""  ""  is flexible in the x direction, middle points are lower
//""      3 "" ""  ""  is flexible in the y direction, middle points are to the right
//""      4 "" ""  ""  is flexible in the y direction, middle points are to the left
int SteinerTree::isFlexible()
{
  //return variable
  int madeFlexible = 0;

  //currently only works for 4 terminal nets
  if(numPins() != 4)
    return madeFlexible;

  //first try looking for the flexibility in the 'x' direction
  //sort nets according to x direction
  sort(pins->begin(), pins->end(), PinXSortSmallToLarge());
  
  bool condition1 = (getPin(0)->getX() < getPin(1)->getX() ) && 
    (getPin(1)->getX() < getPin(2)->getX()) &&
    (getPin(2)->getX() < getPin(3)->getX());
  bool condition2 = (getPin(1)->getY() > getPin(0)->getY()) &&
    (getPin(1)->getY() > getPin(3)->getY()) &&
    (getPin(2)->getY() > getPin(0)->getY()) &&
    (getPin(2)->getY() > getPin(3)->getY());
  bool condition3 = (getPin(1)->getY() < getPin(0)->getY()) &&
    (getPin(1)->getY() < getPin(3)->getY()) &&
    (getPin(2)->getY() < getPin(0)->getY()) &&
    (getPin(2)->getY() < getPin(3)->getY());

  //condition 1 checks that the pins have different x values
  //condition 2 checks that the middle pins are both higher than the outer pins
  //condition 3 checks that the middle pins are both lower than the outer pins

  if(condition1 && condition2)
    madeFlexible = 1;
  if(condition1 && condition3)
    madeFlexible = 2;

  //now try looking in the 'y' direction
  sort(pins->begin(), pins->end(), PinYSortSmallToLarge());

  condition1 = (getPin(0)->getY() < getPin(1)->getY() ) && 
    (getPin(1)->getY() < getPin(2)->getY()) &&
    (getPin(2)->getY() < getPin(3)->getY());
  condition2 = (getPin(1)->getX() > getPin(0)->getX()) &&
    (getPin(1)->getX() > getPin(3)->getX()) &&
    (getPin(2)->getX() > getPin(0)->getX()) &&
    (getPin(2)->getX() > getPin(3)->getX());
  condition3 = (getPin(1)->getX() < getPin(0)->getX()) &&
    (getPin(1)->getX() < getPin(3)->getX()) &&
    (getPin(2)->getX() < getPin(0)->getX()) &&
    (getPin(2)->getX() < getPin(3)->getX());

  //condition 1 checks that the pins have different y values
  //condition 2 checks that the middle pins are both to the right of the outer pins
  //condition 3 checks that the middle pins are both to the left of the outer pins

  if(condition1 && condition2)
     madeFlexible = 3;
  if(condition1 && condition3)
    madeFlexible = 4;

  return madeFlexible;
}

//General function called to make by the Net class to make a Steiner tree.  
//To change Steiner tree implimentation, change function here. 
void SteinerTree::makeSteinerTree()
{
  findFlexibleTree();
}
  
//creates a flexible steiner tree.  Only works on specific nets 
//see isFlexibleSteinerTree for details   
void SteinerTree::findFlexibleTree()
{
  clearSteinerPoints();
  int treeType = isFlexible();
  int steiner;

  //create the Steiner points
  if(treeType == 1)
    {
      sort(pins->begin(), pins->end(), PinXSortSmallToLarge());
      steiner = min(getPin(1)->getY(), getPin(2)->getY());
      steinerPoints->push_back(new Pin(getPin(1)->getX(),steiner));
      steinerPoints->push_back(new Pin(getPin(2)->getX(),steiner));
    }
  else if(treeType == 2)
    {
      sort(pins->begin(), pins->end(), PinXSortSmallToLarge());
      steiner = max(getPin(1)->getY(), getPin(2)->getY());
      steinerPoints->push_back(new Pin(getPin(1)->getX(),steiner));
      steinerPoints->push_back(new Pin(getPin(2)->getX(),steiner));
    }
  else if(treeType == 3)
    {
      sort(pins->begin(), pins->end(), PinYSortSmallToLarge());
      steiner = min(getPin(1)->getX(), getPin(2)->getX());
      steinerPoints->push_back(new Pin(steiner,getPin(1)->getY()));
      steinerPoints->push_back(new Pin(steiner,getPin(2)->getY()));
    }
   else if(treeType == 4)
    {
      sort(pins->begin(), pins->end(), PinYSortSmallToLarge());
      steiner = max(getPin(1)->getX(), getPin(2)->getX());
      steinerPoints->push_back(new Pin(steiner,getPin(1)->getY()));
      steinerPoints->push_back(new Pin(steiner,getPin(2)->getY()));
    }
    
  //make the graph have the appropriate connections
  
  //add all the vertices to the graph
  int counter = 0;
  for(PinVectorItr i = pins->begin(); i != pins->end(); i++)
    {
      char string[5];
      sprintf(string,"pin%d",counter);
      steinerGraph->addVertex(new GridVertex(string,(*i)));
      counter++;
    }
  counter = 0;
  for(PinVectorItr i = steinerPoints->begin(); i != steinerPoints->end(); i++)
    {
      
      char string[80];
      sprintf(string,"steiner%d",counter);
      steinerGraph->addVertex(new GridVertex(string,(*i)));
      counter++;
    }

  //add edges to the graph
  steinerGraph->addEdgeBetween(getPin(0), getSteinerPoint(0),"edge1");
  steinerGraph->addEdgeBetween(getPin(1), getSteinerPoint(0),"edge2");
  steinerGraph->addEdgeBetween(getSteinerPoint(0), getSteinerPoint(1),"edge3");
  steinerGraph->addEdgeBetween(getSteinerPoint(1), getPin(2),"edge4");
  steinerGraph->addEdgeBetween(getSteinerPoint(1), getPin(3),"edge5");

  //steinerGraph->print();
  treeAvailable = true;
      
  
}
  
void SteinerTree::findUnflexibleTree()
{
  clearSteinerPoints();
  int treeType = isFlexible();
  int steiner;

  //create the Steiner points
  if(treeType == 1)
    {
      sort(pins->begin(), pins->end(), PinXSortSmallToLarge());
      steiner = max(getPin(0)->getY(), getPin(3)->getY());
      steinerPoints->push_back(new Pin(getPin(1)->getX(),steiner));
      steinerPoints->push_back(new Pin(getPin(2)->getX(),steiner));
    }
  else if(treeType == 2)
    {
      sort(pins->begin(), pins->end(), PinXSortSmallToLarge());
      steiner = min(getPin(0)->getY(), getPin(3)->getY());
      steinerPoints->push_back(new Pin(getPin(1)->getX(),steiner));
      steinerPoints->push_back(new Pin(getPin(2)->getX(),steiner));
    }
  else if(treeType == 3)
    {
      sort(pins->begin(), pins->end(), PinYSortSmallToLarge());
      steiner = max(getPin(0)->getX(), getPin(3)->getX());
      steinerPoints->push_back(new Pin(steiner,getPin(1)->getY()));
      steinerPoints->push_back(new Pin(steiner,getPin(2)->getY()));
    }
   else if(treeType == 4)
    {
      sort(pins->begin(), pins->end(), PinYSortSmallToLarge());
      steiner = min(getPin(0)->getX(), getPin(3)->getX());
      steinerPoints->push_back(new Pin(steiner,getPin(1)->getY()));
      steinerPoints->push_back(new Pin(steiner,getPin(2)->getY()));
    }
    
  //make the graph have the appropriate connections
  
  //add all the vertices to the graph
  int counter = 0;
  for(PinVectorItr i = pins->begin(); i != pins->end(); i++)
    {
      char string[5];
      sprintf(string,"pin%d",counter);
      steinerGraph->addVertex(new GridVertex(string,(*i)));
      counter++;
    }
  counter = 0;
  for(PinVectorItr i = steinerPoints->begin(); i != steinerPoints->end(); i++)
    {
      
      char string[80];
      sprintf(string,"steiner%d",counter);
      steinerGraph->addVertex(new GridVertex(string,(*i)));
      counter++;
    }

  //add edges to the graph
  steinerGraph->addEdgeBetween(getPin(0), getSteinerPoint(0),"edge1");
  steinerGraph->addEdgeBetween(getPin(1), getSteinerPoint(0),"edge2");
  steinerGraph->addEdgeBetween(getSteinerPoint(0), getSteinerPoint(1),"edge3");
  steinerGraph->addEdgeBetween(getSteinerPoint(1), getPin(2),"edge4");
  steinerGraph->addEdgeBetween(getSteinerPoint(1), getPin(3),"edge5");

  //steinerGraph->print();
  treeAvailable = true;    
  
}

void SteinerTree::clearSteinerPoints()
{
  for(PinVectorItr i = steinerPoints->begin(); i != steinerPoints->end(); i++)
    delete (*i);
  
  delete steinerPoints;
  steinerPoints = new PinVector();
  delete steinerGraph;
  steinerGraph = new GridGraph();
  treeAvailable = false;
}
