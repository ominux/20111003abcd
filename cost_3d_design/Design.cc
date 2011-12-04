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
#include <cfloat>

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
	knob.wafer_bonding = 0;

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
			temp_area += tsv_num[i]*TSV_PITCH * OVERHEAD_RATIO;
		//the topest tier
		else if(i == number_tier -1)
			temp_area += tsv_num[i-1]*TSV_PITCH * OVERHEAD_RATIO;
		//tiers in the middle
		else
			temp_area += (tsv_num[i-1] + tsv_num[i]) * TSV_PITCH * OVERHEAD_RATIO;

		area[i] = temp_area;
	}	

	//calculate the yield and util of the process, util means how many die per wafer
	float density;
	density = process.defect_density/100; //convert from cm scale to mm scale
	//area should be in mm^2 scale 
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
		cout << endl;
		cout << "metal layer cost " << metal_num * process.metal_layer_cost << endl; 
		wafer_cost += process.wafer_sort_cost;
		cout << "inter wafer cost" << wafer_cost << endl;

		if(knob.wafer_bonding){ //d2w bonding
			wafer_cost += process.kgd_cost * util[i];//add KGD test cost
			cost += wafer_cost/util[i]/y[i];
		}

		else{
			cost += wafer_cost/util[i]; //consider the wafer utilization

			die_yield *= y[i];
		}
		cout<<"Cost of Tier"<< i<<" :" <<cost << " Yield: "<< y[i]*100 <<"%"<< endl;
		cout << "Util " << util[i] << endl;
		cout << "Metal layer " << (*stackings)[i]->getMetal_num()<< endl ;
		cout << "Area " << (*stackings)[i]->getTier_area() << endl;
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

	for(i = 0; i< layer_count; i++)
	{
		temp_tier = new Tier();
		best_tiers->push_back(temp_tier);
	}
	
	//module partition, randomly
	for(ModuleLibItr moItr = all_module.begin(); moItr != all_module.end(); moItr++)
	{
		int choice;
		choice = rand() % layer_count;
		cout << "Partitioning" << "Module " << (*moItr)->getID() << "put to tier " << choice <<endl;
		(*moItr)->setTier(choice); //*moItr is type Module
		(*stackings)[choice]->setModule(*moItr);	
		(*best_tiers)[choice]->setModule(*moItr);
	}

	//metal layer reduction iteration
	int count = 0;
	for(TierVectItr tierItr = stackings->begin(); tierItr != stackings->end(); tierItr++)
	{
		//calculate default metal layer and also get the area
		default_metal[count++] = (*tierItr)->calc_metal_num();		
	}	

	for(i = 0; i < layer_count; i++)
	{
		(*best_tiers)[i]->setMetal_num((*stackings)[i]->getMetal_num());
		(*best_tiers)[i]->setTier_area((*stackings)[i]->getTier_area());
	}

	for(TierVectItr tierItr = best_tiers->begin(); tierItr != best_tiers->end(); tierItr++)
	{
		cout << "Best Tier area " <<(*tierItr)->getTier_area();
		cout << "Metal num " << (*tierItr)->getMetal_num() << endl; 
	}
	
	float temp_cost;
	//initial cost
	temp_cost = calc_design_cost(all_module);
	best_cost = temp_cost;

	for(int a = default_metal[0]; a > 2 ; a--){
		(*stackings)[0]->calc_area(a);
		//cout << "tier 0 area "<< (*stackings)[0]->getTier_area() << endl;
		for(int b = default_metal[1]; b > 2; b--){
			(*stackings)[1]->calc_area(b);
			//cout << "tier 1 area" << (*stackings)[1]->getTier_area() << endl;
			if(layer_count > 2){ 
				for(int c = default_metal[2]; c > 2; c--){
					(*stackings)[2]->calc_area(c);
					//cout << "tier 2 area" << (*stackings)[2]->getTier_area() << endl;
					if(layer_count > 3){
						for(int d = default_metal[3]; d > 2; d--){
							(*stackings)[3]->calc_area(d);
							//cout << "tier 3 area" << (*stackings)[3]->getTier_area() << endl;
						
							temp_cost = calc_design_cost(all_module);
							if(temp_cost < best_cost)
							{
								best_cost = temp_cost;
								//best tier stored???
								(*best_tiers)[3]->setMetal_num(d);
								(*best_tiers)[3]->setTier_area((*stackings)[3]->getTier_area());
							
								//cout << "Partition!!" << endl;
								//cout << "Tier 3" << "metal num " << (*best_tiers)[3]->getMetal_num();
								//cout << "area " << (*best_tiers)[3] -> getTier_area();
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
					
						//cout << "Tier 2" << "metal num " << (*best_tiers)[2]->getMetal_num();
					//	cout << "area " << (*best_tiers)[2] -> getTier_area();
					}	
				} 
			}
			else{
				temp_cost = calc_design_cost(all_module);
				if(temp_cost < best_cost)
				{
					best_cost = temp_cost;
					//best tier stored???
				
					cout << "record the best Tiers!!!!" << endl;
	
					(*best_tiers)[1]->setMetal_num(b);
					(*best_tiers)[1]->setTier_area((*stackings)[1]->getTier_area());
					
					(*best_tiers)[0]->setMetal_num(a);
					(*best_tiers)[0]->setTier_area((*stackings)[0]->getTier_area());

					/*cout << "Partition!!" << endl;
					cout << "Tier 0" << "metal num " << (*best_tiers)[0]->getMetal_num();
					cout << "area " << (*best_tiers)[0] -> getTier_area();

					cout << "Tier 1" << "metal num " << (*best_tiers)[1]->getMetal_num();
					cout << "area " << (*best_tiers)[1] -> getTier_area(); */
				}
			}
		}
	}

	for(TierVectItr tierItr = best_tiers->begin(); tierItr != best_tiers->end(); tierItr++)
	{
		cout << "Best Tier area " <<(*tierItr)->getTier_area();
		cout << "Metal num " << (*tierItr)->getMetal_num() << endl; 
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

	cout << "Begin Mutation ... "<<endl;

	for(TierVectItr tier_itr = best_tiers->begin(); tier_itr != best_tiers->end(); tier_itr ++){
		new_metal += (*tier_itr)->getMetal_num();
		layer_count++;
	}

	do{ 	
		old_metal = new_metal;
		float min_area = FLT_MAX;
		float max_area = 0.0;
		unsigned long gate_count = ULONG_MAX;
		for(int i = 0; i < stackings->size(); i++)
		{
			float temp_area;
			temp_area = (*best_tiers)[i]->getTier_area();
			//select the tier with least metal utilization
			if(temp_area < min_area)
			{
				min_area  = temp_area;
				min_tier = i;
			}
			if(temp_area > max_area)
			{
				max_area = temp_area;
				max_tier = i;
			}
		}
	
		//mutation process
		ModuleLst * temp_modules;
		temp_modules = (*best_tiers)[max_tier]->getModules();
	
		//put the least gate count module to another tier	
		int temp_gc;
		ModuleLstItr loser;
		if(temp_modules->size() == 1)
			break;

		//find the module with least gatecount
		for(ModuleLstItr moitr = temp_modules->begin(); moitr != temp_modules->end(); moitr++)
		{
			temp_gc = (*moitr)->getCount();
			//find the smallest one
			if(temp_gc < gate_count){
				gate_count = temp_gc;
				//record the one that with least gate count
				loser = moitr;
			}	
		}

		cout << "Module ID" << (*loser)->getID()<< endl;
		temp_modules->erase(loser);
		(*best_tiers)[min_tier]->setModule(*loser);
	
		//get the new total metal layers
		new_metal = 0;
		for(TierVectItr tier_itr = best_tiers->begin(); tier_itr != best_tiers->end(); tier_itr ++){
			(*tier_itr)->calc_metal_num();
			new_metal += (*tier_itr)->getMetal_num();
		}

		//avoid infinite loop
		iter_times++;
		if(iter_times > 10)
			break;

	}while(new_metal <= old_metal);

	//calculate cost
	stackings->clear();
	
	for(TierVectItr tierItr = best_tiers->begin(); tierItr != best_tiers->end(); tierItr++)
	{
		cout << "Best Tier area " <<(*tierItr)->getTier_area();
		cout << "Metal num " << (*tierItr)->getMetal_num() << endl; 
	}

	//copy the best tiers back to stackings, for cost calculation is based on stackings
	for(TierVectItr tier_itr = best_tiers->begin(); tier_itr != best_tiers->end(); tier_itr++)
		stackings->push_back(*tier_itr);
	
	this->best_cost = calc_design_cost(all_module);

	cout << "End of Mutation..." << endl;	
}

//setting the tsv number
void Design::calc_tsv_num(ModuleLib all_module)
{
	unsigned long tempCount[MAX_TIER];
	
/*	for(one = all_module.begin(); one!= all_module.end(); one++)
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
*/
	int i = 0;
	float rent;
	float temp1, temp2, temp3;
	unsigned long add_result;
	int number;
	for(TierVectItr tieritr = stackings->begin(); tieritr != stackings->end(); tieritr++)
	{	
		tempCount[i] = (*tieritr)->getGatecount();
		i++;	
	}

	for(int j = 0; j < i - 1; j++)
	{
		rent = RENT_P - 1;
		add_result = tempCount[j] + tempCount[j+1];
		temp1 = ALPHA * RENT_K * add_result * (1- pow(add_result, (RENT_P - 1)));
		temp2 = ALPHA * RENT_K * tempCount[j] * (1 - pow(tempCount[j], (RENT_P - 1)));
		temp3 = ALPHA * RENT_K * tempCount[j+1] * (1 - pow(tempCount[j+1], (RENT_P - 1)));

		 number =(int)(temp1 - temp2 - temp3);
		if(number < 0)
			this->tsv_num[j] = 0;

		else
			this->tsv_num[j] = number;	
	}
	
	//test
	//for(int i = 0; i < MAX_TIER; i++)
	//	cout << "TSV numbers: "<<tsv_num[i] << endl;
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
		output << "gate Count: "<<(*tier_itr)->getGatecount() << endl;		
		ModuleLst * module_list;
		module_list = (*tier_itr)->getModules();

		int count = 0;
		for(ModuleLstItr moitr = module_list->begin(); moitr != module_list->end(); moitr++)
		{
			output << "Module ID: "<< (*moitr)->getID() << endl;	
			count++;
		}
		
		output << "contains: " << count << " modules" << endl;
		output << endl;
	}

	output << "End of this design..." << endl;
	output << endl;
}
