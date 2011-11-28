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
#include <cstdlib>

using namespace std;

float Design::calc_design_cost(ModuleLib all_module)
{
	float cost = 0;
	float wafer_cost;
	float util;
	float die_yield = 1, y;
	int i;

	//setting related parameters, from Xiangyu's, can changed to read from input file	
	PROCESS_PARA process;
	PACKAGE_PARA package;
	BONDING_PARA bonding;
	BONDING_KNOB knob;
	
	strcpy(process.name, "300mm - 45nm - TSMC - dula gate ox CMOS Logic");
	process.diameter = 300;
	process.technology_node = 45;
	process.default_metal_layer = 6;
	process.metal_layer_cost = 249.492;
	process.wafer_cost = 4232.795;
	process.wafer_sort_cost = 1182.66;
	process.defect_density = 0.4;
	process.kgd_cost = 4.565;

	bonding.thinning_cost = 250;
	bonding.d2w_bonding_yield = 0.99;
	bonding.w2w_bonding_yield = 0.99;
	bonding.w2w_bonding_cost = 22.9;
	bonding.d2w_bonding_cost = 115/20;
	bonding.tsv_etch_cost = 250;
	bonding.tsv_laser_cost = 0.001;

	knob.face = 0;
	knob.tsv = 1;
	knob.wafer_bonding = 1;

	//package.name = "pBGA";
	package.package_cost = 0.218;
	package.package_yield = 0.995;
	package.package_test_cost = 0.544;
	package.package_test_yield = 0.998;

	//begin the calculation
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

	//calculate the yield and util of the process, util means how many die per wafer
	util = (3.1415926) * process.diameter * process.diameter /4/area - 1 * 3.1415926 * process.diameter /sqrt(2* area);
	float density;
	density = process.defect_density/100;
	y = (1 - exp(0 - 2 * area * density)) / (2 * area * density);

	for(i = 0; i<number_tier; i++)
	{
		wafer_cost = process.wafer_cost;
		int metal_num = (*stackings)[i].getMetal_num();
		wafer_cost += metal_num * process.metal_layer_cost;
		wafer_cost += process.wafer_sort_cost;

		if(knob.wafer_bonding){ //d2w bonding
			wafer_cost += process.kgd_cost * util;//add KGD test cost
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
			cost += tsv_num[i] * bonding.tsv_laser_cost;
		else
			cost += bonding.tsv_etch_cost/util; 
	}	
	//thinning cost
	if(knob.face ==0)
		cost += bonding.thinning_cost * (number_tier - 1)/util;
	else
		cost += bonding.thinning_cost * (number_tier - 2)/util;

	//add bonding cost
	if(knob.wafer_bonding==0){
		cost += bonding.w2w_bonding_cost * (number_tier - 1) /util;
		die_yield *= pow(bonding.w2w_bonding_yield, (number_tier - 1));
	}
	else{
		cost += bonding.d2w_bonding_cost * (number_tier -1);
		die_yield *= pow(bonding.d2w_bonding_yield, (number_tier - 1));
	}

	//add package cost
	cost += package.package_cost;
	die_yield *= package.package_yield;
	
	//add test cost
	cost += package.package_test_cost;
	die_yield *= package.package_test_yield;

	//this die_yield includes each tier yield, bonding yield, package_yield, and pack_test_yield
	cost /= die_yield;

	return cost;
		
}

void Design::partition(ModuleLib all_module, int layer_count)
{

	//flow: first allocate modules to different tier, setup the tier and input num into modules
	//second, calculate the needed metal, and from this default, reduce metal layer to 3
	//third, each reduction, calculate the cost, and store the best design with min cost
	int i;
	int default_metal[MAX_TIER];
	int fo = 4;
	float ew = 0.85;
	float w_pitch = 0.5; //unit um
	float best_cost = 0.0; //current best design cost

	//put initialized tier into design
	for(i = 0; i< layer_count; i++)
	{
		Tier * temp_tier = new Tier();	
		stackings->push_back(*temp_tier);
		delete temp_tier;		 
	}
	
	//module partition, randomly
	for(ModuleLibItr moItr = all_module.begin(); moItr != all_module.end(); moItr++)
	{
		int choice;
		choice = rand() % layer_count;
		moItr->setTier(choice); //*moItr is type Module
		(*stackings)[choice].setModule(&(*moItr));	
	}

	//metal layer reduction iteration
	for(TierVectItr tierItr = stackings->begin(); tierItr != stackings->end(); tierItr++)
	{
		//calculate default metal layer
		tierItr->calc_metal_num(fo, ew, w_pitch);	
	}	
}

void Design::mutate(ModuleLib all_module)
{
	int mutate_tier = 0;
	float utilization =1.0;
	float w_pitch = 0.5;
	int fo = 4;
	float ew = 0.85;

	for(int i = 0; i < stackings->size(); i++)
	{
		float temp_util;
		temp_util = (*stackings)[i].metal_util(w_pitch, ew);
		//select the tier with least metal utilization
		if(temp_util < utilization)
		{
			utilization  = temp_util;
			mutate_tier = i;
		}
	}
	
	//mutation process
	

}

//setting the tsv number
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
