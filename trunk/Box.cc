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

#include "Box.h"

Box::Box(Point *aPoint, int w, int h)
{
  upper_left.setX(aPoint->getX());
  upper_left.setY(aPoint->getY());
  width = w;
  height = h;
}

Box::Box(int x,int y,int w,int h)
{
  upper_left.setX(x);
  upper_left.setY(y);
  width = w;
  height = h;
}



