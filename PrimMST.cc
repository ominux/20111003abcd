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

#include "PrimMST.h"
#include <limits.h>

PrimMST::PrimMST(Net *aNet)
{
  theNet = aNet;
  parentPin = new PinVector(theNet->getNumPins(),(Pin *) 0);
  pinOrder = new PrimOrderVector(theNet->getNumPins()-1, (PrimOrderHolder *) 0);
  primPQ = new PrimPriorityQ();
}

PrimMST::~PrimMST()
{
  delete parentPin;
  for(PrimOrderVectorItr i = pinOrder->begin(); i != pinOrder->end(); i++)
    delete *i;
  delete pinOrder;
  while(!primPQ->empty())
    {
      delete primPQ->top();
      primPQ->pop();
    }
  delete primPQ;
}

void PrimMST::findMST()
{
  //add the first pin edges to queue
  Pin *firstPin = theNet->getPin(0);
  (*parentPin)[0] = firstPin;
  for(int i = 1; i < theNet->getNumPins(); i++)
    {
      primPQ->push(new PrimEdge(firstPin, theNet->getPin(i), 
				firstPin->getManhattanDistance(theNet->getPin(i))));
    }
  int pinOrderNum = 0;
  while(!primPQ->empty())
    {
      PrimEdge *minEdge = primPQ->top();
      primPQ->pop();
      Pin *currentPin = minEdge->to;
      
      for(int i = 0; i < theNet->getNumPins(); i++)
	{
	  Pin *nextPin = theNet->getPin(i);
	  if(nextPin == currentPin && (*parentPin)[i] == 0)
	    {
	      (*parentPin)[i] = minEdge->from;
	      (*pinOrder)[pinOrderNum] = new PrimOrderHolder(minEdge->to, 
							     minEdge->from);
	      pinOrderNum++;
	    }
	  else if((*parentPin)[i] == 0)
	    {
	      primPQ->push(new 
			   PrimEdge(currentPin, nextPin, 
				    currentPin->getManhattanDistance(nextPin)));
	    }
	}
      delete minEdge;
    }
}
    
void PrimMST::twoTerminalNets(NetVector & twoTermNets)
{
  twoTermNets.reserve(twoTermNets.size() + pinOrder->size());
  findMST();
  int num = 0;
  for(PrimOrderVectorItr i = pinOrder->begin(); i != pinOrder->end(); i++)
    {
      Net *thisNet = new Net(2,0,theNet->getName());
      thisNet->setPin(0,(*i)->parent);
      thisNet->setPin(1,(*i)->currentPin);
      twoTermNets.push_back(thisNet);
      num++;
    }
}
