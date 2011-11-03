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

#include "Net.h"
#include <climits>

int EdgeHolder::operator==(const EdgeHolder &right) const
{
  if(right.horizontal == horizontal && right.trackNum == trackNum
     && right.edgeNum == edgeNum)
    return 1;
  else
    return 0;
}

void EdgeHolder::print()
{
  if(horizontal)
    cout << "horizontal edge with ";
  else
    cout << "vertical edge with ";
  cout << "track num " << trackNum << " and edge num " << edgeNum << endl;
}

int Net::getNumSteinerEdges()
{
  return steinerTree->getNumTreeEdges();
}

PinTuple Net::getSteinerEdgeAt(int position)
{
  return steinerTree->getEdgePinsAt(position);
}

Box * Net::getBoundingBox()
{
  if(boundingBox == 0)
    calculateBoundingBox();
  return boundingBox;
}

void Net::calculateBoundingBox()
{
  int lowestX = INT_MAX;
  int lowestY = INT_MAX;
  int highestX = INT_MIN;
  int highestY = INT_MIN;
  for(PinVectorItr i = pinVect->begin(); i != pinVect->end(); i++)
    {
      int currentX = (*i)->getX();
      int currentY = (*i)->getY();
      if(currentX < lowestX)
	lowestX = currentX;
      if(currentX > highestX)
	highestX = currentX;
      if(currentY < lowestY)
	lowestY = currentY;
      if(currentY > highestY)
	highestY = currentY;
    }
  delete boundingBox;
  boundingBox = new Box(lowestX,lowestY,highestX-lowestX,highestY-lowestY);
}

void Net::addEdge(bool horiz, int trackNum, int edgeNum)
{
  edgeList->push_back(new EdgeHolder(horiz,trackNum,edgeNum));
}

void Net::addEdgesBetween(bool horiz, int trackNum, int start, int end)
{
  for(int i = start; i < end; i++)
    addEdge(horiz,trackNum,i);
}


void Net::removeEdge(bool horiz, int trackNum, int edgeNum)
{
  EdgeListItr eraser = edgeList->end();
  for(EdgeListItr i = edgeList->begin(); i != edgeList->end(); i++)
    {
      if(horiz == (*i)->horizontal && trackNum == (*i)->trackNum 
	 && edgeNum == (*i)->edgeNum)
	eraser = i;
    }
  if(eraser != edgeList->end())
    {
      edgeList->erase(eraser);
      delete (*eraser);
    }
}

void Net::removeAllEdges()
{
  for(EdgeListItr i = edgeList->begin(); i != edgeList->end(); i++)
    delete *i;
  edgeList->clear();
}

void Net::removeConnectedFromPins()
{
  for(PinVectorItr i = pinVect->begin(); i != pinVect->end(); i++)
    (*i)->setUnconnected();
}
    

void Net::printEdges()
{
  for(EdgeListItr i = edgeList->begin(); i != edgeList->end(); i++)
    {
      EdgeHolder *edgeHold = (*i);
      if(edgeHold->horizontal)
	cout << "horizontal ";
      else
	cout << "vertical ";
      cout << "edge on track " << edgeHold->trackNum << " and edge " 
	   << edgeHold->edgeNum << endl;
    }
}

void Net::printEdges(ofstream &stream)
{
  stream << "###################################" << endl;
  stream << "net name: " << name << endl << "edges: " << endl;
  
  for(EdgeListItr i = edgeList->begin(); i != edgeList->end(); i++)
    {
      EdgeHolder *edgeHold = (*i);
      if(edgeHold->horizontal)
	stream << "horizontal ";
      else
	stream << "vertical ";
      stream << "edge on track " << edgeHold->trackNum << " and edge " 
	   << edgeHold->edgeNum << endl;
    }
}
SegmentVector * Net::getUpperLSegments()
{
  //only applicable to two pin nets
  if(getNumPins() != 2)
    return NULL;

  SegmentVector *segments = new SegmentVector(2);
  Pin *pinOne = getPin(0);
  Pin *pinTwo = getPin(1);
  
  //make pinOne have minimum x value (reduce cases...)
  if(pinOne->getX() > pinTwo->getX())
    {
      Pin *tempPin = pinOne;
      pinOne = pinTwo;
      pinTwo = tempPin;
    }

  //now pinOne.x <= pinTwo.x
  Segment * horizontalSeg = &((*segments)[0]);
  Segment * verticalSeg = &((*segments)[1]);
 
  //case 1: pinOne.y < pinTwo.y
  if(pinOne->getY() < pinTwo->getY())
    {
      horizontalSeg->setPointOne((Point) (*pinOne));
      horizontalSeg->setPointTwo(pinTwo->getX(), pinOne->getY());
      verticalSeg->setPointOne(pinTwo->getX(), pinOne->getY());
      verticalSeg->setPointTwo((Point) (*pinTwo));
    }
  //case 2: pinOne.y >= pinTwo.y
  else
    {
      horizontalSeg->setPointOne(pinOne->getX(), pinTwo->getY());
      horizontalSeg->setPointTwo((Point) (*pinTwo));
      verticalSeg->setPointOne(pinOne->getX(),pinTwo->getY());
      verticalSeg->setPointTwo((Point) (*pinOne));
    }

  return segments;
}

SegmentVector * Net::getLowerLSegments()
{
  
  //only applicable to two pin nets
  if(getNumPins() != 2)
    return NULL;

  SegmentVector *segments = new SegmentVector(2);
  Pin *pinOne = getPin(0);
  Pin *pinTwo = getPin(1);
  
  //make pinOne have minimum x value (reduces cases considered)
  if(pinOne->getX() > pinTwo->getX())
    {
      Pin *tempPin = pinOne;
      pinOne = pinTwo;
      pinTwo = tempPin;
    }
  
  //now pinOne.x <= pinTwo.x
  Segment *horizontalSeg = &((*segments)[0]);
  Segment *verticalSeg = &((*segments)[1]);
 
  //case 1: pinOne.y < pinTwo.y
  if(pinOne->getY() < pinTwo->getY())
    {
      horizontalSeg->setPointOne(pinOne->getX(), pinTwo->getY());
      horizontalSeg->setPointTwo((Point) (*pinTwo));
      verticalSeg->setPointOne((Point) (*pinOne));
      verticalSeg->setPointTwo(pinOne->getX(), pinTwo->getY());
    }
  //case 2: pinOne.y >= pinTwo.y
  else
    {
      horizontalSeg->setPointOne((Point) (*pinOne));
      horizontalSeg->setPointTwo(pinTwo->getX(), pinOne->getY());
      verticalSeg->setPointOne((Point) (*pinTwo));
      verticalSeg->setPointTwo(pinTwo->getX(), pinOne->getY());
    }

  return segments;
  
}

bool Net::isSteinerTreeMade()
{
  return steinerTree->isTreeMade();
}

void Net::makeSteinerTree()
{
  steinerTree->makeSteinerTree();
}

void Net::makeUnflexibleSteinerTree()
{
  steinerTree->findUnflexibleTree();
}

void Net::setPin(int pinNum, Pin * thePin)
{ 
  (*pinVect)[pinNum] = thePin;
  steinerTree->setPinAt(pinNum, thePin);
}

void Net::removeLastPin()
{
  PinVector *oldPinVect = pinVect;
  pinVect = new PinVector(oldPinVect->size()-1);
  PinVectorItr j = oldPinVect->begin();
  for(PinVectorItr i = pinVect->begin(); i != pinVect->end(); i++)
    {
      if(*j != 0)
	(*i) = *j;
      j++;
    }
  delete oldPinVect;
  steinerTree->removeLastPin();
}

void Net::printPins()
{
  cout << "Pins: ";
  for(int i = 0; i < pinVect->size(); i++)
    {
      getPin(i)->print();
    }
  cout << endl;
}

bool Net::isFlexible()
{
  return steinerTree->isFlexible();
}

void Net::makeFlexibleSteinerTree()
{
  steinerTree->findFlexibleTree();
}
