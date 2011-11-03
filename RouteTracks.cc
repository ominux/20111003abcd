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

#include "RouteTracks.h"
#include <iomanip>

using namespace std;

RouteTracks::RouteTracks(int xNum, int yNum)
{
  verticalCapacity = horizontalCapacity = 10;
  routeLength = 0;
  horizontalTracks = new RouteTrackVector(yNum);
  verticalTracks = new RouteTrackVector(xNum);

  int horizontalTrackNum = 0;
  for(RouteTrackVectorItr i = horizontalTracks->begin(); 
      i != horizontalTracks->end(); i++)
    {
      *i = new RouteEdgeVector(xNum-1);
      int edgeNum = 0;
      for(RouteEdgeVectorItr j = (*i)->begin(); j != (*i)->end(); j++)
	{
	  *j = new RouteEdge(true, horizontalTrackNum, edgeNum);
	  edgeNum++;
	}
      horizontalTrackNum++;
    }

  int verticalTrackNum = 0;
  for(RouteTrackVectorItr i = verticalTracks->begin(); 
      i != verticalTracks->end(); i++)
    {
      *i = new RouteEdgeVector(yNum-1);
      int edgeNum = 0;
      for(RouteEdgeVectorItr j = (*i)->begin(); j != (*i)->end(); j++)
	{
	  *j = new RouteEdge(false, verticalTrackNum, edgeNum);
	  edgeNum++;
	}
      verticalTrackNum++;
    }
}

RouteTracks::~RouteTracks()
{
  for(RouteTrackVectorItr i = horizontalTracks->begin(); 
      i != horizontalTracks->end(); i++)
    {
      for(RouteEdgeVectorItr j = (*i)->begin(); j != (*i)->end(); j++)
	{
	  delete *j;
	}
      delete *i;
    }
  
  for(RouteTrackVectorItr i = verticalTracks->begin(); 
      i != verticalTracks->end(); i++)
    {
      for(RouteEdgeVectorItr j = (*i)->begin(); j != (*i)->end(); j++)
	{
	  delete *j;
	}
      delete *i;
    }
}

int RouteTracks::isValid(int x, int y)
{
  return x >= 0 && x < getWidth() && y >= 0 && y < getHeight();
}

RouteEdge * RouteTracks::getHorizontalEdgeAt(int x, int y)
{ 
  if(isValid(x,y))
    return (*(*horizontalTracks)[y])[x];
  else
    return 0;
}

RouteEdge * RouteTracks::getVerticalEdgeAt(int x, int y)
{ 
  if(isValid(x,y))
    return (*(*verticalTracks)[x])[y];
  else
    return 0;
}

RouteEdge * RouteTracks::getEdgeAt(int x, int y, bool horizontal)
{
  if(horizontal)
    return getHorizontalEdgeAt(x,y);
  else
    return getVerticalEdgeAt(x,y);
}

RouteEdge * RouteTracks::getHorizontalEdgeWith(int trackNumber, int edgeNumber)
{
  if(isValid(edgeNumber,trackNumber))
    return (*(*horizontalTracks)[trackNumber])[edgeNumber];
  else
    return 0;
}

RouteEdge * RouteTracks::getVerticalEdgeWith(int trackNumber, int edgeNumber)
{
  if(isValid(trackNumber,edgeNumber))
    return (*(*verticalTracks)[trackNumber])[edgeNumber];
  else
    return 0;
}

RouteEdge * RouteTracks::getEdgeWith(int trackNum, int edgeNum, bool horiz)
{
  if(horiz)
    return getHorizontalEdgeWith(trackNum, edgeNum);
  else
    return getVerticalEdgeWith(trackNum, edgeNum);
}

int RouteTracks::getCongestionAt(int x, int y, bool horiz)
{
  return getEdgeAt(x,y,horiz)->getNumNets();
}

int RouteTracks::getCongestionWith(int trackNumber, int edgeNumber, bool horiz)
{
  return getEdgeWith(trackNumber, edgeNumber, horiz)->getNumNets();
}

int RouteTracks::getOverflowWith(int trackNumber, int edgeNumber, bool horiz)
{
  int returnOverflow = 0;
  if(isOverflownWith(trackNumber, edgeNumber, horiz))
    {
      int edgeCongestion = getCongestionWith(trackNumber, edgeNumber, horiz);
      if(horiz)
	returnOverflow = edgeCongestion - horizontalCapacity;
      else
	returnOverflow = edgeCongestion - verticalCapacity;
    }
  return returnOverflow;
}

int RouteTracks::getVerticalCongestionBetween(int trackNum, int begin, int end)
{
  int congestion = 0;
  for(int i = begin; i < end; i++)
    congestion += getVerticalEdgeWith(trackNum, i)->getNumNets();
  return congestion;
}

int RouteTracks::getHorizontalCongestionBetween(int trackNum, int begin, int end)
{
  int congestion = 0;
  for(int i = begin; i < end; i++)
    congestion += getHorizontalEdgeWith(trackNum, i)->getNumNets();
  return congestion;
}

void RouteTracks::addHorizontalSegment(Net *aNet, int trackNum, int begin, int end)
{
  for(int i = begin; i < end; i++)
    addNetToEdgeWith(trackNum, i, true, aNet);
}

void RouteTracks::addVerticalSegment(Net *aNet, int trackNum, int begin, int end)
{
  for(int i = begin; i < end; i++)
    addNetToEdgeWith(trackNum, i, false, aNet);
}

void RouteTracks::removeRoutingFromEdges(Net *aNet)
{
  EdgeList *edgeHolders = aNet->getEdgeList();
  for(EdgeListItr j = edgeHolders->begin(); j != edgeHolders->end(); j++)
    {
      EdgeHolder *holder = *j;
      removeNetFromEdgeWith(holder->trackNum, holder->edgeNum, 
			    holder->horizontal, aNet);
    }
  aNet->removeAllEdges();
}

int RouteTracks::getOverflow()
{
  int overflowRet = 0;
  for(RouteTrackVectorItr i = horizontalTracks->begin(); 
      i != horizontalTracks->end(); i++)
    {
      for(RouteEdgeVectorItr j = (*i)->begin(); j != (*i)->end(); j++)
	{
	  if((*j)->getNumNets() > horizontalCapacity)
	    overflowRet += (*j)->getNumNets() - horizontalCapacity;
	}
    }

  for(RouteTrackVectorItr i = verticalTracks->begin(); 
      i != verticalTracks->end(); i++)
    {
      for(RouteEdgeVectorItr j = (*i)->begin(); j != (*i)->end(); j++)
	{
	  if((*j)->getNumNets() > verticalCapacity)
	    overflowRet += (*j)->getNumNets() - verticalCapacity;
	}
    }

  return overflowRet;
}

int RouteTracks::getWireLength()
{
  int wireLengthRet = 0;
  for(RouteTrackVectorItr i = horizontalTracks->begin(); 
      i != horizontalTracks->end(); i++)
    {
      for(RouteEdgeVectorItr j = (*i)->begin(); j != (*i)->end(); j++)
	{
	  wireLengthRet += (*j)->getNumNets();
	}
    }

  for(RouteTrackVectorItr i = verticalTracks->begin(); 
      i != verticalTracks->end(); i++)
    {
      for(RouteEdgeVectorItr j = (*i)->begin(); j != (*i)->end(); j++)
	{
	  wireLengthRet += (*j)->getNumNets();
	}
    }

  return wireLengthRet;
}

bool RouteTracks::isOverflown(int x, int y, bool horiz)
{ 
  int numberOfNets = getEdgeAt(x,y,horiz)->getNumNets();
  if(horiz)
    return numberOfNets > horizontalCapacity; 
  else
    return numberOfNets > verticalCapacity;
}

bool RouteTracks::isOverflownWith(int trackNumber, int edgeNumber, bool horiz)
{
  int numberOfNets = getEdgeWith(trackNumber,edgeNumber,horiz)->getNumNets();
  if(horiz)
    return numberOfNets > horizontalCapacity; 
  else
    return numberOfNets > verticalCapacity;
}

bool RouteTracks::isDensityOneBendRoutable(Net *aNet,int density)
{
  bool routable = false;
  if(aNet->getNumPins() != 2)
    return false;
  Pin *first = aNet->getPin(0);
  Pin *second = aNet->getPin(1);

  int firstPinX = first->getX();
  int secondPinX = second->getX();
  int firstPinY = first->getY();
  int secondPinY = second->getY();

  if(firstPinX == secondPinX && firstPinY == secondPinY)
    return true;

  if(firstPinX > secondPinX) //swap pins to give only two distinct cases
    {
      int tempInt = secondPinX;
      secondPinX = firstPinX;
      firstPinX = tempInt;
      tempInt = secondPinY;
      secondPinY = firstPinY;
      firstPinY = tempInt;
    }
  //now firstPinX < secondPinX
  if(firstPinY < secondPinY)
    {
      if(isVerticalDensityRoutableBetween(firstPinX,firstPinY,secondPinY,density-1) &&
	 isHorizontalDensityRoutableBetween(secondPinY,firstPinX,secondPinX,density-1))
	{ 
	  //net can be routed without going over density on any of the edges. 
	  //so route the fucker
	  addHorizontalSegment(aNet,secondPinY,firstPinX,secondPinX);
	  aNet->addEdgesBetween(true,secondPinY,firstPinX,secondPinX);
	  addVerticalSegment(aNet,firstPinX,firstPinY,secondPinY);
	  aNet->addEdgesBetween(false,firstPinX,firstPinY,secondPinY);
	  aNet->setPermanent(); //don't want to r+r this net
	  return true;
	}
      else if(isVerticalDensityRoutableBetween(secondPinX,firstPinY,secondPinY,density-1) &&
	   isHorizontalDensityRoutableBetween(firstPinY,firstPinX,secondPinX,density-1))
	{ 
	  //same as above, but now routing on upperL
	  addHorizontalSegment(aNet,firstPinY,firstPinX,secondPinX);
	  aNet->addEdgesBetween(true,firstPinY,firstPinX,secondPinX);
	  addVerticalSegment(aNet,secondPinX,firstPinY,secondPinY);
	  aNet->addEdgesBetween(false,secondPinX,firstPinY,secondPinY);
	  aNet->setPermanent(); //no r+r here, buddy
	  return true;
	}
			
    }
  else
    {
      if(isVerticalDensityRoutableBetween(secondPinX,secondPinY,firstPinY,density-1) &&
	 isHorizontalDensityRoutableBetween(firstPinY,firstPinX,secondPinX,density-1))
	{
	  //route the net
	  addHorizontalSegment(aNet,firstPinY,firstPinX,secondPinX);
	  aNet->addEdgesBetween(true,firstPinY,firstPinX,secondPinX);
	  addVerticalSegment(aNet,secondPinX,secondPinY,firstPinY);
	  aNet->addEdgesBetween(false,secondPinX,secondPinY,firstPinY);
	  aNet->setPermanent();
	  return true;
	}
      else if(isVerticalDensityRoutableBetween(firstPinX,secondPinY,firstPinY,density-1) &&
	      isHorizontalDensityRoutableBetween(secondPinY,firstPinX,secondPinX,density-1))
	{
	  addHorizontalSegment(aNet,secondPinY,firstPinX,secondPinX);
	  aNet->addEdgesBetween(true,secondPinY,firstPinX,secondPinX);
	  addVerticalSegment(aNet,firstPinX,secondPinY,firstPinY);
	  aNet->addEdgesBetween(false,firstPinX,secondPinY,firstPinY);
	  aNet->setPermanent();
	  return true;
	}
	  
    }
  return false;
}

bool RouteTracks::isVerticalDensityRoutableBetween(int trackNum, int begin, 
						   int end, int density)
{
  for(int i = begin; i < end; i++)
    {
      if(getVerticalEdgeWith(trackNum,i)->getNumNets() > density)
	return false;
    }
  return true;
}
 
bool RouteTracks::isHorizontalDensityRoutableBetween(int trackNum, int begin, 
						     int end, int density)
{
  for(int i = begin; i < end; i++)
    {
      if(getHorizontalEdgeWith(trackNum,i)->getNumNets() > density)
	return false;
    }
  return true;
} 

void RouteTracks::findHorizontalOverflownEdge(RouteEdge **overflownEdge,
					      int &numOverflows, bool &foundOverflow)
{
  // bool foundOverflow = false;
  static int i = 0;
  static int j = 0;
  numOverflows++;
  for(; i < getHeight(); i++)
    {
      for(; j < getWidth()-1; j++)
	{
	  if(isOverflown(j,i,true))
	    {
	      foundOverflow = true;
	      (*overflownEdge) = getHorizontalEdgeAt(j,i);
	      j++;
	      break;
	    }
	  //cout << j << "," << i << endl;
	}
      if(foundOverflow)
	break;
      if(j == getWidth()-1)
	j = 0;
    }
  if(i == getHeight())
    i = 0;
}

void RouteTracks::findVerticalOverflownEdge(RouteEdge **overflownEdge, 
					    int &numOverflows, bool &foundOverflow)
{
  //look at vertical edges for overflowed edges
  //bool foundOverflow = false;
  static int i1 = 0;
  static int j1 = 0;
  numOverflows++;
  for(; i1 < getHeight()-1 && !foundOverflow; i1++)
    {
      for(; j1 < getWidth() && !foundOverflow; j1++)
	{
	  if(isOverflown(j1,i1,false))
	    {
	      foundOverflow = true;
	      (*overflownEdge) = getVerticalEdgeAt(j1,i1);
	      j1++;
	      break;
	    }
	}
      if(foundOverflow)
	break;
      if(j1 == getWidth())
	j1 = 0;
    }
  if(i1 == getHeight() -1)
    {
      i1 = 0;
    }
}

NetList * RouteTracks::ripNets()
{
  RouteEdge *overflownEdge = 0;
  static int numOverflows = 0;
  bool foundOverflow = false;
  int overflowLimit = 2*(horizontalTracks->size() * verticalTracks->size());
  while(!foundOverflow && numOverflows < overflowLimit)
    {
      if(numOverflows % 2)
	findVerticalOverflownEdge(&overflownEdge,numOverflows,foundOverflow);
      else
	findHorizontalOverflownEdge(&overflownEdge,numOverflows,foundOverflow);
    }

  NetList *returnList = new NetList();
  if(overflownEdge != 0)
    {
      NetList *rerouteNets = overflownEdge->getNetList();
      //remove these nets routing from the tracks and the edges from the nets
      int segmentNets = 0;
      while(rerouteNets->size() > segmentNets)
	{
	  Net *currentNet = rerouteNets->front();
	  if(!currentNet->isPermanent())
	    {
	      rerouteNets->pop_front();
	      overflownEdge->resetNumNets();
	      returnList->push_front(currentNet);
	      EdgeList *edgeHolders = currentNet->getEdgeList();
	      for(EdgeListItr j = edgeHolders->begin(); 
		  j != edgeHolders->end(); j++)
		{
		  EdgeHolder *holder = *j;
		  removeNetFromEdgeWith(holder->trackNum, holder->edgeNum,
					holder->horizontal, currentNet);
		}
	      currentNet->removeAllEdges();
	    }
	  else
	    segmentNets++;
	}
      return returnList;
    }
  return 0;
}  


NetList * RouteTracks::oldRipNets()
{
  bool foundOverflow = false;
  RouteEdge *overflownEdge = 0;
  static int i = 0;
  static int j = 0;
  static int numOverflows = 0;
  int overflowLimit = 2*(horizontalTracks->size() * verticalTracks->size());
  while(!foundOverflow && numOverflows < overflowLimit)
    {
      //look at horizontal edges for overflowed edges
      for(; i < getHeight() && !foundOverflow; i++)
	{
	  for(; j < getWidth()-1 && !foundOverflow; j++)
	    {
	      if(isOverflown(j,i,true))
		{
		  foundOverflow = true;
		  numOverflows++;
		  overflownEdge = getHorizontalEdgeAt(j,i);
		}
	    }
	  if(j == getWidth()-1)
	    j = 0;
	}
      if(i == getHeight())
	i = 0;
      
      static int i1 = 0;
      static int j1 = 0;
      for(; i1 < getHeight()-1 && !foundOverflow; i1++)
	{
	  for(; j1 < getWidth() && !foundOverflow; j1++)
	    {
	      if(isOverflown(j1,i1,false))
		{
		  foundOverflow = true;
		  numOverflows++;
		  overflownEdge = getVerticalEdgeAt(j1,i1);
		}
	    }
	  if(j1 == getWidth())
	    j1 = 0;
	}
      if(i1 == getHeight() -1)
	i1 = 0;
    }
  NetList *returnList = new NetList();
  if(overflownEdge != 0)
    {
      NetList *rerouteNets = overflownEdge->getNetList();
      //remove these nets routing from the tracks and the edges from the nets
      while(rerouteNets->size() != 0)
	{
	  Net *currentNet = rerouteNets->front();
	  rerouteNets->pop_front();
	  if(!currentNet->isPermanent())
	    {
	      overflownEdge->resetNumNets();
	      returnList->push_front(currentNet);
	      EdgeList *edgeHolders = currentNet->getEdgeList();
	      for(EdgeListItr j = edgeHolders->begin(); 
		  j != edgeHolders->end(); j++)
		{
		  EdgeHolder *holder = *j;
		  removeNetFromEdgeWith(holder->trackNum, holder->edgeNum,
					holder->horizontal, currentNet);
		}
	      currentNet->removeAllEdges();
	    }
	}
      return returnList;
    }
  return 0;
}  

void RouteTracks::printVerticalTracks()
{
  for(int i = 0; i < getHeight() -1; i++)
    {
      for(int j = 0; j < getWidth(); j++)
	cout << setw(4) << getVerticalEdgeAt(j,i)->getNumNets();
      cout << endl;
    }
}

void RouteTracks::printHorizontalTracks()
{
  for(int i = 0; i < getHeight(); i++)
    {
      for(int j = 0; j < getWidth()-1; j++)
	cout << setw(4) << getHorizontalEdgeAt(j,i)->getNumNets();
      cout << endl;
    }
}

void RouteTracks::print()
{
  for(int i = 0; i < getHeight(); i++)
    {
      cout << "  ";
      for(int j = 0; j < getWidth()-1; j++)
	{
	  cout << setw(4) << getHorizontalEdgeAt(j,i)->getNumNets();
	}
      cout << endl;
      for(int k = 0; k < getWidth() && i < getHeight()-1; k++)
	{
	  cout << setw(4) << getVerticalEdgeAt(k,i)->getNumNets();
	}
      cout << endl;
    }
}

void RouteTracks::printCongestionDisplayFile(ofstream &stream)
{
  stream << getHeight()+1 << " " << getWidth()+1 << endl;
  for(int i = 0; i < getWidth(); i++)
    stream << "0 ";
  stream << endl;
  for(int i = 0; i < getHeight() -1; i++)
    {
      for(int j = 0; j < getWidth(); j++)
	stream << getVerticalEdgeAt(j,i)->getNumNets() << " ";
      stream  << endl;
    }
  for(int i = 0; i < getWidth(); i++)
    stream << "0 ";
  stream << endl;
  
  for(int i = 0; i < getHeight(); i++)
    stream << "0 ";
  stream << endl;
  for(int i = 0; i < getWidth()-1; i++)
    {
      for(int j = 0; j < getHeight(); j++)
	stream << getHorizontalEdgeAt(i,j)->getNumNets() << " ";
      stream << endl;
    }
  for(int i = 0; i < getHeight(); i++)
    stream << "0 ";
 
}

void RouteTracks::routeOneBendNet(Net *aNet)
{
  PrimMST mst(aNet);
  mst.findMST();
  PrimOrderVector *pinOrder = mst.getPinOrder();
  for(PrimOrderVectorItr i = pinOrder->begin(); i != pinOrder->end(); i++)
    {
      Pin *nextRtPin = (*i)->currentPin;
      Pin *parentPtPin = (*i)->parent;
      findOneBendRoute(aNet,nextRtPin, parentPtPin);
    }
}

void RouteTracks::findOneBendRoute(Net *aNet,Pin *first,Pin *second)
{
  int firstPinX = first->getX();
  int secondPinX = second->getX();
  int firstPinY = first->getY();
  int secondPinY = second->getY();

  if(firstPinX == secondPinX && firstPinY == secondPinY)
    return;

  if(firstPinX > secondPinX) //swap pins to give only two distinct cases
    {
      int tempInt = secondPinX;
      secondPinX = firstPinX;
      firstPinX = tempInt;
      tempInt = secondPinY;
      secondPinY = firstPinY;
      firstPinY = tempInt;
    }
  //now firstPinX < secondPinX
  if(firstPinY < secondPinY)
    {
      int lowerCost = 0;
      int upperCost = 0;
      lowerCost += getVerticalCongestionBetween(firstPinX,firstPinY,secondPinY);
      lowerCost += getHorizontalCongestionBetween(secondPinY,firstPinX,secondPinX);
      upperCost += getVerticalCongestionBetween(secondPinX,firstPinY,secondPinY);
      upperCost += getHorizontalCongestionBetween(firstPinY,firstPinX,secondPinX);
      if(lowerCost < upperCost)
	{
	  addHorizontalSegment(aNet,secondPinY,firstPinX,secondPinX);
	  aNet->addEdgesBetween(true,secondPinY,firstPinX,secondPinX);
	  addVerticalSegment(aNet,firstPinX,firstPinY,secondPinY);
	  aNet->addEdgesBetween(false,firstPinX,firstPinY,secondPinY);
	}
      else
	{
	  addHorizontalSegment(aNet,firstPinY,firstPinX,secondPinX);
	  aNet->addEdgesBetween(true,firstPinY,firstPinX,secondPinX);
	  addVerticalSegment(aNet,secondPinX,firstPinY,secondPinY);
	  aNet->addEdgesBetween(false,secondPinX,firstPinY,secondPinY);
	}
    }
  else
    {
      int lowerCost = 0;
      int upperCost = 0;
      lowerCost += getVerticalCongestionBetween(secondPinX,secondPinY,firstPinY);
      lowerCost += getHorizontalCongestionBetween(firstPinY,firstPinX,secondPinX);
      upperCost += getVerticalCongestionBetween(firstPinX,secondPinY,firstPinY);
      upperCost += getHorizontalCongestionBetween(secondPinY,firstPinX,secondPinX);
      if(lowerCost < upperCost)
	{
	  addHorizontalSegment(aNet,firstPinY,firstPinX,secondPinX);
	  aNet->addEdgesBetween(true,firstPinY,firstPinX,secondPinX);
	  addVerticalSegment(aNet,secondPinX,secondPinY,firstPinY);
	  aNet->addEdgesBetween(false,secondPinX,secondPinY,firstPinY);
	}
      else
	{
	  addHorizontalSegment(aNet,secondPinY,firstPinX,secondPinX);
	  aNet->addEdgesBetween(true,secondPinY,firstPinX,secondPinX);
	  addVerticalSegment(aNet,firstPinX,secondPinY,firstPinY);
	  aNet->addEdgesBetween(false,firstPinX,secondPinY,firstPinY);
	}
    }
}

void RouteTracks::zRouteNet(Net *aNet)
{
  PrimMST mst(aNet);
  mst.findMST();
  PrimOrderVector *pinOrder = mst.getPinOrder();
  for(PrimOrderVectorItr i = pinOrder->begin(); i != pinOrder->end(); i++)
    {
      Pin *nextRtPin = (*i)->currentPin;
      Pin *parentPtPin = (*i)->parent;
      findZRoute(aNet,nextRtPin, parentPtPin);
    }
}

void RouteTracks::findZRoute(Net *aNet,Pin *first,Pin *second)
{
  int firstPinX = first->getX();
  int secondPinX = second->getX();
  int firstPinY = first->getY();
  int secondPinY = second->getY();

  if(firstPinX == secondPinX && firstPinY == secondPinY)
    return;

  if(firstPinX > secondPinX) //swap pins to give only two distinct cases
    {
      int tempInt = secondPinX;
      secondPinX = firstPinX;
      firstPinX = tempInt;
      tempInt = secondPinY;
      secondPinY = firstPinY;
      firstPinY = tempInt;
    }
  //now firstPinX < secondPinX
  int bestCongestion = INT_MAX;
  int bestVerticalTrack;
  int topHorizontalCost;
  int bottomHorizontalCost;
  if(firstPinY < secondPinY)
    {
      for(int i = firstPinX; i <= secondPinX; i++)
	{
	  topHorizontalCost = getHorizontalCongestionBetween(firstPinY,firstPinX,i);
	  bottomHorizontalCost = getHorizontalCongestionBetween(secondPinY,i,secondPinX);
	  if(topHorizontalCost + bottomHorizontalCost + 
	     getVerticalCongestionBetween(i,firstPinY,secondPinY) < bestCongestion)
	    {
	      bestCongestion = topHorizontalCost + bottomHorizontalCost + 
		getVerticalCongestionBetween(i,firstPinY,secondPinY);
	      bestVerticalTrack = i;
	    }
	}
      //found best route, now add it
      addHorizontalSegment(aNet,firstPinY,firstPinX,bestVerticalTrack);
      aNet->addEdgesBetween(true,firstPinY,firstPinX,bestVerticalTrack);
      addHorizontalSegment(aNet,secondPinY,bestVerticalTrack,secondPinX);
      aNet->addEdgesBetween(true,secondPinY,bestVerticalTrack,secondPinX);
      addVerticalSegment(aNet,bestVerticalTrack,firstPinY,secondPinY);
      aNet->addEdgesBetween(false,bestVerticalTrack,firstPinY,secondPinY);
    }
  else
    {
      for(int i = firstPinX; i <= secondPinX; i++)
	{
	  topHorizontalCost = getHorizontalCongestionBetween(secondPinY,i,secondPinX);
	  bottomHorizontalCost = getHorizontalCongestionBetween(firstPinY,firstPinX,i);
	  if(topHorizontalCost + bottomHorizontalCost + 
	     getVerticalCongestionBetween(i,secondPinY,firstPinY) < bestCongestion)
	    {
	      bestCongestion = topHorizontalCost + bottomHorizontalCost + 
		getVerticalCongestionBetween(i,secondPinY,firstPinY);
	      bestVerticalTrack = i;
	    }
	}
      //found best route, now add it
      addHorizontalSegment(aNet,firstPinY,firstPinX,bestVerticalTrack);
      aNet->addEdgesBetween(true,firstPinY,firstPinX,bestVerticalTrack);
      addHorizontalSegment(aNet,secondPinY,bestVerticalTrack,secondPinX);
      aNet->addEdgesBetween(true,secondPinY,bestVerticalTrack,secondPinX);
      addVerticalSegment(aNet,bestVerticalTrack,secondPinY,firstPinY);
      aNet->addEdgesBetween(false,bestVerticalTrack,secondPinY,firstPinY);
      
    }

}
	      
void RouteTracks::routeBBNet(Net *aNet)
{
  Box *boundBox = aNet->getBoundingBox();
  int upperLeftX = boundBox->getUpperLeftX();
  int upperLeftY = boundBox->getUpperLeftY();
  int width = boundBox->getWidth();
  int height = boundBox->getHeight();

  addHorizontalSegment(aNet,upperLeftY,upperLeftX,upperLeftX+width);
  aNet->addEdgesBetween(true,upperLeftY,upperLeftX,upperLeftX+width);
  addHorizontalSegment(aNet,upperLeftY+height,upperLeftX,upperLeftX+width);
  aNet->addEdgesBetween(true,upperLeftY+height,upperLeftX,upperLeftX+width);
  addVerticalSegment(aNet,upperLeftX,upperLeftY,upperLeftY+height);
  aNet->addEdgesBetween(false,upperLeftX,upperLeftY,upperLeftY+height);
  addVerticalSegment(aNet,upperLeftX+width,upperLeftY,upperLeftY+height);
  aNet->addEdgesBetween(false,upperLeftX+width,upperLeftY,upperLeftY+height);  

}

int RouteTracks::findRoutingOverflow(Net *aNet)
{
  EdgeList *edges = aNet->getEdgeList();
  int returnOverflow = 0;
  for(EdgeListItr i =  edges->begin(); i != edges->end(); i++)
    {
      EdgeHolder * holder = (*i);
      returnOverflow += getOverflowWith(holder->trackNum, holder->edgeNum,
					holder->horizontal);
    }
  return returnOverflow;
}
      
int RouteTracks::findRoutingCongestion(Net *aNet)
{
  EdgeList *edges = aNet->getEdgeList();
  int returnCongestion = 0;
  for(EdgeListItr i =  edges->begin(); i != edges->end(); i++)
    {
      EdgeHolder * holder = (*i);
      returnCongestion += getCongestionWith(holder->trackNum, holder->edgeNum,
					holder->horizontal);
    }
  return returnCongestion;
}

//pattern routes (l-shape) the net if the steiner points are already specified
bool RouteTracks::patternRouteSteinerNet(Net *theNet)
{
  theNet->removeConnectedFromPins();
  if(!theNet->isSteinerTreeMade())
    theNet->makeSteinerTree();
  //only route nets with  2 < pins num < 50 
  bool routed = true;
  if(theNet->getNumPins() < 2 || theNet->getNumPins() > 50)
    return routed;
  for(int i = 0; i < theNet->getNumSteinerEdges(); i++)
    {
      PinTuple tuple = theNet->getSteinerEdgeAt(i);
      findOneBendRoute(theNet,tuple.one,tuple.two);
    }
}

//pattern routes (z-shape) the net if the steiner points are already specified
bool RouteTracks::zRouteSteinerNet(Net *theNet)
{
  theNet->removeConnectedFromPins();
  if(!theNet->isSteinerTreeMade())
    theNet->makeSteinerTree();
  //only route nets with  2 < pins num < 50 
  bool routed = true;
  if(theNet->getNumPins() < 2 || theNet->getNumPins() > 50)
    return routed;
  for(int i = 0; i < theNet->getNumSteinerEdges(); i++)
    {
      PinTuple tuple = theNet->getSteinerEdgeAt(i);
      findZRoute(theNet,tuple.one,tuple.two);
    }
}
