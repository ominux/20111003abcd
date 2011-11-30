/* Tier.cc
* Function: calculate metal layer number, tier area,
*	switch modules between tiers, calculate module cost
* Author: Qiaosha Zou
* Time: 2011-11-25
*/

#include <cmath>
#include "Tier.h"

int Tier::calc_metal_num()
{
	unsigned int gateCount;		
	float init_area;
	//calculate the total area by all modules
	for(ModuleLstItr i = modules->begin(); i!= modules->end(); i++)
		init_area += (*i)->getArea();	
	this->tier_area = init_area;
	
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
	num =(int)FANOUT *rm *WIRE_PITCH *sqrt(gateCount/tier_area)/WIRE_EFF;//What is the unit??

	if(num > 10)
		num = 10;	
	this->metal_num = num;
	return num;
}

float Tier::calc_area(int num)
{
	float area;
	//calculation
	unsigned int gateCount;
	for(ModuleLstItr i = modules->begin(); i!= modules->end(); i++)
		gateCount += (*i)->getCount();

	float i, j, k, rm, dg;
	float rent_p = 0.6;

	i= 2.0/9;
	j = (1-pow(4, rent_p-1))/(1-pow(gateCount, rent_p-1));	

	k = (1-pow(gateCount, rent_p-1.5))/(1-pow(4, rent_p - 1.5));

	rm = i * j*(7*(pow(gateCount, rent_p-0.5)-1)/(pow(4, rent_p - 0.5)-1) - k);		
	
	dg = FANOUT * WIRE_PITCH * rm / (WIRE_EFF * num);
	
	area = gateCount * pow(dg, 2);	

	this->tier_area = area;
	this->metal_num = num;	
	return area;
}

float Tier::metal_util()
{
	float supply;
	float demand;
	float utilization;
	unsigned int gateCount;

	int w_pitch[10] = {0.14, 0.19, 0.14, 0.28, 0.28, 0.28, 0.8, 0.8, 1.6, 1.6};
	
	for(ModuleLstItr i = modules->begin(); i!= modules->end(); i++)
		gateCount += (*i)->getCount();

	//calculate average wire length
	float i, j, k;
	float rent_p = 0.6;

	i= 2.0/9;
	j = (1-pow(4, rent_p-1))/(1-pow(gateCount, rent_p-1));	

	k = (1-pow(gateCount, rent_p-1.5))/(1-pow(4, rent_p - 1.5));
	
	//the demand is not related to area!!??
	demand = i * j*(7*(pow(gateCount, rent_p-0.5)-1)/(pow(4, rent_p - 0.5)-1) - k);	

	for(int i = 0; i < metal_num; i++)	
		supply = tier_area * WIRE_EFF / w_pitch[i] ;
	utilization = demand / supply;

	return utilization;
}
