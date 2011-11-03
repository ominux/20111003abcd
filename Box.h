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

#ifndef BOX_H
#define BOX_H

#include "Point.h"

class Box
{
public:
  //constructors
  inline Box() { width = height = 0; upper_left.setX(0); upper_left.setY(0); }
  Box(Point *, int w, int h);
  Box(int x, int y, int w, int h);

  //accessors
  inline int getWidth() { return width; }
  inline int getHeight() { return height; }
  inline Point * getUpperLeft() { return &upper_left; }
  inline int getUpperLeftX() { return upper_left.getX(); }
  inline int getUpperLeftY() { return upper_left.getY(); }
  inline int getPerimeter() { return 2*width + 2*height; }
  inline int getHalfPerimeter() { return width + height; }

private:
  int width;
  int height;
  Point upper_left;
};

#endif
