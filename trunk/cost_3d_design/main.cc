/*
* main.cc
* Function: testing
* Author: Qiaosha Zou qszou@cse.psu.edu
* Time: 2011-11-26
*/


#include "Design.h"
#include "typedef.h"
#include "Tier.h"
#include "Module.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <cstring>
#include <list>

#define POPULATION 9 //total popluation number
#define MAX_ITER  6 //totoal iteration time

using namespace std;

typedef list<Design> DesignList;
typedef DesignList::iterator DesignsItr;

void read_modules(ifstream * file, ModuleLib * lib)
{
	char buffer[80];
	int module_size;
	file->getline(buffer, 80);
	sscanf(buffer, "%d", &module_size);

	for(int i = 0; i< module_size; i++)
	{
		int count; //gate count
		float size; //module area
		int connect_count;
		file->getline(buffer,80);
		sscanf(buffer, "%d %f", &count, &size);

		Module temp = new Module(i, count, size);

		file->getline(buffer,80);
		sscanf(buffer, "%d", &connect_count);

		int id, num;
		for(int m = 0; m < connect_count; m++)
		{
			file->getline(buffer, 80);
			sscanf(buffer, "%d %d", &id, &num);
			Connection connect = new Connection(id, num);
			temp->setConnections(*connect); //copy the content of the connection to connections in module
			delete connect;
		}		

		lib->push_back(*temp); //copy the module to library
		delete temp;
	}
	
}

int main(int argc, char *argv[])
{
	if(argc != 3){
		cout<< "./CostDesign infile outfile"<<endl;
		return 0;
	}
	
	ifstream input;
	ofstream output;

	input.open(argv[1], ios::in);
	if(!input)
	{
		cout<< argv[1] <<"cannot be opened. Exiting..." << endl;
		return -1;
	}

	output.open(argv[2], ios::out);
	if(!output)
	{
		cout<< argv[2] << "cannot be opened. Exiting..." << endl;
		//return -1;
	}
	
	//variables
	DesignList design_list;
	ModuleLib  module_lib;	
	
	//read file	
	read_modules(&input, &module_lib);
	
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

	//used in Tier.cc to calculate metal layer needed
	int fanout = 4;
	float wire_efficient = 0.85;
	float wire_pitch = 0.5; //unit um

	//main body
	for(int i =0; i< POPULATION; i++)
	{
		Design temp = new Design();
		//temp->partition();
		design_list.push_back(*temp);
		(design_list.back()).partition(module_lib);
		(design_list.back()).calc_design_cost(process, package, bonding, knob);
		delete temp;
	}	
		
	//sorting!!!!May need to change
	design_list.sort();

	int iter_time = 0; //iteration times
	while(iter_time < MAX_ITER)
	{
		list<DesignsItr> loser;
		int ingrade = 0;
		for(DesignsItr ditr = design_list.begin(); ditr != design_list.end(); ditr++, ingrade++)
		{
			//mutate part
			if(ingrade > POPULATION/3){
				if(ingrade<2*POPULATION/3){
					ditr->mutate();
					ditr->calc_design_cost(process, package, bonding, knob);
				}else{
					loser.push_back(ditr); //list of repartition design
				}
			}
		}
		for(loser::iterator loseritr = loser.being(); loseritr != loser.end(); loseritr++)
			design_list.erase(*loseritr);
		for(int regen = loser.size(); regen!=0; regen--)
		{
			Design temp = new Design();
			design_list.push_back(*temp);
			(design_list.back()).partition(module_lib);
			(design_list.back()).calc_design_cost(process, package, bonding, knob);
			delete temp;
		}

		loser.clear();
		design_list.sort();

		iter_time++;
	}

	//deconstruct??remember to deconstruct modules
	design_list.clear();
	module_lib.clear();

	return 0;
}

