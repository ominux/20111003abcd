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
#include <fstream>
#include <climits>

#define OVERHEAD_RATIO 0.2

using namespace std;

float Design::calc_design_cost(ModuleLib all_module)
{
	float cost = 0;
	float wafer_cost;
	float util[MAX_TIER]; // wafer utilization of each tier
	float die_yield = 1;
	float y[MAX_TIER]; //die yield of each tier
	int i;

	tsv_pitch = 49.0;
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


	calc_tsv_num(all_module);
	//calculate the area of each tier tier+TSV overhead
	float area[MAX_TIER];
	for(i = 0; i < number_tier; i++)
	{
		float temp_area;
		temp_area = (*stackings)[i]->getTier_area();;
		//the bottom tier
		if(!i)
			temp_area += tsv_num[i]*tsv_pitch * OVERHEAD_RATIO;
		//the topest tier
		else if(i == number_tier -1)
			temp_area += tsv_num[i-1]*tsv_pitch * OVERHEAD_RATIO;
		//tiers in the middle
		else
			temp_area += (tsv_num[i-1] + tsv_num[i]) * tsv_pitch * OVERHEAD_RATIO;

		area[i] = temp_area;
	}	

	//calculate the yield and util of the process, util means how many die per wafer
	float density;
	density = process.defect_density/100;
	for(i = 0; i < number_tier; i++){	
		util[i] = (3.1415926) * process.diameter * process.diameter /4/area[i] - 1 * 3.1415926 * process.diameter /sqrt(2* area[i]);
		y[i] = (1 - exp(0 - 2 * area[i] * density)) / (2 * area[i] * density);
		//y[i] = exp(0 - area[i] * density);
	}

	for(i = 0; i<number_tier; i++)
	{
		wafer_cost = process.wafer_cost;
		int metal_num = (*stackings)[i]->getMetal_num();
		wafer_cost += metal_num * process.metal_layer_cost;
		wafer_cost += process.wafer_sort_cost;

		if(knob.wafer_bonding){ //d2w bonding
			wafer_cost += process.kgd_cost * util[i];//add KGD test cost
			cost += wafer_cost/util[i]/y[i];
		}

		else{
			cost += wafer_cost/util[i]; //consider the wafer utilization

			die_yield *= y[i];
		}
		cout<<"Cost of Tier"<< i<<" :" <<cost << " Yield: "<< y[i]*100 <<"%"<< endl;
	}
	//TSV cost
	for(i = 0; i <number_tier-1; i++)
	{
		if(knob.tsv == 0)
			cost += tsv_num[i] * bonding.tsv_laser_cost;
		else
			cost += bonding.tsv_etch_cost/util[i]; 
	}	
	//thinning cost
	if(knob.face ==0)
		cost += bonding.thinning_cost * (number_tier - 1)/util[i];
	else
		cost += bonding.thinning_cost * (number_tier - 2)/util[i];

	//add bonding cost
	if(knob.wafer_bonding==0){
		cost += bonding.w2w_bonding_cost * (number_tier - 1) /util[i];
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
	cout << "Design: calc_cost "<< cost << endl;

	return cost;
		
}

void Design::partition(ModuleLib all_module, int layer_count)
{

	//flow: first allocate modules to different tier, setup the tier and input num into modules
	//second, calculate the needed metal, and from this default, reduce metal layer to 3
	//third, each reduction, calculate the cost, and store the best design with min cost
	int i;
	int default_metal[MAX_TIER];
	float best_cost = 0.0; //current best design cost

	//put initialized tier into design
	Tier * temp_tier;
	for(i = 0; i< layer_count; i++)
	{
		temp_tier = new Tier();	
		stackings->push_back(temp_tier);
		//delete temp_tier;		 
	}
	
	//module partition, randomly
	for(ModuleLibItr moItr = all_module.begin(); moItr != all_module.end(); moItr++)
	{
		int choice;
		choice = rand() % layer_count;
		(*moItr)->setTier(choice); //*moItr is type Module
		(*stackings)[choice]->setModule(*moItr);	
	}

	//metal layer reduction iteration
	int count = 0;
	for(TierVectItr tierItr = stackings->begin(); tierItr != stackings->end(); tierItr++)
	{
		//calculate default metal layer and also get the area
		default_metal[count++] = (*tierItr)->calc_metal_num();	
	}	

	//initial the best tiers
	for(i = 0; i< layer_count; i++)
		best_tiers->push_back((*stackings)[i]);
	
	float temp_cost;
	//initial cost
	temp_cost = calc_design_cost(all_module);
	best_cost = temp_cost;

	for(int a = default_metal[0]; a > 0 ; a--){
		(*stackings)[0]->calc_area(a);
		cout << "tier 0 area "<< (*stackings)[0]->getTier_area() << endl;
		for(int b = default_metal[1]; b>0; b--){
			(*stackings)[1]->calc_area(b);
			cout << "tier 1 area" << (*stackings)[1]->getTier_area() << endl;
			if(layer_count > 2){ 
				for(int c = default_metal[2]; c > 0; c--){
					(*stackings)[2]->calc_area(c);
					cout << "tier 2 area" << (*stackings)[2]->getTier_area() << endl;
					if(layer_count > 3){
						for(int d = default_metal[3]; d > 0; d--){
							(*stackings)[3]->calc_area(d);
							cout << "tier 3 area" << (*stackings)[3]->getTier_area() << endl;
						
							temp_cost = calc_design_cost(all_module);
							if(temp_cost < best_cost)
							{
								best_cost = temp_cost;
								//best tier stored???
								(*best_tiers)[3]->setMetal_num(d);
								(*best_tiers)[3]->setTier_area((*stackings)[3]->getTier_area());
							}
						}
						
					}
					
					temp_cost = calc_design_cost(all_module);
					if(temp_cost < best_cost)
					{
						best_cost = temp_cost;
						//best tier stored???
					
						(*best_tiers)[2]->setMetal_num(c);
						(*best_tiers)[2]->setTier_area((*stackings)[2]->getTier_area());
					}
					
				} 
			}
			else{
				temp_cost = calc_design_cost(all_module);
				if(temp_cost < best_cost)
				{
					best_cost = temp_cost;
					//best tier stored???
					
					(*best_tiers)[1]->setMetal_num(b);
					(*best_tiers)[1]->setTier_area((*stackings)[1]->getTier_area());
					
					(*best_tiers)[0]->setMetal_num(a);
					(*best_tiers)[0]->setTier_area((*stackings)[0]->getTier_area());
				}
			}
		}
	}
	this->best_cost = best_cost;
}

void Design::mutate(ModuleLib all_module)
{
	int max_tier = 0; //the moudles come out of the least utilization tier
	int min_tier = 0;
	int old_metal = 0; //inital total metal count, used for comparison
	int new_metal; //metal count during mutation
	int iter_times = 0;
	int layer_count = 0;

	for(TierVectItr tier_itr = best_tiers->begin(); tier_itr != best_tiers->end(); tier_itr ++){
		new_metal += (*tier_itr)->getMetal_num();
		layer_count++;
	}

	do{ 	
		old_metal = new_metal;
		float min_util = 1.0;
		float max_util = 0.0;
		int gate_count = INT_MAX;
		for(int i = 0; i < stackings->size(); i++)
		{
			float temp_util;
			temp_util = (*best_tiers)[i]->metal_util();
			//select the tier with least metal utilization
			if(temp_util < min_util)
			{
				min_util  = temp_util;
				min_tier = i;
			}
			else if(temp_util > max_util)
			{
				max_util = temp_util;
				max_tier = i;
			}
		}
	
		//mutation process
		ModuleLst * temp_modules;
		temp_modules = (*best_tiers)[min_tier]->getModules();
	
		//put the least gate count module to another tier	
		int temp_gc;
		for(ModuleLstItr moitr = temp_modules->begin(); moitr != temp_modules->end(); moitr++)
		{
			temp_gc = (*moitr)->getCount();
			if(temp_gc < gate_count){
				gate_count = temp_gc;
				//select the tier that can accept the module
				int accept;
				if(layer_count == 2)
					accept = max_tier;
				else{
					do{
						accept = rand() % layer_count; 		
					}while(accept != min_tier && accept != max_tier);
				}
				//put the moduel into new tier
				(*best_tiers)[accept]->setModule(*moitr);	
				//delete the module from old tier
				temp_modules->erase(moitr);	
			}	
		}
	
		//get the new total metal layers
		for(TierVectItr tier_itr = best_tiers->begin(); tier_itr != best_tiers->end(); tier_itr ++){
			(*tier_itr)->calc_metal_num();
			new_metal += (*tier_itr)->getMetal_num();
		}

		//avoid infinite loop
		iter_times++;
		if(iter_times > 100)
			break;

	}while(new_metal <= old_metal);

	//calculate cost
	stackings->clear();
	//copy the best tiers back to stackings, for cost calculation is based on stackings
	for(TierVectItr tier_itr = best_tiers->begin(); tier_itr != best_tiers->end(); tier_itr++)
		stackings->push_back(*tier_itr);
	
	this->best_cost = calc_design_cost(all_module);
	
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
		ConnectVect * one_con = (*one)->getConnections();
		current_tier = (*one)->getTier();
		for(ConnectVectItr i = one_con->begin(); i!= one_con->end(); i++)
		{
			//get the connected module
			temp_id = (*i)->getID();
			con_tier = all_module[temp_id-1]->getTier(); //the id starts from 1
			if(con_tier>current_tier)
			{
				temp_num = (*i)->getNum();
				//from current tier, every upper tier should add tsv_num
				for(int k = current_tier; k<con_tier; k++)
					tsv_num[k] += temp_num;	
			}			
		}
		cout << "Module ID "<< (*one)->getID() << "Tier: "<<(*one)->getTier() << endl;
				
	}	
	
	for(int i = 0; i < MAX_TIER; i++)
		cout << "TSV numbers: "<<tsv_num[i] << endl;
}


//print design function
void Design::print_design(ofstream &output)
{
	output<<"The best cost is " << this->best_cost << endl;	
	for(TierVectItr tier_itr = best_tiers->begin(); tier_itr != best_tiers->end(); tier_itr++)
	{
		output << "Tier: " << endl;
		output << "metal_num: "<< (*tier_itr)->getMetal_num() << endl;
		output << "tier area: "<< (*tier_itr)->getTier_area() << endl;
		
		ModuleLst * module_list;
		module_list = (*tier_itr)->getModules();

		int count = 0;
		for(ModuleLstItr moitr = module_list->begin(); moitr != module_list->end(); moitr++)
		{
			output << "Module ID: "<< (*moitr)->getID() << endl;	
			count++;
		}
		
		output << "contains: " << count << "modules" << endl;
	}

	output << "End of this design..." << endl;
}
