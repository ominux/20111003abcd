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

//using namespace std;

class Area
{
	private:
		unsigned long gateCount;
		//float gateSize;
		float rent_p;
		float fanout;
		float wire_pitch;
		float utilization;
		float die_area;
		int	metal_layer;


	public:
	
		inline Area() 
			{gateCount = 0;
			// gateSize = 0;
			 metal_layer = 3;
			}
		inline Area(unsigned long count)
		{
			gateCount = count;
			//gateSize = 0;
			rent_p = 0.5;
			fanout = 4.0;
			wire_pitch = 1.9; //what is the unit
			utilization = 0.8;
			metal_layer = 3;
		}
		
		inline Area(unsigned long count, float p, float fo, float pitch, float util, int metal)
		{
			gateCount = count;
			//size = gateSize;
			rent_p = p;
			wire_pitch = pitch;
			fanout = fo;
			utilization = util;
			metal_layer = metal;
		}

		/**********set/get function*****************/
		inline void setGateCount(unsigned long count){gateCount = count;}
		//inline void setGateSize(float size) {gateSize = size;}
		inline void setRent_p(float p){rent_p = p;}
		inline void setFanout(float fo) {fanout = fo;}
		inline void setWire_pitch(float pitch) {wire_pitch = pitch;}
		inline void setMetal_layer(int metal) {metal_layer = metal;}

		/*********area calculation*****************/
		float	 area_calc();

};

#endif

