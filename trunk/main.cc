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

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <algorithm>
#include <time.h>
#include <numeric>
#include "Point.h"
#include "Maze.h"
#include "MazePoint.h"
#include "PrimMST.h"
#include "RouteTracks.h"
#include "Pin.h"
#include "Net.h"
#include "CoupleFree.h"

#define NUM_REROUTE 200

using namespace std;

typedef vector<Net *> NetVector;
typedef NetVector::iterator NetVectorItr;

long my_lrand48(void);
void my_srand48(unsigned long seed);

class InvalidNetCheck //return true if net is not routable
{
public:
  inline bool operator() (Net *theNet)
  { return theNet->getNumPins() >= 50 || theNet->getBoundingBox()->getPerimeter() == 0; }
};

    
class BoundingBoxSort //smallest to largest
{
public:
  inline bool operator() (Net *net1, Net *net2)
  { return net1->getBoundingBox()->getPerimeter() 
      < net2->getBoundingBox()->getPerimeter(); }
};

class BoundingBoxSortLargeToSmall
{
public:
  inline bool operator() (Net *net1, Net *net2)
  { return net1->getBoundingBox()->getPerimeter() > 
      net2->getBoundingBox()->getPerimeter(); }
};
    
class BoundingBoxAccum //largest to smallest
{
public:
  inline int operator() (int value, Net *net2)
  { return value 
      + net2->getBoundingBox()->getPerimeter(); }
};

class RandomGenerator
{
public:
  inline int operator() (int num)
  { my_srand48(time(NULL)); return my_lrand48() % num; }
};


void removeUnroutableNets(NetVector *allNets)
{
  NetVectorItr lastValidNet = 
    remove_if(allNets->begin(), allNets->end(), InvalidNetCheck());
  
  //erase remaining invalid pins
  allNets->erase(lastValidNet, allNets->end());
}

void parseRoutableNets(NetVector *allNets, bool largeToSmall)
{
  removeUnroutableNets(allNets);

  //sort nets largest to smallest according to bounding box
  if(largeToSmall)
    {
      cout << "Sorting nets from largest BB to smallest BB" << endl;
      sort(allNets->begin(), allNets->end(), BoundingBoxSortLargeToSmall());
    }
  //sort nets smallest to largest according to bounding box
  else
    {
      cout << "Sorting nets from smallest BB to largest BB" << endl;
      sort(allNets->begin(), allNets->end(), BoundingBoxSort());
    }
  
}

//JW void main(int argc, char *argv[])
int main(int argc, char *argv[])
{
  ifstream input;
  ofstream lengthOutput;
  ofstream displayOutputFile;
  ofstream routeOutputFile;
  bool routeToNet = false;
  bool outputLength = false;
  bool twoTerminal = false;
  bool displayOutput = false;
  bool routeOutput = false;
  bool boundingBoxRoute = false;
  bool largeToSmall = false;
  bool planar = false;
  bool coupling = false;
  bool flexibility = false;
  int numCouplingNets = 50;
  int planarCapacity = 0;
  double xPercent = 0;
  char coupleFileName[80];
  
  for(int i = 1; i < argc; i++)
    {
      if(argc > 1)
	{
	  if(strcmp(argv[i], "-r") == 0)
	    routeToNet = true;
	  
	  else if(strcmp(argv[i],"-f") == 0)
	    {
	      i++;
	      input.open(argv[i], ios::in);
	      if(!input)
		{
		  cout << argv[i] << " could not be opened.  Exiting" << endl;
		  return -1;
		}
	      else
		cout << argv[i] << " was opened" << endl;
	    }
	  else if(strcmp(argv[i], "-l") == 0)
	    {
	      i++;
	      lengthOutput.open(argv[i], ios::out);
	      if(!lengthOutput)
		cout << argv[i] << " could not be opened.  Length will not be " 
		     << "outputed to file " << endl;
	      else
		{
		  cout << argv[i] << " was opened for length output" << endl;
		  outputLength = true;
		}
	    }
	  else if(strcmp(argv[i], "-d") == 0)
	    {
	      i++;
	      displayOutputFile.open(argv[i], ios::out);
	      if(!displayOutputFile)
		cout << argv[i] << " could not be opened.  Congestion display "
		     << "will not be outputed to file " << endl;
	      else
		{
		  cout << argv[i] << " was opened for congestion display output" << endl;
		  displayOutput = true;
		}
	    }
	  else if(strcmp(argv[i], "-routeOutput") == 0)
	    {
	      i++;
	      routeOutputFile.open(argv[i], ios::out);
	      if(!routeOutputFile)
		cout << argv[i] << " could not be opened.  Net routing "
		     << "will not be outputed to file " << endl;
	      else
		{
		  cout << argv[i] << " was opened for net routing output" << endl;
		  routeOutput = true;
		}
	    }
	  else if(strcmp(argv[i],"-%s") == 0)
	    {
	      i++;
	      xPercent = atoi(argv[i]);
	      largeToSmall = false;
	      cout << "placing " << xPercent << "% of smallest nets w/ one-bend route."
		   << endl;
	    } 
	  else if(strcmp(argv[i],"-%l") == 0)
	    {
	      i++;
	      largeToSmall = true;
	      xPercent = atoi(argv[i]);
	      cout << "placing " << xPercent << "% of largest nets w/ one-bend route."
		   << endl; 
	    }
	  else if(strcmp(argv[i],"-2") == 0)
	    {
	      cout << "splitting nets into two terminal nets" << endl;
	      twoTerminal = true;
	    }
	  else if(strcmp(argv[i],"-flex") == 0)
	    {
	      cout << "setting flexibility option" << endl;
	      flexibility= true;
	    }
	  else if(strcmp(argv[i],"-p") == 0)
	    {
	      i++;
	      planar = true;
	      planarCapacity = atoi(argv[i]);
	      cout << "performing a planar routing with " << planarCapacity 
		   << "-d 1-bend routing" << endl;
	    }
	  else if(strcmp(argv[i],"-b") == 0)
	    {
	      cout << "doing a bounding box routing" << endl;
	      boundingBoxRoute = true;
	    }
	  else if(strcmp(argv[i],"-couple") == 0)
	    {
	      i++;
	      cout << "Performing coupling analysis" << endl;
	      cout << "Setting -2 option (split nets into two terminals)" << endl;
	      numCouplingNets = atoi(argv[i]);
	      cout << "Number of coupling nets is " << numCouplingNets << endl;
	      largeToSmall = true;
	      twoTerminal = true;
	      coupling = true;
	      i++;
	      strcpy(coupleFileName,argv[i]);
	      cout << "Couple base file name: " << coupleFileName << endl;
	    }   
	}
    }
  
  //Read in the row and columns.  grid column row

  //need to change
  int x,y,numNets,verticalCapacity,horizontalCapacity;
  char buffer[80];
  input.getline(buffer, 80);
  sscanf(buffer, "grid %d %d", &x, &y);
  input.getline(buffer, 80);
  sscanf(buffer, "vertical capacity %d",&verticalCapacity);
  input.getline(buffer, 80);
  sscanf(buffer, "horizontal capacity %d",&horizontalCapacity);
  
  //Read in num nets and process nets and their pins
  input.getline(buffer, 80);
  sscanf(buffer, "num net %d", &numNets);
  NetVector nets(numNets);
  for(int i = 0; i < numNets; i++)
    {
      char netName[64];
      int netID,numPins;
      input >> netName;
      input.getline(buffer, 80);
      sscanf(buffer, "%d %d", &netID, &numPins);
      nets[i] = new Net(numPins, netID, netName);
      for(int j = 0; j < numPins; j++)
	{
	  int pinX, pinY;
	  input.getline(buffer, 80);
	  sscanf(buffer, "%d %d", &pinX, &pinY);
	  if(pinX < 0 || pinX >= x || pinY < -1 || pinY >= y)
	    {
	      nets[i]->removeLastPin();
	      cout << "removing invalid pin " << pinX << "," << pinY << endl;
	    }
	  else
	    {
	      Pin *aPin = new Pin(pinX, pinY);
	      nets[i]->setPin(j, aPin);
	    }
	}
    }
  cout << "done getting input" << endl; 
  cout << nets.size() << " initial nets" << endl;
  NetVector *routeNets;
  NetVector twoTerminalNets;

  //get rid of large nets and nets with only one pin
  removeUnroutableNets(&nets);

  if(twoTerminal)
    {
      //break into two terminal nets
      for(NetVectorItr i = nets.begin(); i != nets.end(); i++)
	{
	  PrimMST prim(*i);
	  prim.twoTerminalNets(twoTerminalNets);
	}
      routeNets = &twoTerminalNets;
      cout << twoTerminalNets.size() << " two terminal nets" << endl;
    }
  else
    routeNets = &nets;

  //sort nets large to small or vice versa
  parseRoutableNets(routeNets,largeToSmall);

  //initiate and set route track constraints
  RouteTracks routeTracks(x,y);
  routeTracks.setVerticalCapacity(verticalCapacity);
  routeTracks.setHorizontalCapacity(horizontalCapacity);
  cout << "The vertical capacity (from input file) is " << verticalCapacity << endl;
  cout << "The horizontal capacity (from input file) is " << horizontalCapacity << endl;
  double segNets = routeNets->size()*(xPercent/100);
  int numSegmentNets = (int) segNets;
  NetVectorItr netItr = routeNets->begin();
  
  clock_t start, after_x, after_maze, after_rr;
  //uncomment next line to randomize ordering of the nets
  //random_shuffle(netItr,routeNets->end(),RandomGenerator());
  cout << "one-bend routing " << numSegmentNets << " nets" << endl;
  int totalBoundingBox = accumulate(routeNets->begin(), routeNets->end(), 
				    0, BoundingBoxAccum());
  if(routeNets->size() != 0)
    cout << "Total bb is " << totalBoundingBox << ".  Avg bounding box " 
	 << totalBoundingBox/routeNets->size() << endl;


  if(boundingBoxRoute)
    {
      for(NetVectorItr i = routeNets->begin(); i != routeNets->end(); i++)
	{
	  routeTracks.routeBBNet(*i);
	  cout << (*i)->getBoundingBox()->getPerimeter() << endl;
	}
      routeTracks.print();
      cout << "overflow is " << routeTracks.getOverflow() << endl;
      return 0;
    }
  
  if(coupling)
    {
      //open display output file
      char couplingGreedyFilename[80];
      char couplingForceFilename[80];
      char couplingForceOutFilename[80];
      char couplingGreedyOutFilename[80];
      char couplingImpOutFilename[80];
      char couplingImpFilename[80];
      char couplingFunctionOutFilename[80];
      char couplingFunctionFilename[80];

      ofstream couplingGreedyOutFile,couplingForceOutFile,
	couplingImpOutFile,couplingFunctionOutFile;
      
      strcpy(couplingGreedyFilename,coupleFileName);
      strcat(couplingGreedyFilename,"-greedy.disp");
      strcpy(couplingForceFilename,coupleFileName);
      strcat(couplingForceFilename,"-force.disp");
      strcpy(couplingImpFilename,coupleFileName);
      strcat(couplingImpFilename,"-imp.disp");
      strcpy(couplingFunctionFilename,coupleFileName);
      strcat(couplingFunctionFilename,"-function.disp");
      
      strcpy(couplingForceOutFilename,coupleFileName);
      strcat(couplingForceOutFilename,"-force.out");
      strcpy(couplingGreedyOutFilename,coupleFileName);
      strcat(couplingGreedyOutFilename,"-greedy.out");
      strcpy(couplingImpOutFilename,coupleFileName);
      strcat(couplingImpOutFilename,"-imp.out");
      strcpy(couplingFunctionOutFilename,coupleFileName);
      strcat(couplingFunctionOutFilename,"-function.out");

      
      couplingGreedyOutFile.open(couplingGreedyOutFilename, ios::out);
      if(!couplingGreedyOutFile)
	{
	  cout << couplingGreedyOutFilename << " could not be opened." 
	       << endl;
	}
      else
	cout << couplingGreedyOutFilename << " was opened" << endl;
      
      
      couplingForceOutFile.open(couplingForceOutFilename, ios::out);
      if(!couplingForceOutFile)
	{
	  cout << couplingForceOutFilename<< " could not be opened." 
	       << endl;
	}
      else
	cout << couplingForceOutFilename << " was opened for display output" 
	     << endl;

      couplingImpOutFile.open(couplingImpOutFilename, ios::out);
      if(!couplingImpOutFile)
	{
	  cout << couplingImpOutFilename<< " could not be opened." 
	       << endl;
	}
      else
	cout << couplingImpOutFilename << " was opened for display output" 
	     << endl;

      couplingFunctionOutFile.open(couplingFunctionOutFilename, ios::out);
      if(!couplingFunctionOutFile)
	{
	  cout << couplingFunctionOutFilename<< " could not be opened." 
	       << endl;
	}
      else
	cout << couplingFunctionOutFilename << " was opened for display output" 
	     << endl;	     

      //hack done to preserve the ordering of nets within the functions
      NetVector *coupleNets = new NetVector(numCouplingNets);
      NetVector *coupleNets2 = new NetVector(numCouplingNets);
      NetVector *coupleNets3 = new NetVector(numCouplingNets);
      NetVector *coupleNets4 = new NetVector(numCouplingNets);

      for(int i = 0; i < numCouplingNets; i++)
	{
	  (*coupleNets)[i] = (*routeNets)[i];
	  (*coupleNets2)[i] = (*routeNets)[i];
	  (*coupleNets3)[i] = (*routeNets)[i];
	  (*coupleNets4)[i] = (*routeNets)[i];
	}
      CoupleFree coupleFreeQuick(coupleNets,1,10,true);
      coupleFreeQuick.findQuickCoupleFreeSet();
      //coupleFreeQuick.printRouting(couplingForceFilename);
      coupleFreeQuick.printStats(couplingForceOutFile);
      
      CoupleFree coupleFreeGreedy(coupleNets2,1,10,true);
      coupleFreeGreedy.findGreedyCoupleFreeSet();
      //coupleFreeGreedy.printRouting(couplingGreedyFilename);
      coupleFreeGreedy.printStats(couplingGreedyOutFile);

      CoupleFree coupleFreeImplicate(coupleNets3,1,10,true);
      coupleFreeImplicate.findImplicationCoupleFreeSet();
      //coupleFreeImplicate.printRouting(couplingImpFilename);
      coupleFreeImplicate.printStats(couplingImpOutFile);

      CoupleFree coupleFreeFunction(coupleNets4,1,10,true);
      coupleFreeFunction.findForcingFunctionCoupleFreeSet();
      //coupleFreeQuick.printRouting(couplingFunctionFilename);
      coupleFreeFunction.printStats(couplingFunctionOutFile);

      delete coupleNets;
      delete coupleNets2;
      delete coupleNets3;
      delete coupleNets4;
      
      return 0;

    }

  start = clock();

  if(planar)
    {
      //find set of 1-d routes, route them and lock them
      int numPlanarNets = 0;
      int sumBBPlanarNets = 0;
      for(NetVectorItr i = routeNets->begin(); i != routeNets->end(); i++)
	{
	  if(routeTracks.isDensityOneBendRoutable(*i,planarCapacity))
	    {
	      routeNets->erase(i);
	      numPlanarNets++;
	      sumBBPlanarNets += (*i)->getBoundingBox()->getPerimeter();
	      cout << (*i)->getBoundingBox()->getPerimeter() << endl;
	    }
	}
      double avgBBPlanarNets = sumBBPlanarNets/numPlanarNets;
      cout << "Routed " << numPlanarNets << " planar nets with total wirelength " 
	   << sumBBPlanarNets << " and avg BB of " << avgBBPlanarNets << endl;
    }
  else
    {
      for(int i = 0; i < numSegmentNets; i++)
	{  
	  routeTracks.routeOneBendNet(*netItr);
	  //routeTracks.zRouteNet(*netItr);
	  (*netItr)->setPermanent();
	  netItr++;
	}
    }
 
  
  after_x = clock();
  cout << "time to 1-bend route x% nets " << after_x << endl;
  int totalRoutedBB = accumulate(routeNets->begin(), netItr,0, 
				 BoundingBoxAccum());
  if(numSegmentNets != 0)
    cout << "avg routed bb " << totalRoutedBB/numSegmentNets << endl;
  //return 0;
  cout << "Route track wire length " << routeTracks.getWireLength() << endl;
  //routeTracks.print();
  
  /*
  //randomizes the remaining nets for routing
  NetVector randomNets(netItr,routeNets->end());
  random_shuffle(randomNets.begin(), randomNets.end(), RandomGenerator());
  cout << randomNets.size() << endl;
  for(NetVectorItr j = randomNets.begin(); j != randomNets.end(); j++)
    {
      if((*j)->getNumPins() < 50)
	{
	  Maze aMaze(x,y, &routeTracks);
	  if(routeToNet)
	    aMaze.setCostFnToRoutePts();
	  //cout << "Routing net " << (*j)->getName() << endl;
	  if(!aMaze.routeNet(*j))
	    cout << "could not route net " << (*j)->getName()  << endl;
	}
    }
    */
  
  for(; netItr != routeNets->end(); netItr++)
    {    
      Maze aMaze(x,y, &routeTracks);
      //cout << "Routing net " << (*netItr)->getName() << endl;
      if(!aMaze.routeNet(*netItr))
	cout << "could not route net " << (*netItr)->getName()  << endl;

    }
  after_maze = clock();
  
  //cout << "wire length b/f r+r " << routeTracks.getWireLength() << endl;
  cout << "overflow b/f r+r " << routeTracks.getOverflow() << endl;
  cout << "starting rip and reroute" << endl;
  NetList *rerouteNets = 0;
  int bestOverflow = routeTracks.getOverflow();
  int overflowsPast = 0;
  int numEdgeRips = 0;
  if(routeTracks.getOverflow() != 0)
    rerouteNets = routeTracks.ripNets();
  while(rerouteNets != 0 && overflowsPast < NUM_REROUTE)
    {
      for(NetListItr i = rerouteNets->begin(); i != rerouteNets->end(); i++)
	{    
	  Maze aMaze(x,y, &routeTracks);
	  if(!aMaze.routeNet(*i))
	    cout << "could not route net " << (*i)->getName()  << endl;
	}
      delete rerouteNets;
      rerouteNets = routeTracks.ripNets();
      if(routeTracks.getOverflow() < bestOverflow)
	{
	  overflowsPast = 0;
	  bestOverflow = routeTracks.getOverflow();
	}
      else
	overflowsPast++;
      if(routeTracks.getOverflow() == 0)
	overflowsPast = NUM_REROUTE; //cause the while loop to break
      numEdgeRips++;
    }
  after_rr = clock();
  //routeTracks.print();
  cout << "best overflow: " << bestOverflow << endl;
  cout << "end wire length: " << routeTracks.getWireLength() << endl;
  cout << "end overflow: " << routeTracks.getOverflow() << endl;
  cout << "start " << start << " after_x " << after_x << " after_maze "
       << after_maze << " after_rr " << after_rr << endl;
  cout << "number of edge rips " << numEdgeRips << endl;
  
  if(flexibility)
    {
      cout << "Flex Route Length, Flex Overflow, Flex Congestion," 
	   << "UnFlex Route Length, Unlex Overflow, Unflex Congestion" << endl;
      for(NetVectorItr i = routeNets->begin(); i != routeNets->end(); i++)
	{
	  int flex = (*i)->isFlexible();
	  if(flex)
	    {
	      routeTracks.removeRoutingFromEdges(*i);
	      routeTracks.zRouteSteinerNet(*i);
	      cout << (*i)->getNumEdges() << "," 
		   << routeTracks.findRoutingOverflow(*i) << "," 
		   << routeTracks.findRoutingCongestion(*i) << ",";

	      (*i)->makeUnflexibleSteinerTree();
	      routeTracks.removeRoutingFromEdges(*i);
	      routeTracks.zRouteSteinerNet(*i);
	      cout << (*i)->getNumEdges() << "," 
		   << routeTracks.findRoutingOverflow(*i) << "," 
		   << routeTracks.findRoutingCongestion(*i)
		   << endl;
	    }
	  /*
	  if(flex)
	    {
	      //first route w/ flexible tree
	      routeTracks.removeRoutingFromEdges(*i);
	      (*i)->printPins();
	      Maze aMaze(x,y, &routeTracks);
	      aMaze.routeSteinerNet(*i);
	      cout << "Route length = " << (*i)->getNumEdges() << endl;
	      cout << "Route Overflow = " << routeTracks.findRoutingOverflow(*i)
		   << endl;
	      cout << "Route Congestion = " << routeTracks.findRoutingCongestion(*i)
		   << endl;
	      //now route w/ unflexible tree
	      Maze maze2(x,y,&routeTracks);
	      (*i)->makeUnflexibleSteinerTree();
	      routeTracks.removeRoutingFromEdges(*i);
	      maze2.routeSteinerNet(*i);
	      cout << "Route length = " << (*i)->getNumEdges() << endl;
	      cout << "Route Overflow = " << routeTracks.findRoutingOverflow(*i)
		   << endl;
	      cout << "Route Congestion = " << routeTracks.findRoutingCongestion(*i)
		   << endl;
	    }
	  */
	}
    }
  
  //display output
  if(displayOutput)
    {
      routeTracks.printCongestionDisplayFile(displayOutputFile);
    }
  
  //net routing output
  if(routeOutput)
    {
      for(NetVectorItr i = routeNets->begin(); i != routeNets->end(); i++)
	{
	  (*i)->printEdges(routeOutputFile);
	}
      
    }

  //output wire lengths to lengthOutput file handle
  if(outputLength)
    {
      NetVectorItr i = routeNets->begin();
      for(int j = 0; j < numSegmentNets; j++)
	{
	  lengthOutput << (*i)->getName() << "," << (*i)->getNumEdges() << endl;
	  i++;
	}
    }
  
}
