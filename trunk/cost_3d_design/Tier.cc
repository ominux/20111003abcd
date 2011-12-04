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
	unsigned long count = 0;		
	float init_area = 0.0;
	//calculate the total area by all modules
	for(ModuleLstItr i = modules->begin(); i!= modules->end(); i++){
		init_area += (*i)->getArea();
		count += (*i)->getCount();
		cout << "module id" << (*i)->getID();
		cout << " size " << (*i)->getArea();
		cout << " count " << (*i)->getCount() << endl;	
	}
	this->tier_area = init_area;
	this->gateCount = count;

	cout << "Tier.cc area " << this->tier_area  << " " << init_area << endl;
	cout << "Tier.cc count " << this->gateCount << endl;
	
	//calculate average wire length
	float i, j, k, rm;

	i= 2.0/9;
	j = (1-pow(4, RENT_P - 1))/(1-pow(gateCount, RENT_P - 1));	

	k = (1-pow(gateCount, RENT_P - 1.5))/(1-pow(4, RENT_P - 1.5));

	rm = i * j*(7*(pow(gateCount, RENT_P - 0.5)-1)/(pow(4, RENT_P - 0.5)-1) - k);		
	cout << "Tier.cc calc_metal_num rm " << rm << endl;
	
	//calculate the metal layer needed
	int num;
	num =(int)(FANOUT *rm *WIRE_PITCH *sqrt(gateCount/tier_area)/WIRE_EFF + 0.5);//they should all int mm scale1

	if(num > 10)
		num = 10;

	else if(num < 3)
		num = 3;
	
	this->metal_num = num;

	cout << "Tier.cc calc_metal_num " << num << endl;
	return num;
}

float Tier::calc_area(int num)
{
	float area;
	//calculation

	/*float i, j, k, rm, dg;

	i= 2.0/9;
	j = (1-pow(4, rent_p-1))/(1-pow(gateCount, rent_p-1));	

	k = (1-pow(gateCount, rent_p-1.5))/(1-pow(4, rent_p - 1.5));

	rm = i * j*(7*(pow(gateCount, rent_p-0.5)-1)/(pow(4, rent_p - 0.5)-1) - k);		
	
	dg = FANOUT * WIRE_PITCH * rm / (WIRE_EFF * num);
	
	area = gateCount * pow(dg, 2);	
	*/
	
	//each time reduce one metal layer, the area is divided by 0.8
	int diff;
	if(num < this->metal_num){
		diff = this->metal_num - num;
		area = this->tier_area/pow(AREA_UTIL, diff);
	}

	else if(num > this->metal_num){
		diff = num - this->metal_num;
		area = this->tier_area * pow(AREA_UTIL, diff);
	}

	else
		area = this->tier_area;

	this->tier_area = area;
	this->metal_num = num;	
	return area;
}

float Tier::metal_util()
{
	float supply;
	float demand;
	float utilization;

	float w_pitch[10] = {0.14, 0.19, 0.14, 0.28, 0.28, 0.28, 0.8, 0.8, 1.6, 1.6};
	
	//calculate average wire length
	float i, j, k;

	i= 2.0/9;
	j = (1-pow(4, RENT_P - 1))/(1-pow(gateCount, RENT_P - 1));	

	k = (1-pow(gateCount, RENT_P - 1.5))/(1-pow(4, RENT_P - 1.5));
	
	//the demand is not related to area!!??
	demand = i * j*(7*(pow(gateCount, RENT_P - 0.5)-1)/(pow(4, RENT_P - 0.5)-1) - k);	

	for(int i = 0; i < metal_num; i++)	
		supply = tier_area * WIRE_EFF / w_pitch[i] ;
	utilization = demand / supply;

	return utilization;
}
