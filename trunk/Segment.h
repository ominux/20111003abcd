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

#ifndef SEGMENT_H
#define SEGMENT_H

#include "Point.h"

class Segment
{
public:
  //constructors destructors
  inline Segment() { one.setX(0); one.setY(0); two.setX(0); two.setY(0); }
  inline Segment(int x1,int y1,int x2,int y2) 
  { one.setX(x1); one.setY(y1); two.setX(x2); two.setY(y2); }

  //modifiers
  inline void setPointOne(int x, int y) { one.setX(x); one.setY(y); };
  inline void setPointTwo(int x, int y) { two.setX(x); two.setY(y); };
  inline void setPointOne(Point x) { one.setX(x.getX()); one.setY(x.getY()); }
  inline void setPointTwo(Point x) { two.setX(x.getX()); two.setY(x.getY()); }
  inline void setPoints(Point x, Point y) { setPointOne(x); setPointTwo(y); }
  
  //accessors
  inline Point * getPointOne() { return &one; }
  inline Point * getPointTwo() { return &two; }

  //misc
  inline bool isHorizontal() { return one.getY() == two.getY(); }
  inline bool isVertical() { return one.getX() == two.getX(); }
  inline bool isSamePoint() { return (one.getX() == two.getX() && 
				      one.getY() == two.getY()); }
  bool overlap(Segment *seg);

  double getLength();
  void print();


private:
  Point one,two;

  int min(int,int);
  int max(int,int);

};

#endif
