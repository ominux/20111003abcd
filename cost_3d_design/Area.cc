/*
Area.cc
Author: Qiaosha Zou qszou@cse.psu.edu
Time: 2011-11-10
Description: the  main function to calculate area under certain
*	     metal layer number constraint
*/

#include <cmath>
#include "Area.h"

//using namespace std;

float Area::area_calc()
{
	float rm;
	float dg;
	float i;
	float j;
	float k;

	i= 2.0/9;
	j = (1-pow(4, rent_p-1))/(1-pow(gateCount, rent_p-1));	

	k = (1-pow(gateCount, rent_p-1.5))/(1-pow(4, rent_p - 1.5));

	if(rent_p==0.5)
		rm = i * j*(7*(pow(gateCount, rent_p-0.5)-1)/(pow(4, rent_p - 0.5)-1) - k);		
	else
		rm = i*j*(7*log(gateCount)/log(4) - k);
	
	dg = fanout* rm * wire_pitch / (utilization * metal_layer);
	die_area = gateCount* pow(dg,2);

	return die_area;
}

