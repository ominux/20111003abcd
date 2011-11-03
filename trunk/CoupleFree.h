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

#ifndef COUPLEFREE_H
#define COUPLEFREE_H

#include <vector>
#include <iostream>
#include <fstream>
#include "Net.h"
#include "Graph.h"

using namespace std;


//A class that routes two terminal nets without any coupling.  
//A net couples with another net iff it is coupleDistance bins close AND
//  runs in parallel with that net for more than coupleLength bins
//NOTE: all the nets in (NetVect *) nets are assumed to be two terminal.  
//      There is little checking for this in the code.
//Criticality is defined as the perimeter of a net squared
//The CouplingInteractions hold the amount of coupling.  Therefore, they 
//  might have a coupling value less than the couple distance (which means
//  that they don't actually couple
//The planar variable determines whether the routings must be planar

typedef vector<bool> BoolVector;
typedef BoolVector::iterator BoolVectorItr;
typedef vector<Net *> NetVector;
typedef NetVector::iterator NetVectorItr;
typedef vector<int> IntVector;
typedef IntVector::iterator IntVectorItr;


//an internal class used in NetCoupling Info
class UpperLowerCoupling
{
public:
  int upper;
  int lower;
  bool planarUpper;
  bool planarLower;
};



typedef vector<UpperLowerCoupling> CoupleVector;
typedef CoupleVector::iterator CoupleVectorItr;

//an internal class used in findQuickCoupleFreeSet
class NetCouplingInfo
{
public:
  NetCouplingInfo(int size);
  ~NetCouplingInfo();
  Net *net;
  CoupleVector *upperConflicts;
  CoupleVector *lowerConflicts;
};
  
typedef vector<NetCouplingInfo *> CoupleInteractions;
typedef CoupleInteractions::iterator CoupleInteractionsItr;

class CoupleFree
{
public:
  //constructors/destructor
  inline CoupleFree() 
  { nets = NULL; coupleDistance = 1; coupleLength = 3; planar = false; netsize = 0;}
  CoupleFree(NetVector *netVect); 
  CoupleFree(NetVector *netVect, int dist, int length);
  CoupleFree(NetVector *netVect, int dist, int length, bool plane);
  ~CoupleFree();

  //editors
  inline void setNets(NetVector *netVect) { nets = netVect; }
  inline void setCoupleParameters(int dist, int length)
  { coupleDistance = dist; coupleLength = length; }
  inline void setCoupleDistance(int dist) { coupleDistance = dist; }
  inline void setCoupleLength(int length) { coupleLength = length; }
  inline void setPlanar(bool plane) { planar = plane; }

  //accessors
  inline NetVector * getNets() { return nets; }
  inline int getCoupleDistance() { return coupleDistance; }
  inline int getCoupleLength() { return coupleLength; }
  inline bool isPlanar() { return planar; }
  inline int getNumNets() { return nets->size(); }
  int getNumRoutedNets();
  int getCriticalityRoutedNets();
  double getNRootNCriticalityRoutedNets();
  int getRoutedNetLength();
  int getNetIndex(Net *);

  //we define this a bounding box perimeter squared
  static int getCriticality(Net *);
  static double getNRootNCriticality(Net *);

  //misc
  void findQuickCoupleFreeSet();
  void findGreedyCoupleFreeSet();
  void findImplicationCoupleFreeSet();
  void findForcingFunctionCoupleFreeSet();

  //determines if all nets are coupling-free routable
  bool isCoupleFree();

  //creates the implication graph for the net in NetVector
  Graph * createImplicationGraph(NetVector *);
  //default creates implication graph for class member nets
  Graph * createImplicationGraph();

  //finds the number of nets who cannot be couple-free routed if this net
  //is routed in upper-L (upper == true) or lower-L (upper == false)
  //Note: this only measures the number of direct violations, not indirect
  int numberViolations(Net *, int netNum, bool upper);
  
  //like above, but returns number of forcings
  //Note: this only measures the number of direct forcings, not indirect
  int numberForcings(Net *, int netNum, bool upper);

  
  //IntVector has size of 4.  
  //int[0] has net1.upper couples with net2.upper
  //int[1] has net1.upper couples with net2.lower
  //int[2] has net1.lower couples with net2.upper
  //int[3] has net1.lower couples with net2.lower
  IntVector * findCoupleInteractions(Net *net1, Net *net2);
  inline IntVector * findCoupleInteractions(int net1, int net2)
  { return findCoupleInteractions((*nets)[net1],(*nets)[net2]); }

  //BoolVector has size of 4
  //bool[0] has net1.upper overlaps with net2.upper
  //bool[1] has net1.upper overlaps with net2.lower
  //bool[2] has net1.lower overlaps with net2.upper
  //bool[3] has net1.lower overlaps with net2.lower
  BoolVector * findOverlapInteractions(Net *net1, Net *net2);
  inline BoolVector * findOverlapInteractions(int net1, int net2)
  { return findOverlapInteractions((*nets)[net1],(*nets)[net2]); }

  //returns the amount of coupling
  int findCoupleInteractions(Segment *, Segment *);

  void printRouting();
  void printRouting(char *);
  void printStats(ofstream &);
  

private:
  //internal variables
  NetVector *nets;
  int netsize;
  CoupleInteractions *interactions;
  int coupleDistance;
  int coupleLength;
  IntVector *routings; //0 if not routed, 1 if upper-L, 2 if lower-L
  bool planar;


  //internal functions
  void calculateCoupleInteractions();
};



#endif
