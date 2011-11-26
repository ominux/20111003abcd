/* Design.cc
* Function: calculate design cost
*	perform 3D partitioning
*	calculate the number of TSVs
* Author: Qiaosha Zou
* Time: 2011-11-25
*/

#include "Design.h"
#include <cmath>
#include <stdio.h>
#include <iostream>

using namespace std;

float Design::calc_design_cost(PROCESS_PARA proc, PACKAGE_PARA pack, BONDING_PARA bond, BONDING_KNOB knob)
{
	float cost = 0;
	float wafer_cost;
	float util;
	float die_yield = 1, y;
	int i;

	int number_tier = 0;
	for(TierVectItr m = stackings->begin(); m!= stackings->end(); m++)
		number_tier++;

	//calculate the maximum area, max area among tier+TSV overhead
	float area = 0.0;
	float temp_area = 0.0;
	for(i = 0; i < number_tier; i++)
	{
		temp_area = (*stackings)[i].getTier_area();;
		//the bottom tier
		if(!i)
			temp_area += tsv_num[i]*tsv_pitch;
		//the topest tier
		else if(i == number_tier -1)
			temp_area += tsv_num[i-1]*tsv_pitch;
		//tiers in the middle
		else
			temp_area += (tsv_num[i-1] + tsv_num[i]) * tsv_pitch;

		if(temp_area > area)
			area = temp_area;
	}	

	//calculate the yield and util of the process
	util = (3.1415926) * proc.diameter * proc.diameter /4/area - 1 * 3.1415926 * proc.diameter /sqrt(2* area);
	float density;
	density = proc.defect_density/100;
	y = (1 - exp(0 - 2 * area * density)) / (2 * area * density);

	for(i = 0; i<number_tier; i++)
	{
		wafer_cost = proc.wafer_cost;
		int metal_num = (*stackings)[i].getMetal_num();
		wafer_cost += metal_num * proc.metal_layer_cost;
		wafer_cost += proc.wafer_sort_cost;

		if(knob.wafer_bonding){ //d2w bonding
			wafer_cost += proc.kgd_cost * util;//add KGD test cost
			y = 1; //all the die are good die. yield is 1
		}

		cost += wafer_cost/util; //consider the wafer utilization

		die_yield *= y;
		cout<<"Cost of Tier"<< i<<" :" <<cost << " Yield: "<< y*100 <<"%"<< endl;
	}
	//TSV cost
	for(i = 0; i <number_tier-1; i++)
	{
		if(knob.tsv == 0)
			cost += tsv_num[i] * bond.tsv_laser_cost;
		else
			cost += bond.tsv_etch_cost/util; 
	}	
	//thinning cost
	if(knob.face ==0)
		cost += bond.thinning_cost * (number_tier - 1)/util;
	else
		cost += bond.thinning_cost * (number_tier - 2)/util;

	//add bonding cost
	if(knob.wafer_bonding==0){
		cost += bond.w2w_bonding_cost * (number_tier - 1) /util;
		die_yield *= pow(bond.w2w_bonding_yield, (number_tier - 1));
	}
	else{
		cost += bond.d2w_bonding_cost * (number_tier -1);
		die_yield *= pow(bond.d2w_bonding_yield, (number_tier - 1));
	}

	//add package cost
	cost += pack.package_cost;
	die_yield *= pack.package_yield;
	
	//add test cost
	cost += pack.package_test_cost;
	die_yield *= pack.package_test_yield;

	//this die_yield includes each tier yield, bonding yield, package_yield, and pack_test_yield
	cost /= die_yield;

	return cost;
		
}

void Design::partition_threed(ModuleLib all_module)
{
	
}

void Design::calc_tsv_num(ModuleLib all_module)
{
	ModuleLibItr one;
	int	temp_id;
	int 	temp_num;
	int	current_tier;
	int	con_tier;

	for(one = all_module.begin(); one!= all_module.end(); one++)
	{
		//get the connection of one module
		ConnectVect * one_con = one->getConnections();
		current_tier = one->getTier();
		for(ConnectVectItr i = one_con->begin(); i!= one_con->end(); i++)
		{
			//get the connected module
			temp_id = i->getID();
			con_tier = all_module[temp_id].getTier();
			if(con_tier>current_tier)
			{
				temp_num = i->getNum();
				//from current tier, every upper tier should add tsv_num
				for(int k = current_tier; k<con_tier; k++)
					tsv_num[k] += temp_num;	
			}			
		}		
	}	
}
