/* Tier.cc
* Function: calculate metal layer number, tier area,
*	switch modules between tiers, calculate module cost
* Author: Qiaosha Zou
* Time: 2011-11-25
*/

#include <cmath>
#include "Tier.h"

int Tier::calc_metal_num(int fo, float ew, float w_pitch)
{
	unsigned int gateCount;		
	if(tier_area =0.0)
	{
		float init_area;
		//calculate the total area by all modules
		for(ModuleLstItr i = modules->begin(); i!= modules->end(); i++)
			init_area += (*i)->getArea();	
		setTier_area(init_area);
	}	
	
	for(ModuleLstItr i = modules->begin(); i!= modules->end(); i++)
		gateCount += (*i)->getCount();

	//calculate average wire length
	float i, j, k, rm;
	float rent_p = 0.6;

	i= 2.0/9;
	j = (1-pow(4, rent_p-1))/(1-pow(gateCount, rent_p-1));	

	k = (1-pow(gateCount, rent_p-1.5))/(1-pow(4, rent_p - 1.5));

	rm = i * j*(7*(pow(gateCount, rent_p-0.5)-1)/(pow(4, rent_p - 0.5)-1) - k);		

	//calculate the metal layer needed
	int num;
	num =(int)fo *rm *w_pitch *sqrt(gateCount/tier_area)/ew;//What is the unit??
	
	this->metal_num = num;

	return num;
}

float Tier::calc_area(int num)
{
	
}

