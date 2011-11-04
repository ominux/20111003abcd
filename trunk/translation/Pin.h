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

#ifndef PIN_H
#define PIN_H

#include "Point.h"

class Pin : public Point
{
  
public:
  //constructors
  inline Pin() : Point() { connected = false; }
  inline Pin(int xVal, int yVal) : Point(xVal,yVal) { connected = false;  }
  inline ~Pin() { };

  //accessors
  inline bool isConnected() { return connected; }
  
  //modifiers
  inline void setConnected() { connected = true; }
  inline void setUnconnected() { connected = false; }

  //misc
  inline int calculateCost(Point *sink, int sourceDist)
  { return getManhattanDistance(sink) + sourceDist; }
  

private:
  bool connected;
 
};

#endif
