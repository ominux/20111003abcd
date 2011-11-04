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

#ifndef POINT_H
#define POINT_H

#include <iostream>

using namespace std;

class Point
{

public:
  
  //constructors
  inline Point() { x = y = 0; }
  inline Point(int xValue, int yValue) {x = xValue; y = yValue;}
  inline ~Point() { }

  //modifiers
  inline void setX(int xValue) { x = xValue; }
  inline void setY(int yValue) { y = yValue; }
  
  //accessors
  inline int getX() const { return x; }
  inline int getY() const { return y; }

  //misc
  int getManhattanDistance(Point *aPoint);
  inline void print() { cout << "(" << x << "," << y << ")"; }

 protected:
  int x, y;
};


#endif
