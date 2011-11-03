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

#include "CoupleFree.h"

#include <limits.h>
#include <algorithm>
#include <math.h>

#define FORCE_FACTOR 5

class CriticalitySortLargeToSmall
{
public:
  inline bool operator() (Net *net1, Net *net2)
  { return CoupleFree::getCriticality(net1) > CoupleFree::getCriticality(net2); }
};

class IntNetHolder
{
public:
  int value;
  bool upper;
  Net *net;
};

typedef vector<IntNetHolder> IntNetVector;
typedef IntNetVector::iterator IntNetVectorItr;

class ForcingSortSmallToLarge
{
public:
  inline bool operator() (IntNetHolder net1, IntNetHolder net2)
  { return net1.value < net2.value; }
};

class TupleNetHolder
{
public:
  int direct;
  int indirect;
  bool upper;
  Net *net;
};

typedef vector<TupleNetHolder> TupleNetVector;
typedef TupleNetVector::iterator TupleNetVectorItr;

class ForcingSort2SmallToLarge
{
public:
  inline bool operator() (TupleNetHolder net1, TupleNetHolder net2)
  { 
    cout << FORCE_FACTOR << endl;
    return FORCE_FACTOR*net1.direct +  net1.indirect < 
      FORCE_FACTOR*net2.direct + net2.indirect;
  }
};

//-------end of misc internal classes----------------------------------------------- 

NetCouplingInfo::NetCouplingInfo(int size)
{
  upperConflicts = new CoupleVector(size);
  lowerConflicts = new CoupleVector(size);
}

NetCouplingInfo::~NetCouplingInfo()
{
  delete upperConflicts;
  delete lowerConflicts;
}

CoupleFree::CoupleFree(NetVector *netVect)
{ 
  nets = netVect; 
  netsize = netVect->size();
  coupleDistance = 1; 
  coupleLength = 3; 
  interactions = new CoupleInteractions(nets->size());
  for(int i = 0; i < nets->size(); i++)
    {
      (*interactions)[i] = new NetCouplingInfo(nets->size());
      (*interactions)[i]->net = (*nets)[i];
    }
  calculateCoupleInteractions();
  routings = new IntVector(netVect->size(),0);
  planar = false;
}

CoupleFree::CoupleFree(NetVector *netVect, int dist, int length)
{ 
  nets = netVect; 
  netsize = netVect->size();
  interactions = new CoupleInteractions(nets->size());
  for(int i = 0; i < nets->size(); i++)
    {
      (*interactions)[i] = new NetCouplingInfo(nets->size());
      (*interactions)[i]->net = (*nets)[i];
    }
  setCoupleParameters(dist,length); 
  calculateCoupleInteractions();
  routings = new IntVector(netVect->size(),0);
  planar = false;
}

CoupleFree::CoupleFree(NetVector *netVect, int dist, int length, bool plane)
{ 
  nets = netVect; 
  netsize = netVect->size();
  interactions = new CoupleInteractions(nets->size());
  for(int i = 0; i < nets->size(); i++)
    {
      (*interactions)[i] = new NetCouplingInfo(nets->size());
      (*interactions)[i]->net = (*nets)[i];
    }
  setCoupleParameters(dist,length); 
  calculateCoupleInteractions();
  routings = new IntVector(netVect->size(),0);
  planar = plane;
}

CoupleFree::~CoupleFree() 
{ 
  // need to clean up coupling interactions
  for(int i = 0; i < netsize; i++)
    {
      delete (*interactions)[i];
    }
  delete interactions;
  delete routings;
}

int CoupleFree::getNumRoutedNets()
{
  int routed = 0;
  if(routings != NULL)
    for(IntVectorItr i = routings->begin(); i != routings->end(); i++)
      {
        if((*i) == 1 || (*i) == 2)
	  routed++;
      }
  return routed;
}

int CoupleFree::getCriticalityRoutedNets()
{
  int crit = 0;
  if(routings != NULL)
    {
      int counter = 0;
      for(IntVectorItr i = routings->begin(); i != routings->end(); i++)
	{
	  if((*i) == 1 || (*i) == 2)
	    crit += getCriticality((*nets)[counter]);
	  counter++;
	}
    }
  return crit;
}

double CoupleFree::getNRootNCriticalityRoutedNets()
{
  double crit = 0;
  if(routings != NULL)
    {
      int counter = 0;
      for(IntVectorItr i = routings->begin(); i != routings->end(); i++)
	{
	  if((*i) == 1 || (*i) == 2)
	    crit += getNRootNCriticality((*nets)[counter]);
	  counter++;
	}
    }
  return crit;
}

int CoupleFree::getRoutedNetLength()
{
  int length = 0;
  if(routings != NULL)
    {
      int counter = 0;
      for(IntVectorItr i = routings->begin(); i != routings->end(); i++)
	{
	  if((*i) == 1 || (*i) == 2)
	    length += (*nets)[counter]->getBoundingBox()->getHalfPerimeter();
	  counter++;
	}
    }
  return length;
}

int CoupleFree::getNetIndex(Net *aNet)
{
  int index = -1;
  int counter = 0;
  for(NetVectorItr i = nets->begin(); i != nets->end(); i++)
    {
      if( (*i) == aNet)
	index = counter;
      counter++;
    }
  return index;
}

int CoupleFree::getCriticality(Net *aNet)
{
  Box *bb = aNet->getBoundingBox();
  int BBperimeter = bb->getPerimeter();
  return BBperimeter*BBperimeter;
}

double CoupleFree::getNRootNCriticality(Net *aNet)
{
  Box *bb = aNet->getBoundingBox();
  int BBperimeter = bb->getPerimeter();
  //JW return sqrt(BBperimeter)*BBperimeter;
  return sqrt((double)BBperimeter)*BBperimeter;
}

/*
//given the set of net and routings, this will return the nets that can still be 
//routed (i.e. it will prune initNets of unroutable nets
NetVector * findRoutableNets(NetVector *initNets, BoolVector *routed, 
			     BoolVector *routedUpper, BoolVector *routedLower)
{
  //vectors should be all the same size
  if(!(initNets->size() == routed->size() && routed->size() == routedUpper->size()
       && routedUpper->size() == routedLower->size()))
    cout << "Possible Fatal Error: Vector are not all of the same size in "
	 << "CoupleFree.cc:findRoutableNets" << endl;
  
  int numberRoutableNets = 0;
  for(int i = 0; i < initNets->size(); i++)
    {
*/      
  
//make sure to delete netGraph
/*
void CoupleFree::findGraphBasedCoupleFreeSet()
{
  Graph *netGraph = createImplicationGraph();
 */ 
  
  

void CoupleFree::findQuickCoupleFreeSet()
{
  BoolVector routed(nets->size(),false);
  BoolVector routableUpper(nets->size(), true);
  BoolVector routableLower(nets->size(), true);
  BoolVector routedUpper(nets->size(),false);
  BoolVector routedLower(nets->size(),false);
  
  IntVector upperViolations(nets->size());
  IntVector lowerViolations(nets->size());
  IntVector upperForcings(nets->size());
  IntVector lowerForcings(nets->size());

  //populate the 4 proceeding vectors
  for(int i = 0; i < nets->size(); i++)
    {
      upperViolations[i] = numberViolations((*nets)[i],i,true);
      lowerViolations[i] = numberViolations((*nets)[i],i,false);
      upperForcings[i] = numberForcings((*nets)[i],i,true);
      lowerForcings[i] = numberForcings((*nets)[i],i,false);
    }

  int numRoutedNets = 0;

  //route nets that cause no forcings (truly independent nets)
  for(int i = 0; i < nets->size(); i++)
    {
      if(upperForcings[i] == 0)
	{
	  numRoutedNets++;
	  routed[i] = true;
	  routedUpper[i] = true;
	  (*routings)[i] = 1;
	}
      else if(lowerForcings[i] == 0)
	{
	  numRoutedNets++;
	  routed[i] = true;
	  routedLower[i] = true;
	  (*routings)[i] = 2;
	}
    }

  //now, find net with minimum number of forcings and route it, if possible
  //repeat until no net can be routed; actually only need to repeat numNets times
  //but I figured this out after I already coded the function, hence the ugly 
  //netsConsidered hack
  bool aNetRouted = true;
  int netsConsidered = 0;
  while(aNetRouted)
    {
      //find minimum forcing net that isn't already routed and is routable
      int minIndex = -1;
      int minForcings = INT_MAX;
      bool minRoute;  //true if upper, false if lower
      for(int i = 0; i < nets->size(); i++)
	{
	  if(!routed[i])
	    {
	      if(routableUpper[i] && routableLower[i])
		{
		  minIndex = i;
		  minForcings = min(upperForcings[i],lowerForcings[i]);
		  if(minForcings == upperForcings[i])
		    minRoute = true;
		  else if(minForcings == lowerForcings[i])
		    minRoute = false;
		  else
		    cout << "Possible problem in CoupleFree::findQuickCoupleFreeSet()" 
			 << endl;
		}
	      else if(routableUpper[i])
		{
		  minForcings = upperForcings[i];
		  minIndex = i;
		  minRoute = true;
		}
	      else if(routableLower[i])
		{
		  minIndex = i;
		  minForcings = lowerForcings[i];
		  minRoute = false;
		}
	    }
	}
      
      //check to see if this net is routable
      bool minNetRoutable = true;
      for(int i = 0; i < nets->size(); i++)
	{
	  //find routed nets and see if they couple with net under consideration
	  if(routed[i] && minIndex != -1)
	    {
	      IntVector *coupling = findCoupleInteractions(i,minIndex);
	      if(routedUpper[i] && minRoute)
		{
		  //upper and upper
		  if((*coupling)[0] > coupleLength)
		    minNetRoutable = false;
		    
		}
	      else if(routedUpper[i] && !minRoute)
		{
		  //upper and lower
		  if((*coupling)[1] > coupleLength)
		    minNetRoutable = false;
		}
	      else if(routedLower[i] && minRoute)
		{
		  //lower and upper
		  if((*coupling)[2] > coupleLength)
		    minNetRoutable = false;
		}
	      else if(routedLower[i] && !minRoute)
		{
		  //lower and lower
		  if((*coupling)[3] > coupleLength)
		    minNetRoutable = false;
		}
	      else
		cout << "Problem in CoupleFree::findQuickCoupleFreeSet()."
		     << "  Net listed as routed but not set upper or lower" << endl;
	      delete coupling;

	      //if nets must be planar, check to see if this net overlaps with 
	      //any routed net
	      if(planar)
		{
		  BoolVector *overlap = findOverlapInteractions(i,minIndex);
		  if(routedUpper[i] && minRoute)
		    {
		      //upper and upper
		      if((*overlap)[0])
			minNetRoutable = false;
		    }
		  else if(routedUpper[i] && !minRoute)
		    {
		      //upper and lower
		      if((*overlap)[1])
			minNetRoutable = false;
		    }
		  else if(routedLower[i] && minRoute)
		    {
		      //lower and upper
		      if((*overlap)[2])
			minNetRoutable = false;
		    }
		  else if(routedLower[i] && !minRoute)
		    {
		      //lower and lower
		      if((*overlap)[3])
			minNetRoutable = false;
		    }
		  else
		    cout << "Problem in CoupleFree::findQuickCoupleFreeSet()."
			 << "  Net listed as routed but not set upper or lower" << endl;
		  delete overlap;
		}
	    }
	}

      //if possible, route that net
      if(minNetRoutable)
	{
	  routed[minIndex] = true;
	  if(minRoute)
	    {
	      routedUpper[minIndex] = true;
	      (*routings)[minIndex] = 1;
	    }
	  else
	    {
	      routedLower[minIndex] = true;
	      (*routings)[minIndex] = 2;
	    }
	}
      else
	{
	  if(minRoute)
	    routableUpper[minIndex] = false;
	  else
	    routableLower[minIndex] = false;
	}

      
      netsConsidered++;
      if(minIndex == -1 /*|| netsConsidered == nets->size()*/)
	{
	  //net is routable, stop looping
	  aNetRouted = false;
	}

    }
  
}

void CoupleFree::findGreedyCoupleFreeSet()
{
  BoolVector routed(nets->size(),false);
  BoolVector routedUpper(nets->size(),false);
  BoolVector routedLower(nets->size(),false);

  sort(nets->begin(), nets->end(), CriticalitySortLargeToSmall());
  
  for(int index = 0; index <  nets->size(); index++)
    {
      //first try to route in upper-L
      bool netRoutableUpper = true;
      bool netRoutableLower = true;
      bool route;
      for(int i = 0; i < nets->size(); i++)
	{
	  if(routed[i])
	    {
	      IntVector *coupling = findCoupleInteractions(i,index);
	      route = true;
	      if(routedUpper[i] && route)
		{
		  //upper and upper
		  if((*coupling)[0] > coupleLength)
		    netRoutableUpper = false;
		}
	      else if(routedUpper[i] && !route)
		{
		  //upper and lower
		  if((*coupling)[1] > coupleLength)
		    netRoutableUpper = false;
		}
	      else if(routedLower[i] && route)
		{
		  //lower and upper
		  if((*coupling)[2] > coupleLength)
		    netRoutableUpper = false;
		}
	      else if(routedLower[i] && !route)
		{
		  //lower and lower
		  if((*coupling)[3] > coupleLength)
		    netRoutableUpper = false;
		}
	      else
		cout << "Problem in CoupleFree::findQuickCoupleFreeSet()."
		     << "  Net listed as routed but not set upper or lower" << endl;
	      //delete coupling;
	      
	      //if nets must be planar, check to see if this net overlaps with 
	      //any routed net
	      if(planar)
		{
		  BoolVector *overlap = findOverlapInteractions(i,index);
		  if(routedUpper[i] && route)
		    {
		      //upper and upper
		      if((*overlap)[0])
			netRoutableUpper = false;
		    }
		  else if(routedUpper[i] && !route)
		    {
		      //upper and lower
		      if((*overlap)[1])
			netRoutableUpper = false;
		    }
		  else if(routedLower[i] && route)
		    {
		      //lower and upper
		      if((*overlap)[2])
			netRoutableUpper = false;
		    }
		  else if(routedLower[i] && !route)
		    {
		      //lower and lower
		      if((*overlap)[3])
			netRoutableUpper = false;
		    }
		  else
		    cout << "Problem in CoupleFree::findQuickCoupleFreeSet()."
			 << "  Net listed as routed but not set upper or lower" << endl;
		  delete overlap;
		}
	      
	      //now try lower-L
	      route = false;
	      //IntVector *coupling = findCoupleInteractions(i,index);
	      if(routedUpper[i] && route)
		{
		  //upper and upper
		  if((*coupling)[0] > coupleLength)
		    netRoutableLower = false;
		}
	      else if(routedUpper[i] && !route)
		{
		  //upper and lower
		  if((*coupling)[1] > coupleLength)
		    netRoutableLower = false;
		}
	      else if(routedLower[i] && route)
		{
		  //lower and upper
		  if((*coupling)[2] > coupleLength)
		    netRoutableLower = false;
		}
	      else if(routedLower[i] && !route)
		{
		  //lower and lower
		  if((*coupling)[3] > coupleLength)
		    netRoutableLower = false;
		}
	      else
		cout << "Problem in CoupleFree::findQuickCoupleFreeSet()."
		     << "  Net listed as routed but not set upper or lower" << endl;
	      delete coupling;
	      
	      //if nets must be planar, check to see if this net overlaps with 
	      //any routed net
	      if(planar)
		{
		  BoolVector *overlap = findOverlapInteractions(i,index);
		  if(routedUpper[i] && route)
		    {
		      //upper and upper
		      if((*overlap)[0])
			netRoutableLower = false;
		    }
		  else if(routedUpper[i] && !route)
		    {
		      //upper and lower
		      if((*overlap)[1])
			netRoutableLower = false;
		    }
		  else if(routedLower[i] && route)
		    {
		      //lower and upper
		      if((*overlap)[2])
			netRoutableLower = false;
		    }
		  else if(routedLower[i] && !route)
		    {
		      //lower and lower
		      if((*overlap)[3])
			netRoutableLower = false;
		    }
		  else
		    cout << "Problem in CoupleFree::findQuickCoupleFreeSet()."
			 << "  Net listed as routed but not set upper or lower" << endl;
		  delete overlap;
		}
	      
	    }
	}
      
      //if possible, route that net
      if(netRoutableUpper || netRoutableLower)
	{
	  //determine whether to route upper or lower (if both, default is upper)
	  if(netRoutableUpper == true)
	    route = true;
	  else
	    route = false;

	  routed[index] = true;
	  if(route)
	    {
	      routedUpper[index] = true;
	      (*routings)[index] = 1;
	    }
	  else
	    {
	      routedLower[index] = true;
	      (*routings)[index] = 2;
	    }
	}
    }  
  
}

void CoupleFree::findForcingFunctionCoupleFreeSet()
{
  BoolVector routed(nets->size(),false);
  BoolVector routedUpper(nets->size(),false);
  BoolVector routedLower(nets->size(),false);
  int numNets = nets->size();

  TupleNetVector forcings(2*numNets);
 
  Graph *netGraph = createImplicationGraph();

  //populate dependentForcing vector
  for(int i = 0; i < 2*numNets; i++)
    {
      GraphVertexVect *depthfs = netGraph->dfs(netGraph->getVertexAt(i));

      //figure out the net and type of routing
      bool upper = true;
      int index = i;
      if(i >= numNets)
	{
	  index -= numNets;
	  upper = false;
	}

      //size of depthfs is the number of forcings for this routing 
      forcings[i].indirect = depthfs->size();
      forcings[i].direct = netGraph->getOutDegree(netGraph->getVertexAt(i));
      forcings[i].net = (*nets)[index];
      forcings[i].upper = upper;
      delete depthfs;
    }

  //now sort the routings (small to large) by forcings
  sort(forcings.begin(), forcings.end(), ForcingSort2SmallToLarge());

  //now route the nets
  for(TupleNetVectorItr i = forcings.begin(); i != forcings.end(); i++)
    {
      //cout << (*i).direct << "," << (*i).indirect << endl;
      int origNetNum = getNetIndex((*i).net);
      bool route = (*i).upper;
      bool netRoutable = true;
      if(!routed[origNetNum])
	{
	  //see if this couples with any other routed net
	  for(int otherNet = 0; otherNet < nets->size(); otherNet++)
	    {
	      if(routed[otherNet])
		{
		  IntVector *coupling = findCoupleInteractions(otherNet,origNetNum);
		  if(routedUpper[otherNet] && route)
		    {
		      //upper and upper
		      if((*coupling)[0] > coupleLength)
			netRoutable = false;
		    }
		  else if(routedUpper[otherNet] && !route)
		    {
		      //upper and lower
		      if((*coupling)[1] > coupleLength)
			netRoutable = false;
		    }
		  else if(routedLower[otherNet] && route)
		    {
		      //lower and upper
		      if((*coupling)[2] > coupleLength)
			netRoutable = false;
		    }
		  else if(routedLower[otherNet] && !route)
		    {
		      //lower and lower
		      if((*coupling)[3] > coupleLength)
			netRoutable = false;
		    }
		  else
		    cout << "Problem in CoupleFree::findImplicationCoupleFreeSet() first."
			 << "  Net listed as routed but not set upper or lower" << endl;
		  delete coupling;

		  //if nets must be planar, check to see if this net overlaps with 
		  //any routed net
		  if(planar)
		    {
		      BoolVector *overlap = findOverlapInteractions(otherNet,origNetNum);
		      if(routedUpper[otherNet] && route)
			{
			  //upper and upper
			  if((*overlap)[0])
			    netRoutable = false;
			}
		      else if(routedUpper[otherNet] && !route)
			{
			  //upper and lower
			  if((*overlap)[1])
			    netRoutable = false;
			}
		      else if(routedLower[otherNet] && route)
			{
			  //lower and upper
			  if((*overlap)[2])
			    netRoutable = false;
			}
		      else if(routedLower[otherNet] && !route)
			{
			  //lower and lower
			  if((*overlap)[3])
			    netRoutable = false;
			}
		      else
			cout << "Problem in CoupleFree::findImplicationCoupleFreeSet()."
			     << "  Net listed as routed but not set upper or lower" << endl;
		      delete overlap;
		    }
		}
	    }

	  //if possible, route that net
	  if(netRoutable)
	    {
	      routed[origNetNum] = true;
	      if(route)
		{
		  routedUpper[origNetNum] = true;
		  (*routings)[origNetNum] = 1;
		}
	      else
		{
		  routedLower[origNetNum] = true;
		  (*routings)[origNetNum] = 2;
		}
	    
	    }
	}
    }
  
  delete netGraph;
}

void CoupleFree::findImplicationCoupleFreeSet()
{
  BoolVector routed(nets->size(),false);
  BoolVector routedUpper(nets->size(),false);
  BoolVector routedLower(nets->size(),false);
  int numNets = nets->size();

  IntNetVector forcings(2*numNets);
 
  Graph *netGraph = createImplicationGraph();

  //populate dependentForcing vector
  for(int i = 0; i < 2*numNets; i++)
    {
      GraphVertexVect *depthfs = netGraph->dfs(netGraph->getVertexAt(i));

      //figure out the net and type of routing
      bool upper = true;
      int index = i;
      if(i >= numNets)
	{
	  index -= numNets;
	  upper = false;
	}

      //size of depthfs is the number of forcings for this routing 
      forcings[i].value = depthfs->size();
      forcings[i].net = (*nets)[index];
      forcings[i].upper = upper;
      delete depthfs;
    }

  //now sort the routings (small to large) by forcings
  sort(forcings.begin(), forcings.end(), ForcingSortSmallToLarge());

  //now route the nets
  for(IntNetVectorItr i = forcings.begin(); i != forcings.end(); i++)
    {
      //cout << (*i).value << endl;
      int origNetNum = getNetIndex((*i).net);
      bool route = (*i).upper;
      bool netRoutable = true;
      if(!routed[origNetNum])
	{
	  //see if this couples with any other routed net
	  for(int otherNet = 0; otherNet < nets->size(); otherNet++)
	    {
	      if(routed[otherNet])
		{
		  IntVector *coupling = findCoupleInteractions(otherNet,origNetNum);
		  if(routedUpper[otherNet] && route)
		    {
		      //upper and upper
		      if((*coupling)[0] > coupleLength)
			netRoutable = false;
		    }
		  else if(routedUpper[otherNet] && !route)
		    {
		      //upper and lower
		      if((*coupling)[1] > coupleLength)
			netRoutable = false;
		    }
		  else if(routedLower[otherNet] && route)
		    {
		      //lower and upper
		      if((*coupling)[2] > coupleLength)
			netRoutable = false;
		    }
		  else if(routedLower[otherNet] && !route)
		    {
		      //lower and lower
		      if((*coupling)[3] > coupleLength)
			netRoutable = false;
		    }
		  else
		    cout << "Problem in CoupleFree::findImplicationCoupleFreeSet() first."
			 << "  Net listed as routed but not set upper or lower" << endl;
		  delete coupling;

		  //if nets must be planar, check to see if this net overlaps with 
		  //any routed net
		  if(planar)
		    {
		      BoolVector *overlap = findOverlapInteractions(otherNet,origNetNum);
		      if(routedUpper[otherNet] && route)
			{
			  //upper and upper
			  if((*overlap)[0])
			    netRoutable = false;
			}
		      else if(routedUpper[otherNet] && !route)
			{
			  //upper and lower
			  if((*overlap)[1])
			    netRoutable = false;
			}
		      else if(routedLower[otherNet] && route)
			{
			  //lower and upper
			  if((*overlap)[2])
			    netRoutable = false;
			}
		      else if(routedLower[otherNet] && !route)
			{
			  //lower and lower
			  if((*overlap)[3])
			    netRoutable = false;
			}
		      else
			cout << "Problem in CoupleFree::findImplicationCoupleFreeSet()."
			     << "  Net listed as routed but not set upper or lower" << endl;
		      delete overlap;
		    }
		}
	    }

	  //if possible, route that net
	  if(netRoutable)
	    {
	      routed[origNetNum] = true;
	      if(route)
		{
		  routedUpper[origNetNum] = true;
		  (*routings)[origNetNum] = 1;
		}
	      else
		{
		  routedLower[origNetNum] = true;
		  (*routings)[origNetNum] = 2;
		}
	    
	    }
	}
    }
  
  delete netGraph;
}
    


bool CoupleFree::isCoupleFree()
{
  //uses the cycle based graph formulation
  bool returnBool = true;
  
  //make a 2 vertices for every net (upper and lower L)
  //nodes 0 thru nets->size()-1 are upper-L, 
  //nodes nets->size() thru 2*nets->size() - 1 are lower-L
  int numNets = nets->size();
  Graph *netGraph = createImplicationGraph();
  
  //see if there are any cycles
  BoolVector path(numNets,false);
  
  //first look at the upper-L vertices 
  for(int i = 0; i < numNets; i++)
    {
      GraphVertexVect * depthfs = netGraph->dfs(netGraph->getVertexAt(i));
      
      //search thru dfs graph to see there is a path to lower-L
      for(GraphVertexVectItr j = depthfs->begin(); j != depthfs->end(); j++)
	{
	  if(netGraph->getVertexAt(i+numNets) == (*j))
	    path[i] = true;
	}
      
      //clean shit up
      delete depthfs;
    }
  
  //now look at lower-L vertices (separated from upper-L only for convience)
  for(int i = numNets; i < 2*numNets; i++)
    {
      GraphVertexVect * depthfs = netGraph->dfs(netGraph->getVertexAt(i));
      
      //search thru dfs graph to see if there is a path to upper-L
      for(GraphVertexVectItr j = depthfs->begin(); j != depthfs->end(); j++)
	{
	  //if there is a path to upper-L (from lower-L) and a path from upper-L to
	  //lower-L there is a cycle and the graph is not routable
	  if(netGraph->getVertexAt(i-numNets) == (*j) && path[i-numNets])
	    returnBool = false;
	}
      //clean up
      delete depthfs;
    }
  
  delete netGraph;
  return returnBool;
  
}

Graph * CoupleFree::createImplicationGraph(NetVector *netVect)
{
  //make a 2 vertices for every net (upper and lower L)
  //nodes 0 thru nets->size()-1 are upper-L, 
  //nodes netVect->size() thru 2*netVect->size() - 1 are lower-L
  int numNets = netVect->size();
  Graph * netGraph = new Graph(2*numNets);
  
  //add the directed edges
  int i_counter = 0;
  int j_counter = 0;
  for(NetVectorItr i = netVect->begin(); i != netVect->end(); i++)
    {
      j_counter = 0;
      for(NetVectorItr j = netVect->begin(); j != netVect->end(); j++)
	{
	  if(i != j)
	    {
	      IntVector *coupleVect = findCoupleInteractions((*i),(*j));
	      if((*coupleVect)[0] != 0)
		netGraph->addDirectedEdgeBetween(i_counter,j_counter+numNets);
	      if((*coupleVect)[1] != 0)
		netGraph->addDirectedEdgeBetween(i_counter,j_counter);
	      if((*coupleVect)[2] != 0)
		netGraph->addDirectedEdgeBetween(i_counter+numNets,j_counter+numNets);
	      if((*coupleVect)[3] != 0)
		netGraph->addDirectedEdgeBetween(i_counter+numNets,j_counter);
	      delete coupleVect;
	    }
	  j_counter++;
	}
      i_counter++;
    }
  return netGraph;
}

Graph * CoupleFree::createImplicationGraph()
{
  return createImplicationGraph(nets);
}

int CoupleFree::numberViolations(Net *net, int netNumber, bool upper)
{
  NetCouplingInfo * netInfo = (*interactions)[netNumber];
  if(netInfo->net != net)
    cout << "Possible Fatal Error: Net does not match netNumber in " 
	 << "function CoupleFree::numberViolations()" << endl;
  int numViolations = 0;
  if(upper)
    {
      for(CoupleVectorItr i = netInfo->upperConflicts->begin(); 
	  i != netInfo->upperConflicts->end(); i++)
	{
	  if( (*i).upper > coupleLength && (*i).lower > coupleLength)
	    numViolations++;
	  // added for overlap
	  else if(planar && ((*i).planarUpper && (*i).planarLower))
	    numViolations++;
	  // end add for overlap
	}
    }
  else
    {
      for(CoupleVectorItr i = netInfo->lowerConflicts->begin();
	  i != netInfo->lowerConflicts->end(); i++)
	{
	  if( (*i).upper > coupleLength && (*i).lower > coupleLength)
	    numViolations++;
	  //added for overlap
	  else if(planar && ((*i).planarUpper && (*i).planarLower))
	    numViolations++;
	    
	}
    }

  return numViolations;
}

int CoupleFree::numberForcings(Net *net, int netNumber, bool upper)
{
  NetCouplingInfo * netInfo = (*interactions)[netNumber];
  if(netInfo->net != net) 
    cout << "Possible Fatal Error: Net does not match netNumber in " 
	 << "function CoupleFree::numberViolations()" << endl;
  int numForcings = 0;
  if(upper)
    {
      for(CoupleVectorItr i = netInfo->upperConflicts->begin(); 
	  i != netInfo->upperConflicts->end(); i++)
	{
	  if( (*i).upper > coupleLength)
	    numForcings++;
	  if((*i).lower > coupleLength)
	    numForcings++;
	  //added for overlap
	  if(planar)
	    {
	      if((*i).planarUpper)
		numForcings++;
	      if((*i).planarLower)
		numForcings++;
	    }
	  //end add for overlap
	      
	}
    }
  else
    {
      for(CoupleVectorItr i = netInfo->lowerConflicts->begin();
	  i != netInfo->lowerConflicts->end(); i++)
	{
	  if( (*i).upper > coupleLength)
	    numForcings++;
	  if((*i).lower > coupleLength)
	    numForcings++;
	  //added for overlap
	  if(planar)
	    {
	      if((*i).planarUpper)
		numForcings++;
	      if((*i).planarLower)
		numForcings++;
	    }
	  //end add for overlap
	}
    }

  return numForcings;
}

    

IntVector * CoupleFree::findCoupleInteractions(Net *net1, Net *net2)
{  

  //if the nets are the same, then they don't couple
  if(net1 == net2)
    return new IntVector(4);

  //get the upper-L and lower-L segments of net1 and net2
  SegmentVector *net1UpperSegs = net1->getUpperLSegments();
  SegmentVector *net1LowerSegs = net1->getLowerLSegments();
  SegmentVector *net2UpperSegs = net2->getUpperLSegments();
  SegmentVector *net2LowerSegs = net2->getLowerLSegments();

  //make these segment easier to refer to
  Segment *net1UpperSeg1 = &((*net1UpperSegs)[0]);
  Segment *net1UpperSeg2 = &((*net1UpperSegs)[1]);
  Segment *net1LowerSeg1 = &((*net1LowerSegs)[0]);
  Segment *net1LowerSeg2 = &((*net1LowerSegs)[1]);
  Segment *net2UpperSeg1 = &((*net2UpperSegs)[0]);
  Segment *net2UpperSeg2 = &((*net2UpperSegs)[1]);
  Segment *net2LowerSeg1 = &((*net2LowerSegs)[0]);
  Segment *net2LowerSeg2 = &((*net2LowerSegs)[1]);  

  //finally, do a bunch  of shit to find out which are horizontal and 
  // which are vertical
  Segment *net1UpperVert,*net1UpperHoriz,*net1LowerVert,*net1LowerHoriz;
  Segment *net2UpperVert,*net2UpperHoriz,*net2LowerVert,*net2LowerHoriz;
  if(net1UpperSeg1->isVertical() && net1UpperSeg2->isHorizontal())
    {
      net1UpperVert = net1UpperSeg1;
      net1UpperHoriz = net1UpperSeg2;
    }
  else if(net1UpperSeg1->isHorizontal() && net1UpperSeg2->isVertical())
    {
      net1UpperVert = net1UpperSeg2;
      net1UpperHoriz = net1UpperSeg1;
    }
  if(net1LowerSeg1->isVertical() && net1LowerSeg2->isHorizontal())
    {
      net1LowerVert = net1LowerSeg1;
      net1LowerHoriz = net1LowerSeg2;
    }
  else if(net1LowerSeg1->isHorizontal() && net1LowerSeg2->isVertical())
    {
      net1LowerVert = net1LowerSeg2;
      net1LowerHoriz = net1LowerSeg1;
    }
  if(net2UpperSeg1->isVertical() && net2UpperSeg2->isHorizontal())
    {
      net2UpperVert = net2UpperSeg1;
      net2UpperHoriz = net2UpperSeg2;
    }
  else if(net2UpperSeg1->isHorizontal() && net2UpperSeg2->isVertical())
    {
      net2UpperVert = net2UpperSeg2;
      net2UpperHoriz = net2UpperSeg1;
    }
  if(net2LowerSeg1->isVertical() && net2LowerSeg2->isHorizontal())
    {
      net2LowerVert = net2LowerSeg1;
      net2LowerHoriz = net2LowerSeg2;
    }
  else if(net2LowerSeg1->isHorizontal() && net2LowerSeg2->isVertical())
    {
      net2LowerVert = net2LowerSeg2;
      net2LowerHoriz = net2LowerSeg1;
    }

  //now find out the coupling interactions
  IntVector *returnVect = new IntVector(4);
  
  //find net1.upper coupling with net2.upper?
  (*returnVect)[0] = findCoupleInteractions(net1UpperVert,net2UpperVert) 
    + findCoupleInteractions(net1UpperHoriz,net2UpperHoriz);
  //find net1.upper coupling with net2.lower
  (*returnVect)[1] = findCoupleInteractions(net1UpperVert,net2LowerVert) 
    + findCoupleInteractions(net1UpperHoriz,net2LowerHoriz);
  //find net1.lower coupling with net2.upper
  (*returnVect)[2] = findCoupleInteractions(net1LowerVert,net2UpperVert) 
    + findCoupleInteractions(net1LowerHoriz,net2UpperHoriz);
  //find net1.lower coupling with net2.lower
  (*returnVect)[3] = findCoupleInteractions(net1LowerVert,net2LowerVert) 
    + findCoupleInteractions(net1LowerHoriz,net2LowerHoriz);

  //clean up my mess
  delete net1UpperSegs;
  delete net1LowerSegs;
  delete net2UpperSegs;
  delete net2LowerSegs;

  return returnVect;

}

int min(int one,int two)
{
  if(one < two)
    return one;
  else
    return two;
}

int max(int one, int two)
{
  if(one > two)
    return one;
  else 
    return two;
}

int CoupleFree::findCoupleInteractions(Segment *seg1, Segment *seg2)
{
  //first check if segments are orthogonal
  if((seg1->isVertical() && seg2->isHorizontal()) || 
     (seg1->isHorizontal() && seg2->isVertical()))
    return 0;

  int coupling = 0;
  Point * seg11 = seg1->getPointOne();
  Point * seg12 = seg1->getPointTwo();
  Point * seg21 = seg2->getPointOne();
  Point * seg22 = seg2->getPointTwo();

  if(seg1->isHorizontal() && seg2->isHorizontal())
    {
      int seg2Y = seg22->getY();
      int seg1Y = seg12->getY();
      int seg1Xmin = min(seg11->getX(), seg12->getX());
      int seg1Xmax = max(seg11->getX(), seg12->getX());
      int seg2Xmin = min(seg21->getX(), seg22->getX());
      int seg2Xmax = max(seg21->getX(), seg22->getX());
      
      //check if they are parallelly close enough to couple
      if((seg2Y < seg1Y - coupleDistance) || (seg2Y > seg1Y + coupleDistance))
	return 0;
      else
	{
	  if(seg1Xmin < seg2Xmin && seg2Xmin <= seg1Xmax)
	    coupling = min(seg1Xmax,seg2Xmax) - seg2Xmin;
	  else if(seg2Xmin <= seg1Xmin && seg1Xmin <= seg2Xmax)
	    coupling = min(seg1Xmax,seg2Xmax) - seg1Xmin;
	}
      return coupling;
    }
  else if(seg1->isVertical() && seg2->isVertical())
    {
      int seg2X = seg12->getX();
      int seg1X = seg22->getX();
      int seg1Ymin = min(seg11->getY(), seg12->getY());
      int seg1Ymax = max(seg11->getY(), seg12->getY());
      int seg2Ymin = min(seg22->getY(), seg21->getY());
      int seg2Ymax = max(seg22->getY(), seg21->getY());
      
      //check if they are parallelly close enough to couple
      if((seg2X < seg1X - coupleDistance) || (seg2X > seg1X + coupleDistance))
	return 0;
      else
	{
	  if(seg1Ymin < seg2Ymin && seg2Ymin <= seg1Ymax)
	    coupling = min(seg1Ymax,seg2Ymax) - seg2Ymin;
	  else if(seg2Ymin <= seg1Ymin && seg1Ymin <= seg2Ymax)
	    coupling = min(seg1Ymax,seg2Ymax) - seg1Ymin;
	}
      return coupling;

    }
}

void CoupleFree::calculateCoupleInteractions()
{
  int numNets = nets->size();
 
  //initialize the upper and lower conflict vectors in for each Net
  for(int i = 0; i < numNets; i++)
    {
      Net *currentNet = (*nets)[i];

      //coupleInfo corresponding to this net  
      NetCouplingInfo *tempInfo = (*interactions)[i]; 

      if(currentNet != tempInfo->net)
	{
	  cout << "Fatal Error: a net in couple interaction is mismatched" << endl;
	  exit(1);
	}

      for(int j = 0; j < numNets; j++)
	{
	  Net *compareNet = (*nets)[j];
	  //find if the how nets couple 
	  IntVector *tempVect = findCoupleInteractions(currentNet,compareNet);
	  BoolVector *tempBoolVect = findOverlapInteractions(currentNet,compareNet);

	  //this better not happen
	  if(tempVect->size() != 4 || tempBoolVect->size() != 4)
	    {
	      cout << "Fatal Error in CoupleFree.cc::calculateCoupleInteractions()"
		   << ". Exiting." << endl;
	      exit(1);
	    }
	  
	  (*tempInfo->upperConflicts)[j].upper = (*tempVect)[0];
	  (*tempInfo->upperConflicts)[j].lower = (*tempVect)[1];
	  (*tempInfo->lowerConflicts)[j].upper = (*tempVect)[2];
	  (*tempInfo->lowerConflicts)[j].lower = (*tempVect)[3];

	  (*tempInfo->upperConflicts)[j].planarUpper = (*tempBoolVect)[0];
	  (*tempInfo->upperConflicts)[j].planarLower = (*tempBoolVect)[1];
	  (*tempInfo->lowerConflicts)[j].planarUpper = (*tempBoolVect)[2];
	  (*tempInfo->lowerConflicts)[j].planarLower = (*tempBoolVect)[3];

	  delete tempVect;
	  delete tempBoolVect;

	}
    }
}

BoolVector * CoupleFree::findOverlapInteractions(Net *net1, Net *net2)
{  

  //if the nets are the same, then they don't overlap
  if(net1 == net2)
    return new BoolVector(4);

  //get the upper-L and lower-L segments of net1 and net2
  SegmentVector *net1UpperSegs = net1->getUpperLSegments();
  SegmentVector *net1LowerSegs = net1->getLowerLSegments();
  SegmentVector *net2UpperSegs = net2->getUpperLSegments();
  SegmentVector *net2LowerSegs = net2->getLowerLSegments();

  //make these segment easier to refer to
  Segment *net1UpperSeg1 = &((*net1UpperSegs)[0]);
  Segment *net1UpperSeg2 = &((*net1UpperSegs)[1]);
  Segment *net1LowerSeg1 = &((*net1LowerSegs)[0]);
  Segment *net1LowerSeg2 = &((*net1LowerSegs)[1]);
  Segment *net2UpperSeg1 = &((*net2UpperSegs)[0]);
  Segment *net2UpperSeg2 = &((*net2UpperSegs)[1]);
  Segment *net2LowerSeg1 = &((*net2LowerSegs)[0]);
  Segment *net2LowerSeg2 = &((*net2LowerSegs)[1]);  

  //finally, do a bunch  of shit to find out which are horizontal and 
  // which are vertical
  Segment *net1UpperVert,*net1UpperHoriz,*net1LowerVert,*net1LowerHoriz;
  Segment *net2UpperVert,*net2UpperHoriz,*net2LowerVert,*net2LowerHoriz;
  if(net1UpperSeg1->isVertical() && net1UpperSeg2->isHorizontal())
    {
      net1UpperVert = net1UpperSeg1;
      net1UpperHoriz = net1UpperSeg2;
    }
  else if(net1UpperSeg1->isHorizontal() && net1UpperSeg2->isVertical())
    {
      net1UpperVert = net1UpperSeg2;
      net1UpperHoriz = net1UpperSeg1;
    }
  if(net1LowerSeg1->isVertical() && net1LowerSeg2->isHorizontal())
    {
      net1LowerVert = net1LowerSeg1;
      net1LowerHoriz = net1LowerSeg2;
    }
  else if(net1LowerSeg1->isHorizontal() && net1LowerSeg2->isVertical())
    {
      net1LowerVert = net1LowerSeg2;
      net1LowerHoriz = net1LowerSeg1;
    }
  if(net2UpperSeg1->isVertical() && net2UpperSeg2->isHorizontal())
    {
      net2UpperVert = net2UpperSeg1;
      net2UpperHoriz = net2UpperSeg2;
    }
  else if(net2UpperSeg1->isHorizontal() && net2UpperSeg2->isVertical())
    {
      net2UpperVert = net2UpperSeg2;
      net2UpperHoriz = net2UpperSeg1;
    }
  if(net2LowerSeg1->isVertical() && net2LowerSeg2->isHorizontal())
    {
      net2LowerVert = net2LowerSeg1;
      net2LowerHoriz = net2LowerSeg2;
    }
  else if(net2LowerSeg1->isHorizontal() && net2LowerSeg2->isVertical())
    {
      net2LowerVert = net2LowerSeg2;
      net2LowerHoriz = net2LowerSeg1;
    }

  //now find out the overlap interactions
  BoolVector *returnVect = new BoolVector(4);
  
  //find net1.upper overlap with net2.upper?
  (*returnVect)[0] = net1UpperVert->overlap(net2UpperVert) 
    || net1UpperHoriz->overlap(net2UpperHoriz);
  //find net1.upper overlap with net2.lower
  (*returnVect)[1] = net1UpperVert->overlap(net2LowerVert) 
    || net1UpperHoriz->overlap(net2LowerHoriz);
  //find net1.lower overlap with net2.upper
  (*returnVect)[2] = net1LowerVert->overlap(net2UpperVert) 
    || net1LowerHoriz->overlap(net2UpperHoriz);
  //find net1.lower coupling with net2.lower
  (*returnVect)[3] = net1LowerVert->overlap(net2LowerVert) 
    || net1LowerHoriz->overlap(net2LowerHoriz);

  //clean up my mess
  delete net1UpperSegs;
  delete net1LowerSegs;
  delete net2UpperSegs;
  delete net2LowerSegs;

  return returnVect;

}

void CoupleFree::printRouting()
{
  int counter = 0;
  for(IntVectorItr i = routings->begin(); i != routings->end(); i++)
    {
      cout << "Net " << counter << " is ";
      if((*i) == 0)
	cout << "not routed" << endl;
      else if((*i) == 1)
	cout << "upper-L routed" << endl;
      else if((*i) == 2)
	cout << "lower-L routed" << endl;
      else
	cout << "illegally routed" << endl;
      counter++;
    }
}

void CoupleFree::printRouting(char *filename)
{
  ofstream outFile(filename,ios::out);
  
  if(!outFile)
    {
      cout << "File could not be opened" << endl;
      return;
    }
  

  //first find width and height (largest x, y pin location)
  int xmax = 0;
  int ymax = 0;
  for(NetVectorItr i = nets->begin(); i != nets->end(); i++)
    {
      Net *tempNet = (*i);
      for(int j = 0; j < tempNet->getNumPins(); j++)
	{
	  if(xmax < tempNet->getPin(j)->getX())
	    xmax = tempNet->getPin(j)->getX();
	  if(ymax < tempNet->getPin(j)->getY())
	    ymax = tempNet->getPin(j)->getY();
	}
    }

  //input width and height
  outFile << xmax << " " << ymax << endl;
      
  //input numNets
  outFile << nets->size() << endl;

  //input net info
  int counter = 0;
  for(NetVectorItr i = nets->begin(); i != nets->end(); i++)
    {
      SegmentVector * segs;
      if((*routings)[counter] == 0) { segs = NULL; }
      else if((*routings)[counter] == 1) { segs = (*i)->getUpperLSegments(); }
      else if((*routings)[counter] == 2) { segs = (*i)->getLowerLSegments(); }
      
      if(segs == NULL)
	outFile << (*i)->getName() << " " << (*i)->getNumPins() << " " <<  0 << endl;
      else
	outFile << (*i)->getName() << " " << (*i)->getNumPins() << " " << segs->size()
		<< endl;
      
      //now print the pins
      Net *tempNet = (*i);
      for(int j = 0; j < tempNet->getNumPins(); j++)
	{
	  outFile << tempNet->getPin(j)->getX() << " " << tempNet->getPin(j)->getY() 
		  << " ";
	}

      outFile << endl;
      //now print segments
      if(segs != NULL)
	{
	  for(int j = 0; j < segs->size(); j++)
	    {
	      Point *one = (*segs)[j].getPointOne();
	      Point *two = (*segs)[j].getPointTwo();
	      outFile << one->getX() << " " << one->getY() << " " << two->getX() 
		      << " " << two->getY() << " ";
	    }
	}
      outFile << endl;
      counter++;
      delete segs;
    }
}

void CoupleFree::printStats(ofstream &stream)
{
  stream << "Number of routed nets: " << getNumRoutedNets() << endl;
  stream << "Total number of nets: " << getNumNets() << endl;
  stream << "Percentage routed nets: " 
	 << (float) ((getNumRoutedNets()*100)/getNumNets()) << "%" << endl;
  stream << "Criticality of routed nets: " << getCriticalityRoutedNets() << endl;
  stream << "N-Root-N Criticality: " << getNRootNCriticalityRoutedNets() << endl;
  stream << "Length of routed nets: " << getRoutedNetLength() << endl;
}
