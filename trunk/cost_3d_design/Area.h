/*
Area.h
Function: Calculate the die area with reqired metal layer number
Authro: Qiaosha Zou
Time: 2011-11-10
*/

#ifndef AREA_ESTI
#define	AREA_ESTI

#include <stdio.h>
#include <cmath>

using namespace std;

class Area
{
	private:
		unsigned long gateCount;
		float gateSize;
		float rent_p;
		float fanout;
		float wire_pitch;
		float utilization;
		float die_area;

	public:
		inline Area() 
			{gateCount = 0;
			 gateSize = 0;
			}
		inline Area(unsigned long count, float size )
		{
			gateCount = count;
			gateSize = size;
			rent_p = 0.5;
			fanout = 4.0;
			wire_ptich = 1.9; //what is the unit
			utilization = 0.8;
		}
		
		inline Area(unsigned long count, float size, float p, float fo, float pitch, float util)
		{
			gateCount = count;
			size = gateSize;
			rent_p = p;
			wire_pitch = pitch;
			fanout = fo;
			utilization = util;
		}

		/**********set/get function*****************/
		inline void setGateCount(unsigned long count){gateCount = count;}
		inline void setGateSize(float size) {gateSize = size;}
		inline void setRent_p(float p){rent_p = p;}
		inline void setFanout(float fo) {fanout = fo;}
		inline void setWire_pitch(float pitch) {wire_pitch = pitch;}

		/*********area calculation*****************/
		double area_calc();

}

#endif

