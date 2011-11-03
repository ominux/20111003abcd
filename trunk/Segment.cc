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

#include "Segment.h"
#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <math.h>

using namespace std;

int Segment::min(int one, int two)
{
  if(one < two)
    return one;
  else
    return two;
}

int Segment::max(int one, int two)
{
  if(one > two)
    return one;
  else
    return two;
}

double Segment::getLength()
{ 
  return pow(abs(one.getX() - two.getX()),2) + pow(abs(one.getY()-two.getY()),2); 
}

bool Segment::overlap(Segment *seg)
{
  Point *segOne = seg->getPointOne();
  Point *segTwo = seg->getPointTwo();

  if(isVertical() && seg->isVertical() && one.getX() == segOne->getX())
    {
      int minSegY = min(segOne->getY(), segTwo->getY());
      int maxSegY = max(segOne->getY(), segTwo->getY());
      int minY = min(one.getY(), two.getY());
      int maxY = max(one.getY(), two.getY());
      
      if(minSegY < minY && minY <= maxSegY)
	return true;
      else if(minY <= minSegY && minSegY <= maxY)
	return true;
    }
  else if(isHorizontal() && seg->isHorizontal()&& one.getY() == segOne->getY())
    {
      int minSegX = min(segOne->getX(), segTwo->getX());
      int maxSegX = max(segOne->getX(), segTwo->getX());
      int minX = min(one.getX(), two.getX());
      int maxX = max(one.getX(), two.getX());
      
      if(minSegX < minX && minX <= maxSegX)
	return true;
      else if(minX <= minSegX && minSegX <= maxX)
	return true;
    }
  return false;
}
      
void Segment::print()
{
  one.print();
  cout << ",";
  two.print();
  cout << endl;
}
